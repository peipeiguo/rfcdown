/* stack.h - simple stacking */

#ifndef RFCDOWN_STACK_H
#define RFCDOWN_STACK_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/*********
 * TYPES *
 *********/

struct rfcdown_stack {
	void **item;
	size_t size;
	size_t asize;
};
typedef struct rfcdown_stack rfcdown_stack;


/*************
 * FUNCTIONS *
 *************/

/* rfcdown_stack_init: initialize a stack */
void rfcdown_stack_init(rfcdown_stack *st, size_t initial_size);

/* rfcdown_stack_uninit: free internal data of the stack */
void rfcdown_stack_uninit(rfcdown_stack *st);

/* rfcdown_stack_grow: increase the allocated size to the given value */
void rfcdown_stack_grow(rfcdown_stack *st, size_t neosz);

/* rfcdown_stack_push: push an item to the top of the stack */
void rfcdown_stack_push(rfcdown_stack *st, void *item);

/* rfcdown_stack_pop: retrieve and remove the item at the top of the stack */
void *rfcdown_stack_pop(rfcdown_stack *st);

/* rfcdown_stack_top: retrieve the item at the top of the stack */
void *rfcdown_stack_top(const rfcdown_stack *st);


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_STACK_H **/
