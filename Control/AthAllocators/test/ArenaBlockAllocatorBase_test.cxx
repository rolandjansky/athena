/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBlockAllocatorBase_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaBlockAllocatorBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaBlockAllocatorBase.
 */

#undef NDEBUG
#include "AthAllocators/ArenaBlockAllocatorBase.h"
#include "AthAllocators/ArenaBlock.h"
#include <cassert>


struct Payload
{
  Payload();
  ~Payload();
  Payload& operator= (const Payload&) = default;
  void clear();

  int x;
  int y;
  static int n;
  //static std::vector<int> v;
};

Payload::Payload()
{
  x = n++;
  y = 0;
  //v.push_back (x);
}

Payload::~Payload()
{
  //v.push_back (-x);
}

void Payload::clear ()
{
  y = 0;
}

int Payload::n = 0;
//std::vector<int> Payload::v;


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
  test_stats (bab, 1, 1000);

  bab.reserve (500);
  test_stats (bab, 1, 1000);

  bab.reserve (0);
  test_stats (bab, 0, 0);

  bab.reserve (500);
  test_stats (bab, 1, 500);
  bab.reserve (1000);
  test_stats (bab, 2, 1000);
  bab.reserve (500);
  test_stats (bab, 1, 500);
  bab.erase();
  test_stats (bab, 0, 0);

  bab.reserve (500);
  test_stats (bab, 1, 500);
  SG::ArenaBlock* blocks = bab.m_blocks;
  SG::ArenaBlock* freeblocks = bab.m_freeblocks;
  TestAlloc bab2 (std::move (bab));
  assert (bab.name() == "foo");
  assert (bab.params().name == "foo");
  assert (bab2.name() == "foo");
  assert (bab2.params().name == "foo");
  test_stats (bab, 0, 0);
  test_stats (bab2, 1, 500);
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
  test_stats (bab, 1, 500);
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
  test_stats (bab2, 1, 500);
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
