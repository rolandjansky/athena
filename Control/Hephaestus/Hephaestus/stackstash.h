/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* $Id$ */
/**
 * @file  Hephaestus/src/stackstash.h
 * @author scott snyder
 * @date Feb 2015, derived from earlier memprof work
 * @brief Memory-efficient storage of stack traces.
 *
 * This header allows for storing a set of track traces in a memory-efficient
 * form.  Two strategies are used.  First, we store the traces as a tree,
 * so that common parts of the traces are only stored once.  Second,
 * where possible, we group together multiple addresses in a single node.
 *
 * An element of a stack trace is called a @c StackElement; this is an
 * address on the stack.
 *
 * A given trace is represeted by a unique StackHandle.
 *
 * A StackNode is where the data are actually stored.  The layout of a
 * StackNode looks like this:
 *
 *
 *    Parent
 *    Nelts
 *    Nkids
 *    Handles
 *    0
 *    elt1
 *    ...
 *    eltn
 *    kid1
 *    ...
 *    kidn
 *
 * The start of a node is a pointer back to its parent.  This is null
 * if this is the root node of the tree.  This is followed by the number
 * of elements stored in this tree and its number of children.
 * After this is a pointer to the start of a linked list of StackHandle's
 * associated with this node.
 * This is followed by a word fixed to 0 (see below).  Following this
 * are the elements stored in this node; after that are pointers to the
 * node's children.  The child pointers are sorted in the order of the
 * first element in the child.  This results in a trie-like data structure.
 *
 * A node contains a list of all the StackHandle objects that reference it;
 * these are kept in a linked list.  A handle contains a pointer to the
 * element at the top of its trace.
 *
 * To extract a trace given a handle, we move backwards to the the previous
 * word in the node.  If this is non-zero, then this is the previous element.
 * Otherwise, we go to the parent node (which we can find since the `0' word
 * is at a fixed offset in the node) and continue with the last element
 * in that node.
 *
 * For more general movement through the tree, use the @c StackCursor object.
 */

#ifndef HEPHAESTUS_STACKSTASH_H
#define HEPHAESTUS_STACKSTASH_H


#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/** One element of a stack trace. */
typedef void* StackElement;


/** 
 * @brief Unique reference for a stack trace.
 * See below for possible operations on a handle.
 */
struct StackHandle_
{
  StackElement* element;
  struct StackHandle_* next;
};
typedef struct StackHandle_        StackHandle;


/**
 * @brief Node used to store trace data.
 * See above for the structure.
 */
struct StackNode_
{
  struct StackNode_* parent;
  unsigned short nelts;
  unsigned short nkids;
  StackHandle* handles;
  /* First elt is always 0. */
  void* p[1];
};
typedef struct StackNode_ StackNode;


/**
  * @brief Cursor object used to move between nodes of the tree.
  *        See operations below.
  */
struct StackCursor_
{
  StackNode* node;
  StackElement* elt;
};
typedef struct StackCursor_ StackCursor;


/**
 * @Brief Return the handle for a stack trace.
 * @param addresses Stack trace to store.
 * @param n_addresses Length of the trace.
 *
 * Returns the unique Handle object representing this trace.
 * If this trace has been seen before, the existing Handle object will
 * be returned; otherwise, a new Handle object will be made and returned.
 */
StackHandle* hhh_stackstash_store (const StackElement  *addresses,
                                   int                  n_addresses);


/**
 * @brief Function interface to iterate over a trace.
 * @param elt Pointer to an element of a trace.
 *
 * Returns a pointer to the previous element of the trace (the caller).
 * Returns 0 if we're at the root of the trace.
 */
StackElement* hhh_stack_element_parent (StackElement* elt);


/** Initialize a cursor to point at the root of the tree. */
void hhh_Cursor_initRoot(StackCursor* cursor);

/** Initialize a cursor from a handle */
void hhh_Cursor_initHandle(StackCursor* cursor, StackHandle* handle);

/** Move to the cursor to the parent node. */
void hhh_Cursor_parent(StackCursor* cursor);

/** Move to the cursor to the nth child.. */
void hhh_Cursor_child(StackCursor* cursor, int n);

/** Return the number of children. */
int hhh_Cursor_nchildren(StackCursor* cursor);

/** Visit next element in depth-first order. */
void hhh_Cursor_next(StackCursor* cursor);


/***********************************************************************
 * For internal use.
 */

/** Is the element pointed to by ELEMENT the first one in its node? */
#define STACK_ELEMENT_BEGIN_P(element) (*((element)-1) == 0)

/** ELEMENT should be a pointer to the first element in a node.
    Returns a pointer to the node itself. */
#define STACK_ELEMENT_NODE(element) \
  ((StackNode*)((char*)((element)-1) - offsetof(StackNode, p)))

/** ELEMENT should be a pointer to the first element in a node.
    Returns a pointer to the parent of this node. */
#define STACK_ELEMENT_NODEPARENT(element) (STACK_ELEMENT_NODE(element)->parent)

/** Return the Nth element stored in a node. */
#define STACK_NODE_NTH_ELEMENT(node, n)  ((node)->p[n+1])

/** Return a pointer to the Nth child stored in a node. */
#define STACK_NODE_NTH_CHILD(node, n) \
  (*(StackNode**)&((node)->p[n+1+(node)->nelts]))

/** We can't store a null element in a trace; fold them to 1. */
#define STACK_ELEMENT_NONNULL(elt) ((elt) ? (elt) : (StackElement)1)

/** Set the Nth element stored in a node. */
#define STACK_NODE_SET_ELEMENT(node, n, elt)  do{ (node)->p[n+1] = STACK_ELEMENT_NONNULL(elt); } while(0)



/***********************************************************************
 * Public interface.
 */


/*** Operations on StackHandle. */

/** Return a pointer to the first element in the handle's trace. */
#define STACK_HANDLE_ELEMENT(handle) ((handle)->element)

/** Return the first element in the handle's trace. */
#define STACK_HANDLE_ADDR(handle) \
  STACK_ELEMENT_ADDR(STACK_HANDLE_ELEMENT(handle))

/** Return a pointer to the second in the handle's trace. */
#define STACK_HANDLE_PARENT(handle) \
  STACK_ELEMENT_PARENT(STACK_HANDLE_ELEMENT(handle))

/** Test to see if a handle represents the root of the call tree. */
#define STACK_HANDLE_IS_ROOT(handle) \
  STACK_ELEMENT_IS_ROOT(STACK_HANDLE_ELEMENT(handle))



/*** Operations on StackElement. */

/** Given a pointer to an element, return the element. */
#define STACK_ELEMENT_ADDR(element) (*(element))

/** Given a pointer to an element, return a pointer to the element's parent.
    Gives 0 if element referred to the root. */
#define STACK_ELEMENT_PARENT(element) hhh_stack_element_parent(element)

/** Test if an element is the root of trace. */
#define STACK_ELEMENT_IS_ROOT(element)       (*(element) == 0)


/*** Operations on StackCursor. */


/** Return the address of the element referenced by the cursor. */
#define STACK_CURSOR_ELEMENT(cursor) ((cursor).elt)

/** Return the element referenced by the cursor. */
#define STACK_CURSOR_ELEMENT_ADDR(cursor) STACK_ELEMENT_ADDR(STACK_CURSOR_ELEMENT(cursor))

/** Initialize a cursor to point at the root of the tree. */
#define STACK_CURSOR_INIT_ROOT(cursor) hhh_Cursor_initRoot(&cursor)

/** Initialize a cursor from a handle */
#define STACK_CURSOR_INIT_HANDLE(cursor, handle) hhh_Cursor_initHandle(&cursor, handle)

/** Does the cursor reference the root of the tree? */
#define STACK_CURSOR_IS_ROOT(cursor) STACK_ELEMENT_IS_ROOT(STACK_CURSOR_ELEMENT(cursor))

/** Move to the cursor to the parent node. */
#define STACK_CURSOR_PARENT(cursor) hhh_Cursor_parent(&cursor)

/** Return the number of children. */
#define STACK_CURSOR_NCHILDREN(cursor) hhh_Cursor_nchildren(&cursor)

/** Move to the cursor to the nth child. */
#define STACK_CURSOR_CHILD(cursor, n) hhh_Cursor_child(&cursor, n)

/** Visit next element in depth-first order. */
#define STACK_CURSOR_NEXT(cursor) hhh_Cursor_next(&cursor)


#ifdef __cplusplus
} // extern "C"
#endif


#endif // not HEPHAESTUS_STACKSTASH_H
