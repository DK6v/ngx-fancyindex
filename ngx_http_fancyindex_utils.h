/*
 * Copyright (c) 2023 Dmitry Korobkov
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#ifndef NGX_HTTP_FANCYINDEX_UTILS_H
#define NGX_HTTP_FANCYINDEX_UTILS_H

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "ngx_http_fancyindex_module.h"

#define NGX_HTTP_FANCYINDEX_FILTER_UNDEF    (-1L)

typedef struct {
    long dirsCountMax;
    long filesCountMax;
    long mTimeLessThanMs;
    long mTimeGreaterThanMs;
    long mTimeLessOrEqualMs;
    long mTimeGreaterOrEqualMs;

    // Search results
    long dirsCounter;
    long filesCounter;

} ngx_http_fancyindex_filter_t;


ngx_http_fancyindex_sort_t ngx_http_fancyindex_get_sort_criteria(ngx_http_request_t *r,
                                                                 ngx_http_fancyindex_sort_t defaultSortCriteria);

ngx_http_fancyindex_filter_t ngx_http_fancyindex_get_filter_criteria(ngx_http_request_t *r);

ngx_err_t ngx_http_fancyindex_get_url_args(ngx_http_request_t *r,
                                           ngx_http_fancyindex_sort_t *sortCriteria,
                                           ngx_http_fancyindex_filter_t *filterCriteria,
                                           char *out, size_t maxSize);

const char* ngx_http_fancyindex_get_href(ngx_http_request_t *r,
                                         ngx_http_fancyindex_sort_t sortCriteria,
                                         ngx_http_fancyindex_filter_t filterCriteria);

ngx_err_t ngx_http_fancyindex_getarg_str(ngx_http_request_t *r,
                                         const char *name,
                                         char *out, size_t maxSize);

long ngx_http_fancyindex_getarg_num(ngx_http_request_t *r, const char *name, long defaultValue);

#endif /* NGX_HTTP_FANCYINDEX_UTILS_H */