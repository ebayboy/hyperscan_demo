
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hs.h>

static int on_match(unsigned int id, unsigned long long from,
                        unsigned long long to, unsigned int flags, void *ctx) {
    printf("id:[%d] Match for at offset %llu\n", id, to);
    return 0;
}


int main(int argc, char *argv[]) 
{
    /* data ijklM cross package */
#if 1
    /* multi package check */
    const char *data[] = {"abc teakettleeeeeeeeijk", "lM"};
    unsigned int len[] = {strlen(data[0]), strlen(data[1])};
#else 
    /* single packet check */
    const char *data[] = {"abc teakettleeeeeeeeijk"};
    unsigned int len[] = {strlen(data[0])};
#endif

    hs_database_t *db = NULL;
    hs_compile_error_t *compile_err = NULL;
    const char *expr[] = {
        "abc",              /* 101 */
        "def",              /* 102 */
        "foobar.*gh",       /* 103 */
        "teakettle{4,10}",  /* 104 */
        "ijkl[mMn]",        /* 105 */

        /* 注意: id大的规则在前面, 在执行id大的规则的时候，
         * 保证id小的规则已经匹配完成, 否则未匹配的取非会为True*/
        //"104 & !105"         /*  1001 */ 
        "!105 & 104"         /*  1001 */ 
    }; 

    /* hit 101 & 104 & 105 */
    unsigned flags[] = {
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH};

    unsigned ids[] = {101, 102, 103, 104, 105, 1001};

    hs_error_t err = hs_compile_multi(expr, flags, ids, 
            sizeof(ids)/sizeof(unsigned), HS_MODE_VECTORED,
            NULL, &db, &compile_err);

    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(db, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        hs_free_database(db);
        return -1;
    }

    err = hs_scan_vector(db, data, len, sizeof(len)/sizeof(unsigned int), 0, scratch, on_match, NULL);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        hs_free_database(db);
        return -1;
    }

    hs_free_scratch(scratch);
    hs_free_database(db);

    return 0;
}
