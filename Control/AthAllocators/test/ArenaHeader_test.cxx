/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeader_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHeader_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHeader.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include "AthAllocators/ArenaBase.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


std::string reset;
class TestAlloc
  : public SG::ArenaAllocatorBase
{
public:
  TestAlloc (int x, std::string  name)
    : m_name (std::move(name)) { m_stats.bytes.total = x; }
  virtual void reset() override { ::reset = m_name; }
  virtual void erase() override {}
  virtual void reserve (size_t /*size*/) override {}
  virtual SG::ArenaAllocatorBase::Stats stats() const override
  { return m_stats; }
  virtual const std::string& name() const override { return m_name; }
private:
  SG::ArenaAllocatorBase::Stats m_stats;
  std::string m_name;
};

class Creator
  : public SG::ArenaAllocatorCreator
{
public:
  Creator (int x, const char* name) : m_x (x), m_name (name) {}
  virtual std::unique_ptr<SG::ArenaAllocatorBase> create() override
  { return std::make_unique<TestAlloc> (m_x, m_name); }
private:
  int m_x;
  std::string m_name;
};


#if 0
class TestArena
  : public SG::ArenaBase
{
public:
  TestArena (std::string  name, int x) : m_name (std::move(name)), m_x (x) {}
  virtual void report (std::ostream& os) const override
  { os << "foo " << m_x << "\n"; }
  virtual const std::string& name() const override { return m_name; }
private:
  std::string m_name;
  int m_x;
};
#endif


void test1()
{
  SG::ArenaAllocatorRegistry* reg =
    SG::ArenaAllocatorRegistry::instance();
  assert (reg->registerCreator ("a", std::make_unique<Creator>(0, "a")) == 0);
  assert (reg->registerCreator ("b", std::make_unique<Creator>(1, "b")) == 1);

  SG::ArenaBase a1;
  SG::ArenaBase a2;
  SG::ArenaHeader* head = SG::ArenaHeader::defaultHeader();
  assert (head->setArena (&a1) == nullptr);
  assert (head->allocator (0)->name() == "a");
  assert (head->setArena (&a2) == &a1);
  assert (head->allocator (1)->name() == "b");

  {
    std::ostringstream os;
    a1.report (os);
    assert (os.str() == "Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  a\n");
  }
  {
    std::ostringstream os;
    a2.report (os);
    assert (os.str() == "Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      1       0/      0/      0  b\n");
  }
  
  head->reset();
  assert (reset == "b");

  assert (head->setArena (nullptr) == &a2);
  size_t i = reg->registerCreator ("foo", std::make_unique<Creator> (1, "foo"));
  assert (i == 2);
  assert (head->allocator (i)->stats().bytes.total == 1);

  head->setArenaForSlot (1, &a1);
  head->setArenaForSlot (2, &a2);
  EventContext ctx1 (0, 1);
  EventContext ctx2 (0, 2);
  SG::ArenaAllocatorBase* a1_0 = a1.allocator (0).get();
  SG::ArenaAllocatorBase* a2_1 = a2.allocator (1).get();
  assert (head->allocator (ctx1, 0).get() == a1_0);
  assert (head->allocator (ctx2, 1).get() == a2_1);

  EventContext ctxx (0, 0);
  SG::ArenaAllocatorBase* ax_2 = head->allocator (i).get();
  assert (head->allocator (ctxx, i).get() == ax_2);
}


void test2()
{
  SG::ArenaHeader head;
  SG::ArenaBase a1 ("a1");
  SG::ArenaBase a2 ("a2");
  head.addArena (&a1);
  head.addArena (&a2);
  a1.allocator (0);
  a2.allocator (1);
  std::string s = head.reportStr();
  assert (s == "=== a1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      0       0/      0/      0  a\n\
=== a2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      1       0/      0/      0  b\n\
=== default ===\n");

  head.delArena (&a1);
  assert (head.allocator(1)->stats().bytes.total == 1);
  s = head.reportStr();
  assert (s == "=== a2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      1       0/      0/      0  b\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      1       0/      0/      0  b\n");

}


int main()
{
  test1();
  test2();
  return 0;
}
