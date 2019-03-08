
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

/**
 * Fill a data buffer from the given filename, returning it and filling @a
 * length with its length. Returns NULL on failure.
 */
static char *readInputData(const char *inputFN, unsigned int *length) {
    FILE *f = fopen(inputFN, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: unable to open file \"%s\": %s\n", inputFN,
                strerror(errno));
        return NULL;
    }

    /* We use fseek/ftell to get our data length, in order to keep this example
     * code as portable as possible. */
    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "ERROR: unable to seek file \"%s\": %s\n", inputFN,
                strerror(errno));
        fclose(f);
        return NULL;
    }
    long dataLen = ftell(f);
    if (dataLen < 0) {
        fprintf(stderr, "ERROR: ftell() failed: %s\n", strerror(errno));
        fclose(f);
        return NULL;
    }
    if (fseek(f, 0, SEEK_SET) != 0) {
        fprintf(stderr, "ERROR: unable to seek file \"%s\": %s\n", inputFN,
                strerror(errno));
        fclose(f);
        return NULL;
    }

    /* Hyperscan's hs_scan function accepts length as an unsigned int, so we
     * limit the size of our buffer appropriately. */
    if ((unsigned long)dataLen > UINT_MAX) {
        dataLen = UINT_MAX;
        printf("WARNING: clipping data to %ld bytes\n", dataLen);
    } else if (dataLen == 0) {
        fprintf(stderr, "ERROR: input file \"%s\" is empty\n", inputFN);
        fclose(f);
        return NULL;
    }

    char *inputData = malloc(dataLen);
    if (!inputData) {
        fprintf(stderr, "ERROR: unable to malloc %ld bytes\n", dataLen);
        fclose(f);
        return NULL;
    }

    char *p = inputData;
    size_t bytesLeft = dataLen;
    while (bytesLeft) {
        size_t bytesRead = fread(p, 1, bytesLeft, f);
        bytesLeft -= bytesRead;
        p += bytesRead;
        if (ferror(f) != 0) {
            fprintf(stderr, "ERROR: fread() failed\n");
            free(inputData);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);

    *length = (unsigned int)dataLen;
    return inputData;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return -1;
    }

    char *inputFN = argv[1];

    hs_database_t *db = NULL;
    hs_compile_error_t *compile_err = NULL;
    const char *expr[] = {
        "abc",
        "def",
        "foobar.*gh",
        "teakettle{4,10}",  /* 104 */
        "ijkl[mMn]",        /* 105 */
        "104 & !105",    /* 1001 */
        "(101 & 102 & 103) | (104 & !105)",
        "!101 & 102",
        "!(!101 | 102)",
        "101 & !102", /* 1005 */
        "101 & 105" }; /* 1006 */
    
    unsigned flags[] = {
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,
        HS_FLAG_SINGLEMATCH,      /* 104 */
        HS_FLAG_SINGLEMATCH,      /* 105 */
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH, /* 1001 */
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH, /* 1002 */
        HS_FLAG_COMBINATION,            /* 1003 */
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH, /* 1004 */
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH, /* 1005 */
        HS_FLAG_COMBINATION | HS_FLAG_SINGLEMATCH  /* 1006 */
    };

    unsigned ids[] = {101, 102, 103, 104, 105, 1001, 1002, 1003, 1004, 1005, 1006};
    hs_error_t err = hs_compile_multi(expr, flags, ids, 9, HS_MODE_NOSTREAM,
            NULL, &db, &compile_err);

    /* Next, we read the input data file into a buffer. */
    unsigned int length;
    char *inputData = readInputData(inputFN, &length);
    if (!inputData) {
        hs_free_database(db);
        return -1;
    }

    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(db, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        free(inputData);
        hs_free_database(db);
        return -1;
    }

    printf("Scanning %u bytes with Hyperscan\n", length);

    if (hs_scan(db, inputData, length, 0, scratch, on_match, expr) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        free(inputData);
        hs_free_database(db);
        return -1;
    }

    /* Scanning is complete, any matches have been handled, so now we just
     * clean up and exit.
     */
    hs_free_scratch(scratch);
    free(inputData);
    hs_free_database(db);
    return 0;
}
