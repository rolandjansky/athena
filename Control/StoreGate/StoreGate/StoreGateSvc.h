/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_STOREGATESVC_H
#define STOREGATE_STOREGATESVC_H

//FIXME the CLASS_DEF header is not needed here but was added to work around
//bad include statements in client code: when using CLASS_DEF put the
//include below in your header file!
#include "SGTools/CLASS_DEF.h"

//HIVE wrapper implementation
//base classes
#include "GaudiKernel/Service.h"
#include "AthenaKernel/IProxyDict.h"

#include "AthenaKernel/IHiveStore.h"
#include "AthenaKernel/IHiveStoreMgr.h"
#include "StoreGate/tools/SGImplSvc.h"
#include "StoreGate/SGHiveEventSlot.h"

#include <GaudiKernel/ClassID.h>        // for CLID
#include <GaudiKernel/IInterface.h>     // for InterfaceID
#include <GaudiKernel/IMessageSvc.h>    // for Level
#include "GaudiKernel/Property.h"   /*StringArrayProperty*/
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

#include <cstddef>                     // for size_t
#include <list>
#include <memory>                       /* auto_ptr */
#include <mutex>                       
#include <string>
#include <sys/types.h>                  // for off_t
#include <vector>                       

#include "AthenaKernel/StoreID.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/DefaultKey.h"
#include "AthAllocators/Arena.h"

#include <SGTools/StringPool.h> 
#include "SGTools/ProxyMap.h" /* for SG::ConstProxyIterator */

// includes used in StoreGateSvc.icc
#include "AthenaKernel/IResetable.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "StoreGate/SGHiveEventSlot.h"
#include "StoreGate/SGIterator.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/SGWPtr.h"
#include "SGTools/DataStore.h"
#include "StoreGate/SGObjectWithVersion.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#ifdef SG_DEPRECATION_WARNINGS
# define SG_DEPRECATED __attribute__((deprecated))
#else
# define SG_DEPRECATED
#endif

//forward declarations
namespace SG {
  class DataProxy;
  class TransientAddress;
  struct RemapImpl;
  class TestHiveStoreSvc;
  class HiveMgrSvc; 
}

class DataObject;
class IConversionSvc;
class IIncidentSvc;
class IProxyProviderSvc;
class IHistorySvc;

// python forward
struct _object; typedef _object PyObject;
class StoreGateSvc;
namespace AthenaInternal {
  PyObject* recordObjectToStore(StoreGateSvc*,PyObject*,PyObject*,bool,bool,bool);
  void py_sg_clearProxyPayload(StoreGateSvc* self, SG::DataProxy*);
}
namespace SG {
  class NoAuxStore;
}

//friends...
class AthenaOutputStream;
class IOVDbSvc;
class IOVSvc;
class IOVSvcTool;
class PileUpMergeSvc;
class SGDeleteAlg;
class ThinningSvc;
class ActiveStoreSvc;
class CondSvc;
namespace SG { 
  class VarHandleBase; 
}
namespace PerfMon { class StorePayloadMon; }

/** @class StoreGateSvc 
 * @brief The Athena Transient Store API.
 *
 * @param "Dump" property (default false): set to call dump() at EndEvent
 * @param "FolderNameList" property (default ""): data folders to be created 
 *                                                in this store
 *  Caches a pointer to the currently hive store. 
 *  In most jobs the hive store is simply the default event store 
 *  (named "StoreGateSvc"). When doing pile-up (and perhaps later on in 
 *  multi-threaded jobs, the hive store changes during the event loop
 *  execution. It is important, for example, that converters always refer
 *  to the hive store rather than to the default one.
 *
 *  @author ATLAS Collaboration
 *  $Id: SGStoreGateSvc.h 549999 2013-06-06 00:26:39Z calaf $
 **/
class StoreGateSvc :
  public Service, 
  public IProxyDict, 
  public IHiveStore,
  public IHiveStoreMgr,
  public IIncidentListener
{

public:
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
  /////////////////////////////////////////////////////////////////////////
  /// \name Basic Client Interface: data object creation
  //@{

  /** Create an object with one of its constructors and record it with a key.
   * @param key              a string (or an object convertible to a string) 
   *  identifying the object within the event
   * @param constructorArgs  a variable list of args passed to the constructor
   * @returns a "pointer" to the created object (set to 0 if creation failed)
   * Example: 
   *   struct Foo {
   *     int m_i;
   *     int i() const { return m_i; }
   *     Foo(int i) : m_i(i) {}
   *  };
   *  CLASS_DEF(Foo, 123456, 1);
   *    ....
   *  auto pFoo = pSG.create<Foo>("aFoo", 23);
   *  assert(pFoo->i()==23);
   *  assert(pSG.transientContains<Foo>("aFoo")); 
   */  

  template <typename T, typename TKEY, typename... ARGS>
  SG::WPtr<T> create(const TKEY& key, ARGS... constructorArgs);
  //@}
#endif   /* needs C++11 variadic templates to call arbitrary constructors */
#endif   /* needs "templated typedef" to define SG::WPtr (for now) */

  /////////////////////////////////////////////////////////////////////////
  /// \name Basic Client Interface: data object registration
  //@{

  /// Record an object with a key.
  template <typename T, typename TKEY> 
  StatusCode record(T* p2BRegistered, const TKEY& key);

  /// Record a const object with a key
  template <typename T, typename TKEY> 
  StatusCode record(const T* p2BRegistered, const TKEY& key);

  /// Record an object with a key, take ownership of the auto_pointed obj
  template <typename T, typename TKEY> 
  StatusCode record(std::auto_ptr<T> p2BRegistered, const TKEY& key);

  /// Record an object with a key, allow possibility of specifying 
  /// const-access. 
  template <typename T, typename TKEY> 
  StatusCode record(T* p2BRegistered, const TKEY& key, 
                    bool allowMods, bool resetOnly=true, bool noHist=false);

#if __cplusplus > 201100
  /// Record an object with a key, take ownership of the unique_ptr obj
  template <typename T, typename TKEY> 
  StatusCode record(std::unique_ptr<T> pUnique, const TKEY& key);

  /// Record a const object with a key
  template <typename T, typename TKEY> 
  StatusCode record(std::unique_ptr<const T> pUnique, const TKEY& key);

  /// Record an object with a key, allow possibility of specifying 
  /// const-access. 
  template <typename T, typename TKEY> 
  StatusCode record(std::unique_ptr<T> pUnique, const TKEY& key, 
                    bool allowMods, bool resetOnly=true, bool noHist=false);
#endif

  //@}

  /////////////////////////////////////////////////////////////////////////
  /// \name Basic Client Interface: data object access
  //@{

  /// Retrieve the default object into a const T*
  template <typename T> 
  StatusCode retrieve(const T*& ptr);

  /// Retrieve the default object into a T*
  template <typename T>
  StatusCode retrieve(T*& ptr);

  /// Variant of the above which doesn't return a status code.
  /// Just returns null if the object isn't found.
  template <typename T>
  T* retrieve ();

  /// Variant of the above which doesn't print a warning message.
  /// Just returns null if the object isn't found. Compare to contains
  template <typename T>
  T* tryRetrieve ();
  template <typename T>
  const T* tryConstRetrieve();

  /// Retrieve an object with "key", into a const T*
  template <typename T, typename TKEY> 
  StatusCode retrieve(const T*& ptr, const TKEY& key);

  /// Retrieve an object with "key", into a T*
  template <typename T, typename TKEY>
  StatusCode retrieve(T*& ptr, const TKEY& key);

  /// Variant of the above which doesn't return a status code.
  /// Just returns null if the object isn't found.
  template <typename T, class TKEY>
  T* retrieve (const TKEY& key);

  /// Variant of the above which doesn't print a warning message.
  /// Just returns null if the object isn't found. Compare to contains
  template <typename T, class TKEY>
  T* tryRetrieve (const TKEY& key);
  template <typename T, class TKEY>
  const T* tryConstRetrieve(const TKEY& key);

  /**
   * @brief try to associate a data object to its auxiliary store
   *        if ignoreMissing=false @returns false if the aux store is not found.
   * @param key The key to use for the lookup.
   **/
  template <class DOBJ>
  bool associateAux (DataHandle<DOBJ>&, bool ignoreMissing=true);
  template <class DOBJ>
  bool associateAux (const DataHandle<DOBJ>&, bool ignoreMissing=true);

  /// retrieve a data object deriving from DataVectorAuxBase,
  /// associate the data object to its matching IAuxStore.
  /// The matching is done by name, with the IAuxStore assumed to have key
  /// key + "Aux"
  /// returns null if the object or its IAuxStore isn't found.
  template <typename T, class TKEY>
  T* retrieveAux (const TKEY& key);
  template <typename T, class TKEY>
  const T* constRetrieveAux (const TKEY& key);





  /// Retrieve all objects of type T: returns an SG::ConstIterator range
  template <typename T> 
  StatusCode retrieve(SG::ConstIterator<T>& begin, 
                      SG::ConstIterator<T>& end);

  /** Look up a keyed object in TDS (compare also tryRetrieve)
   *  returns false if object not available in TDS or persistent stores 
   *  Usage: if (!p_store->contains<Foo>("fooKey")) { ... } */
  ///
  template <typename T, typename TKEY> 
  bool contains(const TKEY& key) const;

  /** A "once-per-job" retrieve that binds a data object to a DataHandle,
   *  typically a data member of an Algorithm/AlgTool. 
   *  At the end of every event, or more in general
   *  when the data object is not valid anymore, the DataHandle is reset,
   *  so that the next time the handle is accessed it will point to the
   *  current version of that data object.
   *  For example if MyAlg.h has a data member
   *    DataHandle<Foo> m_myFoo;
   *  after bind is called once per job, usually in MyAlg::initialize:
   *    sc = p_store->bind(m_myFoo, "MyFoo");
   *  m_myFoo will provide to access the current MyFoo e.g. in MyAlg::execute():
   *    m_myFoo->useMe();
   */
  template <typename T, typename TKEY> 
  StatusCode bind(const DataHandle<T>& handle, const TKEY& key);

  //@}

  /////////////////////////////////////////////////////////////////////////
  /// \name Advanced Client Interface: data object registration
  //@{

  /// Record an object with a key, overwriting any existing object with same key
  template <typename T, typename TKEY> 
  StatusCode overwrite(T* p2BRegistered, const TKEY& key);

  /// Record an object with a key, overwriting any existing object with same key
  template <typename T, typename TKEY> 
  StatusCode overwrite(T* p2BRegistered, const TKEY& key, 
                       bool allowMods, bool noHist=false);

  /// Record an object with a key, overwriting any existing object with same key, take ownership of the auto_pointed obj
  template <typename T, typename TKEY> 
  StatusCode overwrite(std::auto_ptr<T> p2BRegistered, const TKEY& key);

#if __cplusplus > 201100
  /// Record an object with a key, overwriting any existing object with same key
  template <typename T, typename TKEY> 
  StatusCode overwrite(std::unique_ptr<T> pUnique, const TKEY& key, 
                       bool allowMods, bool noHist=false);

  /// Record an object with a key, overwriting any existing object with same key, take ownership of the unique_ptr obj
  template <typename T, typename TKEY> 
  StatusCode overwrite(std::unique_ptr<T> pUnique, const TKEY& key);
#endif

  /// Create a proxy object using an IOpaqueAddress and a transient key
  StatusCode recordAddress(const std::string& skey,
                           IOpaqueAddress* pAddress, bool clearAddressFlag=true);
  /// Create a proxy object using an IOpaqueAddress
  StatusCode recordAddress(IOpaqueAddress* pAddress, bool clearAddressFlag=true);

  /// make a soft link to the object T* already registered (non-const)
  template <typename T, typename TLINK> 
  StatusCode symLink (const T* p2BRegistered, TLINK* p2BLinked );

  /// make a soft link to the object T* already registered (const link)
  template <typename T, typename TLINK> 
  StatusCode symLink (const T* p2BRegistered, const TLINK* p2BLinked );

  /// make a soft link to the object pointed by id/key
  template <typename TKEY> 
  StatusCode symLink (const CLID& id, const TKEY& key, const CLID& linkid);

  /// make an alias to a DataObject (provide data type and old key)
  template <typename T, typename TKEY, typename AKEY>
  StatusCode setAlias(const T* p2BAliased, const TKEY& key, const AKEY& aliasKey);

  /// make an alias to a DataObject (provide only valid pointer)
  template <typename T, typename AKEY>
  StatusCode setAlias(const T* p2BAliased, const AKEY& aliasKey);

  /// prevent downstream clients from modifying the pointed-at dobj
  StatusCode setConst(const void* pointer);

  /// Remove pObject,  will remove its proxy if not reset only.
  template <typename T>
  StatusCode remove(const T* pObject);

  /// Remove pObject and its proxy no matter what.       
  template <typename T>          
  StatusCode removeDataAndProxy(const T* pObject);

  /// @brief swap the content of 2 keys
  ///  payload A indexed by keyA will now be accessed via keyB and vice versa
  ///  Note that the swap is invalidated at event boundaries and/or when
  ///  somebody clear the store.
  /// @return false if swap failed
  bool transientSwap( const CLID& id,
                      const std::string& keyA, const std::string& keyB );

  //@}

  /////////////////////////////////////////////////////////////////////////
  /// \name Advanced Client Interface: data object access
  //@{

  /// Retrieve version with highest cycle number for a given T,KEY combination
  /// If there is only one available version of this data object
  /// the returned ObjectWithVersion<T>.versionedKey is set to requestedKey
  /// NOTICE that this method is significantly slower than 
  ///     retrieve(const T*, const TKEY&)
  /// which returns the last recorded version rather than the one with the
  /// highest cycle number.
  /// @returns StatusCode::FAILURE if no dataObject found
  template <typename T, class TKEY>
  StatusCode retrieveHighestVersion(SG::ObjectWithVersion<T>& dobjWithVersion, 
                                    const TKEY& requestedKey);

  /// Retrieve all versions of a given T,KEY combination
  /// sets allVersions, a ref to a vector of ObjectWithVersion<T>
  /// If there is only one available version of this dataObject
  /// allVersions[0].versionedKey is set to requestedKey.
  /// @returns StatusCode::FAILURE if no dataObject found
  //FIXME using a vector exposes ObjectWithVersion definition and hence
  //FIXME VersionedKey.h. Should try an iface like
  //FIXME  retrieveAllVersions(const TKEY& requestedKey,
  //FIXME                      ObjectWithVersion<T>& begin,
  //FIXME                      ObjectWithVersion<T>& end)
  template <typename T, class TKEY>
  StatusCode
  retrieveAllVersions(std::list< SG::ObjectWithVersion<T> >& allVersions,
                      const TKEY& requestedKey);

  

  /// EXPERTS ONLY: reads from disk your very own private copy of a StoreGate 
  /// object of type T and given key, if available and locked.
  /// readPrivateCopy does not look up the object in SG transient memory
  /// so it will fail to return a newly recorded object.
  /// You are responsible for managing the returned object and for keeping it
  /// in sync with the "main" copy in transient memory, if any.
  /// DEPRECATED: Prefer readUniquePrivateCopy.
  /// @param key The key to use for the lookup.
  /// @return null auto_ptr if the object isn't found or if it is unlocked.
  template <typename T>
  std::auto_ptr<T> readPrivateCopy (const std::string& key);

  /// EXPERTS ONLY: reads from disk your very own private copy of a StoreGate 
  /// object of type T and given key, if available and locked.
  /// readPrivateCopy does not look up the object in SG transient memory
  /// so it will fail to return a newly recorded object.
  /// You are responsible for managing the returned object and for keeping it
  /// in sync with the "main" copy in transient memory, if any.
  /// @param key The key to use for the lookup.
  /// @return null auto_ptr if the object isn't found or if it is unlocked.
  template <typename T>
  std::unique_ptr<T> readUniquePrivateCopy (const std::string& key);

  /// readPrivateCopy implementation (possibly useful from python hence public)
  DataObject*
  typeless_readPrivateCopy(const CLID& clid, const std::string& key);


  /// EXPERTS ONLY: like readPrivateCopy this method returns your own private
  /// copy of a data object of type T and given key, if available and locked.
  /// The difference is that it will return you also an object in SG transient 
  /// memory. Since we are taking ownership of it, the object in SG
  /// will be released after retrievePrivateCopy returns, making
  /// all cached references to the object potentially invalid and
  /// obviously preventing it from being written out.
  /// Sequencing becomes critical. Caveat emptor!
  ///
  /// DEPRECATED: Prefer retrieveUniquePrivateCopy.
  ///
  /// @param key The key to use for the lookup.
  /// @return null auto_ptr if the object isn't found or if it is unlocked.
  template <typename T>
  std::auto_ptr<T> retrievePrivateCopy (const std::string& key);

  /// EXPERTS ONLY: like readPrivateCopy this method returns your own private
  /// copy of a data object of type T and given key, if available and locked.
  /// The difference is that it will return you also an object in SG transient 
  /// memory. Since we are taking ownership of it, the object in SG
  /// will be released after retrievePrivateCopy returns, making
  /// all cached references to the object potentially invalid and
  /// obviously preventing it from being written out.
  /// Sequencing becomes critical. Caveat emptor!
  ///
  /// @param key The key to use for the lookup.
  /// @return null auto_ptr if the object isn't found or if it is unlocked.
  template <typename T>
  std::unique_ptr<T> retrieveUniquePrivateCopy (const std::string& key);

  /// Retrieve the main @c CLID of the object recorded in @c StoreGate
  /// with the given "key" 
  /// WARNING: slow!
  template<typename TKEY>
  CLID clid( const TKEY& key ) const;

  /// Retrieve all the @c CLID s (including symlinks) of the object recorded in @c StoreGate
  /// with the given "key" 
  /// WARNING: slow!
  template<typename TKEY>
  std::vector<CLID> clids( const TKEY& key ) const;  

  /** Return the number of instances of an object of type T 
   * int i = p_store->typeCount<T>();     
   * Note that this will return the number of proxies in transient memory
   * only, will not check with proxy providers. */
  template <typename T>
  int typeCount() const;

  /** Return the number of instances of type T (input CLID) */
  int typeCount(const CLID& id) const;

  /** Look up a keyed object in TDS by CLID.
   *  returns false if object not available in TDS or persistent stores 
   *  Usage: if (!p_store->contains(FooID, "fooKey")) { ... } */
  template <typename TKEY>
  bool contains(const CLID& id, const TKEY& key) const;


  /** Look up a transient data object in TDS only (no Proxy lookup)
   *  returns false if object not available in TDS 
   *  Usage: if (!p_store->contains<Foo>("fooKey")) { ... } */
  ///
  template <typename T, typename TKEY> 
  bool transientContains(const TKEY& key) const;

  /** Look up a transient data object in TDS only by CLID.
   *  returns false if object not available in TDS */
  template <typename TKEY>
  bool transientContains(const CLID& id, const TKEY& key) const;

  /** dump objects in store. request forwarded to DataStore
   * this is triggered at EndEvent setting the Dump property to true */
  std::string dump() const;

  /// set store ID. request forwarded to DataStore:
  void setStoreID(StoreID::type id);
  /// get store ID. request forwarded to DataStore:
  StoreID::type storeID() const;


  /** provide list of all StoreGate keys associated with an object.
   *  usage: p_store->keys<T>(vkeys, optional flags);
   *  @param vkeys will be filled with the (possibly empty) list of keys
   *  @param includeAlias (default false) add alias keys as well
   *  @param onlyValid (default true) add only keys of valid dobjs
   */
  template <typename T>
  void
  keys(std::vector<std::string>& vkeys, 
       bool includeAlias = false, bool onlyValid = true); 
 
  /** provide list of all StoreGate keys associated with an object.
   *  usage: p_store->keys(CLID, vkeys, optionalFlags);
   *  @param id CLID for which we are requesting list of keys
   *  @param vkeys will be filled with the (possibly empty) list of keys
   *  @param includeAlias (default false) add alias keys as well
   *  @param onlyValid (default true) add only keys of valid dobjs
   */
  void
  keys(const CLID& id, std::vector<std::string>& vkeys, 
       bool includeAlias = false, bool onlyValid = true); 

  //@}


  /// implements IHiveStore interface
  // FIXME pointer to unsafe impl exposed 
  virtual ::IProxyDict* hiveProxyDict() override final {
    return StoreGateSvc::currentStore();
  }


  ///////////////////////////////////////////////////////////////////////
  /// \name IHiveStoreMgr implementation
  //@{
  /// clear DataStore contents: called by the event loop mgrs
  /// @param forceRemove: if true remove proxies ignoring their resetOnly flag
  virtual StatusCode clearStore(bool forceRemove=false) override final;

  /** Get data objects registered in store since last getNewDataObjects call (or since init for 1st call)
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode getNewDataObjects(DataObjIDColl& products) override final;

  /** Check if something has been added to the store since last getNewDataObjects call
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Boolean indicating the presence of new products
   */
  virtual bool newDataObjectsPresent() override final; 

  /** make newly recorded DataObjects know to the WhiteBoard, by copying
   *    from thread local storage to m_newDataObjects
   */
  virtual void commitNewDataObjects() override final;
  //@}

  ///a new transient object has been recorded
  void addedNewTransObject(CLID clid, const std::string& key);

  void addedNewPersObject(CLID clid, SG::DataProxy* dp);

  ///set the hive event slot pointer: used by the event loop mgrs
  static void setSlot(SG::HiveEventSlot* pSlot);


  ///set pointer to default event store: used by ActiveStoreSvc
  void setDefaultStore(SGImplSvc* pStore);                    


  /////////////////////////////////////////////////////////////////////////
  /// \name IOVSvc interface
  //@{

  template <typename H, typename TKEY>
  StatusCode regHandle( const DataHandle<H>& handle, const TKEY& key );

  /// non-const method - will return an error
  template <typename H, typename TKEY>
  StatusCode regHandle( DataHandle<H>& handle, const TKEY& key);

  /// register a callback function, with handle + key
  template <typename T, typename H, typename TKEY>
  StatusCode regFcn(StatusCode (T::*updFcn)(IOVSVC_CALLBACK_ARGS), 
                    const T* obj, const DataHandle<H>& handle, 
                    const TKEY& key, bool trigger=false);

  /// register a callback function, with handle + key. Non const. Error
  template <typename T, typename H, typename TKEY>
  StatusCode regFcn(StatusCode (T::*updFcn)(IOVSVC_CALLBACK_ARGS), 
                    const T* obj, DataHandle<H>& handle, 
                    const TKEY& key, bool trigger=false);

  /// register a callback function(2) with an already registered function(1)
  template <typename T1, typename T2>
  StatusCode regFcn(StatusCode (T1::*fcn1)(IOVSVC_CALLBACK_ARGS), 
                    const T1* obj1,
                    StatusCode (T2::*fcn2)(IOVSVC_CALLBACK_ARGS), 
                    const T2* obj2, bool trigger=false);

  /// register a callback function(2) with an already registered AlgTool
  template <typename T2>
  StatusCode regFcn(const std::string& toolName,
                    StatusCode (T2::*fcn2)(IOVSVC_CALLBACK_ARGS), 
                    const T2* obj2, bool trigger=false);

  //@}
  /////////////////////////////////////////////////////////////////////////

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy(const void* const pTransient) const override final;

  /// get default proxy with given id. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id) const final;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const override final;
  /// get proxy with given id and key. Returns 0 to flag failure
  /// (overload to prevent a char* to be interpreted as a bool.)
  SG::DataProxy* proxy(const CLID& id, const char* key) const;

  /// Raw addition of a proxy to the store.
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy) override final;

  /**
   * @brief Record an object in the store.
   * @param obj The data object to store.
   * @param key The key as which it should be stored.
   * @param allowMods If false, the object will be recorded as const.
   * @param returnExisting If true, return proxy if this key already exists.
   *                       If the object has been recorded under a different
   *                       key, then make an alias.
   *                       If the object has been recorded under a different
   *                       clid, then make a link.
   *
   * Full-blown record.  @c obj should usually be something
   * deriving from @c SG::DataBucket.
   *
   * Returns the proxy for the recorded object; nullptr on failure.
   * If the requested CLID/key combination already exists in the store,
   * the behavior is controlled by @c returnExisting.  If true, then
   * the existing proxy is returned; otherwise, nullptr is returned.
   * In either case, @c obj is destroyed.
   */
  virtual
  SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                               const std::string& key,
                               bool allowMods,
                               bool returnExisting) override final;


  /**
   * @brief Inform HIVE that an object has been updated.
   * @param id The CLID of the object.
   * @param key The key of the object.
   */
  virtual
  StatusCode updatedObject (CLID id, const std::string& key) override final;


  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const override final;
    

  //@}

  /// \name more proxy dictionary functionality
  //@{
  /// get default proxy with given id, optionally checking validity.
  ///  @returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, bool checkValid) const final;
  /// get proxy with given id and key, optionally checking validity.
  ///  @returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key, bool checkValid) const final;
  /// get proxy with given id and key, optionally checking validity.
  ///  @returns 0 to flag failure
  /// (overload to prevent a char* to be interpreted as a bool.)
  virtual SG::DataProxy* proxy(const CLID& id, const char* key, bool checkValid) const final
  { return this->proxy(id, std::string(key), checkValid); }

  /// return the list of all current proxies in store
  virtual std::vector<const SG::DataProxy*> proxies() const override final;

  /// Return all CLIDs in the store.
  std::vector<CLID> clids() const;

  /// get proxy with given id and key. Does not query ProxyProviderSvc.
  ///  @returns 0 to flag failure
  SG::DataProxy* transientProxy(const CLID& id, const std::string& key) const;

  /// find proxy and access its data. Returns 0 to flag failure
  DataObject* accessData(const CLID& id) const;
  /// find proxy and access its data. Returns 0 to flag failure
  DataObject* accessData(const CLID& id, const std::string& key) const;

  /// associate ProxyProviderSvc to this store
  void setProxyProviderSvc(IProxyProviderSvc* pPPSvc);
  /// Return current ProxyProviderSvc.
  IProxyProviderSvc* proxyProviderSvc();
  //@}

  /////////////////////////////////////////////////////////////////////////
  /// \name String <-> int key conversions.
  //@{

  typedef IStringPool::sgkey_t sgkey_t;

  /**
   * @brief Find the key for a string/CLID pair.
   * @param str The string to look up.
   * @param clid The CLID associated with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will abort in case of a hash collision!
   */
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid) override final;

  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key) const override final;

  /**
   * @brief Find the string and CLID corresponding to a given key.
   * @param key The key to look up.
   * @param clid[out] The found CLID.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key, CLID& clid) const override final;

  /**
   * @brief Remember an additional mapping from key to string/CLID.
   * @param key The key to enter.
   * @param str The string to enter.
   * @param clid The CLID associated with the string.
   * @return True if successful; false if the @c key already
   *         corresponds to a different string.
   *
   * This registers an additional mapping from a key to a string;
   * it can be found later through @c lookup() on the string.
   * Logs an error if @c key already corresponds to a different string.
   */
  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clidid) override final;

  //@}

  /////////////////////////////////////////////////////////////////////////
  /// \name ElementLink remapping
  //@{


  /**
   * @brief Declare a remapping.
   * @brief clid Class ID of the container being remapped.
   * @brief source Key of the container being remapped.
   * @brief target Key of the container being remapped to.
   * @brief index_offset Amount by which the index should be adjusted
   *        between the two containers.
   *
   * This can be used to change the container to which @c ElementLink's
   * (and @c DataLink's) point on output.
   *
   * For example, suppose you have two containers of type T, A and B.
   * There possibly are @c ElementLink's pointing at elements contained
   * in them.  You want to change to a single container C, containing
   * the contents of A and B concatenated.  To get @c ElementLink's
   * updated on output, you can do:
   *
   *@code
   *   m_sg->remap (ClassID_traits<T>::ID(), "A", "C", 0);
   *   m_sg->remap (ClassID_traits<T>::ID(), "B", "C", a.size());;
   @endcode
  */
  template <class TKEY>
  void remap (CLID clid,
              const TKEY& source,
              const TKEY& target,
              off_t index_offset);


  /**
   * @brief Declare a remapping.
   * @brief source Key hash of the container being remapped.
   * @brief target Key hash of the container being remapped to.
   * @brief index_offset Amount by which the index should be adjusted
   *        between the two containers.
   */
  void remap_impl (sgkey_t source,
                   sgkey_t target,
                   off_t index_offset);


  /**
   * @brief Test to see if the target of an ElementLink has moved.
   * @param sgkey_in Original hashed key of the EL.
   * @param index_in Original index of the EL.
   * @param sgkey_out[out] New hashed key for the EL.
   * @param index_out[out] New index for the EL.
   * @return True if there is a remapping; false otherwise.
   */
  virtual bool tryELRemap (sgkey_t sgkey_in, size_t index_in,
                           sgkey_t& sgkey_out, size_t& index_out) override final;

  //@}

  //////////////////////////////////////////////////////////////////
  /// \name Gaudi Standard Service constructors/destructors
  //@{
  StoreGateSvc(const std::string& name, ISvcLocator* svc);
  virtual ~StoreGateSvc() override;
  //@}

  ///////////////////////////////////////////////////////////////////////
  /// \name Gaudi IService implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
  //@}
  /// Should rather be in IStoreGateSvc.h if we had one
  static const InterfaceID& interfaceID();


  //////////////////////////////////////////////////////////////////
  /// \name Gaudi IIncidentListener implementation
  //@{
  virtual void handle(const Incident&) override final;  
  /// load proxies at begin event
  StatusCode loadEventProxies();
  //@}


  /**
   * @brief Call converter to create an object, with locking.
   * @param cvt The converter to call.
   * @param addr Opaque address information for the object to create.
   * @param refpObject Reference to location of the pointer of the
   *                   created object.
   *
   * This calls the @c createObj method on @c cvt to create the referenced
   * transient object, locking the store during the call.
   */
  virtual
  StatusCode createObj (IConverter* cvt,
                        IOpaqueAddress* addr,
                        DataObject*& refpObject) override;


private:

  SGImplSvc* m_defaultStore;
  ServiceHandle<IProxyProviderSvc> m_pPPSHandle; ///< property
  ServiceHandle<IIncidentSvc> m_incSvc; ///< property
  ServiceHandle<ActiveStoreSvc> m_activeStoreSvc; ///< property

  friend class SG::TestHiveStoreSvc;
  static SG::HiveEventSlot* currentSlot();

  friend class SG::HiveMgrSvc;
  ///returns pointer to the current SGImplSvc
  SGImplSvc* currentStore() const;
  ///is the current store an event store being managed by an IHiveWhiteboard?
  bool isHiveStore() const;


  ///access proxyRange()
  friend class AthenaOutputStream;
  ///return a range to all proxies of a given CLID
  StatusCode proxyRange(const CLID& id,
                        SG::ConstProxyIterator& beg,
                        SG::ConstProxyIterator& end) const; 

  ///access releaseObject
  friend class TileInfoLoader;
  /// release object held by proxy, if any. Gives up ownership 
  /// (somebody else must take charge)
  void releaseObject(const CLID& id, const std::string& key);


  /// access emptyTrash
  friend class PileUpMergeSvc;
  ///access  clearProxyPayload
  friend class IOVDbSvc;
  friend class IOVSvcTool;
  friend class SGDeleteAlg;
  friend
  void
  AthenaInternal::py_sg_clearProxyPayload(StoreGateSvc*, SG::DataProxy*);
  friend class PerfMon::StorePayloadMon;

  /// use to reset a proxy (clearing the data object it contains)
  /// Unlike DataProxy::reset this method correctly updates SGSvc internals
  void clearProxyPayload(SG::DataProxy*);

  ///access typeless_record
  friend class ThinningSvc;
  friend class SG::VarHandleBase;
  friend 
  PyObject* 
  AthenaInternal::recordObjectToStore(StoreGateSvc*,PyObject*,PyObject*,bool,bool,bool);
  /// type-less recording of an object with a key, allow possibility of
  /// specifying const-access and history record
  StatusCode typeless_record( DataObject* obj, const std::string& key,
                              const void* const raw_ptr,
                              bool allowMods, bool resetOnly=true,
                              bool noHist=false );
  /// same as typeless_record, allows to overwrite an object in memory or on disk
  StatusCode typeless_overwrite( const CLID& id,
                                 DataObject* obj, const std::string& key,
                                 const void* const raw_ptr,
                                 bool allowMods,
                                 bool noHist=false,
                                 const std::type_info* tinfo=0);

  bool isSymLinked(const CLID& linkID, SG::DataProxy* dp);

  StatusCode addSymLink(const CLID& linkid, SG::DataProxy* dp);

  StatusCode addAlias(const std::string& aliasKey, SG::DataProxy* dp);

  /// creates a key internally if none specified by client
  std::string createKey(const CLID& dataID);

  /// try to locate a proxy or create it if needed
  SG::DataProxy* setupProxy(const CLID& dataID, 
                            const std::string& gK, 
                            DataObject* pDObj,
                            bool allowMods,
                            bool resetOnly);

  ///put a bad (unrecordable) dobj away
  void recycle(DataObject* pBadDObj);  
  ///throw away bad objects
  void emptyTrash();                   
  
  ///name says it all
  bool bindHandleToProxy(const CLID& id, const std::string& key,
                         IResetable* ir, SG::DataProxy*& dp);

  /// remove proxy from store, unless it is reset only.         
  /// provide pTrans!=0 (must match proxy...) to save time
  /// @param forceRemove remove the proxy no matter what
  /// DO NOT USE THIS FUNCTION!
  /// IT IS UNSAFE AND IS LIKELY TO BE REMOVED!
  StatusCode removeProxy(SG::DataProxy* proxy, const void* pTrans, 
                         bool forceRemove=false);
  ///forwarded to DataStore
  StatusCode t2pRegister(const void* const pTrans, SG::DataProxy* const pPers);
  ///forwarded to DataStore
  void t2pRemove(const void* const pTrans);

  /// callback for output level property 
  void msg_update_handler(Property& outputLevel);

  template <class DOBJ, class AUXSTORE>
  bool associateAux_impl(DataHandle<DOBJ>& handle, const AUXSTORE*);
  template <class DOBJ, class AUXSTORE>
  bool associateAux_impl(const DataHandle<DOBJ>& handle, const AUXSTORE*);
  template <class DOBJ>
  bool associateAux_impl(DataHandle<DOBJ>& , const SG::NoAuxStore*) { return true; }
  template <class DOBJ>
  bool associateAux_impl(const DataHandle<DOBJ>&, const SG::NoAuxStore*) { return true; }

  /// Add automatically-made symlinks for DP.
  void addAutoSymLinks (const std::string& key, CLID clid, SG::DataProxy* dp,
                        const std::type_info* tinfo,
                        bool warn_nobib = true);

  friend class ActiveStoreSvc;
  /// The current store is becoming the active store.  Switch the
  /// allocation arena, if needed.
  /// Only intended to be called by ActiveStoreSvc.
  void makeCurrent();

  bool m_DumpStore; ///<  property Dump: triggers dump() at EndEvent 
  bool m_ActivateHistory; ///< property: activate the history service

  ///get the IOVSvc "just in time" (breaks recursion at initialize)
  IIOVSvc* getIIOVSvc();
  IIOVSvc* m_pIOVSvc;

public:
  ///////////////////////////////////////////////////////////////////////
  /// \name Obsolete and Deprecated methods 
  //@{
  /// DEPRECATED: Retrieve the default object into a const DataHandle
  template <typename T> 
  StatusCode SG_DEPRECATED retrieve(const DataHandle<T>& handle);

  /// DEPRECATED: Retrieve the default object into a DataHandle
  template <typename T> 
  StatusCode SG_DEPRECATED retrieve(DataHandle<T>& handle);

  /// DEPRECATED: Retrieve an object with "key", into a const DataHandle
  template <typename T, typename TKEY> 
  StatusCode SG_DEPRECATED retrieve(const DataHandle<T>& handle, const TKEY& key);
  /// DEPRECATED: Retrieve an object with "key", into a DataHandle
  template <typename T, typename TKEY> 
  StatusCode SG_DEPRECATED retrieve(DataHandle<T>& handle, const TKEY& key);

  /// DEPRECATED Retrieve all objects of type T: use iterators version instead
  template <typename T> 
  StatusCode SG_DEPRECATED retrieve(const DataHandle<T>& begin, 
                                                  const DataHandle<T>& end);
  /// DEPRECATED, use version taking ref to vector
  template <typename T>
  std::vector<std::string> //FIXME inefficient. Should take ref to vector
  SG_DEPRECATED keys(bool allKeys = false); 
 
  /// DEPRECATED, use version taking ref to vector
  std::vector<std::string> //FIXME inefficient. Should take ref to vector 
  SG_DEPRECATED keys(const CLID& id, bool allKeys = false);

  /// DEPRECATED:  use recordAddress instead
  StatusCode SG_DEPRECATED createProxy(IOpaqueAddress* pAddress, bool clearAddressFlag=true) {
    return recordAddress(pAddress, clearAddressFlag);
  }  

  /// DEPRECATED put a dobj pointer in a bucket as appropriate
  /// see tools/StorableConversion.h for replacement
  template <typename T>
  static 
  DataObject* SG_DEPRECATED asStorable(T* pDObj);

  /// DEPRECATED gets a dobj pointer from a bucket as appropriate
  /// see tools/StorableConversion.h for replacement
  template <typename T>
  static 
  bool SG_DEPRECATED fromStorable(DataObject* pObject, T*& pData);

  //@}


private:
  StoreGateSvc (const StoreGateSvc&);
  StoreGateSvc& operator= (const StoreGateSvc&);
};


/// These are intended to be easy to call from the debugger.
void SG_dump (StoreGateSvc* sg);
void SG_dump (StoreGateSvc* sg, const char* fname);


#include "StoreGate/StoreGateSvc.icc"


//- PyGate: StoreGate access from python -------------------------------------
template< class T >
struct PyGate {
  // default object retrieval
  static const T* retrieve( StoreGateSvc* psg ) {
    const T* obj = 0;
    if ( StatusCode::SUCCESS == psg->retrieve( obj ) )
      return obj;
    return 0;
  }

  // object retrieval with string key
  static const T* retrieve( StoreGateSvc* psg, const std::string& key ) {
    const T* obj = 0;
    if ( StatusCode::SUCCESS == psg->retrieve( obj, key ) )
      return obj;
    return 0;
  }
};

#endif // STOREGATE_STOREGATESVC_H
