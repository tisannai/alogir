#ifndef AG_HEAP_H
#define AG_HEAP_H

/**
 * @file   ag_heap.h
 * @author Tero Isannainen <tero.isannainen@gmail.com>
 * @date   Sun Apr 22 16:12:17 2018
 *
 * @brief  Heap algorithms over containers.
 *
 *
 * Heap supports heaps in both ascending (min at root) and decending
 * order (max at root). Ascending and decending ordering is defined by
 * the "polarity" property. When polarity is 1, we have ascending
 * heap. When polarity is -1, the order is decending. Heap is based on
 * Gromer and thus it grows if needed.
 *
 * Binary Heap is a tree of items where items are locally ordered, but
 * (potentially) globally unordered, i.e. partial ordering. In
 * min-at-root Heap the parent is smaller than the children.
 *
 * Example of min-at-root:
 *
 *
 *               13          Layer 1
 *              /  \
 *             /    \
 *            /      \
 *          14        16     Layer 2
 *         /  \      /  \
 *        /    \    /    \
 *       19    21  19    68  Layer 3
 *      / \   /  \
 *     65 26 32  31          Layer 4
 *
 *
 * Items are in array folded from top to bottom, left to right:
 *
 *             | L1   | L2    | L3          | L4
 *             +------+-------+-------------+------------
 *        Idx: | 0 1  | 2  3  | 4  5  6  7  | 8  9  10 11
 *       Item: | - 13 | 14 16 | 19 21 19 68 | 65 26 32 31
 *
 * Parent of item at index N is at N/2. Left child of item at index N
 * is at 2*N, and right child of item at index N is at 2*N+1. In order
 * for these relations to be valid, root is at index 1. However we
 * don't want array to start from 1, so all indeces are decremented by
 * one before used to actually access the array.
 *
 * There are two basic operation to heap: insert and delete. Insert is
 * aghp_put() and delete is aghp_get() in this library.
 *
 * Insert means dropping existing items downwards (by travelling
 * towards root) until heap rule is fulfilled for the added item,
 * i.e. inserted item is bigger that its parent.
 *
 * Delete means removing root (the returned data item) and then
 * pushing items upwards (by travelling away from root) from the child
 * that is smaller. The resulting "hole" in the tree is filled with
 * the last entry in the heap. This means there will not be any
 * "holes" in the array. If items are moved from the right side of the
 * tree, the hole will be immediately "fixed", and "incomplete"
 * branches only exist on left, if any.
 *
 * Heap can be sorted with aghp_sort() function. Unordered Gromer must
 * be heapified before running sorting for it. aghp_ify_for_sort() is
 * used for heapifying for sorting. Heapify is performed in inverted
 * polarity for sorting. This is not directly visible to the user,
 * however.
 *
 * Heapify can be used in general for priority queue operations. The
 * plain aghp_ify() is for this.
 *
 */


#include <gromer.h>

/**
 * Heap struct.
 */
struct aghp_s
{
    gr_t            gr;    /**< Gromer. */
    gr_compare_fn_p cmp;   /**< Compare function. */
    gr_size_t       cnt;   /**< Heap item count. */
    gr_pos_t        polar; /**< Polarity of heap (sm=1,gr=-1). */
};

/** Short type for Heap struct. */
typedef struct aghp_s aghp_s;

/** Handle type for Heap. */
typedef struct aghp_s* aghp_t;



/**
 * Create Heap handle from Gromer.
 *
 * Compare function is normalized to compare a to b, and to return 1
 * if a is bigger than b.
 *
 * Polarity of "1" means ascending order in Heap, and "-1" means
 * decending.
 *
 * Compare function's normalized polarity is not affected by "dir".
 *
 * @param gr  Gromer.
 * @param cmp Data compare function.
 * @param dir Polarity (1=ascending).
 *
 * @return Heap.
 */
aghp_t aghp_new( gr_t gr, gr_compare_fn_p cmp, gr_pos_t dir );


/**
 * Initialize Heap handle using Gromer.
 *
 * See aghp_new() for details about parameters.
 *
 * @param h   Heap.
 * @param gr  Gromer.
 * @param cmp Data compare function.
 * @param dir Polarity.
 */
void aghp_init( aghp_t h, gr_t gr, gr_compare_fn_p cmp, gr_pos_t dir );


/**
 * Delete Heap.
 *
 * @param h Heap.
 *
 * @return NULL
 */
aghp_t aghp_del( aghp_t h );


/**
 * Put item to Heap.
 *
 * @param h    Heap.
 * @param item Item.
 */
void aghp_put( aghp_t h, gr_d item );


/**
 * Get item from Heap.
 *
 * Item is either smallest (if polarity is 1) or biggest (if polarity
 * is -1).
 *
 * @param h Heap.
 *
 * @return Item (smallest/biggest).
 */
gr_d aghp_get( aghp_t h );


/**
 * Heapify Heap.
 *
 * The assigned Gromer items are arranged into binary heap.
 *
 * @param h Heap.
 */
void aghp_ify( aghp_t h );


/**
 * Heapify Heap for sorting.
 *
 * Sorting requires inverted polarity. This is internally arranged and
 * after this operation aghp_sort() can be called.
 *
 * @param h Heap.
 */
void aghp_ify_for_sort( aghp_t h );


/**
 * Sort Heap.
 *
 * Heap must have been heapified with abhp_ify_for_sort().
 *
 * @param h
 */
void aghp_sort( aghp_t h );


/**
 * Sort Gromer data.
 *
 * @param gr  Gromer.
 * @param cmp Data compare function.
 * @param dir Sort polarity (1 = ascending, -1 = decending).
 */
void aghp_sort_gromer( gr_t gr, gr_compare_fn_p cmp, gr_pos_t dir );


/**
 * Return 1 if empty.
 *
 * @param h Heap.
 *
 * @return 1 for empty (else 0).
 */
int aghp_is_empty( aghp_t h );


/**
 * Set Heap polarity.
 *
 * Polarity: 1 = ascending, -1 = decending.
 *
 * @param h
 * @param polar
 */
void aghp_set_polar( aghp_t h, gr_pos_t polar );


/**
 * Invert Heap polarity.
 *
 * @param h Heap.
 */
void aghp_inv_polar( aghp_t h );


/**
 * Return Heap polarity.
 *
 * @param h Heap.
 *
 * @return Polarity.
 */
gr_pos_t aghp_get_polar( aghp_t h );



#endif
