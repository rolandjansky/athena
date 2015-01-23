/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandle_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHandle_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHandle.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHandle.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include <cassert>

int count = 0;
int nctor = 0;
int ndtor = 0;
int nclear = 0;
struct Payload
{
  int x;
  int y;
  Payload() { x = ++count;  y = 0; ++nctor; }
  ~Payload() { ++ndtor; }
  void clear() { y = 0; ++nclear; }
};

void test1()
{
  count = 0;
  nctor = 0;
  ndtor = 0;
  nclear = 0;

  typedef
    SG::ArenaHandle<Payload, SG::ArenaPoolAllocator> Handtype;
  SG::ArenaHeader head;
  SG::Arena arena  ("a", &head);
  SG::Arena::Push push (arena);
  

  Handtype hand (&arena,
                 SG::ArenaPoolAllocator::initParams<Payload, true>(50));
  std::vector<Payload*> ptrs;
  for (int i=0; i < 100; i++) {
    Payload* p = new (hand.allocate()) Payload;
    assert (p->y == 0);
    p->y = i+1;
    ptrs.push_back (p);
  }
  assert (nctor == 100);
  assert (ndtor == 0);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == 0);
  assert (hand.stats().elts.total == 100);

  std::vector<int> v;
  for (Handtype::iterator ii = hand.begin();
       ii != hand.end();
       ++ii)
  {
    assert (ii->y != 0);
    v.push_back (ii->x);
  }
  assert (v.size() == 100);
  for (size_t i = 0; i < v.size(); i++) {
    assert (v[i] == 100-(int)i);
  }

  const Handtype& chand = hand;
  v.clear();
  for (Handtype::const_iterator ii = chand.begin();
       ii != chand.end();
       ++ii)
  {
    assert (ii->y != 0);
    v.push_back (ii->x);
  }
  assert (v.size() == 100);
  for (size_t i = 0; i < v.size(); i++) {
    assert (v[i] == 100-(int)i);
  }

  v.clear();
  for (Handtype::const_iterator ii = hand.begin();
       ii != hand.end();
       ++ii)
  {
    assert (ii->y != 0);
    v.push_back (ii->x);
  }
  assert (v.size() == 100);
  for (size_t i = 0; i < v.size(); i++) {
    assert (v[i] == 100-(int)i);
  }

  hand.reset();
  assert (nctor == 100);
  assert (ndtor == 100);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == 100);
  assert (hand.stats().elts.total == 100);

  ptrs.clear();
  for (int i=0; i < 100; i++) {
    Payload* p = new (hand.allocate()) Payload;
    p->y = i + 1;
    ptrs.push_back (p);
  }
  assert (nctor == 200);
  assert (ndtor == 100);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == 0);
  assert (hand.stats().elts.total == 100);

  Handtype hand2 (&head, 0);

  hand2.resetTo (ptrs[50]);
  assert (nctor == 200);
  assert (ndtor == 150);
  assert (nclear == 00);
  assert (hand2.stats().elts.inuse == 50);
  assert (hand2.stats().elts.free == 50);
  assert (hand2.stats().elts.total == 100);

  hand2.erase();
  assert (nctor == 200);
  assert (ndtor == 200);
  assert (nclear == 0);
  assert (hand2.stats().elts.inuse == 0);
  assert (hand2.stats().elts.free == 0);
  assert (hand2.stats().elts.total == 0);
}


void test2()
{
  count = 0;
  nctor = 0;
  ndtor = 0;
  nclear = 0;

  typedef
    SG::ArenaHandle<Payload, SG::ArenaHeapAllocator> Handtype;
  
  Handtype hand (SG::ArenaHeapAllocator::initParams<Payload, true>(50));
  size_t elt_size = hand.params().eltSize;
  assert (elt_size == sizeof (Payload));
  size_t block_ov = SG::ArenaBlock::overhead();

  std::vector<Payload*> ptrs;
  for (int i=0; i < 100; i++) {
    Payload* p = new (hand.allocate()) Payload;
    assert (p->y == 0);
    p->y = i+1;
    ptrs.push_back (p);
  }
  assert (nctor == 100);
  assert (ndtor == 0);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == 0);
  assert (hand.stats().elts.total == 100);

  for (size_t i = 0; i < ptrs.size(); i+=2) {
    hand.free (ptrs[i]);
  }
  assert (nctor == 100);
  assert (ndtor == 50);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 50);
  assert (hand.stats().elts.free == 50);
  assert (hand.stats().elts.total == 100);
  assert (hand.stats().blocks.inuse == 2);
  assert (hand.stats().blocks.free == 0);
  assert (hand.stats().blocks.total == 2);
  assert (hand.stats().bytes.inuse ==  50 * elt_size + 2 * block_ov);
  assert (hand.stats().bytes.free  ==  50 * elt_size);
  assert (hand.stats().bytes.total == 100 * elt_size + 2 * block_ov);

  hand.reset();
  assert (nctor == 100);
  assert (ndtor == 100);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == 100);
  assert (hand.stats().elts.total == 100);

  ptrs.clear();
  for (int i=0; i < 100; i++) {
    Payload* p = new (hand.allocate()) Payload;
    assert (p->y == 0);
    p->y = i+1;
    ptrs.push_back (p);
  }
  assert (nctor == 200);
  assert (ndtor == 100);
  assert (nclear == 0);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == 0);
  assert (hand.stats().elts.total == 100);
}


void test3()
{
  typedef
    SG::ArenaHandle<Payload, SG::ArenaHeapAllocator> Handtype;
  
  SG::ArenaHeader head;
  Handtype hand (&head,
                 SG::ArenaHeapAllocator::initParams<Payload, true>(50));

  hand.reserve (100);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == 100);
  assert (hand.stats().elts.total == 100);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
