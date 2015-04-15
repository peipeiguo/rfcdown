/* autolink.h - versatile autolinker */

#ifndef RFCDOWN_AUTOLINK_H
#define RFCDOWN_AUTOLINK_H

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

typedef enum rfcdown_autolink_flags {
	RFCDOWN_AUTOLINK_SHORT_DOMAINS = (1 << 0)
} rfcdown_autolink_flags;


/*************
 * FUNCTIONS *
 *************/

/* rfcdown_autolink_is_safe: verify that a URL has a safe protocol */
int rfcdown_autolink_is_safe(const uint8_t *data, size_t size);

/* rfcdown_autolink__www: search for the next www link in data */
size_t rfcdown_autolink__www(size_t *rewind_p, rfcdown_buffer *link,
	uint8_t *data, size_t offset, size_t size, rfcdown_autolink_flags flags);

/* rfcdown_autolink__email: search for the next email in data */
size_t rfcdown_autolink__email(size_t *rewind_p, rfcdown_buffer *link,
	uint8_t *data, size_t offset, size_t size, rfcdown_autolink_flags flags);

/* rfcdown_autolink__url: search for the next URL in data */
size_t rfcdown_autolink__url(size_t *rewind_p, rfcdown_buffer *link,
	uint8_t *data, size_t offset, size_t size, rfcdown_autolink_flags flags);


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_AUTOLINK_H **/
