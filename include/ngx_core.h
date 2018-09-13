#pragma once

#include <stdio.h>
#include <stdint.h>

#define NGX_LOG_ERR 0

#define ngx_log_error(level, log, err, fmt, ...) do {   \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__);           \
} while (0)

typedef uintptr_t       ngx_uint_t;
typedef intptr_t       ngx_int_t;
typedef unsigned char u_char;

typedef struct {
    size_t      len;
    u_char     *data;
} ngx_str_t;

#define ngx_palloc(pool, size) malloc(size)
#define ngx_memcpy memcpy

#define DEBUG   NGX_LOG_DEBUG
#define INFO    NGX_LOG_INFO
#define ERR     NGX_LOG_ERR
#define WARN    NGX_LOG_WARN

#define log(level, fmt, ...) do {                           \
    ngx_log_error(level, ngx_cycle->log, 0, "[%s:%d] " fmt, \
            __FUNCTION__, __LINE__, ##__VA_ARGS__);         \
} while (0)

