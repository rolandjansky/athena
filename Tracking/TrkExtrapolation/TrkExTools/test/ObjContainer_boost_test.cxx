/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
 
 
/**
 * @file TrkExTools/test/TrkExToolsStringUtility_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief Some tests for TrkExToolsStringUtility 
 */
 


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRKEXTOOLSOBJCONTAINER

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "TrkExTools/ObjContainer.h"
#include <memory>
#include <atomic>

namespace utf = boost::unit_test;

/* Can run with:
  ~/build/Tracking/TrkExtrapolation/TrkExTools/test-bin/ObjContainer_boost_test.exe -l all
  or, from the build directory can do:
  ctest -V
  (for verbose output)
*/

//copy Goetz's original test object for the template, just take out the output
class TestObj {
public:
   TestObj()                     { ++s_ctorCounter;}
   TestObj(const TestObj &)      { ++s_ctorCounter;}
   TestObj(TestObj &&)            noexcept { ++s_ctorCounter;}
   ~TestObj()                    { ++s_dtorCounter;}
   static int count() {
      return s_ctorCounter - s_dtorCounter;
   }
    static bool isCleanedUp() {
      return count() == 0;
   }

private:
   static std::atomic<int> s_ctorCounter ATLAS_THREAD_SAFE;
   static std::atomic<int> s_dtorCounter ATLAS_THREAD_SAFE;
};

std::atomic<int> TestObj::s_ctorCounter=0;
std::atomic<int> TestObj::s_dtorCounter=0;

template <> TestObj *cloneObj<TestObj>( const TestObj *a) { return (a) ? new TestObj(*a) : nullptr; }

using Registry  = ObjContainer<const TestObj>;
using IndexedObj = ObjRef<>;
using Registrar = ObjPtr<const TestObj>;


//protected methods of ObjContainer made public here for testing
class ObjRegistryStub : public Registry{
public:
  IndexedObj registerObj(TestObj &obj) {
      return Registry::registerObj(&obj, s_externalObj);
   };
    IndexedObj registerObj(TestObj *obj) {
      return Registry::registerObj(replaceManagedPtr(obj), 0);
   }
   IndexedObj share(IndexedObj ref) {
     return Registry::share(ref);
   }
   void drop( IndexedObj &ref){
     return Registry::drop(ref);
   }
   const TestObj *get( IndexedObj ref) const {
     return Registry::get(ref);
   }
   const TestObj *get( IndexedObj ref){
     return Registry::get(ref);
   }
   const TestObj *release( IndexedObj ref){
     return  Registry::release(ref);
   }
   bool isOwned( IndexedObj ref) {
      return Registry::isOwned(ref);
   }
   bool isShared( IndexedObj ref) {
      return Registry::isShared(ref);
   }
   bool isExtern( IndexedObj ref) {
      return Registry::isExtern(ref);
   }
   IndexedObj find( TestObj *obj) const{
     return Registry::find(obj);
   }
   void ensureValidity(IndexedObj ref) const{
     return Registry::ensureValidity(ref);
   }
   void ensureExists(IndexedObj ref) const{
     return Registry::ensureExists(ref);
   }
   void checkCapacity() {
      return Registry::checkCapacity();
   }
   IndexedObj registerObj(TestObj *obj, int initial_count){
     return Registry::registerObj(obj, initial_count);
   }
   std::pair<short, bool > search(TestObj *obj) const {
     return Registry::search(obj);
   }
   short count(IndexedObj ref) const {
     return Registry::count(ref);
   }
   
};


BOOST_AUTO_TEST_SUITE(TrkExToolsObjContainerTest)

  /* 
    Test the probe
  */
  BOOST_AUTO_TEST_CASE(TestObTest){
    {
      TestObj to0;
      TestObj to1;
      BOOST_CHECK(TestObj::count() == 2);
    }
    BOOST_CHECK(TestObj::count()==0);
  }
  
  // Test the ObjRef constructors 
  BOOST_AUTO_TEST_CASE(ObjRefConstructors){
    BOOST_CHECK_NO_THROW(IndexedObj o(1));
    IndexedObj o(1);
    BOOST_CHECK_NO_THROW(IndexedObj q(std::move(o)));
  }
  //... and Methods
  BOOST_AUTO_TEST_CASE(ObjRefMethods){
    IndexedObj o(1);
    BOOST_CHECK(o.isValid());
    bool isValid(o);
    BOOST_CHECK(isValid);
    IndexedObj p;
    BOOST_CHECK (o != p);
    p=o;
    BOOST_CHECK (o == p);
  }
  
  //ObjContainer constructors
  BOOST_AUTO_TEST_CASE(ObjContainerConstructors){
    BOOST_CHECK_NO_THROW(Registry o);
    BOOST_CHECK_NO_THROW(Registry o(100));
  }
  
  //ObjContainer public methods; cannot put anything in the container like this
  BOOST_AUTO_TEST_CASE(ObjContainerMethods){
    Registry o(100);
    BOOST_CHECK(not(o.isValid(1)));
    auto pTestObj = new TestObj();
    auto [count,validity] = o.search(pTestObj);
    BOOST_CHECK(count == 0);
    BOOST_CHECK(validity == false);
    delete pTestObj;
  }
  //ObjContainer protected methods
  BOOST_AUTO_TEST_CASE(ObjContainerWithRef){
    {
      TestObj o;
      ObjRegistryStub *pOcs = new ObjRegistryStub;
      //register an external object
      IndexedObj ref=pOcs->registerObj(o);
      BOOST_CHECK(pOcs->isValid(ref));
      BOOST_CHECK(bool(ref));
      //all false
      BOOST_CHECK(pOcs->isOwned(ref) == false);
      BOOST_CHECK(pOcs->isShared(ref) == false);
      BOOST_CHECK(pOcs->isExtern(ref) == true);
      //this is what 'isExtern' really means
      BOOST_CHECK(pOcs->count(ref) == -2);
      auto pObj = &o;
      auto returnPair = pOcs->search(pObj);
      BOOST_CHECK(returnPair.first == -2); //it's external
      BOOST_CHECK(returnPair.second);
      delete pOcs;
    } 
  }
  
  BOOST_AUTO_TEST_CASE(ObjContainerWithPtr){
    TestObj *pObj{};
    {
      pObj = new TestObj;
      ObjRegistryStub *pOcs = new ObjRegistryStub;
      //register an object
      IndexedObj ref=pOcs->registerObj(pObj);
      BOOST_CHECK(pOcs->isValid(ref));
      BOOST_CHECK(bool(ref));
      //registering an object with plain pointer does not manage it at all
      BOOST_CHECK(pOcs->isOwned(ref) == false);
      BOOST_CHECK(pOcs->isShared(ref) == false);
      BOOST_CHECK(pOcs->isExtern(ref) == false);
      BOOST_CHECK(pOcs->count(ref) == 0);
      //
      auto returnPair = pOcs->search(pObj);
      BOOST_CHECK(returnPair.first == 0);
      BOOST_CHECK(returnPair.second);
      delete pOcs;
    } 
    delete pObj;
  }
  //haven't really understood the interaction of release, drop with the ObjContainer
  //but the only public interface is with ObjPtr...
  
  BOOST_AUTO_TEST_CASE(ObjPtrConstructors){
    //default c'tor
    BOOST_CHECK_NO_THROW([[maybe_unused]] Registrar def);
    //copy c'tor
    Registrar def;
    BOOST_CHECK_NO_THROW([[maybe_unused]] Registrar copyCtor(def));
    //move c'tor
    BOOST_CHECK_NO_THROW([[maybe_unused]] Registrar moveCtor(Registrar{}));
    //c'tor with container and object by reference
    Registry objRegister;
    TestObj externalObj;
    BOOST_CHECK_NO_THROW([[maybe_unused]] Registrar reg(objRegister,externalObj));
    //c'tor with container and unique_ptr to object, giving up ownership
    Registry objRegister2;
    auto uniquePtrTestObj = std::make_unique<TestObj>();
    BOOST_CHECK_NO_THROW([[maybe_unused]] Registrar reg(objRegister2,std::move(uniquePtrTestObj)));
  }
  
  BOOST_AUTO_TEST_CASE(ObjPtrMethodsWithExternal){
    Registry objRegister;
    TestObj externalObj;
    //
    Registrar reg(objRegister,externalObj);//previously tested
    //the following actually checks 'isValid' on the objRegister
    BOOST_CHECK(bool(reg));
    Registrar assignment; //already tested default c'tor
    BOOST_CHECK_NO_THROW(assignment = reg);
    BOOST_CHECK(assignment == reg); //compare the refs
    auto pObj = reg.get();
    BOOST_CHECK(pObj == &externalObj);
    //make a new object to test !=
    Registrar tempRegistrar;
    BOOST_CHECK(tempRegistrar != reg);
    BOOST_CHECK(not reg.isOwned());
    BOOST_CHECK(not reg.isShared());
    BOOST_CHECK(reg.isExtern());
  }
  
  BOOST_AUTO_TEST_CASE(ObjPtrMethodsWithUniquePtr){
    Registry objRegister;
    auto uniquePtrTestObj = std::make_unique<TestObj>();
    //use a newed pointer to be able to explicitly delete later
    auto reg = new Registrar(objRegister,std::move(uniquePtrTestObj)); //previously tested
    BOOST_CHECK(reg->isOwned());
    BOOST_CHECK(not reg->isShared());
    BOOST_CHECK(not reg->isExtern());
    std::unique_ptr<const TestObj> p; //note the const here
    BOOST_CHECK_NO_THROW(p=reg->to_unique());
    BOOST_CHECK(not reg->isOwned());
    BOOST_CHECK(not reg->isShared());
    BOOST_CHECK(not reg->isExtern());
    BOOST_CHECK_NO_THROW(delete reg);
    BOOST_CHECK(TestObj::count() == 1);//that unique_ptr p is still there!
    p.reset();
    //check those objects went away
    BOOST_CHECK(TestObj::isCleanedUp());
    //Now do the same without releasing, just deleting the ObjPtr
    Registry objRegister2;
    auto uniquePtrTestObj2 = std::make_unique<TestObj>();
    auto reg2 = new Registrar(objRegister2,std::move(uniquePtrTestObj2));
    BOOST_CHECK_NO_THROW(delete reg2);
    //check all those objects went away anyway
    BOOST_CHECK(TestObj::isCleanedUp());

  }
BOOST_AUTO_TEST_SUITE_END()
