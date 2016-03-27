/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaAllocatorRegistry_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaAllocatorRegistry_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaAllocatorRegistry.
 */

#undef NDEBUG
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include <cassert>

class Alloc
  : public SG::ArenaAllocatorBase
{
public:
  Alloc(int x) { m_stats.elts.total = x; }
  virtual void reset() {}
  virtual void erase() {}
  virtual void reserve (size_t /*size*/) {}
  virtual const std::string& name() const { return m_name; }
  virtual const SG::ArenaAllocatorBase::Stats& stats() const
  { return m_stats; }
private:
  SG::ArenaAllocatorBase::Stats m_stats;
  std::string m_name;
};

class Creator
  : public SG::ArenaAllocatorCreator
{
public:
  Creator (int x) : m_x (x) {}
  virtual SG::ArenaAllocatorBase* create() { return new Alloc (m_x); }
private:
  int m_x;
};

void test1()
{
  SG::ArenaAllocatorRegistry* reg =
    SG::ArenaAllocatorRegistry::instance();
  assert (reg->lookup ("foo") == std::string::npos);
  assert (reg->registerCreator ("foo", new Creator (0)) == 0);
  assert (reg->registerCreator ("bar", new Creator (1)) == 1);
  assert (reg->lookup ("foo") == 0);
  assert (reg->lookup ("bar") == 1);
  assert (reg->create(0)->stats().elts.total == 0);
  assert (reg->create(1)->stats().elts.total == 1);
}

int main()
{
  test1();
  return 0;
}
