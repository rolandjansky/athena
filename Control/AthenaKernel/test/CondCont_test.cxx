/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaKernel/test/CondCont_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Tests for CondCont.
 */


#undef NDEBUG
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <cassert>
#include <iostream>


namespace SG {
class DataProxy {};
}


class B
{
public:
  B(int x) : m_x(x) {}
  int m_x;
};


class Pad
{
public:
  int m_pad = 0;
};


class D
  : public Pad, public B
{
public:
  D(int x) : B(x*10) {}
};


CONDCONT_BASE(D, B);


CLASS_DEF(CondCont<B>, 932847546, 0)
CLASS_DEF(CondCont<D>, 932847547, 0)


EventIDBase runlbn (int run, int lbn)
{
  return EventIDBase (run,
                      EventIDBase::UNDEFEVT,  // event
                      EventIDBase::UNDEFNUM,  // timestamp
                      EventIDBase::UNDEFNUM,  // timestamp ns
                      lbn);
}


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


const EventIDRange r1 (runlbn (10, 15), runlbn (10, 20));
const EventIDRange r2 (runlbn (20, 17), runlbn (EventIDBase::UNDEFNUM/2, EventIDBase::UNDEFNUM/2));
const EventIDRange r3 (timestamp (123), timestamp (456));


template <class T>
void fillit (CondCont<T>& cc, std::vector<T*> & ptrs)
{
  assert (cc.entries() == 0);
  assert (cc.ranges().empty());
  cc.setProxy (nullptr);
  assert (cc.proxy() == nullptr);
  std::ostringstream ss1;
  cc.list (ss1);
  assert (ss1.str() == "id: 'key'  proxy: 0\n\
clock: [0]\n\
RE: [0]\n");

  assert (cc.clid() == ClassID_traits<CondCont<T> >::ID());

  ptrs.push_back (new T(1));
  ptrs.push_back (new T(2));
  ptrs.push_back (new T(3));

  assert( cc.typelessInsert (r1, ptrs[0]) );
  assert( cc.typelessInsert (r2, ptrs[1]) );
  assert( cc.insert (r3, std::unique_ptr<T> (ptrs[2])) );
  assert (cc.entries() == 3);
  std::ostringstream ss2;
  cc.list (ss2);
  std::ostringstream exp2;
  exp2 << "id: 'key'  proxy: 0\n"
       << "clock: [1]\n"
       << "{[4294967295,t:123] - [4294967295,t:456]} " << ptrs[2] << "\n"
       << "RE: [2]\n"
       << "{[20,l:17] - [2147483647,l:2147483647]} " << ptrs[1] << "\n"
       << "{[10,l:15] - [10,l:20]} " << ptrs[0] << "\n";

  //  std::cout << "ss2: " << ss2.str() << "\nexp2: " << exp2.str() << "\n";

  assert (ss2.str() == exp2.str());

  auto t4 = std::make_unique<T> (4);
  assert( ! cc.insert (EventIDRange (runlbn (40, 2), timestamp (543)), std::move(t4)) );
}


template <class T>
void checkit (const CondCont<T>& cc, const std::vector<T*>& ptrs)
{
  std::vector<EventIDRange> rvec = cc.ranges();
  assert (rvec.size() == 3);
  assert (rvec[0] == r2);
  assert (rvec[1] == r1);
  assert (rvec[2] == r3);

  assert (cc.valid (runlbn (10, 17)));
  assert (cc.valid (runlbn (100, 200)));
  assert (cc.valid (timestamp (234)));
  assert (!cc.valid (runlbn (15, 17)));
  assert (!cc.valid (timestamp (999)));

  const T* t = nullptr;
  assert (cc.find (runlbn (10, 17), t));
  assert (t == ptrs[0]);

  t = nullptr;
  assert (cc.find (runlbn (100, 200), t));
  assert (t == ptrs[1]);

  EventIDRange r;
  t = nullptr;
  assert (cc.find (timestamp (234), t, &r));
  assert (t == ptrs[2]);
  assert (r == r3);

  assert (!cc.find (runlbn (15, 17), t));
  assert (!cc.find (timestamp (999), t));

  assert (cc.range (runlbn (100, 200), r));
  assert (r == r2);
  assert (cc.range (timestamp (234), r));
  assert (r == r3);
  assert (!cc.range (runlbn (15, 17), r));
  assert (!cc.range (timestamp (999), r));
}


void test1()
{
  std::cout << "test1\n";
  SG::DataProxy proxy;
  DataObjID id ("key");
  CondCont<B> cc (id, &proxy);
  assert (cc.proxy() == &proxy);
  assert (cc.id() == id);

  std::vector<B*> bptrs;
  fillit (cc, bptrs);
  checkit (cc, bptrs);
}


void test2()
{
  std::cout << "test2\n";
  SG::DataProxy proxy;
  DataObjID id ("key");
  CondCont<D> cc (id, &proxy);
  assert (cc.proxy() == &proxy);
  assert (cc.id() == id);

  std::vector<D*> dptrs;
  fillit (cc, dptrs);
  checkit (cc, dptrs);

  CondCont<B> &bcc = cc;
  std::vector<B*> bptrs (dptrs.begin(), dptrs.end());
  checkit (bcc, bptrs);

  auto b4 = std::make_unique<B> (4);
  assert( ! bcc.insert (EventIDRange (runlbn (40, 2), timestamp (543)), std::move(b4)) );
}


int main()
{
  // Verify that B is indeed offset in D, so we get to test a nontrivial conversion.
  {
    D d(0);
    B& bd = d;
    assert (reinterpret_cast<char*>(&d) != reinterpret_cast<char*>(&bd));
  }

  test1();
  test2();
  return 0;
}
