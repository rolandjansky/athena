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
#include <cassert>
#include <ostream>

std::string reset;
class TestAlloc
  : public SG::ArenaAllocatorBase
{
public:
  TestAlloc (int x, const std::string& name)
    : m_name (name) { m_stats.bytes.total = x; }
  virtual void reset() { ::reset = m_name; }
  virtual void erase() {}
  virtual void reserve (size_t /*size*/) {}
  virtual const SG::ArenaAllocatorBase::Stats& stats() const
  { return m_stats; }
  virtual const std::string& name() const { return m_name; }
private:
  SG::ArenaAllocatorBase::Stats m_stats;
  std::string m_name;
};

class Creator
  : public SG::ArenaAllocatorCreator
{
public:
  Creator (int x) : m_x (x) {}
  virtual SG::ArenaAllocatorBase* create()
  { return new TestAlloc (m_x, "creat"); }
private:
  int m_x;
};


class TestArena
  : public SG::ArenaBase
{
public:
  TestArena (const std::string& name, int x) : m_name (name), m_x (x) {}
  virtual void report (std::ostream& os) const
  { os << "foo " << m_x << "\n"; }
  virtual const std::string& name() const { return m_name; }
private:
  std::string m_name;
  int m_x;
};


void test1()
{
  SG::ArenaHeader::ArenaAllocVec_t vec1;
  vec1.resize (10);
  vec1[2] = new TestAlloc (0, "a");
  SG::ArenaHeader::ArenaAllocVec_t vec2;
  vec2.resize (10);
  vec2[5] = new TestAlloc (1, "b");

  SG::ArenaHeader* head = SG::ArenaHeader::defaultHeader();
  assert (head->setAllocVec(&vec1) == 0);
  assert (head->allocator(2) == vec1[2]);
  assert (head->setAllocVec(&vec2) == &vec1);
  assert (head->allocator(5) == vec2[5]);
  head->reset();
  assert (reset == "b");

  SG::ArenaHeader::ArenaAllocVec_t vec3;
  assert (head->setAllocVec(0) == &vec2);
  SG::ArenaAllocatorRegistry* reg =
    SG::ArenaAllocatorRegistry::instance();
  size_t i = reg->registerCreator ("foo", new Creator (1));
  assert (i == 0);
  assert (head->allocator(i)->stats().bytes.total == 1);
}


void test2()
{
  SG::ArenaHeader head;
  TestArena a1 ("a1", 1);
  TestArena a2 ("a2", 2);
  head.addArena (&a1);
  head.addArena (&a2);
  std::string s = head.reportStr();
  assert (s == "=== a1 ===\n\
foo 1\n\
=== a2 ===\n\
foo 2\n");
  head.delArena (&a1);
  assert (head.allocator(0)->stats().bytes.total == 1);
  s = head.reportStr();
  assert (s == "=== a2 ===\n\
foo 2\n\
=== default ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       0/      0/      1       0/      0/      0  creat\n");

}


int main()
{
  test1();
  test2();
  return 0;
}
