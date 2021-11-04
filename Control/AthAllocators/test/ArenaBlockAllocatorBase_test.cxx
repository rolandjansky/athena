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
#include "AthAllocators/exceptions.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <atomic>
#include <vector>
#include <iostream>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>


static const size_t pageSize = sysconf (_SC_PAGESIZE);


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


Payload& payload (SG::ArenaBlock* bl, size_t i=0)
{
  while (bl && i >= bl->size()) {
    i -= bl->size();
    bl = bl->link();
  }
  assert (bl != nullptr);
  return *(Payload*)bl->index (i, sizeof(Payload));
}


class TestAlloc
  : public SG::ArenaBlockAllocatorBase
{
public:
  TestAlloc (const Params& params) : SG::ArenaBlockAllocatorBase (params){}
  virtual void reset() override {}

  using SG::ArenaBlockAllocatorBase::getBlock;
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
  std::cout << "test1\n";

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
  bab.reserve (2000);
  const size_t nelt3 = Payload::v.size();
  assert (nelt3 >= 2000);
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


jmp_buf jmp ATLAS_THREAD_SAFE;
void handler (int)
{
  siglongjmp (jmp, 1);
}
void setsig()
{
  struct sigaction act;
  act.sa_handler = handler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction (SIGSEGV, &act, nullptr) != 0) std::abort();
}
void resetsig()
{
  struct sigaction act;
  act.sa_handler = SIG_DFL;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction (SIGSEGV, &act, nullptr) != 0) std::abort();
  sigset_t sigs;
  if (sigemptyset (&sigs) != 0) std::abort();
  if (sigaddset (&sigs, SIGSEGV) != 0) std::abort();
  if (sigprocmask (SIG_UNBLOCK, &sigs, nullptr) != 0) std::abort();
}

template <typename CALLABLE>
void expect_signal (CALLABLE code)
{
  // volatile to avoid gcc -Wclobbered warning.
  volatile bool handled = false;
  if (sigsetjmp (jmp, 0)) {
    handled = true;
  }
  else {
    setsig();
    code();
  }
  resetsig();
  assert (handled);
}


// Test protect().
void test2()
{
  std::cout << "test2\n";
  TestAlloc bab
    (SG::ArenaAllocatorBase::initParams<Payload, true> (100, "foo"));
  while (bab.stats().elts.total < 1000) {
    bab.getBlock();
  }
  payload (bab.m_blocks, 500).x = 42;
  bab.protect();
  expect_signal ([&]() { payload (bab.m_blocks, 500).x = 43; });

  EXPECT_EXCEPTION( SG::ExcProtected, bab.reserve (500) );
  EXPECT_EXCEPTION( SG::ExcProtected, bab.erase() );
  EXPECT_EXCEPTION( SG::ExcProtected, bab.getBlock() );

  bab.unprotect();
  assert (payload (bab.m_blocks, 500).x == 42);
  payload (bab.m_blocks, 500).x = 43;
  assert (payload (bab.m_blocks, 500).x == 43);
}


int main()
{
  std::cout << "AthAllocators/ArenaBlockAllocatorBase_test\n";
  test1();
  test2();
  return 0;
}
