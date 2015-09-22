/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBase_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHandleBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHandleBase.
 */


#undef NDEBUG
#include "AthAllocators/ArenaHandleBase.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include <cassert>

struct Payload
{
  int x;
};

void test1()
{
  SG::ArenaHeader header;
  SG::ArenaHeader::ArenaAllocVec_t vec;
  vec.push_back (new SG::ArenaPoolAllocator
                 (SG::ArenaPoolAllocator::initParams<Payload>(1000)));
  header.setAllocVec (&vec);
  SG::ArenaHandleBase test (&header, 0);
  size_t elt_size =
    dynamic_cast<SG::ArenaPoolAllocator*>(vec[0])->params().eltSize;
  size_t block_ov = SG::ArenaBlock::overhead();

  assert (test.stats().elts.inuse == 0);
  assert (test.stats().elts.free == 0);
  assert (test.stats().elts.total == 0);
  test.reserve (10);
  assert (test.stats().elts.inuse == 0);
  assert (test.stats().elts.free == 1000);
  assert (test.stats().elts.total == 1000);
  assert (test.stats().blocks.inuse == 0);
  assert (test.stats().blocks.free == 1);
  assert (test.stats().blocks.total == 1);
  assert (test.stats().bytes.inuse == 0);
  assert (test.stats().bytes.free == 1000 * elt_size + block_ov);
  assert (test.stats().bytes.total == 1000 * elt_size + block_ov);
  test.reset ();
  assert (test.stats().elts.inuse == 0);
  assert (test.stats().elts.free == 1000);
  assert (test.stats().elts.total == 1000);
  test.erase ();
  assert (test.stats().elts.inuse == 0);
  assert (test.stats().elts.free == 0);
  assert (test.stats().elts.total == 0);

  SG::ArenaHeader::defaultHeader()->setAllocVec (&vec);
  SG::ArenaHandleBase test2 (0, 0);
  test2.reserve (10);
  assert (test2.stats().elts.inuse == 0);
  assert (test2.stats().elts.free == 1000);
  assert (test2.stats().elts.total == 1000);
}

int main()
{
  test1();
  return 0;
}
