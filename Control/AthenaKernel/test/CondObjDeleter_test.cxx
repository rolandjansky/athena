/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/CondObjDeleter_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2022
 * @brief Regression tests for CondObjDeleter.
 */

#undef NDEBUG

#include "AthenaKernel/CondObjDeleter.h"
#include "AthenaKernel/IRCUSvc.h"
#include "AthenaKernel/RCUObject.h"
#include <unordered_set>
#include <cassert>
#include <iostream>


using Athena::IRCUSvc;
using Athena::IRCUObject;
using Athena::CondObjDeleter;


const int nslots = 4;


class TestRCUSvc
  : public IRCUSvc
{
public:
  virtual StatusCode remove (IRCUObject* obj) override
  {
    m_removed = obj;
    return StatusCode::SUCCESS;
  }
  virtual size_t getNumSlots() const override
  { return nslots; }
  virtual void add (IRCUObject* /*obj*/) override
  { }

  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override { std::abort(); }

  IRCUObject* m_removed = nullptr;
};


struct Payload
{
  typedef std::unordered_set<int> Hist;
  Payload (int x, Hist* hist = nullptr)
    : m_x(x), m_hist (hist) { if (m_hist) m_hist->insert (x); }
  ~Payload()
  {
    if (m_hist) {
      assert (m_hist->count (m_x) == 1);
      m_hist->erase (m_x);
    }
  }
  int m_x;
  Hist* m_hist;
};


void test1()
{
  std::cout << "test1\n";
  TestRCUSvc svc;
  Payload::Hist phist;
  {
    CondObjDeleter<Payload> deleter (svc);
    deleter.discard (new Payload (1, &phist));
    assert (phist.size() == 1);
    deleter.quiescent (EventContext (0, 0));
    deleter.quiescent (EventContext (0, 1));
    assert (phist.size() == 1);
    deleter.discard (new Payload (2, &phist));
    assert (phist.size() == 2);
    deleter.quiescent (EventContext (0, 2));
    deleter.quiescent (EventContext (0, 3));
    assert (phist.size() == 1);
    deleter.quiescent (EventContext (0, 0));
    deleter.quiescent (EventContext (0, 1));
    assert (phist.size() == 0);

    Payload* p = new Payload (3, &phist);
    assert (phist.size() == 1);
    deleter.delfcn() (p);
    assert (phist.size() == 0);

    deleter.discard (new Payload (4, &phist));
    assert (phist.size() == 1);
    deleter.quiescent (EventContext (0, 0));
    deleter.quiescent (EventContext (0, 1));
    deleter.discard (new Payload (5, &phist));
    assert (phist.size() == 2);
  }
  assert (phist.size() == 0);
}


int main()
{
  std::cout << "AthenaKernel/CondObjDeleter_test\n";
  test1();
  return 0;
}
