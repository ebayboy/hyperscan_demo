
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


int main(int argc, char *argv[]) {

    //char *data = "abc teakettleeeeeeee";
    char *data = "abcdef teakettleeeeeeee ijklM";
    //char *data = "abcdefxxfoobarrrghabcxdefxteakettleeeeexxxxijklmxxdef";
    fprintf(stdout, "data:[%s]\n", data);

    hs_database_t *db = NULL;
    hs_compile_error_t *compile_err = NULL;

#if 1
    const char *expr[] = {
        "abc",              /* 101 */
        "def",              /* 102 */
        "foobar.*gh",       /* 103 */
        "teakettle{4,10}",  /* 104 */
        "ijkl[mMn]",        /* 105 */

        /* 注意: id大的规则在前面, 在执行id大的规则的时候，
         * 保证id小的规则已经匹配完成, 否则未匹配的取非会为True*/
        //"104 & !105"         /*  1001 */ 
        "101 & !105"         /*  1001 */ 
    }; 

    /* RUN:
     * ./simplegrep_multi
     * data:[abcdef teakettleeeeeeee ijklM]
     * id:[101] Match for at offset 3
     *
     * TODO ? 此时1001表达式逻辑值为真， 但是没有匹配的规则
     * id:[1001] Match for at offset 3  
     * id:[102] Match for at offset 6
     * id:[104] Match for at offset 19
     * id:[105] Match for at offset 29
     **/

    /* hit 101 & 104 & 105 */
    unsigned flags[] = {
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_COMBINATION |HS_FLAG_SINGLEMATCH };

    unsigned ids[] = {101, 102, 103, 104, 105, 1001};
#else 
    const char *expr[] = {
        "abc",
        "def",
        "foobar.*gh",
        "teakettle{4,10}",
        "ijkl[mMn]", /* 105 */
        "(101 & 102 & 103) | (104 & !105)", /* 1001 */
        "!101 & 102", /* 1002 */
        "!(!101 | 102)", /* 1003 */
        "101 & !102" /* 1004 */
    };
    unsigned flags[] = {
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        0,
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH,
        HS_FLAG_COMBINATION,
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH,
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH};
    unsigned ids[] = {101, 102, 103, 104, 105, 1001, 1002, 1003, 1004};
#endif

    /*  ./simplegrep_multi
     *  data:[abcdef teakettleeeeeeee ijklM]
     *  id:[101] Match for at offset 3
     *  id:[1001] Match for at offset 3
     *  id:[102] Match for at offset 6
     *  id:[104] Match for at offset 19
     *  id:[105] Match for at offset 29
     **/

    hs_error_t err = hs_compile_multi(expr, flags, ids, 
            sizeof(ids)/sizeof(unsigned), 
            HS_MODE_NOSTREAM, NULL, 
            &db, &compile_err); 

    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(db, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        hs_free_database(db);
        return -1;
    }

    if (hs_scan(db, data, strlen(data), 0, scratch, on_match, expr) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        hs_free_database(db);
        return -1;
    }

    hs_free_scratch(scratch);
    hs_free_database(db);

    return 0;
}
