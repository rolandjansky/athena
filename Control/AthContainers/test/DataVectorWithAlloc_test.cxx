/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/test/DataVectorWithAlloc_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2021
 * @brief Tests for DataVectorWithAlloc.
 *
 * The DV-like functionality is tested in DataVector_test.icc.
 * This just tests extra functionality.
 */

#undef NDEBUG
#include "AthContainers/DataVectorWithAlloc.h"
#include "AthContainers/exceptions.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <cassert>
#include <setjmp.h>
#include <signal.h>


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


void test1()
{
  std::cout << "test1\n";
  using Vec = DataVectorWithAlloc<DataVector<int> >;
  Vec v (Vec::elt_allocator_type (100));
  Vec::Ptr p = v.allocate();
  v.push_back (std::move (p));
  assert (v.elt_allocator().stats().elts.inuse == 1);
  *v.front() = 42;
  assert (*v.front() == 42);
  v.elt_allocator().protect();
  expect_signal ([&]() { *v.front() = 43; });
  assert (*v.front() == 42);
  v.elt_allocator().unprotect();
  *v.front() = 43;
  assert (*v.front() == 43);
  v.elt_allocator().protect();
}


void test2()
{
  std::cout << "test2\n";

  using Vec = DataVectorWithAlloc<DataVector<int> >;
  Vec v1;
  Vec v2;
  v1.push_back (v1.allocate(1));
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1.push_back (v2.allocate(2)) );

  v1.emplace_back (v1.allocate(1));
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1.emplace_back (v2.allocate(2)) );

  v1.insert (v1.begin(), v1.allocate(1));
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1.insert (v1.begin(), v2.allocate(2)) );

  v1.emplace (v1.begin(), v1.allocate(1));
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1.emplace (v1.begin(), v2.allocate(2)) );

  Vec::Ptr p;

  v1.swapElement (0, v1.allocate(3), p);
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1.swapElement (0, v2.allocate(6), p) );

  v1.swapElement (v1.begin(), v1.allocate(3), p);
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1.swapElement (v1.begin(), v2.allocate(7), p) );

  v1[0] = v1.allocate(8);
  EXPECT_EXCEPTION( SG::ExcAllocOwnership, v1[0] = v2.allocate(9) );
}


// Test using a const type.
void test3()
{
  std::cout << "test3\n";

  using Vec = DataVectorWithAlloc<DataVector<const int> >;
  Vec v1;
  v1.push_back (v1.allocate(1));
}


void test4()
{
  std::cout << "test4\n";

  using Vec = DataVectorWithAlloc<DataVector<const int> >;
  auto v = Vec::make_unique();
  v->push_back (v->allocate (10));
  std::unique_ptr<const DataVector<const int> > v2 = std::move (v);
  assert (!v);
  assert (v2->size() == 1);
  assert (*(*v2)[0] == 10);
}


int main()
{
  std::cout << "AthContainers/DataVectorWithAlloc_test\n";
  test1();
  test2();
  test3();
  test4();
  return 0;
}
