/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaSharedHeapSTLAllocator_test.cxx 470825 2011-11-25 23:20:57Z ssnyder $
/**
 * @file AthAllocators/test/ArenaSharedHeapSTLAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov 2011
 * @brief Regression tests for ArenaSharedHeapSTLAllocator.
 */


#undef NDEBUG
#include "AthAllocators/ArenaSharedHeapSTLAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include "boost/assign/list_of.hpp"
#include <list>
#include <vector>
#include <cassert>
#include <iostream>

using boost::assign::list_of;



//==========================================================================

struct Payload
{
  Payload(int = 0);
  Payload(const Payload& p);
  ~Payload();
  void clear();

  int x;
  int y;
  static int n;
  static std::vector<int> v;
};

Payload::Payload(int the_y)
{
  x = n++;
  y = the_y;
  v.push_back (x);
}

Payload::Payload (const Payload& p)
  : x (p.x),
    y (p.y)
{
  v.push_back (x);
}


Payload::~Payload()
{
  v.push_back (-x);
}

void Payload::clear ()
{
  y = 0;
}

int Payload::n = 0;
std::vector<int> Payload::v;


struct Payload2 : public Payload
{
  Payload2() {}
  Payload2(int y) : Payload(y) {}
};

//==========================================================================

// Test generic specialization.
void test1()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test1\n";

  SG::ArenaSharedHeapSTLAllocator<Payload> a1;
  assert (a1.nblock() == 1000);
  assert (a1.name() == "::ArenaSharedHeapSTLAllocator<Payload>");

  SG::ArenaSharedHeapSTLAllocator<Payload> a2 (100, "a2");
  assert (a2.nblock() == 100);
  assert (a2.name() == "a2::ArenaSharedHeapSTLAllocator<Payload>");

  {
    Payload p;
    Payload& pr = p;
    const Payload& pcr = p;
    assert (a2.address (pr) == &p);
    assert (a2.address (pcr) == &p);
  }
  assert (Payload::n == 1);
  assert (Payload::v.size() == 2);
  Payload::v.clear();

  std::vector<Payload*> pv;
  for (int i=0; i < 10; i++)
    pv.push_back (a2.allocate (1));

  assert (Payload::n == 1);
  assert (Payload::v.size() == 0);

  assert (a2.max_size() == 1);

  a2.construct (pv[0], Payload(10));
  a2.destroy (pv[0]);

  assert (Payload::n == 2);
  std::vector<int> exp1 = list_of(1)(1)(-1)(-1);
  assert (Payload::v == exp1);
  Payload::v.clear();

  for (int i=0; i < 9; i++) {
    if (i > 0)
      assert (pv[i] != pv[i-1]);
    a2.deallocate (pv[i], 1);
  }

  assert (a2.stats().elts.inuse == 1);
  assert (a2.stats().elts.total == 100);

  a2.reset();
  assert (a2.stats().elts.inuse == 0);
  assert (a2.stats().elts.total == 100);

  a2.erase();
  assert (a2.stats().elts.inuse == 0);
  assert (a2.stats().elts.total == 0);

  a2.reserve(5000);
  assert (a2.stats().elts.inuse == 0);
  assert (a2.stats().elts.total == 5000);

  assert (Payload::n == 2);
  assert (Payload::v.size() == 0);

  SG::ArenaSharedHeapSTLAllocator<Payload> a3 (a2);
  assert (a2.nblock() == 100);
  assert (a2.name() == "a2::ArenaSharedHeapSTLAllocator<Payload>");

  Payload* p1 = a2.allocate (1);
  assert (a2.stats().elts.inuse == 1);
  assert (a2.stats().elts.total == 5000);

  Payload* p2 = a3.allocate (1);
  assert (a3.stats().elts.inuse == 2);
  assert (a3.stats().elts.total == 5000);

  SG::ArenaSharedHeapSTLAllocator<int> a4 (a2);
  assert (a4.nblock() == 100);
  assert (a4.name() == "a2::ArenaSharedHeapSTLAllocator<int>");

  int* p3 = a4.allocate(1);
  assert (a3.stats().elts.inuse == 2);
  assert (a3.stats().elts.total == 5000);
  assert (a4.stats().elts.inuse == 1);
  assert (a4.stats().elts.total == 100);

  a2.deallocate (p1, 1);
  a3.deallocate (p2, 1);
  a4.deallocate (p3, 1);
 }


// Test deallocation.
void test2()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test2\n";

  //Payload* p1;
  //Payload2* p2;

  {
    SG::ArenaSharedHeapSTLAllocator<Payload> a1;
    SG::ArenaSharedHeapSTLAllocator<Payload2> a2 (a1);

    a1.allocate(1);
    a2.allocate(1);
  }
}


// In-situ test.
void test3()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test3\n";

  typedef std::list<int, SG::ArenaSharedHeapSTLAllocator<int> > list_t;

  list_t::allocator_type allo (500, "allo");
  list_t list (allo);

  for (int i = 0; i < 10; i++)
    list.push_back (i);

  assert (list.size() == 10);
  SG::ArenaAllocatorBase::Stats stats;
  stats = list.get_allocator().totstats();
  assert (stats.elts.total == 500);
  assert (stats.elts.inuse == 10);

  for (list_t::iterator i = list.begin();
       i != list.end();
       ++i)
  {
    i = list.erase (i);
    if (i == list.end()) break;
  }

  assert (list.size() == 5);
  stats = list.get_allocator().totstats();
  assert (stats.elts.total == 500);
  assert (stats.elts.inuse == 5);

  list_t list2 (allo);
  list2.push_back (10);
  stats = list2.get_allocator().totstats();
  assert (stats.elts.inuse == 6);

  list_t list3 (list2);
  stats = list2.get_allocator().totstats();
  assert (stats.elts.inuse == 7);

  list.clear();
  assert (list.size() == 0);
  stats = list.get_allocator().totstats();
  assert (stats.elts.total == 500);
  assert (stats.elts.inuse == 2);

  list2.clear();
  stats = list.get_allocator().totstats();
  assert (stats.elts.inuse == 1);

  list3.clear();
  stats = list.get_allocator().totstats();
  assert (stats.elts.inuse == 0);

  list.get_allocator().reset();
  stats = list.get_allocator().totstats();
  assert (stats.elts.total == 500);
  assert (stats.elts.inuse == 0);

  list.push_back(1);
  stats = list.get_allocator().totstats();
  assert (stats.elts.inuse == 1);

  list2 = list;
  stats = list.get_allocator().totstats();
  assert (stats.elts.inuse == 2);

  list.swap(list2);
  stats = list.get_allocator().totstats();
  assert (stats.elts.inuse == 2);

  assert (Payload::n == 0);
  assert (Payload::v.size() == 0);
}


int main()
{
  test1();
  test2();
  test3();
  assert (SG::ArenaBlock::nactive() == 0);
  return 0;
}
