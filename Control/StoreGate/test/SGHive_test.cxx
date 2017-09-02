/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
  test SGHiveMgrSvc functionality
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: SGHive_test.cxx 797434 2017-02-15 23:30:43Z ssnyder $


#undef NDEBUG
#include <string>
#include "TestTools/initGaudi.h"
#include "TestTools/SGassert.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "../src/SGHiveMgrSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"


using namespace Athena_test;
using namespace std;
namespace SG {
class Foo {
public:
  Foo() : m_i(0), m_d(0.0) {}
  Foo(int i) : m_i(i), m_d(0.0) {}
  int i() const { return m_i; }
  ~Foo() {
  }
private:
  int m_i;
  double m_d;
};
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(SG::Foo, 81010, 1)
  

namespace SG {
/*** a friend of SG::HiveMgrSvc */
class TestSGHiveMgrSvc {
public:  
  /*** here we test finding the next empty store and assigning to an event */
  static void init(SG::HiveMgrSvc& rSGHM)
  {  
    assert( rSGHM.m_slots.size() == rSGHM.m_nSlots );
    //this is specified in SGHive_test.txt
    assert( rSGHM.m_slots.size() == 4 );
  
    for (size_t evtNum=0; evtNum<rSGHM.m_nSlots; ++evtNum) {
      assert( SG::TestSGHiveMgrSvc::next(rSGHM, evtNum)!=std::string::npos );
    }
  }

  /*** here we test finding the next empty store and assigning to an event */
  static size_t next(::IHiveWhiteBoard& rWB, size_t eventNumber)
  {  
    //    size_t slot=string::npos;
    //    return (((slot=rWB.allocateStore(eventNumber)) != string::npos) &&
    //	    rWB.selectStore(slot).isSuccess()) ? slot : string::npos;
    return rWB.allocateStore(eventNumber);
  }

  /*** here we test the clearing and freeing of stores which are not needed anymore */
  static StatusCode clear(::IHiveWhiteBoard& rWB, size_t slot)
  {  
    return ( rWB.clearStore(slot).isSuccess() && 
	     rWB.freeStore(slot).isSuccess() ) ?
      StatusCode::SUCCESS :
      StatusCode::FAILURE;
  }
};
  /*** a friend of StoreGateSvc */
  class TestHiveStoreSvc {
  private:
    HiveEventSlot* m_pSlot;
    StoreGateSvc& m_rHSG;
  public:  
    TestHiveStoreSvc(StoreGateSvc& rHSG) :
      m_pSlot(StoreGateSvc::currentSlot()),
      m_rHSG(rHSG){}
    void testNewDataObjects(StoreGateSvc& rSG) 
    {  
      DataObjIDColl products;
      rSG.getNewDataObjects(products).ignore(); //reset counter
      assert( !rSG.newDataObjectsPresent() );
      assert( products.empty() );
      //direct call
      rSG.currentStore()->addedNewTransObject(ClassID_traits<SG::Foo>::ID(), "blassed");  
      rSG.commitNewDataObjects();
      rSG.getNewDataObjects(products).ignore();    
      assert( 1 == products.size() );
      assert (products.count (DataObjID (81010,"blassed")) == 1);

      //add something to store
      assert(rSG.record(new SG::Foo(1), "pFoo1").isSuccess());
      assert(rSG.record(new SG::Foo(2), "pFoo2").isSuccess());
      rSG.commitNewDataObjects();
      assert( rSG.newDataObjectsPresent() );
      rSG.getNewDataObjects(products).ignore();    
      assert( 2 == products.size() );
      assert (products.count (DataObjID (81010,"pFoo1")) == 1);
      //we have emptied newdataobject array with call to newDataObjects
      assert( !rSG.newDataObjectsPresent() );
      assert(rSG.record(new SG::Foo(3), "pFoo3").isSuccess());
      rSG.commitNewDataObjects();
      assert( rSG.newDataObjectsPresent() );
      rSG.getNewDataObjects(products).ignore();    
      assert( 1 == products.size() );
      //      assert (products.count (DataObjID (81010,"pFoo3")) == 1);
      assert (products.count (DataObjID ("SG::Foo","pFoo3")) == 1);
    }
    void testNoSlot() {
      cout << "\n*** SGHive_test noSlot BEGINS ***" << endl;
      StoreGateSvc::setSlot(0);
      testNewDataObjects(m_rHSG);
      cout << "\n*** SGHive_test noSlot ENDS ***" << endl;
    }
    void testWithSlot(SG::HiveEventSlot* pSlot=0) {
      cout << "\n*** SGHive_test withSlot BEGINS ***" << endl;
      if (pSlot) m_pSlot=pSlot;
      StoreGateSvc::setSlot(m_pSlot);
      testNewDataObjects(m_rHSG);
      cout << "\n*** SGHive_test withSlot ENDS ***" << endl;
    }
  };


} //ns SG

int main() {
  cout << "*** SGHiveTest BEGINS ***" << endl;
  ISvcLocator* pSvcLoc;
  if (!initGaudi("SGHive_test.txt", pSvcLoc)) {
    return 1;
  }
  IHiveWhiteBoard* pWB(0);
  assert( pSvcLoc->service("SG::HiveMgrSvc/HiveMgrSvc", pWB, true).isSuccess() );

  SG::HiveMgrSvc* pSGHM(dynamic_cast<SG::HiveMgrSvc*>(pWB));
  assert( pSGHM );

  //too late to change the number of stores
  SGASSERTERROR( pWB->setNumberOfStores(99).isSuccess() );

  SG::TestSGHiveMgrSvc::init(*pSGHM);

  //since we have used all slots this should fail
  SGASSERTERROR( SG::TestSGHiveMgrSvc::next(*pWB, 123465)!=string::npos );
  //reallocating a slot for active event should fail
  SGASSERTERROR( SG::TestSGHiveMgrSvc::next(*pWB, 0)!=string::npos );

  //can not clear a non existing slot
  SGASSERTERROR( SG::TestSGHiveMgrSvc::clear(*pWB, 222222).isSuccess() );
  //clear an existing one
  assert( SG::TestSGHiveMgrSvc::clear(*pWB, 1).isSuccess() );
  //now that we cleared, we should be able to allocate another store
  assert( SG::TestSGHiveMgrSvc::next(*pWB, 333333)!=string::npos );
  
  //now test the HiveStoreSvc itself

  StoreGateSvc* pHSG(0);
  assert( pSvcLoc->service("StoreGateSvc", pHSG, true).isSuccess() );

  SG::TestHiveStoreSvc testHSG(*pHSG);
  testHSG.testNoSlot();
  testHSG.testWithSlot();

  assert( pHSG->finalize().isSuccess() );
  assert( pSGHM->finalize().isSuccess() );
  cout << "*** SGHiveTest OK ***\n\n" <<endl;

  return 0;
}
