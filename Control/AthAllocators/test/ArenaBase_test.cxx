/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBase_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaBase.
 */

#undef NDEBUG
#include "AthAllocators/ArenaBase.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include <ostream>
#include <sstream>
#include <atomic>
#include <cassert>


std::atomic<int> xxx;
class Alloc
  : public SG::ArenaAllocatorBase
{
public:
  Alloc(int x);
  virtual void reset() override { xxx += m_x; }
  virtual void erase() override { xxx += 2*m_x; }
  virtual void reserve (size_t /*size*/) override {}
  virtual SG::ArenaAllocatorBase::Stats stats() const override
  { return m_stats; }
  virtual const std::string& name() const override { return m_name; }
private:
  std::string m_name;
  int m_x;
  SG::ArenaAllocatorBase::Stats m_stats;
};

Alloc::Alloc (int x)
  : m_x (x)
{
  m_stats.bytes.inuse = x;
  m_stats.bytes.free = 2*x;
  m_stats.bytes.total = 3*x;

  std::ostringstream os;
  os << "alloc" << x;
  m_name = os.str();
}

class Creator
  : public SG::ArenaAllocatorCreator
{
public:
  Creator (int x) : m_x (x) {}
  virtual std::unique_ptr<SG::ArenaAllocatorBase> create() override
  { return std::make_unique<Alloc> (m_x); }
private:
  int m_x;
};


void test1()
{
  SG::ArenaBase a ("a");

  assert (a.name() == "a");

  SG::ArenaAllocatorRegistry* reg =
    SG::ArenaAllocatorRegistry::instance();
  assert (reg->registerCreator ("1", std::make_unique<Creator>(1)) == 0);
  assert (reg->registerCreator ("2", std::make_unique<Creator>(2)) == 1);
  SG::ArenaAllocatorBase* a0 = a.allocator(0).get();
  assert (a.allocator(0).get() == a0);
  assert (a.allocator(0)->stats().bytes.inuse == 1);
  assert (a.allocator(1)->stats().bytes.inuse == 2);

  assert (a.stats().bytes.inuse == 3);
  assert (a.stats().bytes.free  == 6);
  assert (a.stats().bytes.total == 9);

  std::ostringstream os;
  a.report (os);
  assert (os.str() == "Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/      0/      0       1/      2/      3       0/      0/      0  alloc1\n\
       0/      0/      0       2/      4/      6       0/      0/      0  alloc2\n");

  assert (xxx == 0);
  a.reset();
  assert (xxx == 3);
  a.erase();
  assert (xxx == 9);
}


int main()
{
  test1();
  return 0;
}

