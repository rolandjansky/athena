/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthAllocators/test/LockedAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Regression tests for LockedAllocator.
 */

#undef NDEBUG
#include "AthAllocators/LockedAllocator.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cassert>
#include <iostream>


const std::string foo ("foo");
class TestAllocator
  : public SG::ArenaAllocatorBase
{
public:
  virtual void reset() override {}
  virtual void erase() override {}
  virtual void reserve(size_t) override {}
  virtual Stats stats() const override { return Stats(); }
  virtual const std::string& name() const override { return foo; }
  virtual void report (std::ostream&) const override {}
};


void test1a (SG::LockedAllocator l)
{
  assert (l->name() == "foo");
}


void test1()
{
  std::cout << "test1\n";

  TestAllocator a;
  std::mutex m;

  {
    SG::LockedAllocator l (&a, m);
    assert (l.get() == &a);
    const SG::LockedAllocator& cl = l;
    assert (cl.get() == &a);
    assert (&*l == &a);
    assert (l->name() == "foo");
    test1a (std::move (l));
  }
}


int main()
{
  test1();
  return 0;
}
