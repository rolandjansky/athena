/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaPoolSTLAllocator_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaPoolAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2008
 * @brief Regression tests for ArenaPoolSTLAllocator.
 */


#undef NDEBUG
#include "AthAllocators/ArenaPoolSTLAllocator.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <atomic>
#include <unordered_map>


//==========================================================================

struct Payload
{
  Payload(int = 0);
  Payload(const Payload& p);
  Payload& operator= (const Payload&) = default;
  ~Payload();
  void clear();

  int x;
  int y;
  static std::atomic<int> n;
  static std::vector<int> v ATLAS_THREAD_SAFE;
};

// cppcheck-suppress uninitMemberVar  // false positive
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

std::atomic<int> Payload::n;
std::vector<int> Payload::v;


struct CTest
{
  typedef SG::ArenaPoolSTLAllocator<int> allocator_type;
  CTest (allocator_type& a) : m_alloc(a) {}
  allocator_type get_allocator() const { return m_alloc; }

  allocator_type& m_alloc;
};


//==========================================================================

// Test generic specialization.
void test1()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test1\n";

  SG::ArenaPoolSTLAllocator<Payload> a1;
  assert (a1.nblock() == 1000);
  assert (a1.name() == "");

  SG::ArenaPoolSTLAllocator<Payload> a2 (100, "a2");
  assert (a2.nblock() == 100);
  assert (a2.name() == "a2");

  SG::ArenaPoolSTLAllocator<Payload*, int> a3;// (100, "a3");
  //assert (a3.nblock() == 100);
  //assert (a3.name() == "a3");

  SG::ArenaPoolSTLAllocator<Payload, int> a4 (a2);
  assert (a4.nblock() == 100);
  assert (a4.name() == "a2");
  assert (a4.poolptr() != nullptr);

  {
    Payload p;
    Payload& pr = p;
    const Payload& pcr = p;
    assert (a4.address (pr) == &p);
    assert (a4.address (pcr) == &p);
  }
  assert (Payload::n == 1);
  assert (Payload::v.size() == 2);
  Payload::v.clear();

  std::vector<Payload*> pv;
  for (int i=0; i < 10; i++)
    pv.push_back (a4.allocate (1));

  assert (Payload::n == 1);
  assert (Payload::v.size() == 0);

  assert (a4.max_size() == 1);

  a4.construct (pv[0], Payload(10));
  a4.destroy (pv[0]);

  assert (Payload::n == 2);
  std::vector<int> exp1 {1, 1, -1, -1};
  assert (Payload::v == exp1);
  Payload::v.clear();

  for (int i=0; i < 10; i++) {
    if (i > 0)
      assert (pv[i] != pv[i-1]);
    a4.deallocate (pv[i], 1);
  }

  assert (a4.stats().elts.inuse == 10);
  assert (a4.stats().elts.total == 100);

  a4.reset();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 100);

  a4.erase();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 0);

  a4.reserve(5000);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 5000);

  assert (Payload::n == 2);
  assert (Payload::v.size() == 0);
}


// Test pointer specialization.
void test2()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test2\n";

  SG::ArenaPoolSTLAllocator<Payload*, int> a1;

  SG::ArenaPoolSTLAllocator<Payload*, int> a2;// (100, "a2");

  SG::ArenaPoolSTLAllocator<Payload, int> a3 (500, "a3");
  a3.reserve (1000);
  SG::ArenaPoolSTLAllocator<Payload*, int> a4 (a3);

  (void)a3.allocate (1);
  assert (a3.stats().elts.inuse == 1);
  assert (a3.stats().elts.total == 1000);

  Payload** p = a4.allocate (2, nullptr);
  a4.deallocate (p, 2);

  assert (Payload::n == 0);
  assert (Payload::v.size() == 0);
}


// Test vetoed specialization.
void test3()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test3\n";

  SG::ArenaPoolSTLAllocator<int, int> a1;
  assert (a1.nblock() == 1000);
  assert (a1.name() == "");
  assert (a1.poolptr() == nullptr);

  SG::ArenaPoolSTLAllocator<int, int> a2 (100, "a2");
  assert (a2.nblock() == 100);
  assert (a2.name() == "a2");
  assert (a2.poolptr() == nullptr);

  assert (a2.stats().elts.total == 0);

  SG::ArenaPoolSTLAllocator<Payload, int> a3 (500, "a3");
  a3.reserve (1000);
  SG::ArenaPoolSTLAllocator<int> a4 (a3);
  assert (a4.nblock() == 500);
  assert (a4.name() == "a3");
  assert (a4.poolptr() == a3.poolptr());

  (void)a3.allocate (1);
  assert (a3.stats().elts.inuse == 1);
  assert (a3.stats().elts.total == 1000);

  assert (a4.stats().elts.inuse == 1);
  assert (a4.stats().elts.total == 1000);

  CTest c4 (a4);
  auto a4_nc = SG::ArenaPoolSTLAllocator<int>::get_allocator (c4);
  a4_nc.reset();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 1000);

  a4_nc.erase();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 0);

  a4_nc.reserve(1000);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 1000);

  int* p = a4.allocate (2, nullptr);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 1000);

  a4.deallocate (p, 2);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 1000);

  assert (Payload::n == 0);
  assert (Payload::v.size() == 0);
}


// Copy/assign/move.
void test4()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test4\n";
  SG::ArenaPoolSTLAllocator<Payload, int> b1 (100, "b1");
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");

  (void)b1.allocate(1);
  assert (b1.stats().elts.inuse == 1);
  assert (b1.stats().elts.total == 100);

  SG::ArenaPoolSTLAllocator<Payload, int> b2 (std::move (b1));
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");
  assert (b2.nblock() == 100);
  assert (b2.name() == "b1");
  assert (b1.stats().elts.inuse == 0);
  assert (b1.stats().elts.total == 0);
  assert (b2.stats().elts.inuse == 1);
  assert (b2.stats().elts.total == 100);
  (void)b2.allocate(1);
  assert (b2.stats().elts.inuse == 2);
  assert (b2.stats().elts.total == 100);

  b1 = std::move(b2);
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");
  assert (b2.nblock() == 100);
  assert (b2.name() == "b1");
  assert (b2.stats().elts.inuse == 0);
  assert (b2.stats().elts.total == 0);
  assert (b1.stats().elts.inuse == 2);
  assert (b1.stats().elts.total == 100);
  (void)b1.allocate(1);
  assert (b1.stats().elts.inuse == 3);
  assert (b1.stats().elts.total == 100);

  b1.swap(b2);
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");
  assert (b2.nblock() == 100);
  assert (b2.name() == "b1");
  assert (b1.stats().elts.inuse == 0);
  assert (b1.stats().elts.total == 0);
  assert (b2.stats().elts.inuse == 3);
  assert (b2.stats().elts.total == 100);

  assert (b1 == b1);
  assert (!(b1 != b1));
  assert (!(b1 == b2));
  assert (b1 != b2);
}


// In-situ test.
void test5()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test5\n";

  typedef std::unordered_map<int, int, std::hash<int>, std::equal_to<int>,
    SG::ArenaPoolSTLAllocator<std::pair<const int, int> > > map_t;

  map_t::allocator_type allo (500, "allo");
  map_t map (10, map_t::hasher(), map_t::key_equal(), allo);

  for (int i = 0; i < 10; i++)
    map[i] = i;

  assert (map.size() == 10);
  assert (map.get_allocator().stats().elts.total == 500);
  assert (map.get_allocator().stats().elts.inuse == 10);

  map.clear();
  assert (map.size() == 0);
  assert (map.get_allocator().stats().elts.total == 500);
  assert (map.get_allocator().stats().elts.inuse == 10);

  map_t::allocator_type::get_allocator(map).reset();
  assert (map.get_allocator().stats().elts.total == 500);
  assert (map.get_allocator().stats().elts.inuse == 0);

  assert (Payload::n == 0);
  assert (Payload::v.size() == 0);
}


// Copy/move container.
void test6()
{
  std::cout << "test6\n";

  Payload::v.clear();
  Payload::n = 0;

  typedef std::unordered_map<int, int, std::hash<int>, std::equal_to<int>,
    SG::ArenaPoolSTLAllocator<std::pair<const int, int> > > map_t;

  map_t::allocator_type allo (500, "allo");
  map_t map (10, map_t::hasher(), map_t::key_equal(), allo);

  for (int i = 0; i < 10; i++)
    map[i] = i;

  assert (map.size() == 10);
  assert (map.get_allocator().stats().elts.total == 500);
  assert (map.get_allocator().stats().elts.inuse == 10);

  map_t map2 = map;
  assert (map.size() == 10);
  assert (map.get_allocator().stats().elts.total == 500);
  assert (map.get_allocator().stats().elts.inuse == 10);

  assert (map2.size() == 10);
  assert (map2.get_allocator().stats().elts.total == 500);
  assert (map2.get_allocator().stats().elts.inuse == 10);

  map_t map3;
  map3 = map;
  assert (map.size() == 10);
  assert (map.get_allocator().stats().elts.total == 500);
  assert (map.get_allocator().stats().elts.inuse == 10);

  assert (map3.size() == 10);
  assert (map3.get_allocator().stats().elts.total == 1000);
  assert (map3.get_allocator().stats().elts.inuse == 10);

  map_t map4 (std::move (map2));
  assert (map4.size() == 10);
  assert (map4.get_allocator().stats().elts.total == 500);
  assert (map4.get_allocator().stats().elts.inuse == 10);
  assert (map2.size() == 0);
  assert (map2.get_allocator().stats().elts.total == 0);
  assert (map2.get_allocator().stats().elts.inuse == 0);

  map2 = std::move(map4);
  assert (map2.size() == 10);
  assert (map2.get_allocator().stats().elts.total == 500);
  assert (map2.get_allocator().stats().elts.inuse == 10);
  assert (map4.size() == 0);
  assert (map4.get_allocator().stats().elts.total == 0);
  assert (map4.get_allocator().stats().elts.inuse == 0);

  map2.swap (map4);
  assert (map4.size() == 10);
  assert (map4.get_allocator().stats().elts.total == 500);
  assert (map4.get_allocator().stats().elts.inuse == 10);
  assert (map2.size() == 0);
  assert (map2.get_allocator().stats().elts.total == 0);
  assert (map2.get_allocator().stats().elts.inuse == 0);
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  test6();
  return 0;
}
