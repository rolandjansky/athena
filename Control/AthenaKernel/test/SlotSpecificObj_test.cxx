/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file AthenaKernel/test/SlotSpecificObj_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Regression tests for SlotSpecificObj.
 */

#undef NDEBUG
#include "AthenaKernel/SlotSpecificObj.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include <cassert>
#include <iostream>


const size_t nslots = 4;


class TestWhiteBoard
  : public extends<Service, IHiveWhiteBoard>
{
public:
  TestWhiteBoard (const std::string& name, ISvcLocator* svc)
    : base_class (name, svc)
  {}
  
  virtual StatusCode selectStore(size_t /*partitionIndex*/) override { std::abort(); }
  virtual StatusCode clearStore(size_t /*partitionIndex*/) override { std::abort(); }
  virtual StatusCode setNumberOfStores(size_t /*slots*/) override { std::abort(); }
  virtual DataObjIDColl getNewDataObjects() override { std::abort(); }
  virtual void addNewDataObjects( DataObjIDColl& /*products*/ ){ std::abort(); }
  virtual bool exists( const DataObjID& ) override { std::abort(); } 
  virtual size_t allocateStore( int /*evtnumber*/ ) override { std::abort(); }
  virtual StatusCode freeStore( size_t /*partitionIndex*/ ) override { std::abort(); }
  virtual size_t getPartitionNumber(int /*eventnumber*/) const override { std::abort(); }
  virtual size_t getNumberOfStores() const override { return nslots; }
  virtual unsigned int freeSlots() override { std::abort(); }

};


DECLARE_SERVICE_FACTORY( TestWhiteBoard )


struct Payload
{
  size_t x = 0;
};


void test1()
{
  std::cout << "test1\n";
  SG::SlotSpecificObj<Payload> o;
  const SG::SlotSpecificObj<Payload>& co = o;

  for (size_t i = 0; i < nslots; i++) {
    EventContext ctx (0, i);
    o.get(ctx)->x = (i+1)*10;
  }

  for (size_t i = 0; i < nslots; i++) {
    EventContext ctx (0, i);
    assert (o.get(ctx)->x == (i+1)*10);
    assert (co.get(ctx)->x == (i+1)*10);
  }

  for (size_t i = 0; i < nslots; i++) {
    EventContext ctx (0, i);
    Gaudi::Hive::setCurrentContext (ctx);
    assert (o.get()->x == (i+1)*10);
    assert (co.get()->x == (i+1)*10);
    assert (o->x == (i+1)*10);
    assert (co->x == (i+1)*10);
    assert ((*o).x == (i+1)*10);
    assert ((*co).x == (i+1)*10);

    o->x = (i+2)*20;
  }

  for (size_t i = 0; i < nslots; i++) {
    EventContext ctx (0, i);
    assert (o.get(ctx)->x == (i+2)*20);
  }

  size_t i = 0;
  for (Payload& p : o) {
    assert (p.x == (i+2)*20);
    p.x = (i+2)*30;
    ++i;
  }

  i = 0;
  for (const Payload& p : co) {
    assert (p.x == (i+2)*30);
    ++i;
  }
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  SG::setNSlotsHiveMgrName ("TestWhiteBoard");
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc = 0;
  if (!Athena_test::initGaudi("SlotSpecificObj_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 1;
  }  
  assert(svcloc);

  test1();
  return 0;
}
