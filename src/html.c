#include "html.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "escape.h"

#define USE_XHTML(opt) (opt->flags & RFCDOWN_HTML_USE_XHTML)

rfcdown_html_tag
rfcdown_html_is_tag(const uint8_t *data, size_t size, const char *tagname)
{
	size_t i;
	int closed = 0;

	if (size < 3 || data[0] != '<')
		return RFCDOWN_HTML_TAG_NONE;

	i = 1;

	if (data[i] == '/') {
		closed = 1;
		i++;
	}

	for (; i < size; ++i, ++tagname) {
		if (*tagname == 0)
			break;

		if (data[i] != *tagname)
			return RFCDOWN_HTML_TAG_NONE;
	}

	if (i == size)
		return RFCDOWN_HTML_TAG_NONE;

	if (isspace(data[i]) || data[i] == '>')
		return closed ? RFCDOWN_HTML_TAG_CLOSE : RFCDOWN_HTML_TAG_OPEN;

	return RFCDOWN_HTML_TAG_NONE;
}

static void escape_html(rfcdown_buffer *ob, const uint8_t *source, size_t length)
{
	rfcdown_escape_html(ob, source, length, 0);
}

static void escape_href(rfcdown_buffer *ob, const uint8_t *source, size_t length)
{
	rfcdown_escape_href(ob, source, length);
}

/********************
 * GENERIC RENDERER *
 ********************/
static int
rndr_autolink(rfcdown_buffer *ob, const rfcdown_buffer *link, rfcdown_autolink_type type, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;

	if (!link || !link->size)
		return 0;

	RFCDOWN_BUFPUTSL(ob, "<a href=\"");
	if (type == RFCDOWN_AUTOLINK_EMAIL)
		RFCDOWN_BUFPUTSL(ob, "mailto:");
	escape_href(ob, link->data, link->size);

	if (state->link_attributes) {
		rfcdown_buffer_putc(ob, '\"');
		state->link_attributes(ob, link, data);
		rfcdown_buffer_putc(ob, '>');
	} else {
		RFCDOWN_BUFPUTSL(ob, "\">");
	}

	/*
	 * Pretty printing: if we get an email address as
	 * an actual URI, e.g. `mailto:foo@bar.com`, we don't
	 * want to print the `mailto:` prefix
	 */
	if (rfcdown_buffer_prefix(link, "mailto:") == 0) {
		escape_html(ob, link->data + 7, link->size - 7);
	} else {
		escape_html(ob, link->data, link->size);
	}

	RFCDOWN_BUFPUTSL(ob, "</a>");

	return 1;
}

static void
rndr_blockcode(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_buffer *lang, const rfcdown_renderer_data *data)
{
	if (ob->size) rfcdown_buffer_putc(ob, '\n');

	if (lang) {
		RFCDOWN_BUFPUTSL(ob, "<pre><code class=\"language-");
		escape_html(ob, lang->data, lang->size);
		RFCDOWN_BUFPUTSL(ob, "\">");
	} else {
		RFCDOWN_BUFPUTSL(ob, "<pre><code>");
	}

	if (text)
		escape_html(ob, text->data, text->size);

	RFCDOWN_BUFPUTSL(ob, "</code></pre>\n");
}

static void
rndr_blockquote(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (ob->size) rfcdown_buffer_putc(ob, '\n');
	RFCDOWN_BUFPUTSL(ob, "<blockquote>\n");
	if (content) rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</blockquote>\n");
}

static int
rndr_codespan(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data)
{
	RFCDOWN_BUFPUTSL(ob, "<code>");
	if (text) escape_html(ob, text->data, text->size);
	RFCDOWN_BUFPUTSL(ob, "</code>");
	return 1;
}

static int
rndr_strikethrough(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size)
		return 0;

	RFCDOWN_BUFPUTSL(ob, "<del>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</del>");
	return 1;
}

static int
rndr_double_emphasis(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size)
		return 0;

	RFCDOWN_BUFPUTSL(ob, "<strong>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</strong>");

	return 1;
}

static int
rndr_emphasis(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size) return 0;
	RFCDOWN_BUFPUTSL(ob, "<em>");
	if (content) rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</em>");
	return 1;
}

static int
rndr_underline(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size)
		return 0;

	RFCDOWN_BUFPUTSL(ob, "<u>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</u>");

	return 1;
}

static int
rndr_highlight(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size)
		return 0;

	RFCDOWN_BUFPUTSL(ob, "<mark>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</mark>");

	return 1;
}

static int
rndr_quote(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size)
		return 0;

	RFCDOWN_BUFPUTSL(ob, "<q>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</q>");

	return 1;
}

static int
rndr_linebreak(rfcdown_buffer *ob, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;
	rfcdown_buffer_puts(ob, USE_XHTML(state) ? "<br/>\n" : "<br>\n");
	return 1;
}

static void
rndr_header(rfcdown_buffer *ob, const rfcdown_buffer *content, int level, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;

	if (ob->size)
		rfcdown_buffer_putc(ob, '\n');

	if (level <= state->toc_data.nesting_level)
		rfcdown_buffer_printf(ob, "<h%d id=\"toc_%d\">", level, state->toc_data.header_count++);
	else
		rfcdown_buffer_printf(ob, "<h%d>", level);

	if (content) rfcdown_buffer_put(ob, content->data, content->size);
	rfcdown_buffer_printf(ob, "</h%d>\n", level);
}

static int
rndr_link(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_buffer *link, const rfcdown_buffer *title, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;

	RFCDOWN_BUFPUTSL(ob, "<a href=\"");

	if (link && link->size)
		escape_href(ob, link->data, link->size);

	if (title && title->size) {
		RFCDOWN_BUFPUTSL(ob, "\" title=\"");
		escape_html(ob, title->data, title->size);
	}

	if (state->link_attributes) {
		rfcdown_buffer_putc(ob, '\"');
		state->link_attributes(ob, link, data);
		rfcdown_buffer_putc(ob, '>');
	} else {
		RFCDOWN_BUFPUTSL(ob, "\">");
	}

	if (content && content->size) rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</a>");
	return 1;
}

static void
rndr_list(rfcdown_buffer *ob, const rfcdown_buffer *content, rfcdown_list_flags flags, const rfcdown_renderer_data *data)
{
	if (ob->size) rfcdown_buffer_putc(ob, '\n');
	rfcdown_buffer_put(ob, (const uint8_t *)(flags & RFCDOWN_LIST_ORDERED ? "<ol>\n" : "<ul>\n"), 5);
	if (content) rfcdown_buffer_put(ob, content->data, content->size);
	rfcdown_buffer_put(ob, (const uint8_t *)(flags & RFCDOWN_LIST_ORDERED ? "</ol>\n" : "</ul>\n"), 6);
}

static void
rndr_listitem(rfcdown_buffer *ob, const rfcdown_buffer *content, rfcdown_list_flags flags, const rfcdown_renderer_data *data)
{
	RFCDOWN_BUFPUTSL(ob, "<li>");
	if (content) {
		size_t size = content->size;
		while (size && content->data[size - 1] == '\n')
			size--;

		rfcdown_buffer_put(ob, content->data, size);
	}
	RFCDOWN_BUFPUTSL(ob, "</li>\n");
}

static void
rndr_paragraph(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;
	size_t i = 0;

	if (ob->size) rfcdown_buffer_putc(ob, '\n');

	if (!content || !content->size)
		return;

	while (i < content->size && isspace(content->data[i])) i++;

	if (i == content->size)
		return;

	RFCDOWN_BUFPUTSL(ob, "<p>");
	if (state->flags & RFCDOWN_HTML_HARD_WRAP) {
		size_t org;
		while (i < content->size) {
			org = i;
			while (i < content->size && content->data[i] != '\n')
				i++;

			if (i > org)
				rfcdown_buffer_put(ob, content->data + org, i - org);

			/*
			 * do not insert a line break if this newline
			 * is the last character on the paragraph
			 */
			if (i >= content->size - 1)
				break;

			rndr_linebreak(ob, data);
			i++;
		}
	} else {
		rfcdown_buffer_put(ob, content->data + i, content->size - i);
	}
	RFCDOWN_BUFPUTSL(ob, "</p>\n");
}

static void
rndr_raw_block(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data)
{
	size_t org, sz;

	if (!text)
		return;

	/* FIXME: Do we *really* need to trim the HTML? How does that make a difference? */
	sz = text->size;
	while (sz > 0 && text->data[sz - 1] == '\n')
		sz--;

	org = 0;
	while (org < sz && text->data[org] == '\n')
		org++;

	if (org >= sz)
		return;

	if (ob->size)
		rfcdown_buffer_putc(ob, '\n');

	rfcdown_buffer_put(ob, text->data + org, sz - org);
	rfcdown_buffer_putc(ob, '\n');
}

static int
rndr_triple_emphasis(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size) return 0;
	RFCDOWN_BUFPUTSL(ob, "<strong><em>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</em></strong>");
	return 1;
}

static void
rndr_hrule(rfcdown_buffer *ob, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;
	if (ob->size) rfcdown_buffer_putc(ob, '\n');
	rfcdown_buffer_puts(ob, USE_XHTML(state) ? "<hr/>\n" : "<hr>\n");
}

static int
rndr_image(rfcdown_buffer *ob, const rfcdown_buffer *link, const rfcdown_buffer *title, const rfcdown_buffer *alt, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;
	if (!link || !link->size) return 0;

	RFCDOWN_BUFPUTSL(ob, "<img src=\"");
	escape_href(ob, link->data, link->size);
	RFCDOWN_BUFPUTSL(ob, "\" alt=\"");

	if (alt && alt->size)
		escape_html(ob, alt->data, alt->size);

	if (title && title->size) {
		RFCDOWN_BUFPUTSL(ob, "\" title=\"");
		escape_html(ob, title->data, title->size); }

	rfcdown_buffer_puts(ob, USE_XHTML(state) ? "\"/>" : "\">");
	return 1;
}

static int
rndr_raw_html(rfcdown_buffer *ob, const rfcdown_buffer *text, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;

	/* ESCAPE overrides SKIP_HTML. It doesn't look to see if
	 * there are any valid tags, just escapes all of them. */
	if((state->flags & RFCDOWN_HTML_ESCAPE) != 0) {
		escape_html(ob, text->data, text->size);
		return 1;
	}

	if ((state->flags & RFCDOWN_HTML_SKIP_HTML) != 0)
		return 1;

	rfcdown_buffer_put(ob, text->data, text->size);
	return 1;
}

static void
rndr_table(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
    if (ob->size) rfcdown_buffer_putc(ob, '\n');
    RFCDOWN_BUFPUTSL(ob, "<table>\n");
    rfcdown_buffer_put(ob, content->data, content->size);
    RFCDOWN_BUFPUTSL(ob, "</table>\n");
}

static void
rndr_table_header(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
    if (ob->size) rfcdown_buffer_putc(ob, '\n');
    RFCDOWN_BUFPUTSL(ob, "<thead>\n");
    rfcdown_buffer_put(ob, content->data, content->size);
    RFCDOWN_BUFPUTSL(ob, "</thead>\n");
}

static void
rndr_table_body(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
    if (ob->size) rfcdown_buffer_putc(ob, '\n');
    RFCDOWN_BUFPUTSL(ob, "<tbody>\n");
    rfcdown_buffer_put(ob, content->data, content->size);
    RFCDOWN_BUFPUTSL(ob, "</tbody>\n");
}

static void
rndr_tablerow(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	RFCDOWN_BUFPUTSL(ob, "<tr>\n");
	if (content) rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</tr>\n");
}

static void
rndr_tablecell(rfcdown_buffer *ob, const rfcdown_buffer *content, rfcdown_table_flags flags, const rfcdown_renderer_data *data)
{
	if (flags & RFCDOWN_TABLE_HEADER) {
		RFCDOWN_BUFPUTSL(ob, "<th");
	} else {
		RFCDOWN_BUFPUTSL(ob, "<td");
	}

	switch (flags & RFCDOWN_TABLE_ALIGNMASK) {
	case RFCDOWN_TABLE_ALIGN_CENTER:
		RFCDOWN_BUFPUTSL(ob, " style=\"text-align: center\">");
		break;

	case RFCDOWN_TABLE_ALIGN_LEFT:
		RFCDOWN_BUFPUTSL(ob, " style=\"text-align: left\">");
		break;

	case RFCDOWN_TABLE_ALIGN_RIGHT:
		RFCDOWN_BUFPUTSL(ob, " style=\"text-align: right\">");
		break;

	default:
		RFCDOWN_BUFPUTSL(ob, ">");
	}

	if (content)
		rfcdown_buffer_put(ob, content->data, content->size);

	if (flags & RFCDOWN_TABLE_HEADER) {
		RFCDOWN_BUFPUTSL(ob, "</th>\n");
	} else {
		RFCDOWN_BUFPUTSL(ob, "</td>\n");
	}
}

static int
rndr_superscript(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (!content || !content->size) return 0;
	RFCDOWN_BUFPUTSL(ob, "<sup>");
	rfcdown_buffer_put(ob, content->data, content->size);
	RFCDOWN_BUFPUTSL(ob, "</sup>");
	return 1;
}

static void
rndr_normal_text(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	if (content)
		escape_html(ob, content->data, content->size);
}

static void
rndr_footnotes(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;

	if (ob->size) rfcdown_buffer_putc(ob, '\n');
	RFCDOWN_BUFPUTSL(ob, "<div class=\"footnotes\">\n");
	rfcdown_buffer_puts(ob, USE_XHTML(state) ? "<hr/>\n" : "<hr>\n");
	RFCDOWN_BUFPUTSL(ob, "<ol>\n");

	if (content) rfcdown_buffer_put(ob, content->data, content->size);

	RFCDOWN_BUFPUTSL(ob, "\n</ol>\n</div>\n");
}

static void
rndr_footnote_def(rfcdown_buffer *ob, const rfcdown_buffer *content, unsigned int num, const rfcdown_renderer_data *data)
{
	size_t i = 0;
	int pfound = 0;

	/* insert anchor at the end of first paragraph block */
	if (content) {
		while ((i+3) < content->size) {
			if (content->data[i++] != '<') continue;
			if (content->data[i++] != '/') continue;
			if (content->data[i++] != 'p' && content->data[i] != 'P') continue;
			if (content->data[i] != '>') continue;
			i -= 3;
			pfound = 1;
			break;
		}
	}

	rfcdown_buffer_printf(ob, "\n<li id=\"fn%d\">\n", num);
	if (pfound) {
		rfcdown_buffer_put(ob, content->data, i);
		rfcdown_buffer_printf(ob, "&nbsp;<a href=\"#fnref%d\" rev=\"footnote\">&#8617;</a>", num);
		rfcdown_buffer_put(ob, content->data + i, content->size - i);
	} else if (content) {
		rfcdown_buffer_put(ob, content->data, content->size);
	}
	RFCDOWN_BUFPUTSL(ob, "</li>\n");
}

static int
rndr_footnote_ref(rfcdown_buffer *ob, unsigned int num, const rfcdown_renderer_data *data)
{
	rfcdown_buffer_printf(ob, "<sup id=\"fnref%d\"><a href=\"#fn%d\" rel=\"footnote\">%d</a></sup>", num, num, num);
	return 1;
}

static int
rndr_math(rfcdown_buffer *ob, const rfcdown_buffer *text, int displaymode, const rfcdown_renderer_data *data)
{
	rfcdown_buffer_put(ob, (const uint8_t *)(displaymode ? "\\[" : "\\("), 2);
	escape_html(ob, text->data, text->size);
	rfcdown_buffer_put(ob, (const uint8_t *)(displaymode ? "\\]" : "\\)"), 2);
	return 1;
}

static void
toc_header(rfcdown_buffer *ob, const rfcdown_buffer *content, int level, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state = data->opaque;

	if (level <= state->toc_data.nesting_level) {
		/* set the level offset if this is the first header
		 * we're parsing for the document */
		if (state->toc_data.current_level == 0)
			state->toc_data.level_offset = level - 1;

		level -= state->toc_data.level_offset;

		if (level > state->toc_data.current_level) {
			while (level > state->toc_data.current_level) {
				RFCDOWN_BUFPUTSL(ob, "<ul>\n<li>\n");
				state->toc_data.current_level++;
			}
		} else if (level < state->toc_data.current_level) {
			RFCDOWN_BUFPUTSL(ob, "</li>\n");
			while (level < state->toc_data.current_level) {
				RFCDOWN_BUFPUTSL(ob, "</ul>\n</li>\n");
				state->toc_data.current_level--;
			}
			RFCDOWN_BUFPUTSL(ob,"<li>\n");
		} else {
			RFCDOWN_BUFPUTSL(ob,"</li>\n<li>\n");
		}

		rfcdown_buffer_printf(ob, "<a href=\"#toc_%d\">", state->toc_data.header_count++);
		if (content) rfcdown_buffer_put(ob, content->data, content->size);
		RFCDOWN_BUFPUTSL(ob, "</a>\n");
	}
}

static int
toc_link(rfcdown_buffer *ob, const rfcdown_buffer *content, const rfcdown_buffer *link, const rfcdown_buffer *title, const rfcdown_renderer_data *data)
{
	if (content && content->size) rfcdown_buffer_put(ob, content->data, content->size);
	return 1;
}

static void
toc_finalize(rfcdown_buffer *ob, int inline_render, const rfcdown_renderer_data *data)
{
	rfcdown_html_renderer_state *state;

	if (inline_render)
		return;

	state = data->opaque;

	while (state->toc_data.current_level > 0) {
		RFCDOWN_BUFPUTSL(ob, "</li>\n</ul>\n");
		state->toc_data.current_level--;
	}

	state->toc_data.header_count = 0;
}

rfcdown_renderer *
rfcdown_html_toc_renderer_new(int nesting_level)
{
	static const rfcdown_renderer cb_default = {
		NULL,

		NULL,
		NULL,
		toc_header,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,

		NULL,
		rndr_codespan,
		rndr_double_emphasis,
		rndr_emphasis,
		rndr_underline,
		rndr_highlight,
		rndr_quote,
		NULL,
		NULL,
		toc_link,
		rndr_triple_emphasis,
		rndr_strikethrough,
		rndr_superscript,
		NULL,
		NULL,
		NULL,

		NULL,
		rndr_normal_text,

		NULL,
		toc_finalize
	};

	rfcdown_html_renderer_state *state;
	rfcdown_renderer *renderer;

	/* Prepare the state pointer */
	state = rfcdown_malloc(sizeof(rfcdown_html_renderer_state));
	memset(state, 0x0, sizeof(rfcdown_html_renderer_state));

	state->toc_data.nesting_level = nesting_level;

	/* Prepare the renderer */
	renderer = rfcdown_malloc(sizeof(rfcdown_renderer));
	memcpy(renderer, &cb_default, sizeof(rfcdown_renderer));

	renderer->opaque = state;
	return renderer;
}

rfcdown_renderer *
rfcdown_html_renderer_new(rfcdown_html_flags render_flags, int nesting_level)
{
	static const rfcdown_renderer cb_default = {
		NULL,

		rndr_blockcode,
		rndr_blockquote,
		rndr_header,
		rndr_hrule,
		rndr_list,
		rndr_listitem,
		rndr_paragraph,
		rndr_table,
		rndr_table_header,
		rndr_table_body,
		rndr_tablerow,
		rndr_tablecell,
		rndr_footnotes,
		rndr_footnote_def,
		rndr_raw_block,

		rndr_autolink,
		rndr_codespan,
		rndr_double_emphasis,
		rndr_emphasis,
		rndr_underline,
		rndr_highlight,
		rndr_quote,
		rndr_image,
		rndr_linebreak,
		rndr_link,
		rndr_triple_emphasis,
		rndr_strikethrough,
		rndr_superscript,
		rndr_footnote_ref,
		rndr_math,
		rndr_raw_html,

		NULL,
		rndr_normal_text,

		NULL,
		NULL
	};

	rfcdown_html_renderer_state *state;
	rfcdown_renderer *renderer;

	/* Prepare the state pointer */
	state = rfcdown_malloc(sizeof(rfcdown_html_renderer_state));
	memset(state, 0x0, sizeof(rfcdown_html_renderer_state));

	state->flags = render_flags;
	state->toc_data.nesting_level = nesting_level;

	/* Prepare the renderer */
	renderer = rfcdown_malloc(sizeof(rfcdown_renderer));
	memcpy(renderer, &cb_default, sizeof(rfcdown_renderer));

	if (render_flags & RFCDOWN_HTML_SKIP_HTML || render_flags & RFCDOWN_HTML_ESCAPE)
		renderer->blockhtml = NULL;

	renderer->opaque = state;
	return renderer;
}

void
rfcdown_html_renderer_free(rfcdown_renderer *renderer)
{
	free(renderer->opaque);
	free(renderer);
}
