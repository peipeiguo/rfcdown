/* buffer.h - simple, fast buffers */

#ifndef RFCDOWN_BUFFER_H
#define RFCDOWN_BUFFER_H

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define __attribute__(x)
#define inline __inline
#define __builtin_expect(x,n) x
#endif


/*********
 * TYPES *
 *********/

typedef void *(*rfcdown_realloc_callback)(void *, size_t);
typedef void (*rfcdown_free_callback)(void *);

struct rfcdown_buffer {
	uint8_t *data;	/* actual character data */
	size_t size;	/* size of the string */
	size_t asize;	/* allocated size (0 = volatile buffer) */
	size_t unit;	/* reallocation unit size (0 = read-only buffer) */

	rfcdown_realloc_callback data_realloc;
	rfcdown_free_callback data_free;
	rfcdown_free_callback buffer_free;
};

typedef struct rfcdown_buffer rfcdown_buffer;


/*************
 * FUNCTIONS *
 *************/

/* allocation wrappers */
void *rfcdown_malloc(size_t size) __attribute__ ((malloc));
void *rfcdown_calloc(size_t nmemb, size_t size) __attribute__ ((malloc));
void *rfcdown_realloc(void *ptr, size_t size) __attribute__ ((malloc));

/* rfcdown_buffer_init: initialize a buffer with custom allocators */
void rfcdown_buffer_init(
	rfcdown_buffer *buffer,
	size_t unit,
	rfcdown_realloc_callback data_realloc,
	rfcdown_free_callback data_free,
	rfcdown_free_callback buffer_free
);

/* rfcdown_buffer_uninit: uninitialize an existing buffer */
void rfcdown_buffer_uninit(rfcdown_buffer *buf);

/* rfcdown_buffer_new: allocate a new buffer */
rfcdown_buffer *rfcdown_buffer_new(size_t unit) __attribute__ ((malloc));

/* rfcdown_buffer_reset: free internal data of the buffer */
void rfcdown_buffer_reset(rfcdown_buffer *buf);

/* rfcdown_buffer_grow: increase the allocated size to the given value */
void rfcdown_buffer_grow(rfcdown_buffer *buf, size_t neosz);

/* rfcdown_buffer_put: append raw data to a buffer */
void rfcdown_buffer_put(rfcdown_buffer *buf, const uint8_t *data, size_t size);

/* rfcdown_buffer_puts: append a NUL-terminated string to a buffer */
void rfcdown_buffer_puts(rfcdown_buffer *buf, const char *str);

/* rfcdown_buffer_putc: append a single char to a buffer */
void rfcdown_buffer_putc(rfcdown_buffer *buf, uint8_t c);

/* rfcdown_buffer_putf: read from a file and append to a buffer, until EOF or error */
int rfcdown_buffer_putf(rfcdown_buffer *buf, FILE* file);

/* rfcdown_buffer_set: replace the buffer's contents with raw data */
void rfcdown_buffer_set(rfcdown_buffer *buf, const uint8_t *data, size_t size);

/* rfcdown_buffer_sets: replace the buffer's contents with a NUL-terminated string */
void rfcdown_buffer_sets(rfcdown_buffer *buf, const char *str);

/* rfcdown_buffer_eq: compare a buffer's data with other data for equality */
int rfcdown_buffer_eq(const rfcdown_buffer *buf, const uint8_t *data, size_t size);

/* rfcdown_buffer_eq: compare a buffer's data with NUL-terminated string for equality */
int rfcdown_buffer_eqs(const rfcdown_buffer *buf, const char *str);

/* rfcdown_buffer_prefix: compare the beginning of a buffer with a string */
int rfcdown_buffer_prefix(const rfcdown_buffer *buf, const char *prefix);

/* rfcdown_buffer_slurp: remove a given number of bytes from the head of the buffer */
void rfcdown_buffer_slurp(rfcdown_buffer *buf, size_t size);

/* rfcdown_buffer_cstr: NUL-termination of the string array (making a C-string) */
const char *rfcdown_buffer_cstr(rfcdown_buffer *buf);

/* rfcdown_buffer_printf: formatted printing to a buffer */
void rfcdown_buffer_printf(rfcdown_buffer *buf, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

/* rfcdown_buffer_put_utf8: put a Unicode character encoded as UTF-8 */
void rfcdown_buffer_put_utf8(rfcdown_buffer *buf, unsigned int codepoint);

/* rfcdown_buffer_free: free the buffer */
void rfcdown_buffer_free(rfcdown_buffer *buf);


/* RFCDOWN_BUFPUTSL: optimized rfcdown_buffer_puts of a string literal */
#define RFCDOWN_BUFPUTSL(output, literal) \
	rfcdown_buffer_put(output, (const uint8_t *)literal, sizeof(literal) - 1)

/* RFCDOWN_BUFSETSL: optimized rfcdown_buffer_sets of a string literal */
#define RFCDOWN_BUFSETSL(output, literal) \
	rfcdown_buffer_set(output, (const uint8_t *)literal, sizeof(literal) - 1)

/* RFCDOWN_BUFEQSL: optimized rfcdown_buffer_eqs of a string literal */
#define RFCDOWN_BUFEQSL(output, literal) \
	rfcdown_buffer_eq(output, (const uint8_t *)literal, sizeof(literal) - 1)


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_BUFFER_H **/
