#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hs.h>

static int eventHandler(unsigned int id, unsigned long long from,
        unsigned long long to, unsigned int flags, void *ctx) {
    printf("Match for pattern \"%s\" at offset %llu\n", (char *)ctx, to);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pattern> <input>\n", argv[0]);
        return -1;
    }

    char *pattern = argv[1];
    char *inputData = argv[2];

    hs_database_t *database;
    hs_compile_error_t *compile_err;
    /* database */
    if (hs_compile(pattern, HS_FLAG_DOTALL, HS_MODE_BLOCK, NULL, &database,
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

    /* scan */
    if (hs_scan(database, inputData, strlen(inputData), 
                0, scratch, eventHandler, pattern) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        free(inputData);
        hs_free_database(database);
        return -1;
    }

    /* free scratch & db */
    hs_free_scratch(scratch);
    hs_free_database(database);
    return 0;
}


