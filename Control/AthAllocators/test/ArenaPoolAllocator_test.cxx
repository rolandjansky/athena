/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/test/ArenaPoolAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaPoolAllocator.
 */

#undef NDEBUG
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <atomic>
#include <unistd.h>


static const size_t pageSize = sysconf (_SC_PAGESIZE);


//==========================================================================


struct Payload
{
  Payload();
  ~Payload();
  Payload& operator= (const Payload&) = default;
  void clear();

  int x;
  int y;
  char pad[40-2*sizeof(int)];
  static std::atomic<int> n;
  static std::vector<int> v ATLAS_THREAD_SAFE;
};

// Work around cppcheck 1.87 bug, which warns about clear().
// cppcheck-suppress uninitMemberVar
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
}

std::atomic<int> Payload::n;
std::vector<int> Payload::v;

//==========================================================================

void test1()
{
  std::cout << "test1\n";
  SG::ArenaPoolAllocator apa
    (SG::ArenaPoolAllocator::initParams<Payload, true>(100, "foo"));
  assert (apa.name() == "foo");
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 0);
  assert (apa.stats().elts.total == 0);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free == 0);
  assert (apa.stats().blocks.total == 0);
  assert (apa.params().eltSize == sizeof (Payload));
  const size_t elt_size = apa.params().eltSize;

  int nptr = 987;
  std::vector<Payload*> ptrs;
  for (int i=0; i < nptr; i++) {
    Payload* p = reinterpret_cast<Payload*> (apa.allocate());
    if (!p) std::abort();
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

  assert (apa.stats().elts.inuse == 987);
  assert (apa.stats().elts.free == nelt - 987);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == nblocks);
  assert (apa.stats().blocks.free == 0);
  assert (apa.stats().blocks.total == nblocks);

  Payload::v.clear();
  apa.reset();
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == nelt);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free == nblocks);
  assert (apa.stats().blocks.total == nblocks);

  for (int i=0; i < 300; i++) {
    Payload* p = reinterpret_cast<Payload*> (apa.allocate());
    assert (p->y == 0);
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  assert (Payload::v.size() == 0);
  assert (apa.stats().elts.inuse == 300);
  assert (apa.stats().elts.free == nelt - 300);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == (300 + elts_per_block-1) / elts_per_block);
  assert (apa.stats().blocks.free  ==  nblocks - apa.stats().blocks.inuse);
  assert (apa.stats().blocks.total == nblocks);

  apa.allocate();
  std::vector<int> vv;
  for (SG::ArenaPoolAllocator::iterator ii = apa.begin();
       ii != apa.end();
       ++ii)
  {
    vv.push_back (reinterpret_cast<Payload*> (&*ii)->x);
  }
  std::sort (vv.begin(), vv.end());
  assert (vv.size() == 301);
  for (size_t jj = 0; jj < vv.size(); jj++) {
    assert (vv[jj] == (int)jj);
  }

  vv.clear();
  const SG::ArenaPoolAllocator& capa = apa;
  for (SG::ArenaPoolAllocator::const_iterator ii = capa.begin();
       ii != capa.end();
       ++ii)
  {
    vv.push_back (reinterpret_cast<const Payload*> (&*ii)->x);
  }
  std::sort (vv.begin(), vv.end());
  assert (vv.size() == 301);
  for (size_t jj = 0; jj < vv.size(); jj++) {
    assert (vv[jj] == (int)jj);
  }

  vv.clear();
  for (SG::ArenaPoolAllocator::const_iterator ii = apa.begin();
       ii != apa.end();
       ++ii)
  {
    vv.push_back (reinterpret_cast<const Payload*> (&*ii)->x);
  }
  std::sort (vv.begin(), vv.end());
  assert (vv.size() == 301);
  for (size_t jj = 0; jj < vv.size(); jj++) {
    assert (vv[jj] == (int)jj);
  }

  Payload::v.clear();
  apa.reset();
  for (int i=0; i < 400; i++) {
    Payload* p = reinterpret_cast<Payload*> (apa.allocate());
    assert (p->y == 0);
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  assert (Payload::v.size() == 0);
  assert (apa.stats().elts.inuse == 400);
  assert (apa.stats().elts.free == nelt - 400);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == (400 + elts_per_block-1) / elts_per_block);
  assert (apa.stats().blocks.free  ==  nblocks - apa.stats().blocks.inuse);
  assert (apa.stats().blocks.total == nblocks);

  Payload::v.clear();
  apa.reserve (550);
  assert (apa.stats().elts.inuse == 400);
  assert (apa.stats().elts.total == ((550 + elts_per_block-1) / elts_per_block) * elts_per_block);
  assert (apa.stats().elts.free == apa.stats().elts.total - 400);
  assert (apa.stats().blocks.inuse == (400 + elts_per_block-1) / elts_per_block);
  assert (apa.stats().blocks.total == (550 + elts_per_block-1) / elts_per_block);
  assert (apa.stats().blocks.free  ==  apa.stats().blocks.total - apa.stats().blocks.inuse);
  const size_t freed_blocks = (nelt - 550)/elts_per_block;
  assert (Payload::v.size() == freed_blocks * elts_per_block);
  std::sort (Payload::v.begin(), Payload::v.end());
  const size_t max_del = (apa.stats().blocks.inuse + freed_blocks)*elts_per_block-1;
  for (size_t i=0; i < Payload::v.size(); i++) {
    assert (Payload::v[i] == static_cast<int>(i-max_del));
  }

  Payload::v.clear();
  const size_t nelt2 = apa.stats().elts.total;
  apa.reserve (nelt2 + elts_per_block / 2);
  assert (apa.stats().elts.inuse == 400);
  assert (apa.stats().elts.total == nelt2 + elts_per_block);
  assert (apa.stats().elts.free == apa.stats().elts.total - apa.stats().elts.inuse);
  assert (apa.stats().blocks.inuse == (400 + elts_per_block-1) / elts_per_block);
  assert (apa.stats().blocks.total == (550 + elts_per_block-1) / elts_per_block + 1);
  assert (apa.stats().blocks.free  ==  apa.stats().blocks.total - apa.stats().blocks.inuse);
  assert (Payload::v.size() == elts_per_block);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i=0; i < Payload::v.size(); i++) {
    assert (Payload::v[i] == static_cast<int>(i+nelt));
  }

  
  Payload::v.clear();
  apa.erase();
  assert (Payload::v.size() == nelt2 + elts_per_block);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i=0; i < Payload::v.size() ; i++) {
    assert (Payload::v[i] <= 0);
  }
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 0);
  assert (apa.stats().elts.total == 0);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == 0);
}


void test2()
{
  std::cout << "test2\n";
  Payload::v.clear();
  SG::ArenaPoolAllocator apa
    (SG::ArenaPoolAllocator::initParams<Payload, true>(100));
  for (int i=0; i < 150; i++) {
    apa.allocate();
  }

  assert (Payload::v.size() >= 150);

  SG::ArenaPoolAllocator::pointer p = apa.allocate();
  assert (Payload::v.size() >= 151);
  for (int i=0; i < 150; i++) {
    apa.allocate();
  }
  const size_t nelt = Payload::v.size();
  assert (nelt >= 301);
  const size_t elt_size = apa.params().eltSize;
  const size_t elts_per_block = (pageSize - SG::ArenaBlockBodyOffset) / elt_size;
  const size_t nblocks1 = (150 + elts_per_block-1) / elts_per_block;
  const size_t nblocks2 = (301 + elts_per_block-1) / elts_per_block;

  assert (apa.stats().elts.inuse == 301);
  assert (apa.stats().elts.free == nelt - 301);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == nblocks2);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == nblocks2);

  const size_t block_ov = SG::ArenaBlock::overhead();

  apa.resetTo (p);

  assert (apa.stats().elts.inuse == 150);
  assert (apa.stats().elts.free == nelt - 150);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == nblocks1);
  assert (apa.stats().blocks.free  == nblocks2 - nblocks1);
  assert (apa.stats().blocks.total == nblocks2);

  assert (apa.stats().bytes.inuse == (150 * elt_size + nblocks1 * block_ov));
  assert (apa.stats().bytes.free == ((nelt - 150) * elt_size + (nblocks2-nblocks1) * block_ov));
  assert (apa.stats().bytes.total == (nelt * elt_size + nblocks2 * block_ov));
}


void test3()
{
  std::cout << "test3\n";

  Payload::v.clear();
  SG::ArenaPoolAllocator apa
    (SG::ArenaPoolAllocator::initParams<Payload, true>(100, "bar"));
  for (int i=0; i < 150; i++) {
    apa.allocate();
  }

  const size_t nelt = Payload::v.size();
  const size_t elt_size = apa.params().eltSize;
  const size_t elts_per_block = (pageSize - SG::ArenaBlockBodyOffset) / elt_size;
  const size_t nblocks = (150 + elts_per_block-1) / elts_per_block;

  assert (apa.name() == "bar");
  assert (apa.params().name == "bar");
  assert (apa.stats().elts.inuse == 150);
  assert (apa.stats().elts.free == nelt - 150);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == nblocks);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == nblocks);

  SG::ArenaPoolAllocator apa2 (std::move (apa));
  assert (apa.name() == "bar");
  assert (apa.params().name == "bar");
  assert (apa2.name() == "bar");
  assert (apa2.params().name == "bar");
  assert (apa2.stats().elts.inuse == 150);
  assert (apa2.stats().elts.free == nelt - 150);
  assert (apa2.stats().elts.total == nelt);
  assert (apa2.stats().blocks.inuse == nblocks);
  assert (apa2.stats().blocks.free  == 0);
  assert (apa2.stats().blocks.total == nblocks);
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 0);
  assert (apa.stats().elts.total == 0);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == 0);

  apa = std::move (apa2);
  assert (apa.name() == "bar");
  assert (apa.params().name == "bar");
  assert (apa2.name() == "bar");
  assert (apa2.params().name == "bar");
  assert (apa.stats().elts.inuse == 150);
  assert (apa.stats().elts.free == nelt - 150);
  assert (apa.stats().elts.total == nelt);
  assert (apa.stats().blocks.inuse == nblocks);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == nblocks);
  assert (apa2.stats().elts.inuse == 0);
  assert (apa2.stats().elts.free == 0);
  assert (apa2.stats().elts.total == 0);
  assert (apa2.stats().blocks.inuse == 0);
  assert (apa2.stats().blocks.free  == 0);
  assert (apa2.stats().blocks.total == 0);

  apa.swap (apa2);
  assert (apa.name() == "bar");
  assert (apa.params().name == "bar");
  assert (apa2.name() == "bar");
  assert (apa2.params().name == "bar");
  assert (apa2.stats().elts.inuse == 150);
  assert (apa2.stats().elts.free == nelt - 150);
  assert (apa2.stats().elts.total == nelt);
  assert (apa2.stats().blocks.inuse == nblocks);
  assert (apa2.stats().blocks.free  == 0);
  assert (apa2.stats().blocks.total == nblocks);
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 0);
  assert (apa.stats().elts.total == 0);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == 0);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
