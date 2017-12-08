/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 "library" of SG test functions
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

#undef NDEBUG

// $Id: SGtests.cxx,v 1.42 2009-01-09 21:24:20 calaf Exp $

// INCLUDES
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include <boost/config.hpp>

#include "StoreGate/SGtests.h"
#include "TestTools/SGassert.h"

#ifndef NOGAUDI

#include "SGTools/DataHandleBase.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/SGWPtr.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataStore.h"
#include "SGTools/SGVersionedKey.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"

using namespace std;
using namespace SG;

using SG::DataStore;
using std::make_unique;

class Base {};
class Foo : public Base {
public:
  static std::vector<int> dtor_log;
  Foo() : m_i(0) {}
  Foo(int i) : m_i(i) {}
  int i() const { return m_i; }
  ~Foo() {
    dtor_log.push_back (m_i);
#ifdef MAKEITBOMB
    //int* ifg(0);
    //std::cout << *ifg << std::endl; 
#endif
    //    std::cout << "~Foo @" << this << " i() " << i() << std::endl;
  }
private:
  int m_i;
};
std::vector<int> Foo::dtor_log;
class Bar : public Base {};
class NotThere {};

class SillyKey {
public:
  SillyKey(const std::string& aString) : m_string(aString) {}
  operator std::string() const {return m_string;}

private:
  std::string m_string;

};

//-----------------------------------
static int b1_dtor = 0;
struct B1 { B1() : b(0){} virtual ~B1() { ++b1_dtor; } int b; };
struct D1 : virtual public B1 { int d; };
CLASS_DEF(B1, 8111, 1)
CLASS_DEF(D1, 8112, 1)
SG_BASE (D1, B1);



#include "SGTools/CLASS_DEF.h"
CLASS_DEF(Base, 8100, 1)
CLASS_DEF(Foo, 8101, 1)
CLASS_DEF(Bar, 8107, 1)
CLASS_DEF(NotThere, 8103, 1)

#include "AthContainersInterfaces/IAuxElement.h"
struct BX
  : public IAuxElement
{
  int x;
  BX(int the_x=0) : x(the_x), m_store(0), m_constStore(nullptr) {}

  bool hasStore() const { return m_store != 0; }
  void setStore (SG::IAuxStore* store) { m_store = store; }
  void setStore (SG::IConstAuxStore* store) { m_constStore = store; }
  bool usingStandaloneStore() const { return hasStore(); }
  SG::IAuxStore* getStore() { return m_store; }

  SG::IAuxStore* m_store;
  SG::IConstAuxStore* m_constStore;
};
struct BBX
  : public IAuxElement
{
  int x;
  BBX(int the_x=0) : x(the_x) {}
};
template <class T>
class  TestVector
  : public std::vector<T*>
{
public:
  typedef T base_value_type;
  TestVector() : m_constStore(0) {}
  void setStore (SG::IAuxStore* s) { m_constStore = s; }
  void setStore (const SG::IConstAuxStore* s) { m_constStore = s; }
  bool hasStore() const { return m_constStore != 0; }
private:
  const SG::IConstAuxStore* m_constStore;
};
CLASS_DEF( BX , 82835621, 1 )
CLASS_DEF( TestVector<BX> , 82735621, 1 )
CLASS_DEF( TestVector<BBX> , 125040193 , 1 )

class TestAuxStore
  : public SG::IAuxStore, public ILockable
{
public:
  TestAuxStore() : m_locked(false) {}
  virtual const void* getData (SG::auxid_t /*auxid*/) const { return 0; }
  virtual const SG::auxid_set_t& getAuxIDs() const { return m_set; }
  virtual void* getData (auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/) { return 0; }
  virtual const SG::auxid_set_t& getWritableAuxIDs() const { return m_set; }
  virtual bool resize (size_t /*sz*/) { return false; }
  virtual void reserve (size_t /*sz*/) {}
  virtual void shift (size_t /*pos*/, ptrdiff_t /*offs*/) {}
  virtual bool insertMove (size_t, IAuxStore&, const SG::auxid_set_t&) { std::abort(); }
  virtual void* getDecoration (auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/) { std::abort(); }
  virtual void lock() { m_locked = true; }
  virtual bool clearDecorations() { std::abort(); }
  virtual size_t size() const { std::abort(); }
  virtual void lockDecoration (SG::auxid_t) { std::abort(); }

  bool m_locked;

private:
  SG::auxid_set_t m_set;
};
CLASS_DEF( TestAuxStore , 125030194 , 1 )


class SymlinkDataObject
  : public DataBucketBase
{
public:
  SymlinkDataObject (CLID clid, void* obj) : m_clid (clid), m_obj (obj) {}
  virtual const CLID& clID() const override { return m_clid; }
  virtual void* object() override { return m_obj; }
  virtual const std::type_info& tinfo() const override { return typeid(Foo); }
  virtual void* cast (CLID, SG::IRegisterTransient*, bool) override { std::abort(); }
  virtual void* cast (const std::type_info&, SG::IRegisterTransient*, bool) override { std::abort(); }
  virtual DataBucketBase* clone() const override { std::abort(); }
  virtual void relinquish() override { std::abort(); }
  virtual void lock() override { std::abort(); }

  
private:
  CLID m_clid;
  void* m_obj;
};


#include "AthContainersInterfaces/CLASS_AUXSTORE.h"
CLASS_AUXSTORE3( TestVector<BBX> , TestAuxStore, TestAuxStore )

namespace Athena_test 
{
  static const bool LOCKED(false);
  static const bool RESET(true), DELETE(false);

  void checkCLIDs (::StoreGateSvc& rSG, const std::set<CLID>& expCLIDs)
  {
    std::vector<CLID> clids = rSG.clids();
    std::set<CLID> clid_set (clids.begin(), clids.end());
    if (expCLIDs != clid_set) {
      cout << "ERROR: CLID set mismatch\n";
      cout << "  Expected ";
      for (CLID id : expCLIDs) cout << id << " ";
      cout << "\n  Got: ";
      for (CLID id : clid_set) cout << id << " ";
      cout << "\n";
      std::abort();
    }
  }

  void testRecord(::StoreGateSvc& rSG) 
  {  
    cout << "*** StoreGateSvcClient_test record BEGINS ***" << endl;
    std::vector<CLID> initCLIDs = rSG.clids();
    std::set<CLID> expCLIDs (initCLIDs.begin(), initCLIDs.end());
    Foo* pFoo = new Foo(1);    
    //    cout << "pFoo=" << hex << pFoo << dec << endl;
    assert(rSG.record(pFoo, "pFoo1").isSuccess());
    expCLIDs.insert (ClassID_traits<Foo>::ID());
    checkCLIDs (rSG, expCLIDs);
    //can't record with same key
    SGASSERTERROR(rSG.record(new Foo(3), "pFoo1", LOCKED).isSuccess());
    //can't record same object twice
    SGASSERTERROR(rSG.record(pFoo, "pFoo2", !LOCKED).isSuccess());
    //check we haven't left any trace of "pFoo2" in DataStore
    assert(rSG.record(new Foo(2), "pFoo2", !LOCKED).isSuccess());

    const Foo* cpFoo = new Foo;
    assert(rSG.record(cpFoo, "cpFoo").isSuccess());

    /// cpFoo CLID is 8101 : check it
    assert (8101 == rSG.clid("cpFoo"));
    vector<CLID> ids(rSG.clids("cpFoo"));
    assert(1 == ids.size());
    
    SillyKey key("silly");
    assert(rSG.record(new Foo(4), key).isSuccess());
    //can't record with same key
    SGASSERTERROR(rSG.record(new Foo(5), key).isSuccess());
    SGASSERTERROR(rSG.record(new Foo(6), key, LOCKED).isSuccess());
#if __cplusplus > 201100
    {
      std::unique_ptr<Foo> foo5 (new Foo(5));
      SGASSERTERROR(rSG.record(std::move(foo5), key).isSuccess());
      assert (foo5.get() == 0);
    }
#endif

    assert(rSG.record(new Foo(7), "UnLocked", !LOCKED).isSuccess());
    assert(rSG.record(new Foo(8), "Locked", LOCKED).isSuccess());
    assert(rSG.record(new Foo(9), "LockedReset", LOCKED, RESET).isSuccess());
    assert(rSG.record(new Foo(10), "UnLockedReset", !LOCKED, RESET).isSuccess());
    assert(rSG.record(new Foo(11), "LockedDelete", LOCKED, DELETE).isSuccess());

#if __cplusplus > 201100
    {
      std::unique_ptr<Foo> foo12 (new Foo(12));
      assert(rSG.record(std::move(foo12),
                        "UnLockedDelete", !LOCKED, DELETE).isSuccess());
      assert(foo12.get() == 0);
    }
#else
    assert(rSG.record(new Foo(12), "UnLockedDelete", !LOCKED, DELETE).isSuccess());
#endif

    assert(rSG.record(cpFoo=new Foo(13), "Const").isSuccess());

#if __cplusplus > 201100
    std::unique_ptr<const Foo> foo13a (new Foo(130));
    assert(rSG.record(std::move(foo13a), "Const2").isSuccess());
    assert(foo13a.get() == 0);
#else
    assert(rSG.record(cpFoo=new Foo(130), "Const2").isSuccess());
#endif

    //FIXME!!! assert(rSG.record(cpFoo=new Foo(14), "ConstUnLocked", !LOCKED).isSuccess());
    SGASSERTERROR(rSG.record(cpFoo=new Foo(15), "Const").isSuccess());


    /// Test overwriting.
    assert (rSG.record(new Foo(101), "ow").isSuccess());
    assert (rSG.overwrite(new Foo(102), "ow").isSuccess());
#if __cplusplus > 201100
    assert (rSG.overwrite(make_unique<Foo>(103), "ow").isSuccess());
#else
    assert (rSG.overwrite(new Foo(103), "ow").isSuccess());
#endif

    assert (rSG.record(new Foo(104), "ow2", LOCKED).isSuccess());
    assert (rSG.overwrite(new Foo(105), "ow2", LOCKED).isSuccess());
#if __cplusplus > 201100
    assert (rSG.overwrite(make_unique<Foo>(106), "ow2", LOCKED).isSuccess());
#else
    assert (rSG.overwrite(new Foo(106), "ow2", LOCKED).isSuccess());
#endif

    /// 14 Foo objects recorded above : check it
    assert(rSG.typeCount<Foo>() == 14);

    checkCLIDs (rSG, expCLIDs);

    cout << "*** StoreGateSvcClient_test records OK ***\n\n" <<endl;
  }

  void testRemove(::StoreGateSvc& rSG) 
  {  
    cout << "*** StoreGateSvcClient_test remove BEGINS ***" << endl;
    // retrieve a keyless object: Should fail because ambiguous retrieve
    Foo* pFoo;
    SGASSERTERROR(rSG.retrieve(pFoo).isSuccess());

    /// retrieve a keyless object with default key: should also fail
    /// because of ambiguity
    SGASSERTERROR(rSG.retrieve(pFoo, SG::DEFAULTKEY).isSuccess());

    // record a keyed object and remove. this will only reset proxy
    pFoo = new Foo();
    assert(rSG.record(pFoo, "pFooKey").isSuccess());
    assert(rSG.remove(pFoo).isSuccess());
    cout << " Now we expect to see an error for invalid proxy >>> " << endl;
    assert(rSG.retrieve(pFoo, "pFooKey").isFailure());
    cout << " end of error >>>>>>>>>>>>> " << endl;

    pFoo = new Foo();
    assert(rSG.record(pFoo, "pFooKey").isSuccess());
    assert(rSG.remove(pFoo).isSuccess());
    assert(0 == rSG.proxy(pFoo));
    assert(0 != rSG.proxy(ClassID_traits<Foo>::ID(), "pFooKey"));
    const bool DONTCHECK(false);
    assert(0 != rSG.proxy(ClassID_traits<Foo>::ID(), "pFooKey", DONTCHECK));
    
    pFoo = new Foo();
    assert(rSG.record(pFoo, "pFooKey").isSuccess());
    assert(rSG.removeDataAndProxy(pFoo).isSuccess());
    assert(0 == rSG.proxy(pFoo));
    assert(0 == rSG.proxy(ClassID_traits<Foo>::ID(), "pFooKey"));
    assert(0 == rSG.proxy(ClassID_traits<Foo>::ID(), "pFooKey", DONTCHECK));
    cout << "*** StoreGateSvcClient_test removes OK ***\n\n" <<endl;
  }

  void testSymLink(::StoreGateSvc& rSG) 
  {  
    const Foo *cFoo;
    Foo *noFoo(0);
    Base* bDum(0);

    cout << "*** StoreGateSvcClient_test symlinks BEGINS ***" <<endl;
    SGASSERTERROR(rSG.retrieve(cFoo).isSuccess());
    SGASSERTERROR(rSG.symLink(cFoo, bDum).isSuccess());

#ifdef TEST_BOOST_CONCEPT_CHECK
    NotThere* notDum;
    //line below should not compile: Foo is not convertible to a NotThere! 
    rSG.symLink(cFoo, notDum);
#endif

    SGASSERTERROR(rSG.symLink(noFoo, bDum).isSuccess());


    assert(rSG.symLink(ClassID_traits<Foo>::ID(), 
		       "UnLocked", 
		       ClassID_traits<Base>::ID()).isSuccess()); 
    //let's check we got all clids for "UnLocked"
    vector<CLID> unids(rSG.clids("UnLocked"));
    assert (1 == count(unids.begin(), unids.end(), ClassID_traits<Foo>::ID()));
    assert (1 == count(unids.begin(), unids.end(), ClassID_traits<Base>::ID()));
    assert (2 == rSG.clids("UnLocked").size());

    Base* pB;
    assert(rSG.retrieve(pB, "UnLocked").isSuccess());

    //FIXME this will work when we'll have the inheritance tree from reflection
    //FIXME    SGASSERTERROR(rSG.symLink(ClassID_traits<Foo>::ID(), 
    //FIXME			      "UnLocked", 
    //FIXME			      ClassID_traits<NotThere>::ID()).isSuccess()); 
    SGASSERTERROR(rSG.symLink(ClassID_traits<Foo>::ID(), "is not there", 
		       ClassID_traits<Base>::ID()).isSuccess()); 



    //    assert(rSG.contains<Base>());
    assert(rSG.contains<Base>("UnLocked"));

    const Base* base;
    assert(rSG.retrieve(base).isSuccess());
    assert(rSG.retrieve(base, "UnLocked").isSuccess());
    SGASSERTERROR(rSG.retrieve(base, "modSully").isSuccess());

#ifdef TEST_DEPRECATED
    const DataHandle<Base> chBase;
    assert(rSG.retrieve(chBase).isSuccess());
    assert(rSG.retrieve(chBase, "UnLocked").isSuccess());

    DataHandle<Base> hBase;
    assert(rSG.retrieve(hBase).isSuccess());
    assert(rSG.retrieve(hBase, "UnLocked").isSuccess());

    const DataHandle<Base> chBaseBeg, chBaseEnd;
    assert(rSG.retrieve(chBaseBeg, chBaseEnd).isSuccess());
    assert(chBaseBeg != chBaseEnd);
#endif
#ifdef DHR_COMPILEERROR
    DataHandle<Base> hBaseBeg, hBaseEnd;
    assert(rSG.retrieve(hBaseBeg, hBaseEnd).isSuccess());
    assert(hBaseBeg != hBaseEnd);
#endif

    SG::ConstIterator<Base> ciBaseBeg,ciBaseEnd;
    assert(rSG.retrieve(ciBaseBeg, ciBaseEnd).isSuccess());
    assert(ciBaseBeg != ciBaseEnd);

    cout << "*** StoreGateSvcClient_test symlinks OK ***\n\n" <<endl;

  }

  void test_symlink2 (::StoreGateSvc& sg)
  {
    std::vector<CLID> initCLIDs = sg.clids();
    std::set<CLID> expCLIDs (initCLIDs.begin(), initCLIDs.end());

    // More symlink tests.
    // Check that we can put an object in and get the properly converted
    // base type out via the symlink.  Also tests auto symlink making.
    D1* d1 = new D1;
    B1* b1a = d1;
    assert ((void*)b1a != (void*)d1);

    assert (sg.record (d1, "d1", true, false).isSuccess());
    B1* b1b = 0;
    assert (sg.retrieve (b1b, "d1").isSuccess());
    assert (b1b == b1a);
    //get d1 proxy
    SG::DataProxy* dp = sg.proxy (ClassID_traits<D1>::ID(), std::string("d1"));
    assert (dp != 0);
    assert (dp->refCount() == 2); // since auto-symLink made
    std::cout << dp->store() << std::endl;
    std::cout << &sg << sg.name()  << std::endl;
    //    assert (dp->store() == &sg);

    expCLIDs.insert (ClassID_traits<B1>::ID());
    expCLIDs.insert (ClassID_traits<D1>::ID());
    checkCLIDs (sg, expCLIDs);

    // create alias with type, key
    assert (sg.setAlias(d1, "d1Alias").isSuccess());
    D1* d1Alias = 0;
    assert (sg.retrieve(d1Alias, "d1Alias").isSuccess());
    assert(d1Alias == d1);
    assert (dp->refCount() == 3); // add one alias

    // create alias with pointer
    assert (sg.setAlias(d1, "d1AnotherAlias").isSuccess());
    D1* d1AnotherAlias = 0;
    assert (sg.retrieve(d1AnotherAlias, "d1AnotherAlias").isSuccess());
    assert (d1AnotherAlias == d1);
    assert (dp->refCount() == 4); // add another alias

    // record another object
    D1* d2 = new D1;
    assert (sg.record(d2, "d2", true, false).isSuccess());

    // retrieve it
    D1* d2b = 0;
    assert (sg.retrieve(d2b, "d2").isSuccess());

    // set same alias as D1
    assert (sg.setAlias(d2, "d1Alias").isSuccess());

    // retrieve with alias and make sure that you get the new one.
    d1Alias = 0;
    assert (sg.retrieve(d1Alias, "d1Alias").isSuccess());
    assert (d1Alias == d2);

    // Check refcount handling for making duplicate symlinks.

    assert (dp->refCount() == 3);

    assert (sg.symLink (d1, (B1*)0).isSuccess());
    // Refcount should be the same.
    assert (dp->refCount() == 3);

    // Loop up by transient addr.  Both should work.
    assert (dp == sg.proxy (d1));
    assert (dp == sg.proxy (b1a));

    // Now remove it.  The symlink needs to go away too.
    assert (b1_dtor == 0);
    assert (sg.removeDataAndProxy(d1).isSuccess());
    assert (b1_dtor == 1);

    assert (!sg.contains<D1> ("d1"));
    assert (!sg.contains<B1> ("d1"));
    assert (0 == sg.proxy (d1));
    assert (0 == sg.proxy (b1a));

    // print all keys 
    std::vector<std::string> keyList;
    sg.keys<D1>(keyList, true);
    std::vector<std::string>::const_iterator iter = keyList.begin();
    for (; iter != keyList.end(); iter++) {
      std::cout << "Found key = " << *iter << " for object D1 in StoreGate " 
		<< std::endl; 
    } 
    /// type D1 was recorded with "d1" and "d2"
    /// two alias to d1 but sg.remove() remove alled d1
    /// one alias to d2... so size must be 2 after remove.
    assert(keyList.size() == 2);

  }


  //************************************************************************

  class TestConversion
    : public IConversionSvc
  {
  public:
    virtual StatusCode addConverter(IConverter*) { abort(); }
    virtual StatusCode addConverter(const CLID&) { abort(); }
    virtual StatusCode removeConverter(const CLID&) { abort(); }
    virtual IConverter* converter(const CLID&) { abort(); }
    virtual StatusCode connectOutput(const std::string&) { abort(); }
    virtual StatusCode connectOutput(const std::string&,
                                     const std::string&) { abort(); }
    virtual StatusCode commitOutput(const std::string&,
                                    bool) { abort(); }
    virtual StatusCode initialize() { abort(); }
    virtual StatusCode finalize() { abort(); }
    virtual const CLID& objType() const { abort(); }
    virtual long repSvcType() const { abort(); }
    virtual StatusCode setDataProvider(IDataProviderSvc*) { abort(); }
#ifdef ATLAS_GAUDI_V21
    virtual SmartIF<IDataProviderSvc>& dataProvider() const { abort(); }
    virtual StatusCode setConversionSvc(IConversionSvc*) { abort(); }
    virtual SmartIF<IConversionSvc>& conversionSvc()    const { abort(); }
    virtual StatusCode setAddressCreator(IAddressCreator*) { abort(); }
    virtual SmartIF<IAddressCreator>& addressCreator()    const { abort(); }
#else
    virtual IDataProviderSvc* dataProvider() const { abort(); }
    virtual StatusCode setConversionSvc(IConversionSvc*) { abort(); }
    virtual IConversionSvc* conversionSvc()    const { abort(); }
    virtual StatusCode setAddressCreator(IAddressCreator*) { abort(); }
    virtual IAddressCreator* addressCreator()    const { abort(); }
#endif
    virtual StatusCode fillObjRefs(IOpaqueAddress*, DataObject*) { abort(); }
    virtual StatusCode updateObj(IOpaqueAddress*, DataObject*) { abort(); }
    virtual StatusCode updateObjRefs(IOpaqueAddress*, DataObject*) { abort(); }
    virtual StatusCode createRep(DataObject*, IOpaqueAddress*&) { abort(); }
    virtual StatusCode fillRepRefs(IOpaqueAddress*, DataObject*) { abort(); }
    virtual StatusCode updateRep(IOpaqueAddress*, DataObject*)  { abort(); }
    virtual StatusCode updateRepRefs(IOpaqueAddress*, DataObject*) { abort(); }
    virtual StatusCode queryInterface(const InterfaceID&,
                                      void**) { abort(); }

    virtual StatusCode createObj(IOpaqueAddress*, DataObject*&);
    virtual unsigned long addRef() { return 0; }
    virtual unsigned long release() { return 0; }
  };


  StatusCode TestConversion::createObj (IOpaqueAddress* /*pAddress*/,
                                        DataObject*& refpObject)
  {
    D1* d1 = new D1;
    refpObject = SG::asStorable (d1);
    return StatusCode::SUCCESS;
  }


  class TestIOA
    : public IOpaqueAddress
  {
  public:
    virtual unsigned long        addRef     () { return 0; }
    virtual unsigned long        release    () { return 0; }
    virtual const CLID&          clID       () const { abort(); }
    virtual long                 svcType    () const { abort(); }
    virtual IRegistry*           registry   () const { abort(); }
    virtual void                 setRegistry(IRegistry*) { abort(); }
    virtual const std::string*   par        () const { abort(); }
    virtual const unsigned long* ipar       () const { abort(); }
  };
} // namespace Athena_test



namespace Athena_test {
  void test_symlink3 (::StoreGateSvc& sg)
  {
    // Test that t->proxy map is set correctly for auto-created
    // objs with symlinks.
    TestConversion cnv;

    IOpaqueAddress *pIOA(new TestIOA);

    SG::TransientAddress* taddr =
      new SG::TransientAddress (ClassID_traits<D1>::ID(), "dd",
                                pIOA, false);
    DataProxy* dp = new SG::DataProxy (taddr, &cnv);
    assert (sg.addToStore (ClassID_traits<D1>::ID(), dp).isSuccess());
    IOpaqueAddress *pIOAB(new TestIOA);
    taddr =
      new SG::TransientAddress (ClassID_traits<D1>::ID(), "dd",
                                pIOAB, false);
    taddr->setTransientID (ClassID_traits<B1>::ID());
    DataProxy* dpB = new SG::DataProxy (taddr, &cnv);
    assert (sg.addToStore (ClassID_traits<B1>::ID(), dpB).isSuccess());

    D1* d1 = 0;
    B1* b1 = 0;

    assert (sg.retrieve(d1, "dd").isSuccess());
    assert (sg.retrieve(b1, "dd").isSuccess());
    //    assert (d1 == b1);
    assert (d1->b == 0);
    d1->b = 1;

    assert (sg.proxy(d1) == dp);
    assert (sg.proxy(b1) == dpB);

    sg.clearStore();
    assert (sg.retrieve(d1, "dd").isSuccess());
    assert (sg.retrieve(b1, "dd").isSuccess());
    //    assert (d1 == b1);
    assert (d1->b == 0);

    assert (sg.proxy(d1) == dp);
    assert (sg.proxy(b1) == dpB);

    sg.removeDataAndProxy(d1);
    sg.removeDataAndProxy(b1);
    //FIXME WHY?
    delete pIOA;
    delete pIOAB;
  }



  //************************************************************************

  void testContains(::StoreGateSvc& rSG) 
  {  
    cout << "*** StoreGateSvcClient_test contains BEGINS ***" << endl;
    //    assert(rSG.contains<Foo>());
    //    assert(!rSG.contains<NotThere>());

    assert(rSG.contains<Foo>("UnLocked"));
    assert(!rSG.contains<Foo>("sully"));
    assert(rSG.transientContains<Foo>("UnLocked"));
    assert(!rSG.transientContains<Foo>("sully"));

    cout << "*** StoreGateSvcClient_test contains OK ***\n\n" <<endl;
  }

#ifdef FOLDERS_ASDOBJ
#include "SGTools/SGIFolder.h"
  void testFolders(::StoreGateSvc& rSG) 
  {  
    cout << "*** StoreGateSvcClient_test folders BEGINS ***" <<endl;
    //this of course depend on the job opts
    SG::IFolder *pMyFolder, *pYourFolder, *pNotFolder;
    assert(rSG.retrieve(pMyFolder, "MyFolder").isSuccess());
    assert( 2 == std::distance(pMyFolder->begin(), pMyFolder->end()) );
    assert(rSG.retrieve(pYourFolder, "YourFolder").isSuccess());
    SGASSERTERROR((rSG.retrieve(pNotFolder, "NotAFolder").isSuccess()));

    cout << "*** StoreGateSvcClient_test folders OK ***\n\n" <<endl;
  }
#endif

  void testRetrieve(::StoreGateSvc& rSG) {

    cout << "*** StoreGateSvcClient_test retrieve BEGINS ***" <<endl;
    const Foo *cFoo;
    const NotThere* notThere;
    SGASSERTERROR(rSG.retrieve(cFoo).isSuccess());
    SGASSERTERROR(rSG.retrieve(notThere).isSuccess());
    SGASSERTERROR(rSG.retrieve<Foo> () != 0);
    SGASSERTERROR(rSG.retrieve<const Foo> () != 0);

    assert(rSG.retrieve(cFoo, "UnLocked").isSuccess());
    assert (rSG.retrieve<const Foo> ("UnLocked") == cFoo);
    SGASSERTERROR(rSG.retrieve<const Foo> ("UnLockedxxx") != 0);

#ifdef TEST_DEPRECATED
    const DataHandle<Foo> chFoo;
    const DataHandle<NotThere> chNotThere;
    SGASSERTERROR(rSG.retrieve(chFoo).isSuccess());
    SGASSERTERROR(rSG.retrieve(chNotThere).isSuccess());

    assert(rSG.retrieve(chFoo, "UnLocked").isSuccess());
    SGASSERTERROR(rSG.retrieve(chFoo, "modSully").isSuccess());

    DataHandle<Foo> hFoo;
    SGASSERTERROR(rSG.retrieve(hFoo).isSuccess());

    assert(rSG.retrieve(hFoo, "silly").isSuccess());
    assert(rSG.setConst(hFoo.cptr()).isSuccess());
    SGASSERTERROR(rSG.retrieve(hFoo, "silly").isSuccess());
#endif

    SG::ConstIterator<Foo> ciFooBeg, ciFooEnd;
    assert(rSG.retrieve(ciFooBeg, ciFooEnd).isSuccess());
    assert(ciFooBeg != ciFooEnd);
    SG::ConstIterator<NotThere> ciNotThereBeg, ciNotThereEnd;
    assert(rSG.retrieve(ciNotThereBeg, ciNotThereEnd).isFailure());
    assert(ciNotThereBeg == ciNotThereEnd);

    cout << "*** StoreGateSvcClient_test retrieve OK ***\n\n" <<endl;
  }

  void testTryRetrieve(::StoreGateSvc& rSG) {

    cout << "*** StoreGateSvcClient_test tryRetrieve BEGINS ***" <<endl;
    cout << "we should see no WARNING message for failed retrieves" <<endl; 
    assert(rSG.tryRetrieve<Foo> () == 0);
    assert(rSG.tryConstRetrieve<Foo> () == 0);

    const Foo *cFoo;
    assert(rSG.retrieve(cFoo, "UnLocked").isSuccess());
    assert (rSG.retrieve<const Foo> ("UnLocked") == cFoo);
    assert(rSG.tryConstRetrieve<Foo> ("UnLockedxxx") == 0);

    cout << "*** StoreGateSvcClient_test tryRetrieve OK ***\n\n" <<endl;
  }


  void testReadPrivateCopy(::StoreGateSvc& rSG) {
    cout << "*** StoreGateSvcClient_test readPrivateCopy BEGINS ***" <<endl;
    const Foo *cFoo(0);
    assert(rSG.retrieve<Foo>(cFoo,"silly").isSuccess());
    std::unique_ptr<Foo> apFoo(rSG.readUniquePrivateCopy<Foo>("silly"));
    assert(cFoo != apFoo.get());
    assert(rSG.retrieve(cFoo, "silly").isSuccess());
    cout << "*** StoreGateSvcClient_test readPrivateCopy OK ***" <<endl;
  }


  void testRetrievePrivateCopy(::StoreGateSvc& rSG) {
    cout << "*** StoreGateSvcClient_test retrievePrivateCopy BEGINS ***" <<endl;
    const Foo *cFoo(0);
    assert(rSG.retrieve<Foo>(cFoo,"silly").isSuccess());
    std::unique_ptr<Foo> apFoo(rSG.retrieveUniquePrivateCopy<Foo>("silly"));
    assert(cFoo == apFoo.get());
    SGASSERTERROR(rSG.retrieve(cFoo, "silly").isSuccess());
    cout << "*** StoreGateSvcClient_test retrievePrivateCopy OK ***" <<endl;
  }


  void testBind(::StoreGateSvc& rSG) {

    cout << "*** StoreGateSvcClient_test bind BEGINS ***" <<endl;
    DataHandle<Foo> chFoo;
    Foo *cFoo = new Foo;
    std::string dbKey="fooKey";

    assert(rSG.record(cFoo,dbKey).isSuccess());

    assert(rSG.bind(chFoo,dbKey).isSuccess());
    assert(chFoo.ptr() == cFoo);

//FIXME      cout << "** bind it a second time with same key" << endl;
//      // try to bind it twice
//FIXME      assert(rSG.bind(chFoo,dbKey).isSuccess());

//FIXME      Foo *cFoo2 = new Foo;
//FIXME      std::string dbKey2="fooKey2";
//FIXME      assert(rSG.record(cFoo2,dbKey2).isSuccess());
//FIXME      cout << "** bind it a third time with a different key, obj: " 
//FIXME    	 << hex << cFoo << "  " << cFoo2 << dec << endl;
//      // try to bind it twice with a different key
//FIXME      assert(rSG.bind(chFoo,dbKey2).isSuccess());

    IProxyProviderSvc* pIPPSvc;
    assert((rSG.serviceLocator()->service("ProxyProviderSvc", pIPPSvc, true)).isSuccess());

//FIXME      TransientID id(ClassID_traits<Foo>::ID(), dbKey);
//FIXME      DataProxy *dp = pIPPSvc->getProxy(id, rSG);
//FIXME      assert (dp != 0);

//FIXME      //    rSG.clearStore().ignore();
//FIXME      dp->reset();

//FIXME      SGASSERTERROR(chFoo.ptr() != 0);

    cout << "*** StoreGateSvcClient_test bind OK ***\n\n" <<endl;

  }

  void testClear(::StoreGateSvc& rSG) {

    Foo* pFoo;    
    assert(rSG.record(pFoo=new Foo, "LockReset", LOCKED, RESET).isSuccess());
    assert(rSG.record(pFoo=new Foo, "UnLockReset", !LOCKED, RESET).isSuccess());
    assert(rSG.record(pFoo=new Foo, "LockDelete", LOCKED, DELETE).isSuccess());
    assert(rSG.record(pFoo=new Foo, "UnLockDelete", !LOCKED, DELETE).isSuccess());
    Bar* pBar = new Bar();
    Base* bDum(0);
    assert(rSG.record(pBar, "aBar", LOCKED, DELETE).isSuccess());
    assert(rSG.symLink(pBar, bDum).isSuccess());


    rSG.clearStore(true).ignore();
    rSG.clearStore().ignore();
 }

  void testTransientSwap(::StoreGateSvc& rSG) 
  {  
    cout << "\n*** StoreGateSvcClient_test transientSwap BEGINS ***" << endl;
    Foo* pFoo1 = new Foo(1);    
    //    cout << "pFoo1=" << hex << pFoo1 << dec << endl;
    assert(rSG.record(pFoo1, "pSwapFoo1").isSuccess());
    assert(pFoo1->i() == 1);
    pFoo1 = 0;

    Foo* pFoo2 = new Foo(2);
    //    cout << "p2Foo2=" << hex << pFoo2 << dec << endl;
    assert(rSG.record(pFoo2, "pSwapFoo2").isSuccess());
    assert(pFoo2->i() == 2);
    pFoo2 = 0;

    assert(rSG.transientSwap( 8101, "pSwapFoo1", "pSwapFoo2" ));
    assert(rSG.retrieve( pFoo1, "pSwapFoo1" ).isSuccess());
    assert(rSG.retrieve( pFoo2, "pSwapFoo2" ).isSuccess());
    assert(pFoo1->i() == 2);
    assert(pFoo2->i() == 1);

    cout << "*** StoreGateSvcClient_test transientSwap OK ***\n\n" <<endl;
  }

  void testVersionedKey(::StoreGateSvc& rSG) 
  {  
    cout << "\n*** StoreGateSvcClient_test VersionedKey BEGINS ***" << endl;
    //start by creating an unversioned object to test handling of legacy keys
    assert(rSG.record(new Foo(11), "aVersObj").isSuccess());
    const Foo* pFoo(0);
    assert(0 != (pFoo = rSG.retrieve<Foo>("aVersObj")));
    assert(pFoo->i() == 11);
    
    //try to put a VersionedKey on top
    VersionedKey myKey("aVersObj", 77);
    assert(rSG.record(new Foo(77), (std::string)myKey).isSuccess());
    const Foo* pFoo77(0);
    assert(0 != (pFoo77 = rSG.retrieve<Foo>(myKey)));
    assert(pFoo77->i() == 77);
    //test that we can retrieve the same object with an unversioned key
    assert(0 != (pFoo = rSG.retrieve<Foo>("aVersObj")));
    assert(pFoo->i() == 77);
    
    //check we can retrieve the old object with a default unversioned key
    VersionedKey defVK("aVersObj");
    assert(0 != (pFoo = rSG.retrieve<Foo>((std::string)defVK)));
    assert(pFoo->i() == 11);


    const std::string baseKey("aVersObj");
    VersionedKey my2Key(baseKey, 88);
    assert(rSG.record(new Foo(88), (std::string)my2Key).isSuccess());
    const Foo* pFoo88(0);
    assert(0 != (pFoo88 = rSG.retrieve<Foo>(my2Key)));
    assert(pFoo88->i() == 88);

    SGASSERTERROR(rSG.record(new Foo(66), (std::string)my2Key).isSuccess());
    VersionedKey my3Key(baseKey, 66);
    assert(rSG.record(new Foo(66), (std::string)my3Key).isSuccess());

    //test that a generic retrieve now returns the third recorded object
    //Notice how a generic retrieve will always return the last recorded
    //object with that key, independent from the numeric value of its version
    assert(0 != (pFoo = rSG.retrieve<Foo>(baseKey)));
    assert(pFoo->i() == 66);

    

    std::list<SG::ObjectWithVersion<Foo> > allVersions;
    assert(rSG.retrieveAllVersions(allVersions, baseKey));
    SG::ObjectWithVersion<Foo> highest;
    assert(rSG.retrieveHighestVersion(highest, baseKey));
    assert(highest.versionedKey.version() == 88);
    
    cout << "*** StoreGateSvcClient_test VersionedKey OK ***\n\n" <<endl;
  }

  void testKeys(::StoreGateSvc& rSG) {

    cout << "\n*** StoreGateSvcClient_test Keys BEGINS ***" << endl;
    rSG.clearStore().ignore();

    Foo* pFoo;    
    assert(rSG.record(pFoo=new Foo, "LockReset", LOCKED, RESET).isSuccess());
    assert(rSG.record(pFoo=new Foo, "UnLockReset", !LOCKED, RESET).isSuccess());
    assert(rSG.record(pFoo=new Foo, "LockDelete", LOCKED, DELETE).isSuccess());
    assert(rSG.record(pFoo=new Foo, "UnLockDelete", !LOCKED, DELETE).isSuccess());
    assert (rSG.setAlias(pFoo, "fooAlias").isSuccess());

    std::vector<std::string> keys;
    rSG.keys<Foo>(keys);
    //    copy(keys.begin(), keys.end(),ostream_iterator<string>(cout," - "));
    //    cout << endl;
    assert(4==keys.size());
    rSG.keys<Foo>(keys, /*allKeys=*/true);
    //    copy(keys.begin(), keys.end(),ostream_iterator<string>(cout," - "));
    //    cout << endl;
    assert(5==keys.size());

    rSG.clearStore().ignore();
    rSG.keys<Foo>(keys, /*allKeys=*/true, /*onlyValid=*/false);
    //    copy(keys.begin(), keys.end(),ostream_iterator<string>(cout," - "));
    //    cout << endl;
    assert(2==keys.size());

    rSG.keys<Foo>(keys);
    //    copy(keys.begin(), keys.end(),ostream_iterator<string>(cout," - "));
    //    cout << endl;
    assert(0==keys.size());

    //get rid of the two RESET dobjs
    rSG.clearStore(/*force=*/true).ignore();

    assert(rSG.record(pFoo=new Foo, "LockReset", LOCKED, DELETE).isSuccess());
    assert(rSG.record(pFoo=new Foo, "UnLockReset", !LOCKED, DELETE).isSuccess());
    assert(rSG.record(pFoo=new Foo, "LockDelete", LOCKED, DELETE).isSuccess());
    assert(rSG.record(pFoo=new Foo, "UnLockDelete", !LOCKED, DELETE).isSuccess());
    rSG.clearStore().ignore();
    rSG.keys<Foo>(keys);
    //    copy(keys.begin(), keys.end(),ostream_iterator<string>(cout," - "));
    //    cout << endl;

    assert(0==keys.size());

    rSG.keys<Foo>(keys, /*allKeys=*/true, /*onlyValid=*/false);
    //    copy(keys.begin(), keys.end(),ostream_iterator<string>(cout," - "));
    //    cout << endl;
    assert(0==keys.size());

    rSG.clearStore(/*force=*/true).ignore();

    cout << "\n*** StoreGateSvcClient_test Keys ENDS ***" << endl;

  }

  void testRetrieveAux(::StoreGateSvc& rSG) 
  {  
    cout << "\n*** StoreGateSvcClient_test retrieveAux BEGINS ***" << endl;
    TestVector<BX>* pVec = new TestVector<BX>;
    pVec->push_back(new BX(1));
    pVec->push_back(new BX(2));
    assert(rSG.record(pVec, "BVec").isSuccess());
    TestAuxStore* pAux = new TestAuxStore;
    assert(rSG.record(pAux, "BVecAux.").isSuccess());

    //testing const objects (locked as if coming from disk)    
    pVec = new TestVector<BX>;
    pVec->push_back(new BX(21));
    pVec->push_back(new BX(22));
    assert(rSG.record(pVec, "CVec", /*allowMods=*/false).isSuccess());
    pAux = new TestAuxStore;
    assert(rSG.record(pAux, "CVecAux.", /*allowMods=*/false).isSuccess());

    //testing missing AuxStore
    pVec = new TestVector<BX>;
    assert(rSG.record(pVec, "ErrorVec").isSuccess());
    
    //testing CLASS_AUX3
    TestVector<BBX>* pVecc = new TestVector<BBX>;
    pVecc->push_back(new BBX(1));
    pVecc->push_back(new BBX(2));
    assert(rSG.record(pVecc, "BBVec").isSuccess());
    pAux = new TestAuxStore;
    assert(rSG.record(pAux, "BBVecAux.").isSuccess());

    assert(rSG.retrieve(pVec,"BVec").isSuccess());    
    //second retrieve does not trigger retrieve of AuxStore
    assert( 0 != (pVec=rSG.retrieve<TestVector<BX> >("BVec")) );    

    const TestVector<BX>* cpVec(0);
    assert(rSG.retrieve(cpVec, "CVec").isSuccess());    
    // a regular retrieve ignores a missing aux store
    assert( 0 != (cpVec=rSG.retrieve<const TestVector<BX> >("ErrorVec")) );    
    // while a retrieveAux fails
    SGASSERTERROR( (cpVec=rSG.constRetrieveAux<TestVector<BX> >("ErrorVec")) ); 
    
    //deprecated but we need to test it nonetheless...
#ifdef TEST_DEPRECATED
    DataHandle<TestVector<BBX> > hBBX;
    assert(rSG.retrieve(hBBX, "BBVec").isSuccess());    
#endif
    
    // Test standalone object.
    BX* pb = new BX;
    assert(rSG.record(pb, "BStand").isSuccess());
    TestAuxStore* pAux_b = new TestAuxStore;
    assert(rSG.record(pAux_b, "BStandAux.").isSuccess());

    assert( 0 != (pb=rSG.retrieve<BX>("BStand")) );
    //assert (pb->usingStandAloneStore());
    //assert (pb->getStore() == pAux_b);
    
    cout << "*** StoreGateSvcClient_test retrieveAux OK ***\n\n" <<endl;
  }

#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
  void testCreate(::StoreGateSvc& rSG) 
  {  
    cout << "\n*** StoreGateSvcClient_test testCreate BEGINS ***" << endl;

    //recommended usage
    auto pFooD = rSG.create<Foo>("pCreateFooDefault");
    assert(rSG.contains<Foo>("pCreateFooDefault"));
    assert(pFooD->i() == 0);
    //if you really need to know the return type...
    SG::WPtr<Foo> pFoo1 = rSG.create<Foo>("pCreateFoo1", 1);
    assert(rSG.transientContains<Foo>("pCreateFoo1"));
    assert(pFoo1->i() == 1);
    //not allowed to overwrite an existing object
    SGASSERTERROR(rSG.create<Foo>("pCreateFoo1",3));
    //make sure we left the old one alone
    assert(rSG.retrieve<Foo>("pCreateFoo1")->i() == 1);

    cout << "*** StoreGateSvcClient_test testCreate OK ***\n\n" <<endl;
  }
#else
  void testCreate(::StoreGateSvc&) 
  {  
  }
#endif


  void testBoundReset(StoreGateSvc& rSG)
  {
    cout << "\n*** StoreGateSvcClient_test testBoundReset BEGINS ***" << endl;
    rSG.commitNewDataObjects();

    {
      SG::WriteHandle<int> h ("testBoundReset", rSG.name());
      h = std::make_unique<int> (10);
      assert (h.isValid());
      assert (*h.cachedPtr() == 10);
      rSG.commitNewDataObjects();
      assert (h.cachedPtr() == nullptr);
    }

    {
      SG::ReadHandle<int> h ("testBoundReset", rSG.name());
      assert (h.isValid());
      assert (*h.cachedPtr() == 10);
    }

    // Force the existing proxy to be deleted.
    assert (rSG.overwrite (std::make_unique<int> (20),
                           "testBoundReset",
                           true));
    rSG.commitNewDataObjects();

    cout << "\n*** StoreGateSvcClient_test testBoundReset OK ***\n\n" << endl;
  }


  void testRecordObject(StoreGateSvc& rSG)
  {
    cout << "\n*** StoreGateSvcClient_test testRecordObject BEGINS ***" << endl;
    Foo::dtor_log.clear();

    auto obj101_uptr = std::make_unique<Foo> (101);
    Foo* obj101_ptr = obj101_uptr.get();
    SG::DataObjectSharedPtr<DataObject> obj101 
      (SG::asStorable (std::move (obj101_uptr)));
    SG::DataProxy* proxy101 = rSG.recordObject (obj101, "obj101", false, false);
    assert (proxy101->name() == "obj101");
    assert (proxy101->object() == obj101.get());
    assert (obj101->refCount() == 2);
    assert (proxy101->refCount() == 1);
    assert (proxy101->isConst());

    SG::DataObjectSharedPtr<DataObject> obj102
      (SG::asStorable (std::make_unique<Foo> (102)));
    SG::DataProxy* proxy102 = rSG.recordObject (obj102, "obj102", true, false);
    assert (proxy102->name() == "obj102");
    assert (proxy102->object() == obj102.get());
    assert (obj102->refCount() == 2);
    assert (!proxy102->isConst());

    assert (Foo::dtor_log.empty());

    // Dup with returnExisting false.
    std::cout << ">>> test duplicate record1\n";
    SG::DataObjectSharedPtr<DataObject> obj103
      (SG::asStorable (std::make_unique<Foo> (103)));
    SG::DataProxy* proxy103 = rSG.recordObject (obj103, "obj101", false, false);
    assert (proxy103 == nullptr);
    assert (obj103->refCount() == 2); // Held by m_trash
    std::cout << "<<< test duplicate record1\n";

    // Dup with returnExisting true.
    SG::DataObjectSharedPtr<DataObject> obj104
      (SG::asStorable (std::make_unique<Foo> (104)));
    SG::DataProxy* proxy104 = rSG.recordObject (obj104, "obj101", false, true);
    assert (proxy104 == proxy101);
    assert (obj104->refCount() == 1);

    // Dup with returnExisting false but different key.
    std::cout << ">>> test duplicate record2\n";
    SG::DataProxy* proxy999 = rSG.recordObject (obj101, "obj999", false, false);
    assert (proxy999 == nullptr);
    assert (obj101->refCount() == 3); // Held by m_trash
    std::cout << "<<< test duplicate record2\n";

    // Making alias.
    assert (proxy101->refCount() == 1);
    proxy999 = rSG.recordObject (obj101, "obj999", false, true);
    assert (proxy999 == proxy101);
    assert (proxy101->refCount() == 2);
    assert (obj101->refCount() == 3);

    // Making symlink.
    SG::DataObjectSharedPtr<DataObject> obj105
      (new SymlinkDataObject (ClassID_traits<Base>::ID(), static_cast<Base*>(obj101_ptr)));
    SG::DataProxy* proxy105 = rSG.recordObject (obj105, "obj101", false, true);
    assert (proxy105 == proxy101);
    assert (obj101->refCount() == 3);
    assert (proxy101->refCount() == 3);

    // Error handling.
    std::cout << ">>> test duplicate error\n";
    SG::DataObjectSharedPtr<DataObject> obj106
      (new SymlinkDataObject (543543, static_cast<Base*>(obj101_ptr)));
    SG::DataProxy* proxy106 = rSG.recordObject (obj106, "obj101xxx", false, true);
    assert (proxy106 == nullptr);
    std::cout << "<<< test duplicate error\n";

    rSG.clearStore();
    assert (obj101->refCount() == 1);
    assert (obj102->refCount() == 1);
    assert (obj103->refCount() == 1);
    assert (obj104->refCount() == 1);

    SG::DataProxy* proxy101a= rSG.recordObject (obj101, "obj101", false, false);
    assert (proxy101 == proxy101a);
    assert (proxy101->name() == "obj101");
    assert (proxy101->object() == obj101.get());
    assert (obj101->refCount() == 2);
    assert (proxy101->refCount() == 3);
    assert (proxy101->isConst());

    rSG.clearStore();

    SG::DataProxy* proxy101b = rSG.recordObject (obj101, "obj101", false, true);
    assert (proxy101 == proxy101b);
    assert (proxy101->name() == "obj101");
    assert (proxy101->object() == obj101.get());
    assert (obj101->refCount() == 2);
    assert (proxy101->refCount() == 3);
    assert (proxy101->isConst());

    cout << "\n*** StoreGateSvcClient_test testRecordObject OK ***" << endl;
  }


  void testWriteAux(StoreGateSvc& rSG)
  {
    cout << "\n*** StoreGateSvcClient_test testWriteAux BEGINS ***" << endl;

    TestAuxStore* paux = nullptr;
    {
      SG::WriteHandle<BX> h ("testWriteAux", rSG.name());
      auto obj = std::make_unique<BX> (10);
      auto objAux = std::make_unique<TestAuxStore>();
      paux = objAux.get();
      assert (h.record (std::move(obj), std::move(objAux)).isSuccess());
      assert (!paux->m_locked);
    }
    cout << "\n*** StoreGateSvcClient_test testWriteAux OK ***" << endl;
  }

} //end namespace
#endif /*NOGAUDI*/
 
