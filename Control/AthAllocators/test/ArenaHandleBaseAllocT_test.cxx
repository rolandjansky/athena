/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  TestAlloc (Params  params) : m_stats(), m_params (std::move(params)) {}
  virtual void reset() override {}
  virtual void erase() override {}
  virtual void reserve(size_t) override {}
  virtual const std::string& name() const override { return m_params.name; }
  virtual Stats stats() const override { return m_stats; }
  const Params& params() const { return m_params; }
  int foo() { return 42; }

  static std::unique_ptr<SG::ArenaAllocatorBase>
  makeAllocator (const Params& params)
  { return std::make_unique<TestAlloc> (params); }

private:
  Stats m_stats;
  Params m_params;
};

class TestHandle
  : public SG::ArenaHandleBaseAllocT<TestAlloc>
{
public:
  typedef SG::ArenaHandleBaseAllocT<TestAlloc> Base;
  using Base::Base;
  using Base::makeIndex;
  int foo() { return allocator()->foo(); }
};


void test1()
{
  SG::ArenaHeader head;
  typedef typename TestAlloc::initParams<int> defaultParams_t;

  TestAlloc::Params params0 = defaultParams_t (1000, "foo");
  size_t i0 = TestHandle::makeIndex<TestAlloc, defaultParams_t> (&params0);

  {
    TestHandle hand (&head, i0);
    assert (hand.params().name == "foo");
    assert (hand.foo() == 42);
  }

  SG::ArenaBase a1 ("1");
  SG::ArenaBase a2  ("2");
  head.addArena (&a1);
  head.addArena (&a2);
  head.setArenaForSlot (1, &a1);
  head.setArenaForSlot (2, &a2);

  assert (head.reportStr() == "\
=== 1 ===\n\
=== 2 ===\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  foo\n");

  TestAlloc::Params params1 = defaultParams_t (1000, "bar");
  size_t i1 = TestHandle::makeIndex<TestAlloc, defaultParams_t> (&params1);

  {
    TestHandle hand (&head, EventContext (0, 1), i1);
    assert (hand.params().name == "bar");
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
=== 2 ===\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  foo\n");

  {
    TestHandle hand (&head, EventContext (0, 2), i1);
    assert (hand.foo() == 42);
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  foo\n");

  TestAlloc::Params params2 = defaultParams_t (1000, "fee");
  size_t i2 = TestHandle::makeIndex<TestAlloc, defaultParams_t> (&params2);
  {
    TestHandle hand (&a1, i2);
    assert (hand.params().name == "fee");
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
       0/      0/      0       0/      0/      0       0/      0/      0  fee\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  foo\n");

  {
    TestHandle hand (&a2, i2);
    assert (hand.foo() == 42);
  }
  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
       0/      0/      0       0/      0/      0       0/      0/      0  fee\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
       0/      0/      0       0/      0/      0       0/      0/      0  fee\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  foo\n");
}


int main()
{
  test1();
  return 0;
}

