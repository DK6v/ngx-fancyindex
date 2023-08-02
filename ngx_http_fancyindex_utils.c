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

#include <string.h>

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_log.h>

#include "ngx_http_fancyindex_module.h"
#include "ngx_http_fancyindex_utils.h"

#define LOG_TAG "FI"

#define NGX_FANCY_INDEX_FORMAT_STRING_MAX_LEN   128
#define NGX_FANCY_INDEX_FORMAT_STRING_MAX_NUM   16

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define strnchr(_str, _chr, _num) memchr(_str, _chr, strnlen(_str, _num))

static uint stringBufferIx = 0;
static char stringBuffers[NGX_FANCY_INDEX_FORMAT_STRING_MAX_NUM]
                         [NGX_FANCY_INDEX_FORMAT_STRING_MAX_LEN];

static char* ngx_http_fancyindex_format_string(char* format, ...) {

    char* retVal = NULL;

    stringBufferIx = (stringBufferIx + 1) % NGX_FANCY_INDEX_FORMAT_STRING_MAX_NUM;
    retVal = &stringBuffers[stringBufferIx][0];

    if (format != NULL) {

        va_list args;
        va_start(args, format);

        (void)vsnprintf(retVal, NGX_FANCY_INDEX_FORMAT_STRING_MAX_LEN, format, args);

        va_end(args);
    }
    else {
        memset(retVal, 0, NGX_FANCY_INDEX_FORMAT_STRING_MAX_LEN);
    }

    return retVal;
}

ngx_http_fancyindex_sort_t ngx_http_fancyindex_get_sort_criteria(ngx_http_request_t *r,
                                                                 ngx_http_fancyindex_sort_t defaultSortCriteria) {

    ngx_http_fancyindex_sort_t retSortCriteria = defaultSortCriteria;

    char sortBy[2] = {0};
    char sortOrder[2] = {0};

    ngx_http_fancyindex_getarg_str(r, "C", &sortBy[0], sizeof(sortBy));
    ngx_http_fancyindex_getarg_str(r, "O", &sortOrder[0], sizeof(sortOrder));

    switch (*sortBy) {

        case 'M': /* Sort by time */
            retSortCriteria = (*sortOrder == 'D')
                            ? NGX_HTTP_FANCYINDEX_SORT_CRITERION_DATE_DESC
                            : NGX_HTTP_FANCYINDEX_SORT_CRITERION_DATE;
            break;

        case 'S': /* Sort by size */
            retSortCriteria = (*sortOrder == 'D')
                            ? NGX_HTTP_FANCYINDEX_SORT_CRITERION_SIZE_DESC
                            : NGX_HTTP_FANCYINDEX_SORT_CRITERION_SIZE;
            break;

        case 'N': /* Sort by name */
            retSortCriteria = (*sortOrder == 'D')
                            ? NGX_HTTP_FANCYINDEX_SORT_CRITERION_NAME_DESC
                            : NGX_HTTP_FANCYINDEX_SORT_CRITERION_NAME;
            break;
    }

    return retSortCriteria;
}

ngx_http_fancyindex_filter_t ngx_http_fancyindex_get_filter_criteria(ngx_http_request_t *r) {

    ngx_http_fancyindex_filter_t retFilterCriteria = {0};

    retFilterCriteria.dirsCountMax =
        ngx_http_fancyindex_getarg_num(r, "dirs", NGX_HTTP_FANCYINDEX_FILTER_UNDEF);

    retFilterCriteria.filesCountMax =
        ngx_http_fancyindex_getarg_num(r, "files", NGX_HTTP_FANCYINDEX_FILTER_UNDEF);

    retFilterCriteria.mTimeLessOrEqualMs =
        ngx_http_fancyindex_getarg_num(r, "le", NGX_HTTP_FANCYINDEX_FILTER_UNDEF);

    retFilterCriteria.mTimeGreaterOrEqualMs =
        ngx_http_fancyindex_getarg_num(r, "ge", NGX_HTTP_FANCYINDEX_FILTER_UNDEF);

    return retFilterCriteria;
}

ngx_err_t ngx_http_fancyindex_get_url_args(ngx_http_request_t *r,
                                           ngx_http_fancyindex_sort_t *sortCriteria,
                                           ngx_http_fancyindex_filter_t *filterCriteria,
                                           char *out, size_t maxSize) {
    char *npos = out;
    char *epos = out + maxSize;

    if (out == NULL) {
        return NGX_ERR_INVAL;
    }

    if (sortCriteria != NULL) {

        switch(*sortCriteria) {

            case NGX_HTTP_FANCYINDEX_SORT_CRITERION_NAME:
                npos += snprintf(npos, (epos - npos), "?C=N&amp;O=A");
                break;

            case NGX_HTTP_FANCYINDEX_SORT_CRITERION_SIZE:
                npos += snprintf(npos, (epos - npos), "?C=S&amp;O=A");
                break;

            case NGX_HTTP_FANCYINDEX_SORT_CRITERION_DATE:
                npos += snprintf(npos, (epos - npos), "?C=M&amp;O=A");
                break;

            case NGX_HTTP_FANCYINDEX_SORT_CRITERION_NAME_DESC:
                npos += snprintf(npos, (epos - npos), "?C=N&amp;O=D");
                break;

            case NGX_HTTP_FANCYINDEX_SORT_CRITERION_SIZE_DESC:
                npos += snprintf(npos, (epos - npos), "?C=S&amp;O=D");
                break;

            case NGX_HTTP_FANCYINDEX_SORT_CRITERION_DATE_DESC:
                npos += snprintf(npos, (epos - npos), "?C=M&amp;O=D");
                break;
        }
    }

    if (filterCriteria != NULL) {

        if (filterCriteria->dirsCountMax != NGX_HTTP_FANCYINDEX_FILTER_UNDEF) {
            npos += snprintf(npos, (epos - npos),
                             "%sdirs=%ld", (npos == out) ? "?" : "&amp;",
                             filterCriteria->dirsCountMax);
        }

        if (filterCriteria->filesCountMax != NGX_HTTP_FANCYINDEX_FILTER_UNDEF) {
            npos += snprintf(npos, (epos - npos),
                             "%sfiles=%ld", (npos == out) ? "?" : "&amp;",
                             filterCriteria->filesCountMax);
        }

        if (filterCriteria->mTimeLessOrEqualMs != NGX_HTTP_FANCYINDEX_FILTER_UNDEF) {
            npos += snprintf(npos, (epos - npos),
                             "%sle=%ld", (npos == out) ? "?" : "&amp;",
                             filterCriteria->mTimeLessOrEqualMs);
        }

        if (filterCriteria->mTimeGreaterOrEqualMs != NGX_HTTP_FANCYINDEX_FILTER_UNDEF) {
            npos += snprintf(npos, (epos - npos),
                             "%sge=%ld", (npos == out) ? "?" : "&amp;",
                             filterCriteria->mTimeGreaterOrEqualMs);
        }
    }

    return NGX_ERR_OK;
}

const char* ngx_http_fancyindex_get_href(ngx_http_request_t *r,
                                         ngx_http_fancyindex_sort_t sortCriteria,
                                         ngx_http_fancyindex_filter_t filterCriteria) {

    char *href = ngx_http_fancyindex_format_string(NULL);

    ngx_http_fancyindex_get_url_args(r, &sortCriteria, &filterCriteria,
                                     &href[0], NGX_FANCY_INDEX_FORMAT_STRING_MAX_LEN);

    return href;
}

ngx_err_t ngx_http_fancyindex_getarg_str(ngx_http_request_t *r,
                                         const char *name,
                                         char *out, size_t maxSize) {

    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                  LOG_TAG "::ngx_http_fancyindex_getarg_str() length: %d, args: %s",
                  r->args.len, (r->args.len == 0) ? "none" : (char*)r->args.data);

    if (r->args.len != 0) {

        /* The argument string is not null-terminated, and includes additional characters. */
        char *npos = (char *)&r->args.data[0];
        char *epos = (char *)&r->args.data[r->args.len];

        size_t nameLength = strlen(name);

        while ((size_t)(epos - npos) > nameLength) {

            ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,
                          LOG_TAG "::ngx_http_fancyindex_getarg_str() length: %d, arg: '%s'",
                          (epos - npos), npos);

            if ((strncmp(npos, name, nameLength) == 0) && (npos[nameLength] == '=')) {

                /* Calculate the start position of the value */
                char *vpos = &npos[nameLength + 1];

                /* Move the pointer to the beginning of the next argument */
                while ((npos != epos) && (*npos != '&')) {
                    ++npos;
                };

                snprintf(out, MIN((size_t)(npos - vpos) + 1, maxSize), "%s", vpos);

                ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                              LOG_TAG "::ngx_http_fancyindex_getarg_str() name: %s, value: %s",
                              name, out);

                return NGX_ERR_OK;
            }

            /* Go to the next argument after an ampersand */
            while ((npos != epos) && (*npos++ != '&')) {};

            /* ... or &amp; character reference */
            if ((epos - npos > 4) && (strncmp(npos, "amp;", 4) == 0)) {
                npos += 4;
            }
        }
    }

    ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,
                  LOG_TAG "::ngx_http_fancyindex_getarg_str(), name: %s, value: NOT FOUND",
                  name);

    return NGX_ERR_NOENT;
}

long ngx_http_fancyindex_getarg_num(ngx_http_request_t *r, const char *name, long defaultValue) {

    char buffer[16] = {0};
    long retVal = defaultValue;

    ngx_err_t rc = ngx_http_fancyindex_getarg_str(r, name, buffer, sizeof(buffer));

    if (NGX_ERR_OK == rc) {

        char *npos = buffer;
        char *epos = npos + strlen(buffer);

        while (npos != epos) {

            if (!isdigit(*npos) && !((npos == buffer) && (*npos == '-'))) {
                return retVal;
            }

            ++npos;
        }

        retVal = atol(buffer);
    }

    return retVal;
}
