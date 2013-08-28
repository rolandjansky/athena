/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeaderGaudiClear_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHeaderGaudiClear_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2007
 * @brief Regression tests for ArenaHeaderGaudiClear.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHeaderGaudiClear.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cassert>


int nreset = 0;
class TestAlloc
  : public SG::ArenaAllocatorBase
{
public:
  virtual void reset() { ++nreset; }
  virtual void erase() {}
  virtual void reserve (size_t /*size*/) { }
  virtual const ArenaAllocatorBase::Stats& stats() const
  { return m_stats; }
  virtual const std::string& name() const { return m_name; }
  ArenaAllocatorBase::Stats m_stats;
  std::string m_name;
};


int ndel = 0;
class TestHeader
  : public SG::ArenaHeaderGaudiClear
{
public:
  virtual ~TestHeader() { ++ndel; }
};


void test1()
{
  ndel = nreset = 0;

  SG::ArenaHeaderGaudiClear* h = new TestHeader;
  h->initialize();

  SG::ArenaHeader::ArenaAllocVec_t vec;
  vec.push_back (new TestAlloc);
  h->setAllocVec (&vec);
  assert (nreset == 0);
  h->handle (Incident ("test", "BeginEvent"));
  assert (nreset == 1);
  h->handle (Incident ("test", "EndEvent"));
  assert (nreset == 1);
  h->handle (Incident ("test", "BeginEvent"));
  assert (nreset == 2);

  h->addRef();
  assert (ndel == 0);
  h->release();
  assert (ndel == 1);
}


void test2()
{
  ndel = nreset = 0;
  SG::ArenaHeaderGaudiClear* h = new TestHeader;
  SG::ArenaHeaderGaudiClear::disable();
  h->initialize();

  assert (nreset == 0);
  h->handle (Incident ("test", "BeginEvent"));
  assert (nreset == 0);

  h->addRef();
  assert (ndel == 0);
  h->release();
  assert (ndel == 1);
}


int main()
{
  test1();
  test2();
  return 0;
}
