/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file  AthenaKernel/test/RCUUpdater_test.cxx
 * @author scott snyder
 * @date Nov, 2017
 * @brief Unit test for RCUUpdater.
 */


#include "AthenaKernel/RCUUpdater.h"
#include <vector>
#include <cassert>
#include <iostream>


static const int nslots = 2;


class TestRCUSvc
  : public Athena::IRCUSvc
{
public:
  virtual StatusCode remove (Athena::IRCUObject* obj) override
  {
    m_removed = obj;
    return StatusCode::SUCCESS;
  }
  virtual size_t getNumSlots() const override
  { return nslots; }
  virtual void add (Athena::IRCUObject* /*obj*/) override
  { }

  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override { std::abort(); }

  Athena::IRCUObject* m_removed = nullptr;
};


class Payload
{
public:
  typedef std::vector<int> vec_t;
  Payload (vec_t& vec, int x) : m_vec(vec), m_x (x)
  { vec.push_back (x); }
  ~Payload()
  {
    vec_t::iterator i = std::find (m_vec.begin(), m_vec.end(), m_x);
    if (i != m_vec.end()) m_vec.erase (i);
  }
  vec_t& m_vec;
  int m_x;
};


void test1()
{
  std::cout << "test1\n";

  Payload::vec_t vec;
  {
    TestRCUSvc rcusvc;
    typedef Athena::RCUUpdater<Payload> Updater_t;
    Updater_t u (rcusvc, vec, 10);
    assert (u.get().m_x == 10);
    assert (vec.size() == 1);

    EventContext c0 (0, 0);
    EventContext c1 (0, 1);
    u.update (std::make_unique<Payload> (vec, 20), c0);
    assert (u.get().m_x == 20);
    assert (vec.size() == 2);

    Gaudi::Hive::setCurrentContext (c1);

    u.quiescent (c0);
    assert (vec.size() == 2);
    u.quiescent (Updater_t::defaultContext());
    assert (vec.size() == 1);

    u.discard (std::make_unique<Payload> (vec, 30));
    assert (u.get().m_x == 20);
    assert (vec.size() == 2);

    u.quiescent (c0);
    assert (vec.size() == 2);
    u.quiescent (c1);
    assert (vec.size() == 1);
    assert (u.get().m_x == 20);
  }
  assert (vec.size() == 0);
}


int main()
{
  test1();
  return 0;
}
