/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBaseAllocT_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHandleBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHandleBaseAllocT.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHandleBaseAllocT.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cassert>

class TestAlloc
  : public SG::ArenaAllocatorBase
{
public:
  TestAlloc (const Params& params) : m_params (params) {}
  virtual void reset() {}
  virtual void erase() {}
  virtual void reserve(size_t) {}
  virtual const std::string& name() const { return m_params.name; }
  virtual const Stats& stats() const { return m_stats; }
  const Params& params() const { return m_params; }
  int foo() { return 42; }

  static SG::ArenaAllocatorBase* makeAllocator (const Params& params)
  { return new TestAlloc (params); }

private:
  Stats m_stats;
  Params m_params;
};

class TestHandle
  : public SG::ArenaHandleBaseAllocT<TestAlloc>
{
public:
  typedef SG::ArenaHandleBaseAllocT<TestAlloc> Base;
  TestHandle (SG::ArenaHeader* header, size_t index)
    : Base (header, index) {}
  TestHandle (SG::ArenaHeader* header, const Creator& creator)
    : Base (header, creator) {}
  int foo() { return allocator()->foo(); }
};

void test1()
{
  SG::ArenaHeader head;
  typedef typename TestAlloc::initParams<int> defaultParams_t;
  TestAlloc::Params params = defaultParams_t (1000, "foo");
  TestHandle hand (&head, TestHandle::Creator (static_cast<TestAlloc*>(0),
                                               params));
  assert (hand.params().name == "foo");

  TestHandle hand2 (&head, 0);
  assert (hand2.foo() == 42);
}

int main()
{
  test1();
  return 0;
}

