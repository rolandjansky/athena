/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* $Id$ */
/**
 * @file  Hephaestus/test/stackstash_test.cxx
 * @author scott snyder
 * @date Apr 2015, derived from earlier memprof work
 * @brief Regression test for stackstash.
 */


#undef NDEBUG
#include "Hephaestus/stackstash.h"
#include <vector>
#include <cassert>
#include <stdlib.h>
#include <stdio.h>


#define MIN(a,b) ((a)<(b)?(a):(b))


//*********************************************************************


typedef std::vector<unsigned long> tvec;
StackHandle* add_trace (const tvec& t)
{
  return hhh_stackstash_store (reinterpret_cast<const StackElement*>(t.data()),
                               t.size());
}


void test_cursor()
{
  add_trace ({4, 3, 2, 1});
  add_trace ({5, 3, 2, 1});
  StackHandle* h3 = add_trace ({6, 1});

  StackCursor cur;
  STACK_CURSOR_INIT_ROOT (cur);
  assert (STACK_CURSOR_IS_ROOT (cur));

  assert (STACK_CURSOR_NCHILDREN (cur) == 1);
  STACK_CURSOR_CHILD (cur, 0);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)1);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_CHILD (cur, 0);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)2);

  assert (STACK_CURSOR_NCHILDREN (cur) == 1);
  STACK_CURSOR_CHILD (cur, 0);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)3);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_CHILD (cur, 0);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)4);

  assert (STACK_CURSOR_NCHILDREN (cur) == 0);
  STACK_CURSOR_PARENT (cur);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)3);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_CHILD (cur, 1);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)5);

  assert (STACK_CURSOR_NCHILDREN (cur) == 0);
  STACK_CURSOR_PARENT (cur);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)3);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_PARENT (cur);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)2);

  assert (STACK_CURSOR_NCHILDREN (cur) == 1);
  STACK_CURSOR_PARENT (cur);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)1);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_CHILD (cur, 1);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)6);

  assert (STACK_CURSOR_NCHILDREN (cur) == 0);
  STACK_CURSOR_PARENT (cur);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)1);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_PARENT (cur);
  assert (STACK_CURSOR_IS_ROOT (cur));

  STACK_CURSOR_INIT_HANDLE (cur, h3);

  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)6);

  assert (STACK_CURSOR_NCHILDREN (cur) == 0);
  STACK_CURSOR_PARENT (cur);
  assert (!STACK_CURSOR_IS_ROOT (cur));
  assert (STACK_CURSOR_ELEMENT_ADDR (cur) == (StackElement)1);

  assert (STACK_CURSOR_NCHILDREN (cur) == 2);
  STACK_CURSOR_PARENT (cur);
  assert (STACK_CURSOR_IS_ROOT (cur));

  tvec nodes;
  STACK_CURSOR_INIT_ROOT(cur);
  for (STACK_CURSOR_NEXT(cur);
       !STACK_CURSOR_IS_ROOT(cur);
       STACK_CURSOR_NEXT(cur)) 
  {
    nodes.push_back (reinterpret_cast<unsigned long> (STACK_CURSOR_ELEMENT_ADDR (cur)));
  }
  
  assert (nodes == tvec({1, 2, 3, 4, 5, 6}));
}


//*********************************************************************


#define NTRACES 1000
const int MAXDEPTH = 200;

typedef struct _Trace {
  int depth;
  StackElement* addresses;
  StackHandle* handle;
} Trace;

int ntraces = 0;
Trace traces[NTRACES];

void fill_trace (Trace* t)
{
  int depth = (int)(drand48() * MAXDEPTH) + 1;
  int oldtrace = 0;
  int nshare;
  int i;

  if (ntraces == 0)
    nshare = 0;
  else {
    oldtrace = (int)(drand48() * ntraces);
    nshare = (int)(drand48() * (MIN (traces[oldtrace].depth, depth)+1));
  }
  t->depth = depth;
  t->addresses = (StackElement*) malloc (depth * sizeof(StackElement));
  for (i = 0; i < depth - nshare; i++)
    t->addresses[i] = (StackElement*)lrand48();
  for (; i < depth; i++)
    t->addresses[i] = traces[oldtrace].addresses[traces[oldtrace].depth -
                                                 (depth-i)];
}

void fill_traces()
{
  int i;
  for (i = 0; i < NTRACES; i++) {
    fill_trace (&traces[i]);
    ++ntraces;
  }
}

void check_trace (Trace* t)
{
  int i;
  StackElement* elt = STACK_HANDLE_ELEMENT (t->handle);
  for (i = 0; i < t->depth; i++) {
    assert (!STACK_ELEMENT_IS_ROOT(elt));
    assert (STACK_ELEMENT_ADDR(elt) == t->addresses[i]);
    elt = STACK_ELEMENT_PARENT(elt);
  }
  assert (STACK_ELEMENT_IS_ROOT(elt));
}


void check_stash(int n)
{
  int i;
  for (i = 0; i < n; i++)
    check_trace (&traces[i]);
}


void fill_stash()
{
  int i;
  for (i = 0; i < NTRACES;  i++)  {
    traces[i].handle = hhh_stackstash_store (traces[i].addresses,
                                             traces[i].depth);
    check_stash(i+1);
  }
}


int main()
{
  test_cursor();
  fill_traces();
  fill_stash();
  check_stash(NTRACES);
  return 0;
}
