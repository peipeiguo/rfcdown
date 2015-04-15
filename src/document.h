/* document.h - generic markdown parser */

#ifndef RFCDOWN_DOCUMENT_H
#define RFCDOWN_DOCUMENT_H

#include "buffer.h"
#include "autolink.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

typedef enum rfcdown_extensions {
	/* block-level extensions */
	RFCDOWN_EXT_TABLES = (1 << 0),
	RFCDOWN_EXT_FENCED_CODE = (1 << 1),
	RFCDOWN_EXT_FOOTNOTES = (1 << 2),

	/* span-level extensions */
	RFCDOWN_EXT_AUTOLINK = (1 << 3),
	RFCDOWN_EXT_STRIKETHROUGH = (1 << 4),
	RFCDOWN_EXT_UNDERLINE = (1 << 5),
	RFCDOWN_EXT_HIGHLIGHT = (1 << 6),
	RFCDOWN_EXT_QUOTE = (1 << 7),
	RFCDOWN_EXT_SUPERSCRIPT = (1 << 8),
	RFCDOWN_EXT_MATH = (1 << 9),

	/* other flags */
	RFCDOWN_EXT_NO_INTRA_EMPHASIS = (1 << 11),
	RFCDOWN_EXT_SPACE_HEADERS = (1 << 12),
	RFCDOWN_EXT_MATH_EXPLICIT = (1 << 13),

	/* negative flags */
	RFCDOWN_EXT_DISABLE_INDENTED_CODE = (1 << 14)
} rfcdown_extensions;

#define RFCDOWN_EXT_BLOCK (\
	RFCDOWN_EXT_TABLES |\
	RFCDOWN_EXT_FENCED_CODE |\
	RFCDOWN_EXT_FOOTNOTES )

#define RFCDOWN_EXT_SPAN (\
	RFCDOWN_EXT_AUTOLINK |\
	RFCDOWN_EXT_STRIKETHROUGH |\
	RFCDOWN_EXT_UNDERLINE |\
	RFCDOWN_EXT_HIGHLIGHT |\
	RFCDOWN_EXT_QUOTE |\
	RFCDOWN_EXT_SUPERSCRIPT |\
	RFCDOWN_EXT_MATH )

#define RFCDOWN_EXT_FLAGS (\
	RFCDOWN_EXT_NO_INTRA_EMPHASIS |\
	RFCDOWN_EXT_SPACE_HEADERS |\
	RFCDOWN_EXT_MATH_EXPLICIT )

#define RFCDOWN_EXT_NEGATIVE (\
	RFCDOWN_EXT_DISABLE_INDENTED_CODE )

typedef enum rfcdown_list_flags {
	RFCDOWN_LIST_ORDERED = (1 << 0),
	RFCDOWN_LI_BLOCK = (1 << 1)	/* <li> containing block data */
} rfcdown_list_flags;

typedef enum rfcdown_table_flags {
	RFCDOWN_TABLE_ALIGN_LEFT = 1,
	RFCDOWN_TABLE_ALIGN_RIGHT = 2,
	RFCDOWN_TABLE_ALIGN_CENTER = 3,
	RFCDOWN_TABLE_ALIGNMASK = 3,
	RFCDOWN_TABLE_HEADER = 4
} rfcdown_table_flags;

typedef enum rfcdown_autolink_type {
	RFCDOWN_AUTOLINK_NONE,		/* used internally when it is not an autolink*/
	RFCDOWN_AUTOLINK_NORMAL,	/* normal http/http/ftp/mailto/etc link */
	RFCDOWN_AUTOLINK_EMAIL		/* e-mail link without explit mailto: */
} rfcdown_autolink_type;


/*********
 * TYPES *
 *********/

struct rfcdown_document;
typedef struct rfcdown_document rfcdown_document;

struct rfcdown_renderer_data {
	void *opaque;
};
typedef struct rfcdown_renderer_data rfcdown_renderer_data;

/* rfcdown_renderer - functions for rendering parsed data */
struct rfcdown_renderer {
	/* state object */
	void *opaque;

	/* block level callbacks - NULL skips the block */
	void (*blockcode)(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_buffer *lang, const rfcdown_renderer_data *data);
	void (*blockquote)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*header)(rfcdown_buffer *ob, const rfcdown_buffer *content, int level, const rfcdown_renderer_data *data);
	void (*hrule)(rfcdown_buffer *ob, const rfcdown_renderer_data *data);
	void (*list)(rfcdown_buffer *ob, const rfcdown_buffer *content, rfcdown_list_flags flags, const rfcdown_renderer_data *data);
	void (*listitem)(rfcdown_buffer *ob, const rfcdown_buffer *content, rfcdown_list_flags flags, const rfcdown_renderer_data *data);
	void (*paragraph)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*table)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*table_header)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*table_body)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*table_row)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*table_cell)(rfcdown_buffer *ob, const rfcdown_buffer *content, rfcdown_table_flags flags, const rfcdown_renderer_data *data);
	void (*footnotes)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	void (*footnote_def)(rfcdown_buffer *ob, const rfcdown_buffer *content, unsigned int num, const rfcdown_renderer_data *data);
	void (*blockhtml)(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data);

	/* span level callbacks - NULL or return 0 prints the span verbatim */
	int (*autolink)(rfcdown_buffer *ob, const rfcdown_buffer *link, rfcdown_autolink_type type, const rfcdown_renderer_data *data);
	int (*codespan)(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data);
	int (*double_emphasis)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*emphasis)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*underline)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*highlight)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*quote)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*image)(rfcdown_buffer *ob, const rfcdown_buffer *link, const rfcdown_buffer *title, const rfcdown_buffer *alt, const rfcdown_renderer_data *data);
	int (*linebreak)(rfcdown_buffer *ob, const rfcdown_renderer_data *data);
	int (*link)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_buffer *link, const rfcdown_buffer *title, const rfcdown_renderer_data *data);
	int (*triple_emphasis)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*strikethrough)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*superscript)(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data);
	int (*footnote_ref)(rfcdown_buffer *ob, unsigned int num, const rfcdown_renderer_data *data);
	int (*math)(rfcdown_buffer *ob, const rfcdown_buffer *text, int displaymode, const rfcdown_renderer_data *data);
	int (*raw_html)(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data);

	/* low level callbacks - NULL copies input directly into the output */
	void (*entity)(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data);
	void (*normal_text)(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data);

	/* miscellaneous callbacks */
	void (*doc_header)(rfcdown_buffer *ob, int inline_render, const rfcdown_renderer_data *data);
	void (*doc_footer)(rfcdown_buffer *ob, int inline_render, const rfcdown_renderer_data *data);
};
typedef struct rfcdown_renderer rfcdown_renderer;


/*************
 * FUNCTIONS *
 *************/

/* rfcdown_document_new: allocate a new document processor instance */
rfcdown_document *rfcdown_document_new(
	const rfcdown_renderer *renderer,
	rfcdown_extensions extensions,
	size_t max_nesting
) __attribute__ ((malloc));

/* rfcdown_document_render: render regular Markdown using the document processor */
void rfcdown_document_render(rfcdown_document *doc, rfcdown_buffer *ob, const uint8_t *data, size_t size);

/* rfcdown_document_render_inline: render inline Markdown using the document processor */
void rfcdown_document_render_inline(rfcdown_document *doc, rfcdown_buffer *ob, const uint8_t *data, size_t size);

/* rfcdown_document_free: deallocate a document processor instance */
void rfcdown_document_free(rfcdown_document *doc);


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_DOCUMENT_H **/
