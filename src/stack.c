#include "stack.h"

#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void
rfcdown_stack_init(rfcdown_stack *st, size_t initial_size)
{
	assert(st);

	st->item = NULL;
	st->size = st->asize = 0;

	if (!initial_size)
		initial_size = 8;

	rfcdown_stack_grow(st, initial_size);
}

void
rfcdown_stack_uninit(rfcdown_stack *st)
{
	assert(st);

	free(st->item);
}

void
rfcdown_stack_grow(rfcdown_stack *st, size_t neosz)
{
	assert(st);

	if (st->asize >= neosz)
		return;

	st->item = rfcdown_realloc(st->item, neosz * sizeof(void *));
	memset(st->item + st->asize, 0x0, (neosz - st->asize) * sizeof(void *));

	st->asize = neosz;

	if (st->size > neosz)
		st->size = neosz;
}

void
rfcdown_stack_push(rfcdown_stack *st, void *item)
{
	assert(st);

	if (st->size >= st->asize)
		rfcdown_stack_grow(st, st->size * 2);

	st->item[st->size++] = item;
}

void *
rfcdown_stack_pop(rfcdown_stack *st)
{
	assert(st);

	if (!st->size)
		return NULL;

	return st->item[--st->size];
}

void *
rfcdown_stack_top(const rfcdown_stack *st)
{
	assert(st);

	if (!st->size)
		return NULL;

	return st->item[st->size - 1];
}
