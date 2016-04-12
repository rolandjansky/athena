/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* $Id$ */
/**
 * @file  Hephaestus/src/stackstash.c
 * @author scott snyder
 * @date Feb 2015, derived from earlier memprof work
 * @brief Memory-efficient storage of stack traces.
 */


#include "Hephaestus/stackstash.h"
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <pthread.h>


/* Optionally collect statistics. */
/* #define STATS */


/* Statistics counters. */
#ifdef STATS
static int stash_n_nodes = 0;
static int stash_node_bytes = 0;
static int stash_node_sizes[10];
static int stash_n_handles = 0;
static int stash_tot_elts = 0;
static int stash_tot_kids = 0;
#endif


/** The root node of all traces. */
static StackNode* stash_root = 0;


#define MIN(a,b) ((a)<(b)?(a):(b))

static pthread_mutex_t gStashLock = PTHREAD_MUTEX_INITIALIZER;


/***************************************************************************
 * Minimal chunked memory allocator.
 * We don't return memory to the system from these.
 */


#define ALLOC_NOBJ 16384


struct AllocElt
{
  struct AllocElt* next;
};


struct Allocator
{
  size_t eltsize;
  struct AllocElt* free;
};


static void allocator_init (struct Allocator* a, size_t eltsize)
{
  a->eltsize = eltsize;
  a->free = NULL;
}


static void allocate_ool (struct Allocator* a)
{
  size_t eltsize = a->eltsize;
  size_t blocksize = eltsize * ALLOC_NOBJ;
  char* block = (char*)malloc (blocksize);
  char* bptr = block;
  int i;
  for (i=0; i < ALLOC_NOBJ; i++) {
    struct AllocElt* elt = (struct AllocElt*) bptr;
    if (i == ALLOC_NOBJ-1)
      elt->next = a->free;
    else
      elt->next = (struct AllocElt*)(bptr + eltsize);
    bptr += eltsize;
  }
  a->free = (struct AllocElt*)block;
}


static void* allocate (struct Allocator* a)
{
  if (!a->free)
    allocate_ool (a);

  void* ret = a->free;
  a->free = a->free->next;
  return ret;
}


static void deallocate (struct Allocator* a, void* p)
{
  struct AllocElt* elt = (struct AllocElt*)p;
  elt->next = a->free;
  a->free = elt;
}


/***************************************************************************/


static struct Allocator stash_handle_chunk = {0, NULL};
#define STASH_CHUNK_MAXTOT 10
static struct Allocator stash_node_chunks[STASH_CHUNK_MAXTOT];

static
StackNode* stack_stash_new_node (StackNode* parent,
                                 unsigned int nelts,
                                 unsigned int nkids)
{
  StackNode* node;
  int ntot = nelts + nkids;
  if (ntot < STASH_CHUNK_MAXTOT) {
    if (stash_node_chunks[ntot].eltsize == 0) {
      allocator_init (&stash_node_chunks[ntot],
                      sizeof (StackNode) + ntot * sizeof(void*));
    }
    node = (StackNode*)allocate (&stash_node_chunks[ntot]);
  }
  else
    node = (StackNode*)malloc(sizeof (StackNode) + (nelts+nkids)*sizeof(void*));
  assert (nelts < USHRT_MAX);
  assert (nkids < USHRT_MAX && nkids != 1);
  node->parent = parent;
  node->nelts = nelts;
  node->nkids = nkids;
  node->handles = 0;
  node->p[0] = 0;
#ifdef STATS
  ++stash_n_nodes;
  stash_tot_elts += nelts;
  stash_tot_kids += nkids;
  stash_node_bytes += sizeof (StackNode) + (nelts+nkids)*sizeof(void*);
  ++stash_node_sizes[MIN (ntot, 9)];
#endif
  return node;
}

static
void stack_stash_free_node (StackNode* node)
{
  int ntot = node->nelts + node->nkids;
#ifdef STATS
  --stash_n_nodes;
  stash_tot_elts -= node->nelts;
  stash_tot_kids -= node->nkids; 
  stash_node_bytes -= sizeof (StackNode) + (node->nelts+node->nkids)*sizeof(void*);
  --stash_node_sizes[MIN(ntot, 9)];
#endif
  if (ntot < STASH_CHUNK_MAXTOT)
    deallocate (&stash_node_chunks[ntot], node);
  else
    free (node);
}


/**
 * @brief Replace NODE in the tree with NEWNODE.
 * @param node The node to replace.
 * @param newnode The node with which to replace it.
 *
 * Updates all in the tree that refer to NODE to refer to NEWNODE instead.
 * Frees NODE.
 */
static
void stack_stash_replace_node (StackNode* node, StackNode* newnode)
{
  StackHandle* handle;
  StackNode* parent = node->parent;

  /* If the node has a parent, then update the parent's child pointer. */
  if (parent) {
    int i;
    for (i = 0; i < parent->nkids; i++) {
      if (STACK_NODE_NTH_CHILD (parent, i) == node) {
        STACK_NODE_NTH_CHILD (parent, i) = newnode;
        break;
      }
    }
  }
  else {
    /* Otherwise, the node is the root.  Update the root pointer. */
    stash_root = newnode;
  }


  /* Update the handles to point to the new node. */
  newnode->handles = node->handles;
  handle = newnode->handles;
  while (handle) {
    unsigned int index = handle->element - &STACK_NODE_NTH_ELEMENT(node, 0);
    assert (index < newnode->nelts);
    handle->element = &STACK_NODE_NTH_ELEMENT(newnode, index);
    handle = handle->next;
  }

  /* Free the old node. */
  stack_stash_free_node (node);
}


static
StackHandle* stack_stash_find_handle (StackNode* node, int index)
{
  StackElement* elt = &STACK_NODE_NTH_ELEMENT (node, index);
  StackHandle* handle = node->handles;
  while (handle) {
    if (handle->element == elt)
      return handle;
    handle = handle->next;
  }

  if (stash_handle_chunk.eltsize == 0) {
    allocator_init (&stash_handle_chunk, sizeof(StackHandle));
  }
  handle  = (StackHandle*) allocate (&stash_handle_chunk);
  handle->element = elt;
  handle->next = node->handles;
  node->handles = handle;

#ifdef STATS
  ++stash_n_handles;
#endif
  return handle;
}


/**
 * @brief Return the handle for a stack trace.
 * @param addresses Stack trace to store.
 * @param n_addresses Length of the trace.
 *
 * Returns the unique Handle object representing this trace.
 * If this trace has been seen before, the existing Handle object will
 * be returned; otherwise, a new Handle object will be made and returned.
 */
static
StackHandle* hhh_stackstash_store1 (const StackElement *addresses,
                                    int n_addresses)
{
  StackNode* current = stash_root;
  int curelt = 0;

  /* If we have no traces currently stored, allocate a root node
     and copy the trace there. */
  if (!current) {
    int i;
    stash_root = stack_stash_new_node (0, n_addresses, 0);
    for (i=0; i < n_addresses; i++)
      STACK_NODE_SET_ELEMENT (stash_root, i, addresses[n_addresses-1-i]);
    return stack_stash_find_handle (stash_root, n_addresses-1);
  }

  /* Search starting from the root.  Match elements of our trace
     against traces already stored. */

  while (n_addresses) {
    StackElement address = STACK_ELEMENT_NONNULL (addresses[n_addresses - 1]);
    if (curelt < current->nelts) {
      /* There are more elementes in the current node.
         Compare against the next one. */
      if (STACK_NODE_NTH_ELEMENT(current, curelt) != address)
        break;
      else
        ++curelt;
    }
    else if (current->nkids == 0) {
      /* No more elements and no children.  We're finished. */
      break;
    }
    else {
      /* See if the element we want is in one of the children.
         Kids are sorted, so use a binary search. */
      StackNode** first = &STACK_NODE_NTH_CHILD(current, 0);
      StackNode** last = &STACK_NODE_NTH_CHILD(current, current->nkids);
      int found = 0;
      while (first < last) {
        StackNode** mid = first + (last-first)/2;
        StackElement kidaddr = (*mid)->p[1];
        if (address == kidaddr) {
          found = 1;
          current = *mid;
          curelt = 1;
          break;
        } else if (address < kidaddr) {
          last = mid;
        } else {
          first = mid + 1;
        }
      }
      if (!found)
        break;
    }

    --n_addresses;
  }

  /* If we managed to match all of the input trace, then return the handle
     for the trace we found. */
  if (n_addresses == 0) {
    return stack_stash_find_handle (current, curelt-1);
  }

  if (curelt == current->nelts) {
    /* We matched up to the end of a node.
       If the node has no children, then extend the list of elements
       in this node.  Otherwise, add a new child to this node. */

    if (current->nkids == 0) {
      /* Extend current node with another address. */
      int i;
      StackNode* newnode = stack_stash_new_node (current->parent,
                                                 current->nelts + n_addresses,
                                                 0);
      memcpy (&STACK_NODE_NTH_ELEMENT(newnode, 0),
              &STACK_NODE_NTH_ELEMENT(current, 0),
              current->nelts * sizeof(StackElement));
      for (i=0; i < n_addresses; i++)
        STACK_NODE_SET_ELEMENT (newnode, current->nelts + i, 
                                addresses[n_addresses-1-i]);
      stack_stash_replace_node (current, newnode);
      return stack_stash_find_handle (newnode, newnode->nelts - 1);
    }
    else {
      /* Add to an existing list of children. */
      int i, n_children;
      StackElement topaddr;
      StackNode** old_child;
      StackNode** new_child;
      StackNode* newnode = stack_stash_new_node (current->parent,
                                                 current->nelts,
                                                 current->nkids + 1);
      memcpy (&STACK_NODE_NTH_ELEMENT(newnode, 0),
              &STACK_NODE_NTH_ELEMENT(current, 0),
              current->nelts * sizeof(StackElement));

      StackNode* newchild = stack_stash_new_node (newnode,
                                                  n_addresses,
                                                  0);
      for (i=0; i < n_addresses; i++)
        STACK_NODE_SET_ELEMENT (newchild, i, addresses[n_addresses-1-i]);

      old_child = &STACK_NODE_NTH_CHILD(current, 0);
      new_child = &STACK_NODE_NTH_CHILD(newnode, 0);
      n_children = current->nkids;
      topaddr = STACK_ELEMENT_NONNULL (addresses[n_addresses - 1]);

      while (n_children && STACK_NODE_NTH_ELEMENT(*old_child, 0) < topaddr) {
        (*old_child)->parent = newnode;
        *new_child++ = *old_child++;
        --n_children;
      }
      *new_child++ = newchild;
      while (n_children) {
        (*old_child)->parent = newnode;
        *new_child++ = *old_child++;
        --n_children;
      }

      stack_stash_replace_node (current, newnode);
      return stack_stash_find_handle (newchild, newchild->nelts - 1);
    }
  }

  /* Split a node. */
  assert (curelt > 0 || current->parent == 0);
  {
    int i;
    StackNode* child1;
    StackNode* child2;
    StackNode* newnode = stack_stash_new_node (current->parent,
                                               curelt,
                                               2);
    memcpy (&STACK_NODE_NTH_ELEMENT(newnode, 0),
            &STACK_NODE_NTH_ELEMENT(current, 0),
            curelt * sizeof(StackElement));

    child1 = stack_stash_new_node (newnode,
                                   current->nelts - curelt,
                                   current->nkids);
    memcpy (&STACK_NODE_NTH_ELEMENT(child1, 0),
            &STACK_NODE_NTH_ELEMENT(current, curelt),
            (current->nelts - curelt) * sizeof(StackElement));
    memcpy (&STACK_NODE_NTH_CHILD(child1, 0),
            &STACK_NODE_NTH_CHILD(current, 0),
            current->nkids * sizeof (StackNode*));
    for (i=0; i < current->nkids; i++)
      STACK_NODE_NTH_CHILD(child1, i)->parent = child1;

    child2 = stack_stash_new_node (newnode,
                                   n_addresses,
                                   0);
    for (i=0; i < n_addresses; i++)
      STACK_NODE_SET_ELEMENT (child2, i, addresses[n_addresses-1-i]);

    if (STACK_NODE_NTH_ELEMENT (child1, 0) <
        STACK_NODE_NTH_ELEMENT (child2, 0))
    {
      STACK_NODE_NTH_CHILD(newnode, 0) = child1;
      STACK_NODE_NTH_CHILD(newnode, 1) = child2;
    }
    else {
      STACK_NODE_NTH_CHILD(newnode, 0) = child2;
      STACK_NODE_NTH_CHILD(newnode, 1) = child1;
    }

    {
      StackHandle* handle = current->handles;
      StackHandle* prev = 0;
      while (handle) {
        unsigned int index =
          handle->element - &STACK_NODE_NTH_ELEMENT(current, 0);
        assert (index < current->nelts);
        if (index >= curelt) {
          /* Remove node from current's list; add to child1's list. */
          StackHandle* next = handle->next;
          if (prev)
            prev->next = handle->next;
          else
            current->handles = handle->next;
          handle->next = child1->handles;
          child1->handles = handle;
          
          handle->element = &STACK_NODE_NTH_ELEMENT (child1, index-curelt);
          handle = next;
        }
        else {
          prev = handle;
          handle = handle->next;
        }
      }
    }

    stack_stash_replace_node (current, newnode);
    return stack_stash_find_handle (child2, child2->nelts - 1);
  }
}


/**
 * @brief Return the handle for a stack trace.
 * @param addresses Stack trace to store.
 * @param n_addresses Length of the trace.
 *
 * Returns the unique Handle object representing this trace.
 * If this trace has been seen before, the existing Handle object will
 * be returned; otherwise, a new Handle object will be made and returned.
 */
StackHandle* hhh_stackstash_store (const StackElement *addresses,
                                   int n_addresses)
{
  pthread_mutex_lock( &gStashLock );
  StackHandle* ret = hhh_stackstash_store1 (addresses, n_addresses);
  pthread_mutex_unlock (&gStashLock);
  return ret;
}


/**
 * @brief Function interface to iterate over a trace.
 * @param elt Pointer to an element of a trace.
 *
 * Returns a pointer to the previous element of the trace (the caller).
 * Returns 0 if we're at the root of the trace.
 */
StackElement* hhh_stack_element_parent (StackElement* elt)
{
  if (STACK_ELEMENT_BEGIN_P(elt)) {
    StackNode* parent = STACK_ELEMENT_NODEPARENT(elt);
    if (parent != 0)
      return &STACK_NODE_NTH_ELEMENT (parent, parent->nelts-1);
  }
  return elt-1;
}



/** Initialize a cursor to point at the root of the tree. */
void hhh_Cursor_initRoot(StackCursor* cursor)
{
  cursor->node = stash_root;
  if (stash_root)
    cursor->elt = &STACK_NODE_NTH_ELEMENT(stash_root, -1);
  else
    cursor->elt = 0;
}


/** Initialize a cursor from a handle */
void hhh_Cursor_initHandle(StackCursor* cursor, StackHandle* handle)
{
  StackElement* elt = STACK_HANDLE_ELEMENT(handle);
  cursor->elt = elt;
  while (!STACK_ELEMENT_BEGIN_P(elt))
    --elt;
  cursor->node = STACK_ELEMENT_NODE(elt);
}


/** Move to the cursor to the parent node. */
void hhh_Cursor_parent(StackCursor* cursor)
{
  if (STACK_ELEMENT_BEGIN_P(cursor->elt) && cursor->node->parent) {
    cursor->node = cursor->node->parent;
    cursor->elt = &STACK_NODE_NTH_ELEMENT(cursor->node, cursor->node->nelts-1);
  }
  else
    --cursor->elt;
}


/** Move to the cursor to the nth child.. */
void hhh_Cursor_child(StackCursor* cursor, int n)
{
  if (cursor->elt == &STACK_NODE_NTH_ELEMENT(cursor->node, cursor->node->nelts-1)) {
    cursor->node = STACK_NODE_NTH_CHILD(cursor->node, n);
    cursor->elt = &STACK_NODE_NTH_ELEMENT(cursor->node, 0);
  }
  else {
    assert (n == 0);
    ++cursor->elt;
  }
}


/** Return the number of children. */
int hhh_Cursor_nchildren(StackCursor* cursor)
{
  if (cursor->elt == &STACK_NODE_NTH_ELEMENT(cursor->node, cursor->node->nelts-1))
    return cursor->node->nkids;
  else
    return 1;
}


/** Visit next element in depth-first order. */
void hhh_Cursor_next(StackCursor* cursor)
{
  if (cursor->node->nelts > 0 && cursor->elt != &STACK_NODE_NTH_ELEMENT(cursor->node, cursor->node->nelts-1)) {
    ++cursor->elt;
    return;
  }

  if (cursor->node->nkids > 0) {
    cursor->node = STACK_NODE_NTH_CHILD(cursor->node, 0);
    cursor->elt = &STACK_NODE_NTH_ELEMENT(cursor->node, 0);
    return;
  }

  StackNode* node = cursor->node;
  while (1) {
    StackNode* parent = node->parent;
    if (!parent) {
      STACK_CURSOR_INIT_ROOT (*cursor);
      return;
    }

    {
      int i;
      for (i=0; i < parent->nkids-1; i++) {
        if (STACK_NODE_NTH_CHILD(parent, i) == node) {
          cursor->node = STACK_NODE_NTH_CHILD(parent, i+1);
          cursor->elt = &STACK_NODE_NTH_ELEMENT(cursor->node, 0);
          return;
        }
      }
    }

    node = parent;
  }
}
