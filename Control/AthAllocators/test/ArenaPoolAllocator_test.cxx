/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaPoolAllocator_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaPoolAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaPoolAllocator.
 */

#undef NDEBUG
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include <vector>
#include <cassert>
#include <algorithm>

//==========================================================================

struct Payload
{
  Payload();
  ~Payload();
  void clear();

  int x;
  int y;
  static int n;
  static std::vector<int> v;
};

Payload::Payload()
{
  x = n++;
  y = 0;
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

//==========================================================================

void test1()
{
  SG::ArenaPoolAllocator apa
    (SG::ArenaPoolAllocator::initParams<Payload, true>(100, "foo"));
  assert (apa.name() == "foo");
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 0);
  assert (apa.stats().elts.total == 0);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free == 0);
  assert (apa.stats().blocks.total == 0);
  assert (apa.params().eltSize == sizeof (Payload));

  int nptr = 987;
  std::vector<Payload*> ptrs;
  for (int i=0; i < nptr; i++) {
    Payload* p = reinterpret_cast<Payload*> (apa.allocate());
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  assert  (Payload::v.size() == 1000);
  for (int i=0; i < 1000; ++i) {
    assert (Payload::v[i] == i);
  }
  assert (apa.stats().elts.inuse == 987);
  assert (apa.stats().elts.free == 13);
  assert (apa.stats().elts.total == 1000);
  assert (apa.stats().blocks.inuse == 10);
  assert (apa.stats().blocks.free == 0);
  assert (apa.stats().blocks.total == 10);

  Payload::v.clear();
  apa.reset();
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 1000);
  assert (apa.stats().elts.total == 1000);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free == 10);
  assert (apa.stats().blocks.total == 10);

  for (int i=0; i < 300; i++) {
    Payload* p = reinterpret_cast<Payload*> (apa.allocate());
    assert (p->y == 0);
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  assert (Payload::v.size() == 0);
  assert (apa.stats().elts.inuse == 300);
  assert (apa.stats().elts.free == 700);
  assert (apa.stats().elts.total == 1000);
  assert (apa.stats().blocks.inuse == 3);
  assert (apa.stats().blocks.free == 7);
  assert (apa.stats().blocks.total == 10);

  apa.allocate();
  std::vector<int> vv;
  for (SG::ArenaPoolAllocator::iterator ii = apa.begin();
       ii != apa.end();
       ++ii)
  {
    vv.push_back (reinterpret_cast<Payload*> (&*ii)->x);
  }
  std::sort (vv.begin(), vv.end());
  assert (vv.size() == 301);
  for (size_t jj = 0; jj < vv.size(); jj++) {
    assert (vv[jj] == (int)jj);
  }

  vv.clear();
  const SG::ArenaPoolAllocator& capa = apa;
  for (SG::ArenaPoolAllocator::const_iterator ii = capa.begin();
       ii != capa.end();
       ++ii)
  {
    vv.push_back (reinterpret_cast<const Payload*> (&*ii)->x);
  }
  std::sort (vv.begin(), vv.end());
  assert (vv.size() == 301);
  for (size_t jj = 0; jj < vv.size(); jj++) {
    assert (vv[jj] == (int)jj);
  }

  vv.clear();
  for (SG::ArenaPoolAllocator::const_iterator ii = apa.begin();
       ii != apa.end();
       ++ii)
  {
    vv.push_back (reinterpret_cast<const Payload*> (&*ii)->x);
  }
  std::sort (vv.begin(), vv.end());
  assert (vv.size() == 301);
  for (size_t jj = 0; jj < vv.size(); jj++) {
    assert (vv[jj] == (int)jj);
  }

  Payload::v.clear();
  apa.reset();
  for (int i=0; i < 400; i++) {
    Payload* p = reinterpret_cast<Payload*> (apa.allocate());
    assert (p->y == 0);
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  assert (Payload::v.size() == 0);
  assert (apa.stats().elts.inuse == 400);
  assert (apa.stats().elts.free == 600);
  assert (apa.stats().elts.total == 1000);
  assert (apa.stats().blocks.inuse == 4);
  assert (apa.stats().blocks.free == 6);
  assert (apa.stats().blocks.total == 10);

  Payload::v.clear();
  apa.reserve (550);
  assert (apa.stats().elts.inuse == 400);
  assert (apa.stats().elts.free == 200);
  assert (apa.stats().elts.total == 600);
  assert (apa.stats().blocks.inuse == 4);
  assert (apa.stats().blocks.free == 2);
  assert (apa.stats().blocks.total == 6);
  assert (Payload::v.size() == 400);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i=0; i < 100; i++) {
    assert (Payload::v[i] == (int)i-799);
  }
  for (size_t i=100; i < Payload::v.size(); i++) {
    assert (Payload::v[i] == (int)i-799);
  }

  Payload::v.clear();
  apa.reserve (950);
  assert (apa.stats().elts.inuse == 400);
  assert (apa.stats().elts.free == 550);
  assert (apa.stats().elts.total == 950);
  assert (apa.stats().blocks.inuse == 4);
  assert (apa.stats().blocks.free == 3);
  assert (apa.stats().blocks.total == 7);
  assert (Payload::v.size() == 350);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i=0; i < Payload::v.size(); i++) {
    assert (Payload::v[i] == (int)i+1000);
  }

  Payload::v.clear();
  apa.erase();
  assert (Payload::v.size() == 950);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i=0; i < 550; i++) {
    assert (Payload::v[i] == (int)i-1349);
  }
  for (size_t i=550; i < Payload::v.size(); i++) {
    assert (Payload::v[i] == (int)i-949);
  }
  assert (apa.stats().elts.inuse == 0);
  assert (apa.stats().elts.free == 0);
  assert (apa.stats().elts.total == 0);
  assert (apa.stats().blocks.inuse == 0);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == 0);
}


void test2()
{
  Payload::v.clear();
  SG::ArenaPoolAllocator apa
    (SG::ArenaPoolAllocator::initParams<Payload, true>(100));
  for (int i=0; i < 150; i++) {
    apa.allocate();
  }
  assert (Payload::v.size() == 200);
  SG::ArenaPoolAllocator::pointer p = apa.allocate();
  assert (Payload::v.size() == 200);
  for (int i=0; i < 150; i++) {
    apa.allocate();
  }
  assert (Payload::v.size() == 400);
  assert (apa.stats().elts.inuse == 301);
  assert (apa.stats().elts.free == 99);
  assert (apa.stats().elts.total == 400);
  assert (apa.stats().blocks.inuse == 4);
  assert (apa.stats().blocks.free  == 0);
  assert (apa.stats().blocks.total == 4);

  size_t elt_size = apa.params().eltSize;
  size_t block_ov = SG::ArenaBlock::overhead();

  apa.resetTo (p);

  assert (apa.stats().elts.inuse == 150);
  assert (apa.stats().elts.free == 250);
  assert (apa.stats().elts.total == 400);
  assert (apa.stats().blocks.inuse == 2);
  assert (apa.stats().blocks.free  == 2);
  assert (apa.stats().blocks.total == 4);

  assert (apa.stats().bytes.inuse == (150 * elt_size + 2 * block_ov));
  assert (apa.stats().bytes.free == (250 * elt_size + 2 * block_ov));
  assert (apa.stats().bytes.total == (400 * elt_size + 4 * block_ov));
}


int main()
{
  test1();
  test2();
  return 0;
}
