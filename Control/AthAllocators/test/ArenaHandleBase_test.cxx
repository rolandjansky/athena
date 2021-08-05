/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
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
#include "AthAllocators/ArenaAllocatorCreator.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include <cassert>


struct Payload
{
  int x;
};


class Creator
  : public SG::ArenaAllocatorCreator
{
public:
  Creator() {}
  virtual std::unique_ptr<SG::ArenaAllocatorBase> create() override
  {
    return std::make_unique<SG::ArenaPoolAllocator>
      (SG::ArenaPoolAllocator::initParams<Payload>(1000));
  }
};


void test1()
{
  SG::ArenaAllocatorRegistry* reg =
    SG::ArenaAllocatorRegistry::instance();
  assert (reg->registerCreator ("a", std::make_unique<Creator>()) == 0);

  SG::ArenaHeader header;
  SG::ArenaBase arena;
  header.setArena (&arena);
  size_t elt_size =
    dynamic_cast<SG::ArenaPoolAllocator*>(arena.allocator (0).get())->params().eltSize;
  size_t block_ov = SG::ArenaBlock::overhead();

  {
    SG::ArenaHandleBase test (&header, 0);
    assert (test.stats().elts.inuse == 0);
    assert (test.stats().elts.free == 0);
    assert (test.stats().elts.total == 0);
    test.reserve (10);
    assert (test.stats().elts.total >= 1000);
    assert (test.stats().elts.inuse == 0);
    assert (test.stats().elts.free == test.stats().elts.total);
    assert (test.stats().blocks.inuse == 0);
    assert (test.stats().blocks.free == 1);
    assert (test.stats().blocks.total == 1);
    const size_t nelt = test.stats().elts.total;
    assert (test.stats().bytes.inuse == 0);
    assert (test.stats().bytes.free == nelt * elt_size + block_ov);
    assert (test.stats().bytes.total == nelt * elt_size + block_ov);
    test.reset ();
    assert (test.stats().elts.total >= 1000);
    assert (test.stats().elts.inuse == 0);
    assert (test.stats().elts.free == test.stats().elts.total);
    test.erase ();
    assert (test.stats().elts.inuse == 0);
    assert (test.stats().elts.free == 0);
    assert (test.stats().elts.total == 0);
  }

  SG::ArenaHeader::defaultHeader()->setArena (&arena);
  {
    SG::ArenaHandleBase test2 (static_cast<SG::ArenaHeader*>(nullptr), 0);
    test2.reserve (10);
    assert (test2.stats().elts.total >= 1000);
    assert (test2.stats().elts.inuse == 0);
    assert (test2.stats().elts.free == test2.stats().elts.total);
  }
}


// Test access via arena and slot.
void test2()
{
  SG::ArenaAllocatorRegistry* reg =
    SG::ArenaAllocatorRegistry::instance();
  int i = reg->registerCreator ("b", std::make_unique<Creator>());

  SG::ArenaHeader header;
  SG::ArenaBase a1 ("1");
  SG::ArenaBase a2 ("2");
  header.addArena (&a1);
  header.addArena (&a2);
  header.setArenaForSlot (1, &a1);
  header.setArenaForSlot (2, &a2);

  assert (header.reportStr() == "=== 1 ===\n\
=== 2 ===\n\
=== default ===\n");

  {
    SG::ArenaHandleBase h1 (&header, EventContext(0, 1), i);
    h1.reserve (10);
  }
#if 0
  assert (header.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/   1000/   1000       0/   4032/   4032       0/      1/      1  \n\
=== 2 ===\n\
=== default ===\n");
#endif

  {
    SG::ArenaHandleBase h2 (&a2, i);
    h2.reserve (10);
  }

#if 0
  assert (header.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/   1000/   1000       0/   4032/   4032       0/      1/      1  \n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/   1000/   1000       0/   4032/   4032       0/      1/      1  \n\
=== default ===\n");
#endif

  {
    SG::ArenaHandleBase h2 (&header, EventContext(0, 2), i);
    h2.reserve (2000);
  }

#if 0
  assert (header.reportStr() == "\
=== 1 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/   1000/   1000       0/   4032/   4032       0/      1/      1  \n\
=== 2 ===\n\
Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total\n\
       0/   2000/   2000       0/   8064/   8064       0/      2/      2  \n\
=== default ===\n");
#endif
}


int main()
{
  test1();
  test2();
  return 0;
}
