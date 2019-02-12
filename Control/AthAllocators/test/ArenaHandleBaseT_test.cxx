/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBaseT_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHandleBaseT_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHandleBaseT.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHandleBaseT.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cassert>
#include <iterator>
#include <atomic>


class Payload
{
public:
  Payload (int i) : m_x (i) {}
  Payload& operator= (const Payload&) = default;
  int m_x;
};


class TestAlloc
  : public SG::ArenaAllocatorBase
{
public:
  TestAlloc (Params  params);
  virtual void reset() override {}
  virtual void erase() override {}
  virtual void reserve(size_t) override {}
  virtual const std::string& name() const override { return m_params.name; }
  virtual Stats stats() const override { return m_stats; }
  const Params& params() const { return m_params; }
  int foo() { return 42; }
  void free (pointer p) { ptmp = p; }
  void resetTo (pointer p) { ptmp = p+1; }
  static std::atomic<pointer> ptmp;

  class const_iterator;

  class iterator
    : public std::iterator<std::forward_iterator_tag, Payload>
  {
  public:
    iterator (std::vector<Payload>::iterator it)
      : m_it (std::move(it)) {}
    reference operator*() const { return *m_it; }
    iterator& operator++() { ++m_it; return *this; }
    bool operator== (const iterator& other) const
    { return m_it == other.m_it; }

    friend class const_iterator;

  private:
    std::vector<Payload>::iterator m_it;
  };

  class const_iterator
    : public std::iterator<std::forward_iterator_tag, const Payload>
  {
  public:
    const_iterator (std::vector<Payload>::const_iterator it)
      : m_it (std::move(it)) {}
    const_iterator (TestAlloc::iterator it)
      : m_it (&*it) {}
    reference operator*() const { return *m_it; }
    const_iterator& operator++() { ++m_it; return *this; }
    bool operator== (const const_iterator& other) const
    { return m_it == other.m_it; }
    bool operator== (const TestAlloc::iterator& other) const
    { return m_it == other.m_it; }

  private:
    std::vector<Payload>::const_iterator m_it;
  };

  iterator begin()  { return iterator (m_vec.begin()); }
  iterator end()    { return iterator (m_vec.end()); }
  const_iterator begin() const  { return const_iterator (m_vec.begin()); }
  const_iterator end() const    { return const_iterator (m_vec.end()); }

  static std::unique_ptr<SG::ArenaAllocatorBase>
  makeAllocator (const Params& params)
  { return std::make_unique<TestAlloc> (params); }

private:
  std::vector<Payload> m_vec;
  Stats m_stats;
  Params m_params;
};


TestAlloc::TestAlloc (Params  params)
  : m_stats(),
    m_params (std::move(params))
{
  for (int i = 0; i < 10; i++)
    m_vec.push_back (i);
}

std::atomic<TestAlloc::pointer> TestAlloc::ptmp;

class TestHandle
  : public SG::ArenaHandleBaseT<Payload, TestAlloc>
{
public:
  typedef SG::ArenaHandleBaseT<Payload, TestAlloc> Base;
  using Base::Base;
  using Base::makeIndex;
  int foo() { return allocator()->foo(); }
};

void test1()
{
  typedef typename TestAlloc::initParams<int> defaultParams_t;
  SG::ArenaHeader head;
  TestAlloc::Params params;
  params.name = "foo";

  // These are never used, but fully initialize anyway in order to avoid
  // ubsan errors.
  params.eltSize = 0;
  params.minSize = 0;
  params.nblock = 0;
  params.linkOffset = 0;
  params.constructor = nullptr;
  params.destructor = nullptr;
  params.clear = nullptr;
  params.canReclear = false;
  params.mustClear = false;

  size_t i0 = TestHandle::makeIndex<TestAlloc, defaultParams_t> (&params);
  {
    TestHandle hand (&head, i0);
    assert (hand.params().name == "foo");
    TestHandle::pointer p = reinterpret_cast<TestHandle::pointer>(0x1234);
    hand.free (p);
    assert (TestAlloc::ptmp == reinterpret_cast<TestAlloc::pointer>(p));
    hand.resetTo (p);
    assert (TestAlloc::ptmp == reinterpret_cast<TestAlloc::pointer>(p)+1);
  }

  TestHandle hand2 (&head, 0);
  int i = 0;
  for (TestHandle::iterator it = hand2.begin();
       it != hand2.end();
       ++it)
  {
    assert ((*it).m_x == i);
    assert (it->m_x == i);
    ++i;
  }

  i = 0;
  const TestHandle& chand = hand2;
  for (TestHandle::const_iterator it = chand.begin();
       it != chand.end();
       ++it)
  {
    assert ((*it).m_x == i);
    assert (it->m_x == i);
    ++i;
  }

  i = 0;
  for (TestHandle::const_iterator it = hand2.begin();
       it != hand2.end();
       ++it)
  {
    assert ((*it).m_x == i);
    assert (it->m_x == i);
    ++i;
  }
}


void
test2()
{
  typedef typename TestAlloc::initParams<int> defaultParams_t;
  SG::ArenaHeader head;

  SG::ArenaBase a1 ("1");
  SG::ArenaBase a2  ("2");
  head.addArena (&a1);
  head.addArena (&a2);
  head.setArenaForSlot (1, &a1);
  head.setArenaForSlot (2, &a2);

  TestAlloc::Params params;

  // These are never used, but fully initialize anyway in order to avoid
  // ubsan errors.
  params.eltSize = 0;
  params.minSize = 0;
  params.nblock = 0;
  params.linkOffset = 0;
  params.constructor = nullptr;
  params.destructor = nullptr;
  params.clear = nullptr;
  params.canReclear = false;
  params.mustClear = false;

  params.name = "bar";
  size_t i1 = TestHandle::makeIndex<TestAlloc, defaultParams_t> (&params);
  {
    TestHandle hand (&head, EventContext (0, 1), i1);
    assert (hand.params().name == "bar");
  }

  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
=== 2 ===\n\
=== default ===\n");

  params.name = "fee";
  size_t i2 = TestHandle::makeIndex<TestAlloc, defaultParams_t> (&params);

  {
    TestHandle hand (&a2, i2);
    assert (hand.params().name == "fee");
  }

  assert (head.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  bar\n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  fee\n\
=== default ===\n");
}


int main()
{
  test1();
  test2();
  return 0;
}
