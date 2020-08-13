#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hs.h>
#include <time.h>


long long now() {
    struct timeval tv; 
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000*1000 + tv.tv_usec;
}

/* API document: 
 * http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SINGLEMATCH
 **/

static int eventHandler(unsigned int id, unsigned long long from,
        unsigned long long to, unsigned int flags, void *ctx) {
    //printf("Match for pattern \"%s\" at from %llu  to %llu  flags:%u\n", (char *)ctx, from, to, flags);
    return 0;
}

int main(int argc, char *argv[]) {
#if 0
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pattern> <input>\n", argv[0]);
        return -1;
    }

    char *pattern = argv[1];
    char *inputData = argv[2];
#else
    char *pattern = "10014";
    
    //hyperscan 对^和$都有严格校验， 必须出现在正确位置 char *pattern = "/testsss/path/@#$%";
    //char *inputData = "/10014/prefix 10014 aa10111bb 10112 10113 10114 foo";
    char *inputData = "10101555adsfasdfasdfasdfasf123123safasseafs";
#endif

    hs_database_t *database;
    hs_compile_error_t *compile_err;


#define RULES_HS_FLAGS   (HS_FLAG_CASELESS    | \
                HS_FLAG_SINGLEMATCH | \
                HS_FLAG_DOTALL)


#define RULES_HS_FLAGS_LEFTMOST        (HS_FLAG_CASELESS    | \
                HS_FLAG_DOTALL      | \
                HS_FLAG_SOM_LEFTMOST) 

    /* 如果要获取回调函数的from位置，则编译时增加 HS_FLAG_SOM_LEFTMOST */
    /* database */
    if (hs_compile(pattern, RULES_HS_FLAGS, HS_MODE_BLOCK, NULL, &database,
                &compile_err) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to compile pattern \"%s\": %s\n",
                pattern, compile_err->message);
        hs_free_compile_error(compile_err);
        return -1;
    }

    /* scratch */
    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(database, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        hs_free_database(database);
        return -1;
    }

    long long st = now();
    /* scan */
    if (hs_scan(database, inputData, strlen(inputData), 
                0, scratch, eventHandler, pattern) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        free(inputData);
        hs_free_database(database);
        return -1;
    }

    fprintf(stderr, "%s:%d str[%s] pattern[%s] cost:%lu(us) \n", 
			__func__, __LINE__, inputData, pattern, now() - st);

    /* free scratch & db */
    hs_free_scratch(scratch);
    hs_free_database(database);
    return 0;
}


