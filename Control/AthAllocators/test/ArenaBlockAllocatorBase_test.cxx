/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/test/ArenaBlockAllocatorBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaBlockAllocatorBase.
 */

#undef NDEBUG
#include "AthAllocators/ArenaBlockAllocatorBase.h"
#include "AthAllocators/ArenaBlock.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <atomic>
#include <vector>
#include <iostream>


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


class TestAlloc
  : public SG::ArenaBlockAllocatorBase
{
public:
  TestAlloc (const Params& params) : SG::ArenaBlockAllocatorBase (params){}
  virtual void reset() override {}

  using SG::ArenaBlockAllocatorBase::m_blocks;
  using SG::ArenaBlockAllocatorBase::m_freeblocks;
};


void test_stats (const SG::ArenaBlockAllocatorBase& bab,
                 size_t nblock,
                 size_t nelt)
{
  size_t elt_size = bab.params().eltSize;
  size_t block_ov = SG::ArenaBlock::overhead();
  const SG::ArenaAllocatorBase::Stats& stats = bab.stats();
  assert (stats.blocks.total == nblock);
  assert (stats.blocks.inuse == 0);
  assert (stats.blocks.free  == nblock);
  assert (stats.elts.total == nelt);
  assert (stats.elts.inuse == 0);
  assert (stats.elts.free  == nelt);
  assert (stats.bytes.total == nelt * elt_size + nblock * block_ov);
  assert (stats.bytes.inuse == 0);
  assert (stats.bytes.free  == nelt * elt_size + nblock * block_ov);
}


void test1()
{
  TestAlloc bab
    (SG::ArenaAllocatorBase::initParams<Payload, true> (100, "foo"));
  assert (bab.name() == "foo");
  assert (bab.params().name == "foo");
  test_stats (bab, 0, 0);

  bab.reserve (1000);
  const size_t nelt1 = Payload::v.size();
  assert (nelt1 >= 1000);
  test_stats (bab, 1, nelt1);

  bab.reserve (500);
  test_stats (bab, 1, nelt1);

  bab.reserve (0);
  test_stats (bab, 0, 0);

  Payload::v.clear();
  bab.reserve (500);
  const size_t nelt2 = Payload::v.size();
  assert (nelt2 >= 500);
  test_stats (bab, 1, nelt2);
  bab.reserve (1000);
  const size_t nelt3 = Payload::v.size();
  assert (nelt3 >= 1000);
  test_stats (bab, 2, nelt3);
  bab.reserve (500);
  test_stats (bab, 1, nelt2);
  bab.erase();
  test_stats (bab, 0, 0);

  bab.reserve (500);
  test_stats (bab, 1, nelt2);
  SG::ArenaBlock* blocks = bab.m_blocks;
  SG::ArenaBlock* freeblocks = bab.m_freeblocks;
  TestAlloc bab2 (std::move (bab));
  assert (bab.name() == "foo");
  assert (bab.params().name == "foo");
  assert (bab2.name() == "foo");
  assert (bab2.params().name == "foo");
  test_stats (bab, 0, 0);
  test_stats (bab2, 1, nelt2);
  assert (bab.m_blocks == nullptr);
  assert (bab.m_freeblocks == nullptr);
  assert (bab2.m_blocks == blocks);
  assert (bab2.m_freeblocks == freeblocks);

  bab = std::move(bab2);
  assert (bab.name() == "foo");
  assert (bab.params().name == "foo");
  assert (bab2.name() == "foo");
  assert (bab2.params().name == "foo");
  test_stats (bab2, 0, 0);
  test_stats (bab, 1, nelt2);
  assert (bab2.m_blocks == nullptr);
  assert (bab2.m_freeblocks == nullptr);
  assert (bab.m_blocks == blocks);
  assert (bab.m_freeblocks == freeblocks);

  bab.swap (bab2);
  assert (bab.name() == "foo");
  assert (bab.params().name == "foo");
  assert (bab2.name() == "foo");
  assert (bab2.params().name == "foo");
  test_stats (bab, 0, 0);
  test_stats (bab2, 1, nelt2);
  assert (bab.m_blocks == nullptr);
  assert (bab.m_freeblocks == nullptr);
  assert (bab2.m_blocks == blocks);
  assert (bab2.m_freeblocks == freeblocks);
}

int main()
{
  test1();
  return 0;
}
