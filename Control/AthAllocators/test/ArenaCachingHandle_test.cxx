/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaCachingHandle_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaCachingHandle_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaCachingHandle.
 */

#undef NDEBUG
#include "AthAllocators/ArenaCachingHandle.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include <cassert>
#include <atomic>


std::atomic<int> count;
std::atomic<int> nctor;
std::atomic<int> ndtor;
std::atomic<int> nclear;
struct Payload
{
  int x;
  int y;
  Payload() { x = ++count;  y = 0; ++nctor; }
  ~Payload() { ++ndtor; }
  Payload& operator= (const Payload&) = default;
  void clear() { y = 0; ++nclear; }
};

void test1()
{
  count = 0;
  nctor = 0;
  ndtor = 0;
  nclear = 0;

  typedef
    SG::ArenaCachingHandle<Payload, SG::ArenaPoolAllocator> Handtype;
  SG::ArenaHeader head;
  SG::Arena arena  ("a", &head);
  SG::Arena::Push push (arena);

  size_t block_ov = SG::ArenaBlock::overhead();
  std::vector<Payload*> ptrs;

  {
    auto params = Handtype::alloc_t::initParams<Payload, true>(50).params();
    
    Handtype hand (&arena, &params);
    for (int i=0; i < 100; i++) {
      Payload* p = hand.allocate();
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
    assert (ndtor == 0);
    assert (nclear == 100);
    assert (hand.stats().elts.inuse == 0);
    assert (hand.stats().elts.free == 100);
    assert (hand.stats().elts.total == 100);

    ptrs.clear();
    for (int i=0; i < 100; i++) {
      Payload* p = hand.allocate();
      assert (p->y == 0);
      p->y = i + 1;
      ptrs.push_back (p);
    }
    assert (nctor == 100);
    assert (ndtor == 0);
    assert (nclear == 100);
    assert (hand.stats().elts.inuse == 100);
    assert (hand.stats().elts.free == 0);
    assert (hand.stats().elts.total == 100);
  }

  {
    Handtype hand2 (&head, static_cast<size_t>(0));
    size_t elt_size = hand2.params().eltSize;

    hand2.resetTo (ptrs[50]);
    assert (nctor == 100);
    assert (ndtor == 0);
    assert (nclear == 150);
    assert (hand2.stats().elts.inuse == 50);
    assert (hand2.stats().elts.free == 50);
    assert (hand2.stats().elts.total == 100);
    assert (hand2.stats().blocks.inuse == 1);
    assert (hand2.stats().blocks.free == 1);
    assert (hand2.stats().blocks.total == 2);
    assert (hand2.stats().bytes.inuse == 50 * elt_size + block_ov);
    assert (hand2.stats().bytes.free == 50 * elt_size + block_ov);
    assert (hand2.stats().bytes.total == 100 * elt_size + 2*block_ov);

    hand2.erase();
    assert (nctor == 100);
    assert (ndtor == 100);
    assert (nclear == 150);
    assert (hand2.stats().elts.inuse == 0);
    assert (hand2.stats().elts.free == 0);
    assert (hand2.stats().elts.total == 0);
  }
}


void test2()
{
  count = 0;
  nctor = 0;
  ndtor = 0;
  nclear = 0;

  typedef
    SG::ArenaCachingHandle<Payload, SG::ArenaHeapAllocator> Handtype;


  auto params = Handtype::alloc_t::initParams<Payload, true>(50).params();
  Handtype hand (&params);
  size_t elt_size = hand.params().eltSize;
  assert (elt_size > sizeof (Payload));

  std::vector<Payload*> ptrs;
  for (int i=0; i < 100; i++) {
    Payload* p = hand.allocate();
    assert (p->y == 0);
    p->y = i+1;
    ptrs.push_back (p);
  }
  //printf ("%d %d %d\n", nctor, ndtor, nclear);
  //printf ("%d %d %d\n", hand.stats().elts.inuse, hand.stats().elts.free, hand.stats().elts.total());
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
  assert (ndtor == 0);
  assert (nclear == 50);
  assert (hand.stats().elts.inuse == 50);
  assert (hand.stats().elts.free == 50);
  assert (hand.stats().elts.total == 100);

  hand.reset();
  assert (nctor == 100);
  assert (ndtor == 0);
  assert (nclear == 150);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == 100);
  assert (hand.stats().elts.total == 100);

  ptrs.clear();
  for (int i=0; i < 100; i++) {
    Payload* p = hand.allocate();
    assert (p->y == 0);
    p->y = i+1;
    ptrs.push_back (p);
  }
  assert (nctor == 100);
  assert (ndtor == 0);
  assert (nclear == 150);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == 0);
  assert (hand.stats().elts.total == 100);
}


void test3()
{
  typedef
    SG::ArenaCachingHandle<Payload, SG::ArenaHeapAllocator> Handtype;
  
  SG::ArenaHeader head;
  auto params = Handtype::alloc_t::initParams<Payload, true>(50).params();
  Handtype hand (&head, &params);

  hand.reserve (100);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == 100);
  assert (hand.stats().elts.total == 100);
}


void test4()
{
  SG::ArenaHeader head;

  SG::ArenaBase a1 ("1");
  SG::ArenaBase a2  ("2");
  head.addArena (&a1);
  head.addArena (&a2);
  head.setArenaForSlot (1, &a1);
  head.setArenaForSlot (2, &a2);

  typedef
    SG::ArenaCachingHandle<Payload, SG::ArenaHeapAllocator> Handtype;

  {
    Handtype hand (&head, EventContext (0, 1));
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  SG::ArenaCachingHandle<Payload,SG::ArenaHeapAllocator>\n\
=== 2 ===\n\
=== default ===\n");

  {
    Handtype hand (&head, EventContext (0, 2));
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  SG::ArenaCachingHandle<Payload,SG::ArenaHeapAllocator>\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  SG::ArenaCachingHandle<Payload,SG::ArenaHeapAllocator>\n\
=== default ===\n");
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  return 0;
}
