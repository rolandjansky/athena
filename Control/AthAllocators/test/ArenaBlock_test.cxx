/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBlock_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaBlock_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaBlock.
 */

#undef NDEBUG
#include "AthAllocators/ArenaBlock.h"
#include <cassert>
#include <vector>

//==========================================================================

struct Payload
{
  Payload();
  ~Payload();
  static void constructor (char*);
  static void destructor (char*);
  static void scan (char*);

  int x;
  static int n;
  static std::vector<int> v;
};

Payload::Payload()
{
  x = n++;
  v.push_back (x);
}

Payload::~Payload()
{
  v.push_back (-x);
}

void Payload::constructor (char* p)
{
  new (p) Payload;
}

void Payload::destructor (char* p)
{
  reinterpret_cast<Payload*>(p)->~Payload();
}

void Payload::scan (char* p)
{
  Payload::v.push_back (reinterpret_cast<Payload*>(p)->x);
}

int Payload::n = 0;
std::vector<int> Payload::v;

//==========================================================================

const size_t elt_size = sizeof (Payload);

int& word (SG::ArenaBlock* bl, size_t i=0)
{
  return *(int*)bl->index (i, elt_size);
}

Payload& payload (SG::ArenaBlock* bl, size_t i=0)
{
  return *(Payload*)bl->index (i, elt_size);
}

void test1()
{
  assert (SG::ArenaBlock::nactive() == 0);
  SG::ArenaBlock* bl = SG::ArenaBlock::newBlock (20, elt_size, 0);
  assert (SG::ArenaBlock::nactive() == 1);
  assert (bl->overhead() > 0 && bl->overhead() < 100);
  assert (bl->size() == 20);
  assert (bl->eltSize() == elt_size);
  word(bl, 0) = 0;
  word(bl, 1) = 0;
  assert ((char*)bl->index(1) -
          (char*)bl->index(0) == (int)elt_size);
  assert (bl->link() == 0);
  bl->link() = bl;
  assert (bl->link() == bl);
  SG::ArenaBlock::destroy (bl, 0);
  assert (SG::ArenaBlock::nactive() == 0);
}


void test2()
{
  SG::ArenaBlock* b1 = SG::ArenaBlock::newBlock (20, elt_size,
                                                 Payload::constructor);
  SG::ArenaBlock* b2 = SG::ArenaBlock::newBlock (20, elt_size,
                                                 Payload::constructor);
  SG::ArenaBlock* b3 = SG::ArenaBlock::newBlock (20, elt_size,
                                                 Payload::constructor);
  int i = 0;
  for (size_t j = 0; j < b1->size(); j++) {
    assert (payload(b1, j).x == i);
    assert (Payload::v[i] == i);
    ++i;
  }
  b1->link() = b2;
  SG::ArenaBlock::appendList (&b1, b3);
  assert (payload(b1).x == 0);
  assert (payload(b1->link()).x == 20);
  assert (payload(b1->link()->link()).x == 40);
  assert(b1->link()->link()->link() == 0);
  SG::ArenaBlock* bb = 0;
  SG::ArenaBlock::appendList (&bb, b1);
  assert (bb == b1);

  Payload::v.clear();
  SG::ArenaBlock::applyList (b1, Payload::scan, 10);
  assert (Payload::v.size() == 50);
  for (size_t j = 0; j < 10; ++j) {
    assert (Payload::v[j] == (int)j);
  }
  for (size_t j = 10; j < Payload::v.size(); ++j) {
    assert (Payload::v[j] == (int)j+10);
  }

  Payload::v.clear();
  SG::ArenaBlock::destroyList (b1, Payload::destructor);
  assert (Payload::v.size() == 60);
  for (size_t j = 0; j < Payload::v.size(); ++j) {
    assert (Payload::v[j] == -(int)j);
  }
}


int main()
{
  test1();
  test2();
  return 0;
}
