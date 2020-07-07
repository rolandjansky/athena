/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Compile tests for WriteCondHandleKeyArray
 * @TODO extend to also test functionality
 */


#undef NDEBUG
#include "StoreGate/CondHandleKeyArray.h"
#include "StoreGate/exceptions.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/StorableConversions.h"
#include "SGTools/DataProxy.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CxxUtils/checker_macros.h"
#include "StoreGate/StoreGate.h"
#include <cassert>
#include <iostream>

#define DEBUG_VHB 1


class MyObjAux
  : public SG::IConstAuxStore, public ILockable
{
public:
  MyObjAux(int x=0) : x(x) {}
  ~MyObjAux() { deleted.push_back (x); }
  int x;
  bool m_locked = false;

  virtual const void* getData (SG::auxid_t /*auxid*/) const override { return 0; }
  virtual void* getDecoration (SG::auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/) override { return 0; }
  virtual const SG::auxid_set_t& getAuxIDs() const override { std::abort(); }
  virtual bool isDecoration(SG::auxid_t /*auxid*/) const override { std::abort(); }
  virtual void lock() override { m_locked = true; }
  virtual bool clearDecorations() override { return false; }
  virtual size_t size() const override { return 0; }
  virtual void lockDecoration (SG::auxid_t) override { std::abort(); }

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObjAux::deleted;
CLASS_DEF (MyObjAux, 293847296, 1)

class MyObj {
public:
  MyObj(int x=0) : x(x) {}
  ~MyObj() { deleted.push_back (x); }
  SG::IAuxStore* getStore() const { return nullptr; }
  void setStore (SG::IConstAuxStore* store) {aux = dynamic_cast<MyObjAux*>(store); }
  int x;
  MyObjAux* aux {nullptr};

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObj::deleted;
CLASS_DEF( MyObj , 86839352 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( MyObj , 265987730 );

CLASS_DEF( DataVector<MyObj> , 58451955 , 1 )

class MyObj2 {
public:
  MyObj2(int x=0) : x(x) {}
  ~MyObj2() { deleted.push_back (x); }
  int x;

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObj2::deleted;
CLASS_DEF( MyObj2 , 210631107 , 1 )
CONDCONT_DEF( MyObj2 , 128580061 );

class MyObj3 {
public:
  MyObj3(int x=0) : x(x) {}
  ~MyObj3() { deleted.push_back (x); }
  int x;

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObj3::deleted;
CLASS_DEF( MyObj3 , 210631106 , 1 )
CONDCONT_DEF( MyObj3 , 127580062 );


class MyDObj : public DataObject
{
public:
  MyDObj(int x=0) : x(x) {}
  ~MyDObj() { deleted.push_back (x); }
  int x;

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyDObj::deleted;
CLASS_DEF (MyDObj, 293847297, 1)
CONDCONT_DEF( MyDObj , 45988109 );


std::pair<std::unique_ptr<MyObj>, std::unique_ptr<MyObjAux> >
makeWithAux (int x=0)
{
  auto obj = std::make_unique<MyObj>(x);
  auto aux = std::make_unique<MyObjAux>(x+100);
  obj->setStore (aux.get());
  return std::make_pair (std::move(obj), std::move(aux));
}


void test1()
{
  std::cout << "test1\n";

  SG::WriteCondHandleKey<MyObj> k1 {"MyObj1"};
  SG::WriteCondHandleKey<MyObj> k2 {"MyObj2"};
  SG::WriteCondHandleKey<MyObj> k3 {"MyObj3"};

  SG::WriteCondHandleKey<MyDObj> kD {"MyObjD"};

  assert ( k1.mode() == Gaudi::DataHandle::Writer);
  assert ( k1.key() == "MyObj1");
  assert ( k1.initialize().isSuccess() );

  assert ( k2.mode() == Gaudi::DataHandle::Writer);
  assert ( k2.key() == "MyObj2");
  assert ( k2.initialize().isSuccess() );

  assert ( k3.mode() == Gaudi::DataHandle::Writer);
  assert ( k3.key() == "MyObj3");
  assert ( k3.initialize().isSuccess() );

  assert ( kD.mode() == Gaudi::DataHandle::Writer);
  assert ( kD.key() == "MyObjD");
  assert ( kD.initialize().isSuccess() );

  
  SG::WriteCondHandle<MyObj> wch1(k1);
  SG::WriteCondHandle<MyObj> wch2(k2);
  SG::WriteCondHandle<MyObj> wch3(k3);

  EventIDBase s1(0, EventIDBase::UNDEFEVT, 0);
  EventIDBase e1(0, EventIDBase::UNDEFEVT, 10);
  EventIDRange r1 (s1,e1);

  EventIDBase s2(0, EventIDBase::UNDEFEVT, 3);
  EventIDBase e2(0, EventIDBase::UNDEFEVT, 8);
  EventIDRange r2 (s2,e2);

  EventIDBase s3(0, EventIDBase::UNDEFEVT, 5);
  EventIDBase e3(0, EventIDBase::UNDEFEVT, 15);
  EventIDRange r3 (s3,e3);

  
  std::cout << "r1: " << r1 << std::endl;
  std::cout << "r2: " << r2 << std::endl;
  std::cout << "r3: " << r3 << std::endl;

  assert ( wch1.record( r1, std::make_unique<MyObj>( MyObj(1) ) ).isSuccess());
  assert ( wch2.record( r2, std::make_unique<MyObj>( MyObj(2) ) ).isSuccess());
  assert ( wch3.record( r3, std::make_unique<MyObj>( MyObj(3) ) ).isSuccess());

  SG::ReadCondHandleKey<MyObj> rk1 {"MyObj1"};
  assert ( rk1.mode() == Gaudi::DataHandle::Reader);
  assert ( rk1.key() == "MyObj1");
  assert ( rk1.initialize().isSuccess() );

  SG::ReadCondHandleKey<MyObj> rk2 {"MyObj2"};
  assert ( rk2.mode() == Gaudi::DataHandle::Reader);
  assert ( rk2.key() == "MyObj2");
  assert ( rk2.initialize().isSuccess() );

  SG::ReadCondHandleKey<MyObj> rk3 {"MyObj3"};
  assert ( rk3.mode() == Gaudi::DataHandle::Reader);
  assert ( rk3.key() == "MyObj3");
  assert ( rk3.initialize().isSuccess() );


  EventIDBase now(0, EventIDBase::UNDEFEVT, 6);
  SGTest::TestStore dumstore;
  EventContext ctx(1, 1);
  ctx.setEventID( now );
  ctx.setExtension( Atlas::ExtendedEventContext(&dumstore) );
  Gaudi::Hive::setCurrentContext(ctx);
  
  SG::ReadCondHandle<MyObj> rch1(rk1);
  SG::ReadCondHandle<MyObj> rch2(rk2);
  SG::ReadCondHandle<MyObj> rch3(rk3);

  auto o1 = rch1.retrieve( now );
  EventIDRange rr1 = rch1.getRange();

  auto o2 = rch2.retrieve( now );
  EventIDRange rr2 = rch2.getRange();

  auto o3 = rch3.retrieve( now );
  EventIDRange rr3 = rch3.getRange();
  
  assert( rr1 == r1 );
  assert( rr2 == r2 );
  assert( rr3 == r3 );
  
  std::cout << "o1: " << o1->x << "  " << rr1 << std::endl;
  std::cout << "o2: " << o2->x << "  " << rr2 << std::endl;
  std::cout << "o3: " << o3->x << "  " << rr3 << std::endl;
  

  SG::WriteCondHandle<MyDObj> wchD(kD);
  // can't record without a range
  assert ( wchD.record( std::make_unique<MyDObj>( MyDObj(11) ) ).isFailure() );

  wchD.addDependency( rch1 );
  assert ( wchD.getRange() == r1 );
  wchD.addDependency( rch2, rch3 );
  assert (wchD.record( std::make_unique<MyDObj>( MyDObj(11) ) ).isSuccess() );


  EventIDRange rD = wchD.getRange();
  std::cout << "rD: " << rD << std::endl;

  EventIDRange rI;
  rI = EventIDRange::intersect(r1,r2,r3);
  std::cout << "rI: " << rI << std::endl;

  assert (rI == rD);
  
}

void testDep (int timeVal,
              EventIDRange& r1, EventIDRange& r2, EventIDRange &r3,
              SG::ReadCondHandleKey<MyObj>& rk1,
              SG::ReadCondHandleKey<MyObj2>& rk2,
              SG::ReadCondHandleKey<MyObj3>& rk3,
              SG::WriteCondHandleKey<MyDObj>& kD,
              SGTest::TestStore *store, int testval) {

    EventIDBase now(0, EventIDBase::UNDEFEVT, timeVal);
    EventContext ctx(1, 1);
    ctx.setEventID( now );
    ctx.setExtension( Atlas::ExtendedEventContext(store) );
    Gaudi::Hive::setCurrentContext(ctx);

    std::cout << "test EventID: " << now << std::endl;
    
    SG::ReadCondHandle<MyObj>  rch1(rk1);
    SG::ReadCondHandle<MyObj2> rch2(rk2);
    SG::ReadCondHandle<MyObj3> rch3(rk3);
    
    auto o1 = rch1.retrieve( now );
    EventIDRange rr1 = rch1.getRange();
    
    auto o2 = rch2.retrieve( now );
    EventIDRange rr2 = rch2.getRange();
    
    auto o3 = rch3.retrieve( now );
    EventIDRange rr3 = rch3.getRange();
    
    assert( rr1 == r1 );
    assert( rr2 == r2 );
    assert( rr3 == r3 );
    
    std::cout << "o1: " << o1->x << "  " << rr1 << " " << rch1.getRange() << std::endl;
    std::cout << "o2: " << o2->x << "  " << rr2 << std::endl;
    std::cout << "o3: " << o3->x << "  " << rr3 << std::endl;

    SG::WriteCondHandle<MyDObj> wchD(kD);
    
    wchD.addDependency( rch1 );
    assert ( wchD.getRange() == r1 );
    
    wchD.addDependency( rch2, rch3 );
    int val = rch1->x + rch2->x + rch3->x;
    assert (val == testval);
    assert (wchD.record( std::make_unique<MyDObj>( MyDObj(val) ) ).isSuccess() );
    
    EventIDRange rD = wchD.getRange();
    EventIDRange rI = EventIDRange::intersect(r1,r2,r3);
    
    assert (rI == rD);
    std::cout << "rD: " << rD << "  ==  " << rI << std::endl;

}

void test2( StoreGateSvc* cs )
{
  std::cout << "test2\n";

  SG::WriteCondHandleKey<MyObj>  k1 {"MyObj1"};
  SG::WriteCondHandleKey<MyObj2> k2 {"MyObj2"};
  SG::WriteCondHandleKey<MyObj3> k3 {"MyObj3"};

  SG::WriteCondHandleKey<MyDObj> kD {"MyObjD"};

  assert ( k1.mode() == Gaudi::DataHandle::Writer);
  assert ( k1.key() == "MyObj1");
  assert ( k1.initialize().isSuccess() );

  assert ( k2.mode() == Gaudi::DataHandle::Writer);
  assert ( k2.key() == "MyObj2");
  assert ( k2.initialize().isSuccess() );

  assert ( k3.mode() == Gaudi::DataHandle::Writer);
  assert ( k3.key() == "MyObj3");
  assert ( k3.initialize().isSuccess() );

  assert ( kD.mode() == Gaudi::DataHandle::Writer);
  assert ( kD.key() == "MyObjD");
  assert ( kD.initialize().isSuccess() );

  // fill up the ConditionStore with some test data:
  //    time -->
  // MyObj1  1112222333
  // MyObj2  xxxxxyyyyy
  // MyObj3  aabbbbcccc
  //
  SG::WriteCondHandle<MyObj>  wch1(k1);
  SG::WriteCondHandle<MyObj2> wch2(k2);
  SG::WriteCondHandle<MyObj3> wch3(k3);

  EventIDBase s1_1(0, EventIDBase::UNDEFEVT, 0);
  EventIDBase e1_1(0, EventIDBase::UNDEFEVT, 3);
  EventIDRange r1_1 (s1_1,e1_1);
  EventIDBase s1_2(0, EventIDBase::UNDEFEVT, 3);
  EventIDBase e1_2(0, EventIDBase::UNDEFEVT, 7);
  EventIDRange r1_2 (s1_2,e1_2);
  EventIDBase s1_3(0, EventIDBase::UNDEFEVT, 7);
  EventIDBase e1_3(0, EventIDBase::UNDEFEVT, 10);
  EventIDRange r1_3 (s1_3,e1_3);

  EventIDBase s2_1(0, EventIDBase::UNDEFEVT, 0);
  EventIDBase e2_1(0, EventIDBase::UNDEFEVT, 5);
  EventIDRange r2_1 (s2_1,e2_1);
  EventIDBase s2_2(0, EventIDBase::UNDEFEVT, 5);
  EventIDBase e2_2(0, EventIDBase::UNDEFEVT, 10);
  EventIDRange r2_2 (s2_2,e2_2);

  EventIDBase s3_1(0, EventIDBase::UNDEFEVT, 0);
  EventIDBase e3_1(0, EventIDBase::UNDEFEVT, 2);
  EventIDRange r3_1 (s3_1,e3_1);
  EventIDBase s3_2(0, EventIDBase::UNDEFEVT, 2);
  EventIDBase e3_2(0, EventIDBase::UNDEFEVT, 6);
  EventIDRange r3_2 (s3_2,e3_2);
  EventIDBase s3_3(0, EventIDBase::UNDEFEVT, 6);
  EventIDBase e3_3(0, EventIDBase::UNDEFEVT, 10);
  EventIDRange r3_3 (s3_3,e3_3);
  
  std::cout << "r1: " << r1_1 << " " << r1_2 << " " << r1_3 << std::endl;
  std::cout << "r2: " << r2_1 << " " << r2_2 << std::endl;
  std::cout << "r3: " << r3_1 << " " << r3_2 << " " << r3_3 << std::endl;

  CondCont<MyObj>  *cc1;
  assert( cs->retrieve(cc1, k1.key()).isSuccess() );
  CondCont<MyObj2> *cc2;
  assert( cs->retrieve(cc2, k2.key()).isSuccess() );
  CondCont<MyObj3> *cc3;
  assert( cs->retrieve(cc3, k3.key()).isSuccess() );
  
  assert( cc1->insert( r1_1, std::make_unique<MyObj>( MyObj(1) ) ).isSuccess() );
  assert( cc1->insert( r1_2, std::make_unique<MyObj>( MyObj(2) ) ).isSuccess() );
  assert( cc1->insert( r1_3, std::make_unique<MyObj>( MyObj(3) ) ).isSuccess() );
  assert( cc2->insert( r2_1, std::make_unique<MyObj2>( MyObj2(21) ) ).isSuccess() );
  assert( cc2->insert( r2_2, std::make_unique<MyObj2>( MyObj2(22) ) ).isSuccess() );
  assert( cc3->insert( r3_1, std::make_unique<MyObj3>( MyObj3(131) ) ).isSuccess() );
  assert( cc3->insert( r3_2, std::make_unique<MyObj3>( MyObj3(132) ) ).isSuccess() );
  assert( cc3->insert( r3_3, std::make_unique<MyObj3>( MyObj3(133) ) ).isSuccess() );

  SG::ReadCondHandleKey<MyObj> rk1 {"MyObj1"};
  assert ( rk1.mode() == Gaudi::DataHandle::Reader);
  assert ( rk1.key() == "MyObj1");
  assert ( rk1.initialize().isSuccess() );

  SG::ReadCondHandleKey<MyObj2> rk2 {"MyObj2"};
  assert ( rk2.mode() == Gaudi::DataHandle::Reader);
  assert ( rk2.key() == "MyObj2");
  assert ( rk2.initialize().isSuccess() );

  SG::ReadCondHandleKey<MyObj3> rk3 {"MyObj3"};
  assert ( rk3.mode() == Gaudi::DataHandle::Reader);
  assert ( rk3.key() == "MyObj3");
  assert ( rk3.initialize().isSuccess() );

  SGTest::TestStore dumstore;


  // test some error conditions
  {    
    SG::WriteCondHandleKey<MyDObj> kDe1 {"MyObjD_err"};
    assert( kDe1.initialize().isSuccess() );
    SG::WriteCondHandle<MyDObj> we1(kDe1);

    EventIDBase now(0, EventIDBase::UNDEFEVT, 1);
    EventContext ctx(1, 1);
    ctx.setEventID( now );
    ctx.setExtension( Atlas::ExtendedEventContext(&dumstore) );
    Gaudi::Hive::setCurrentContext(ctx);
    
    // can't record without a range
    std::cout << "expected ERROR: ";
    assert ( we1.record( std::make_unique<MyDObj>( MyDObj(0) ) ).isFailure() );

    SG::ReadCondHandle<MyObj>  rerr(rk1);
    we1.addDependency( rerr );
    assert ( we1.getRange() == r1_1 );
    
    // can't record with a range, when dep already set
    std::cout << "expected ERROR: ";
    assert ( we1.record( r2_1, std::make_unique<MyDObj>( MyDObj(0) ) ).isFailure() );
  }
    

  testDep(1, r1_1, r2_1, r3_1, rk1, rk2, rk3, kD, &dumstore, 153);
  testDep(4, r1_2, r2_1, r3_2, rk1, rk2, rk3, kD, &dumstore, 155);
  testDep(6, r1_2, r2_2, r3_3, rk1, rk2, rk3, kD, &dumstore, 157);
  testDep(9, r1_3, r2_2, r3_3, rk1, rk2, rk3, kD, &dumstore, 158);

  
}


int main()
{
  ISvcLocator* svcloc;
  //need MessageSvc
  if (!Athena_test::initGaudi("StoreGate/WriteCondHandle_test.txt", svcloc)) {
    return 1;
  }

  StoreGateSvc *cs=nullptr;
  assert (svcloc->service("StoreGateSvc/ConditionStore",cs).isSuccess());

  // test1();
  // std::cout << "clearing ConditionStore\n";
  // cs->clearStore();
  
  test2(cs);
  return 0;
}
