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

#ifndef NGX_HTTP_FANCYINDEX_MODULE_H
#define NGX_HTTP_FANCYINDEX_MODULE_H

typedef enum {
    NGX_ERR_OK = 0,
    NGX_ERR_NOENT = 1,
    NGX_ERR_INVAL = 2
} ngx_http_fancyindex_error_t;

typedef enum {
    NGX_HTTP_FANCYINDEX_SORT_CRITERION_NAME = 0,
    NGX_HTTP_FANCYINDEX_SORT_CRITERION_SIZE = 1,
    NGX_HTTP_FANCYINDEX_SORT_CRITERION_DATE = 2,
    NGX_HTTP_FANCYINDEX_SORT_CRITERION_NAME_DESC = 3,
    NGX_HTTP_FANCYINDEX_SORT_CRITERION_SIZE_DESC = 4,
    NGX_HTTP_FANCYINDEX_SORT_CRITERION_DATE_DESC = 5
} ngx_http_fancyindex_sort_t;

#endif /* NGX_HTTP_FANCYINDEX_MODULE_H */