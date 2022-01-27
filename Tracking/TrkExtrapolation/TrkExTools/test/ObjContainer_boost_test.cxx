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
    static bool isCleanedup() {
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
   static bool isOwned( IndexedObj ref) {
      return Registry::isOwned(ref);
   }
   static bool isShared( IndexedObj ref) {
      return Registry::isShared(ref);
   }
   static bool isExtern( IndexedObj ref) {
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
  BOOST_AUTO_TEST_CASE(ObjContainerWithRef, * utf::expected_failures(1)){
    {
      TestObj o;
      ObjRegistryStub *pOcs = new ObjRegistryStub;
      //register an external object
      IndexedObj ref=pOcs->registerObj(o);
      BOOST_CHECK(pOcs->isValid(ref));
      BOOST_CHECK(int(ref) == 1);
      //all false
      BOOST_CHECK(pOcs->isOwned(ref) == false);
      BOOST_CHECK(pOcs->isShared(ref) == false);
      //the following fails
      BOOST_TEST(pOcs->isExtern(ref) == true);
      auto pObj = &o;
      auto returnPair = pOcs->search(pObj);
      BOOST_CHECK(returnPair.first == -2); //it's external?
      BOOST_CHECK(returnPair.second);
      delete pOcs;
    } 
  }
  
  BOOST_AUTO_TEST_CASE(ObjContainerWithPtr,* utf::expected_failures(1)){
    TestObj *pObj{};
    {
      pObj = new TestObj;
      ObjRegistryStub *pOcs = new ObjRegistryStub;
      //register an object
      IndexedObj ref=pOcs->registerObj(pObj);
      BOOST_CHECK(pOcs->isValid(ref));
      BOOST_CHECK(int(ref) == 1);
      //next one fails
      BOOST_CHECK(pOcs->isOwned(ref) == true);
      BOOST_CHECK(pOcs->isShared(ref) == false);
      BOOST_TEST(pOcs->isExtern(ref) == false);
      //
      auto returnPair = pOcs->search(pObj);
      BOOST_CHECK(returnPair.first == 0); //it's owned?
      BOOST_CHECK(returnPair.second);
      delete pOcs;
    } 
    delete pObj;
  }
  //haven't really understood the interaction of release, drop with the ObjContainer
  //but the only public interface is with ObjPtr...
  
  BOOST_AUTO_TEST_CASE(ObjPtrConstructors){
    Registry objRegister;
    TestObj externalObj;
    //default c'tor
    BOOST_CHECK_NO_THROW(Registrar def);
    BOOST_CHECK_NO_THROW(Registrar reg(objRegister,externalObj));
    //register same object twice; the test passes but aborts the test case
    //BOOST_CHECK_THROW(Registrar p(container,externalObj), std::logic_error);
    TestObj externalObj2;
    Registrar reg2(objRegister,externalObj2);
    //copy c'tor
    BOOST_CHECK_NO_THROW(Registrar reg3(reg2));
    /** The following would result in fatal exception upon destruction
      Container container2;
      auto pManagedObj = new TestObj();
      Registrar ptr(container2,pManagedObj);**/
    
  }
  BOOST_AUTO_TEST_CASE(ObjPtrMethods,* utf::expected_failures(1)){
    Registry objRegister;
    TestObj externalObj;
    Registrar reg(objRegister,externalObj);//previously tested
    //the following actually checks 'isValid' on the objRegister
    BOOST_CHECK(bool(reg));
    auto pObj = reg.get();
    BOOST_CHECK(pObj == &externalObj);
    BOOST_CHECK(not reg.isOwned());
    BOOST_CHECK(not reg.isShared());
    //the following fails
    BOOST_CHECK(reg.isExtern());
    
  }
BOOST_AUTO_TEST_SUITE_END()
