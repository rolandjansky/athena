/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
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
    SG::ArenaHandle<Payload, SG::ArenaPoolAllocator> Handtype;
  SG::ArenaHeader head;
  SG::Arena arena  ("a", &head);
  SG::Arena::Push push (arena);
  

  std::vector<Payload*> ptrs;
  {
    auto params = Handtype::alloc_t::initParams<Payload, true>(50).params();
    Handtype hand (&arena, &params);
    for (int i=0; i < 100; i++) {
      Payload* p = new (hand.allocate()) Payload;
      assert (p->y == 0);
      p->y = i+1;
      ptrs.push_back (p);
    }
    assert (nctor == 100);
    assert (ndtor == 0);
    assert (nclear == 0);
    assert (hand.stats().elts.total >= 100);
    assert (hand.stats().elts.inuse == 100);
    assert (hand.stats().elts.free == hand.stats().elts.total - 100);

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
    assert (hand.stats().elts.total >= 100);
    assert (hand.stats().elts.inuse == 0);
    assert (hand.stats().elts.free == hand.stats().elts.total);

    ptrs.clear();
    for (int i=0; i < 100; i++) {
      Payload* p = new (hand.allocate()) Payload;
      p->y = i + 1;
      ptrs.push_back (p);
    }
    assert (nctor == 200);
    assert (ndtor == 100);
    assert (nclear == 0);
    assert (hand.stats().elts.total >= 100);
    assert (hand.stats().elts.inuse == 100);
    assert (hand.stats().elts.free == hand.stats().elts.total - 100);
  }

  {
    Handtype hand2 (&head, static_cast<size_t>(0));

    hand2.resetTo (ptrs[50]);
    assert (nctor == 200);
    assert (ndtor == 150);
    assert (nclear == 00);
    assert (hand2.stats().elts.total >= 100);
    assert (hand2.stats().elts.inuse == 50);
    assert (hand2.stats().elts.free == hand2.stats().elts.total - 50);

    hand2.erase();
    assert (nctor == 200);
    assert (ndtor == 200);
    assert (nclear == 0);
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
    SG::ArenaHandle<Payload, SG::ArenaHeapAllocator> Handtype;
  
  auto params = Handtype::alloc_t::initParams<Payload, true>(50).params();
  Handtype hand (&params);
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
  assert (hand.stats().elts.total >= 100);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == hand.stats().elts.total - 100);

  for (size_t i = 0; i < ptrs.size(); i+=2) {
    hand.free (ptrs[i]);
  }
  assert (nctor == 100);
  assert (ndtor == 50);
  assert (nclear == 0);
  assert (hand.stats().elts.total >= 100);
  assert (hand.stats().elts.inuse == 50);
  assert (hand.stats().elts.free == hand.stats().elts.total - 50);
  const size_t nblock = hand.stats().blocks.total;
  assert (nblock >= 1);
  const size_t nelt = hand.stats().elts.total;
  assert (hand.stats().blocks.inuse == nblock);
  assert (hand.stats().blocks.free == 0);
  assert (hand.stats().blocks.total == nblock);
  assert (hand.stats().bytes.inuse ==  50 * elt_size + nblock * block_ov);
  assert (hand.stats().bytes.free  ==  (nelt - 50) * elt_size);
  assert (hand.stats().bytes.total == nelt * elt_size + nblock * block_ov);

  hand.reset();
  assert (nctor == 100);
  assert (ndtor == 100);
  assert (nclear == 0);
  assert (hand.stats().elts.total >= 100);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == hand.stats().elts.total);

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
  assert (hand.stats().elts.total >= 100);
  assert (hand.stats().elts.inuse == 100);
  assert (hand.stats().elts.free == hand.stats().elts.total - 100);
}


void test3()
{
  typedef
    SG::ArenaHandle<Payload, SG::ArenaHeapAllocator> Handtype;
  
  SG::ArenaHeader head;
  auto params = Handtype::alloc_t::initParams<Payload, true>(50).params();
  Handtype hand (&head, &params);

  hand.reserve (100);
  assert (hand.stats().elts.total >= 100);
  assert (hand.stats().elts.inuse == 0);
  assert (hand.stats().elts.free == hand.stats().elts.total);
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
    SG::ArenaHandle<Payload, SG::ArenaHeapAllocator> Handtype;

  {
    Handtype hand (&head, EventContext (0, 1));
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  SG::ArenaHandle<Payload,SG::ArenaHeapAllocator>\n\
=== 2 ===\n\
=== default ===\n");

  {
    Handtype hand (&head, EventContext (0, 2));
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  SG::ArenaHandle<Payload,SG::ArenaHeapAllocator>\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  SG::ArenaHandle<Payload,SG::ArenaHeapAllocator>\n\
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
