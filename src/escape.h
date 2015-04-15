/* escape.h - escape utilities */

#ifndef RFCDOWN_ESCAPE_H
#define RFCDOWN_ESCAPE_H

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * FUNCTIONS *
 *************/

/* rfcdown_escape_href: escape (part of) a URL inside HTML */
void rfcdown_escape_href(rfcdown_buffer *ob, const uint8_t *data, size_t size);

/* rfcdown_escape_html: escape HTML */
void rfcdown_escape_html(rfcdown_buffer *ob, const uint8_t *data, size_t size, int secure);


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_ESCAPE_H **/
