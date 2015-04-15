/* html.h - HTML renderer and utilities */

#ifndef RFCDOWN_HTML_H
#define RFCDOWN_HTML_H

#include "document.h"
#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

typedef enum rfcdown_html_flags {
	RFCDOWN_HTML_SKIP_HTML = (1 << 0),
	RFCDOWN_HTML_ESCAPE = (1 << 1),
	RFCDOWN_HTML_HARD_WRAP = (1 << 2),
	RFCDOWN_HTML_USE_XHTML = (1 << 3)
} rfcdown_html_flags;

typedef enum rfcdown_html_tag {
	RFCDOWN_HTML_TAG_NONE = 0,
	RFCDOWN_HTML_TAG_OPEN,
	RFCDOWN_HTML_TAG_CLOSE
} rfcdown_html_tag;


/*********
 * TYPES *
 *********/

struct rfcdown_html_renderer_state {
	void *opaque;

	struct {
		int header_count;
		int current_level;
		int level_offset;
		int nesting_level;
	} toc_data;

	rfcdown_html_flags flags;

	/* extra callbacks */
	void (*link_attributes)(rfcdown_buffer *ob, const rfcdown_buffer *url, const rfcdown_renderer_data *data);
};
typedef struct rfcdown_html_renderer_state rfcdown_html_renderer_state;


/*************
 * FUNCTIONS *
 *************/

/* rfcdown_html_smartypants: process an HTML snippet using SmartyPants for smart punctuation */
void rfcdown_html_smartypants(rfcdown_buffer *ob, const uint8_t *data, size_t size);

/* rfcdown_html_is_tag: checks if data starts with a specific tag, returns the tag type or NONE */
rfcdown_html_tag rfcdown_html_is_tag(const uint8_t *data, size_t size, const char *tagname);


/* rfcdown_html_renderer_new: allocates a regular HTML renderer */
rfcdown_renderer *rfcdown_html_renderer_new(
	rfcdown_html_flags render_flags,
	int nesting_level
) __attribute__ ((malloc));

/* rfcdown_html_toc_renderer_new: like rfcdown_html_renderer_new, but the returned renderer produces the Table of Contents */
rfcdown_renderer *rfcdown_html_toc_renderer_new(
	int nesting_level
) __attribute__ ((malloc));

/* rfcdown_html_renderer_free: deallocate an HTML renderer */
void rfcdown_html_renderer_free(rfcdown_renderer *renderer);


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_HTML_H **/
