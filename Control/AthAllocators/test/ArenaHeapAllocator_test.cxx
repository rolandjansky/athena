/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/test/ArenaHeapAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHeapAllocator.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <unistd.h>


static const size_t pageSize = sysconf (_SC_PAGESIZE);


//==========================================================================

std::atomic<int> nclear;
struct Payload
{
  Payload();
  ~Payload();
  Payload& operator= (const Payload&) = default;
  void clear();

  int x;
  int y;
  char pad[40-2*sizeof(int)-sizeof(void*)];
  static std::atomic<int> n;
  static std::vector<int> v ATLAS_THREAD_SAFE;
};

Payload::Payload()
{
  x = n++;
  y = 0;
  v.push_back (x);
}

Payload::~Payload()
{
  v.push_back (-x);
}

void Payload::clear ()
{
  y = 0;
  ++nclear;
}

std::atomic<int> Payload::n;
std::vector<int> Payload::v;

//==========================================================================

void test1()
{
  std::cout << "test1\n";
  SG::ArenaHeapAllocator aha
    (SG::ArenaHeapAllocator::initParams<Payload, true>(100, "foo"));
  assert (aha.name() == "foo");
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse == 0);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 0);
  const size_t elt_size = aha.params().eltSize;
  const size_t block_ov = SG::ArenaBlock::overhead();

  int nptr = 987;
  std::vector<Payload*> ptrs;
  for (int i=0; i < nptr; i++) {
    Payload* p = reinterpret_cast<Payload*> (aha.allocate());
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  const size_t nelt = Payload::v.size();
  assert  (nelt >= 1000);
  for (size_t i=0; i < nelt; ++i) {
    assert (Payload::v[i] == static_cast<int>(i));
  }

  const size_t elts_per_block = (pageSize - SG::ArenaBlockBodyOffset) / elt_size;
  const size_t nblocks = (1000 + elts_per_block-1) / elts_per_block;

  assert (aha.stats().elts.inuse == 987);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().elts.free == nelt - 987);
  assert (aha.stats().blocks.inuse == nblocks);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total == nblocks);

  for (size_t i = 0; i < ptrs.size(); i += 2)
    aha.free ((char*)ptrs[i]);
  assert (aha.stats().elts.inuse == 493);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().elts.free == nelt - 493);
  assert (aha.stats().blocks.inuse == nblocks);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total == nblocks);
  for (size_t i = 0; i < ptrs.size(); i += 2)
    assert (ptrs[i]->y == 0);
  for (size_t i = 0; i < 300; i++)
    ptrs.push_back (reinterpret_cast<Payload*>(aha.allocate()));
  //printf ("%d %d %d\n", aha.stats().elts.inuse, aha.stats().elts.free, aha.stats().elts.total);
  assert (aha.stats().elts.inuse == 793);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().elts.free == nelt - 793);
  assert (aha.stats().blocks.inuse == nblocks);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total == nblocks);

  assert (aha.stats().bytes.inuse ==  (793 * elt_size + nblocks * block_ov));
  assert (aha.stats().bytes.free  ==  (nelt - 793) * elt_size);
  assert (aha.stats().bytes.total == (nelt * elt_size + nblocks * block_ov));

  aha.reset();
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == nelt);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  == nblocks);
  assert (aha.stats().blocks.total == nblocks);

  ptrs.clear();
  for (size_t i = 0; i < 300; i++)
    ptrs.push_back (reinterpret_cast<Payload*>(aha.allocate()));
  assert (aha.stats().elts.inuse == 300);
  assert (aha.stats().elts.free == nelt - 300);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().blocks.inuse == (300 + elts_per_block-1) / elts_per_block);
  assert (aha.stats().blocks.free  ==  nblocks - aha.stats().blocks.inuse);
  assert (aha.stats().blocks.total == nblocks);

  aha.reserve (550);
  assert (aha.stats().elts.inuse == 300);
  assert (aha.stats().elts.total == ((550 + elts_per_block-1) / elts_per_block) * elts_per_block);
  assert (aha.stats().elts.free == aha.stats().elts.total - 300);
  assert (aha.stats().blocks.inuse ==  (300 + elts_per_block-1) / elts_per_block);
  assert (aha.stats().blocks.total ==  (550 + elts_per_block-1) / elts_per_block);
  assert (aha.stats().blocks.free  ==  aha.stats().blocks.total - aha.stats().blocks.inuse);

  aha.reserve (2000);
  //printf ("%d %d %d\n", aha.stats().elts.inuse, aha.stats().elts.free, aha.stats().elts.total);
  assert (aha.stats().elts.inuse == 300);
  assert (aha.stats().elts.total >= 2000);
  assert (aha.stats().elts.free == aha.stats().elts.total - 300);
  assert (aha.stats().blocks.inuse ==  (300 + elts_per_block-1) / elts_per_block);
  assert (aha.stats().blocks.total == (550 + elts_per_block-1) / elts_per_block + 1); 
  assert (aha.stats().blocks.free  ==  aha.stats().blocks.total - aha.stats().blocks.inuse);
  const size_t nelt2 = aha.stats().elts.total;

  Payload::v.clear();
  aha.erase();
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total ==  0);
  assert (Payload::v.size() == nelt2);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i = 0; i < Payload::v.size(); i++) {
    assert (Payload::v[i] <= 0);
  }
}


void test2()
{
  std::cout << "test2\n";
  Payload::v.clear();
  
  SG::ArenaHeapAllocator::Params params = 
    SG::ArenaHeapAllocator::initParams<Payload, true>(100);
  params.mustClear = true;
  params.canReclear = false;
  SG::ArenaHeapAllocator aha (params);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total ==  0);

  nclear = 0;
  std::vector<Payload*> ptrs;
  for (int i=0; i < 987; i++) {
    Payload* p = reinterpret_cast<Payload*> (aha.allocate());
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  for (size_t i = 0; i < ptrs.size(); i+=2) {
    aha.free ((char*)ptrs[i]);
    assert (ptrs[i]->y == 0);
    ptrs[i]->y = 1;
  }
  aha.reset();
  assert (nclear == 987);

  size_t nelt = Payload::v.size();
  assert  (nelt >= 1000);

  const size_t elt_size = aha.params().eltSize;
  const size_t elts_per_block = (pageSize - SG::ArenaBlockBodyOffset) / elt_size;
  const size_t nblocks = (1000 + elts_per_block-1) / elts_per_block;

  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == nelt);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  == nblocks);
  assert (aha.stats().blocks.total == nblocks);

  for (size_t i = 0; i < ptrs.size(); i++) {
    if ((i&1) != 0)
      assert (ptrs[i]->y == 0);
    else
      assert (ptrs[i]->y == 1);
  }
}


void test3()
{
  std::cout << "test3\n";

  Payload::v.clear();
  SG::ArenaHeapAllocator aha
    (SG::ArenaHeapAllocator::initParams<Payload, true>(100, "bar"));
  for (int i=0; i < 150; i++) {
    aha.allocate();
  }

  const size_t nelt = Payload::v.size();
  assert  (nelt >= 150);

  const size_t elt_size = aha.params().eltSize;
  const size_t elts_per_block = (pageSize - SG::ArenaBlockBodyOffset) / elt_size;
  const size_t nblocks = (150 + elts_per_block-1) / elts_per_block;

  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha.stats().elts.inuse == 150);
  assert (aha.stats().elts.free == nelt - 150);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().blocks.inuse == nblocks);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == nblocks);

  SG::ArenaHeapAllocator aha2 (std::move (aha));
  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha2.name() == "bar");
  assert (aha2.params().name == "bar");
  assert (aha2.stats().elts.inuse == 150);
  assert (aha2.stats().elts.free == nelt - 150);
  assert (aha2.stats().elts.total == nelt);
  assert (aha2.stats().blocks.inuse == nblocks);
  assert (aha2.stats().blocks.free  == 0);
  assert (aha2.stats().blocks.total == nblocks);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse == 0);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 0);

  aha = std::move (aha2);
  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha2.name() == "bar");
  assert (aha2.params().name == "bar");
  assert (aha.stats().elts.inuse == 150);
  assert (aha.stats().elts.free == nelt - 150);
  assert (aha.stats().elts.total == nelt);
  assert (aha.stats().blocks.inuse == nblocks);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == nblocks);
  assert (aha2.stats().elts.inuse == 0);
  assert (aha2.stats().elts.free == 0);
  assert (aha2.stats().elts.total == 0);
  assert (aha2.stats().blocks.inuse == 0);
  assert (aha2.stats().blocks.free  == 0);
  assert (aha2.stats().blocks.total == 0);

  aha.swap (aha2);
  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha2.name() == "bar");
  assert (aha2.params().name == "bar");
  assert (aha2.stats().elts.inuse == 150);
  assert (aha2.stats().elts.free == nelt - 150);
  assert (aha2.stats().elts.total == nelt);
  assert (aha2.stats().blocks.inuse == nblocks);
  assert (aha2.stats().blocks.free  == 0);
  assert (aha2.stats().blocks.total == nblocks);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse == 0);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 0);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
