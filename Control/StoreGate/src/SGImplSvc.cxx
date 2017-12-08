/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define SLOW_NEWDATAOBJECTS 1
#include <algorithm>
#include <cassert>
#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>

#include <sstream>
#include <fstream>
#include <iomanip>

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/CLIDRegistry.h"
#include "AthenaKernel/errorcheck.h"
//#include "CxxUtils/PageAccessControl.h"
#include "GaudiKernel/IHistorySvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataHistory.h"
#include "SGTools/CurrentEventStore.h"
#include "SGTools/DataBucketBase.h"
#include "SGTools/DataProxy.h"
#include "SGTools/DataStore.h"
#include "SGTools/StringPool.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/SGVersionedKey.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreClearedIncident.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/StoreID.h"

// StoreGateSvc. must come before SGImplSvc.h
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/tools/SGImplSvc.h"
#include "boost/foreach.hpp"

using std::ostringstream;
using std::setw;
using std::bind1st;
using std::find_if;
using std::mem_fun;
using std::not1;
using std::hex;
using std::dec;
using std::endl;
using std::ends;
using std::pair;
using std::setw;
using std::string;
using std::vector;

using SG::DataProxy;
using SG::DataStore;
using SG::TransientAddress;

///////////////////////////////////////////////////////////////////////////
// Remapping implementation.

thread_local DataObjIDColl s_newObjs;

namespace SG {


  struct RemapImpl
  {
    typedef IStringPool::sgkey_t sgkey_t;

    // Hash function for the key.
    // Just cast the low bits to a size_t.
    struct keyhash
    {
      std::size_t operator() (sgkey_t key) const
      { return static_cast<std::size_t> (key); }
    };

    struct remap_t {
      sgkey_t target;
      off_t index_offset;
    };
    typedef std::unordered_map<sgkey_t, remap_t, keyhash> remap_map_t;
    remap_map_t m_remaps;
  };


} // namespace SG


///////////////////////////////////////////////////////////////////////////
/// Standard Constructor
SGImplSvc::SGImplSvc(const string& name,ISvcLocator* svc)
  : Service(name, svc), m_pCLIDSvc(0), m_pDataLoader(0), 
    m_pPPSHandle("ProxyProviderSvc", name),
    m_pPPS(nullptr),
    m_pHistorySvc(0), m_pStore(new DataStore(*this)), 
    m_pIncSvc("IncidentSvc", name),
    m_DumpStore(false), 
    m_ActivateHistory(false),
    m_pIOVSvc(0),
    m_storeLoaded(false),
    m_remap_impl (new SG::RemapImpl),
    m_arena (name),
    m_msg(msgSvc(), name),
    m_slotNumber(-1),
    m_numSlots(1)
{
  //our properties
  declareProperty("ProxyProviderSvc", m_pPPSHandle);
  declareProperty("Dump", m_DumpStore);
  declareProperty("ActivateHistory", m_ActivateHistory);
  //StoreGateSvc properties
  declareProperty("IncidentSvc", m_pIncSvc);
  //add handler for Service base class property
  m_outputLevel.declareUpdateHandler(&SGImplSvc::msg_update_handler, this);
  
  SG::ArenaHeader* header = SG::ArenaHeader::defaultHeader();
  header->addArena (&m_arena);
}


/// Standard Destructor
SGImplSvc::~SGImplSvc()  {
  SG::ArenaHeader* header = SG::ArenaHeader::defaultHeader();
  header->delArena (&m_arena);

  delete m_pStore;
  delete m_remap_impl;
}

//////////////////////////////////////////////////////////////
/// Service initialization
StatusCode SGImplSvc::initialize()    {

  msg() << MSG::VERBOSE <<  "Initializing " << name() 
        << " - package version " << PACKAGE_VERSION << endmsg;

  CHECK( Service::initialize() );

  if (!m_pStore)
    m_pStore = new DataStore (*this);
  if (!m_remap_impl)
    m_remap_impl = new SG::RemapImpl;

  //properties accessible from now on
  
  store()->setStoreID(StoreID::findStoreID(name()));
  // If this is the default event store (StoreGateSvc), then declare
  // our arena as the default for memory allocations.
  if (this->storeID() == StoreID::EVENT_STORE) {
    m_arena.makeCurrent();
    SG::CurrentEventStore::setStore (this);
  }
  // set up the incident service:
  if (!(m_pIncSvc.retrieve()).isSuccess()) {
    msg() << MSG::ERROR 
          << "Could not locate IncidentSvc "
          << endmsg;
    return StatusCode::FAILURE;
  }

  //start listening to "EndEvent"
  // const int PRIORITY = 100;
  // Mother svc should register these incidents
  // m_pIncSvc->addListener(this, "EndEvent", PRIORITY);
  // m_pIncSvc->addListener(this, "BeginEvent", PRIORITY);

  const bool CREATEIF(true);
  // cache pointer to Persistency Service
  if (!(service("EventPersistencySvc", m_pDataLoader, CREATEIF)).isSuccess()) {
    m_pDataLoader = 0;
    msg() << MSG::ERROR
          << "Could not get pointer to Persistency Service"
          << endmsg;
    return StatusCode::FAILURE;;
  }

  if (!(service("ClassIDSvc", m_pCLIDSvc, CREATEIF)).isSuccess()) {
    msg() << MSG::ERROR
          << "Could not get pointer to ClassID Service"
          << endmsg;
    return StatusCode::FAILURE;;
  }

  if (!m_pPPSHandle.empty()) {
    CHECK( m_pPPSHandle.retrieve() );
    m_pPPS = &*m_pPPSHandle;
  }
 
  if ( m_pPPS && (m_pPPS->preLoadProxies(*m_pStore)).isFailure() )
    {
      SG_MSG_DEBUG(" Failed to preLoad proxies");
      return StatusCode::FAILURE;
    }

  // Get hold of History Service
  if (m_ActivateHistory &&
      !(service("HistorySvc", m_pHistorySvc, CREATEIF)).isSuccess()) {
    msg() << MSG::ERROR
          << "Could not locate History Service"
          << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
/// Service start
StatusCode SGImplSvc::start()    {

  msg() << MSG::VERBOSE << "Start " << name() << endmsg;
  /*
  // This will need regFcn clients to be updated first.
  if ( 0 == m_pPPS || (m_pPPS->preLoadProxies(*m_pStore)).isFailure() )
  {
  msg() << MSG::DEBUG
  << " Failed to preLoad proxies"
  << endmsg;
  return StatusCode::FAILURE;
  }
  */

  return StatusCode::SUCCESS;
}
/// Service stop
StatusCode SGImplSvc::stop()    {

  msg() << MSG::VERBOSE << "Stop " << name() << endmsg;
  //HACK ALERT: ID event store objects refer to det store objects
  //by setting an ad-hoc priority for event store(s) we make sure they are finalized and hence cleared first
  // see e.g. https://savannah.cern.ch/bugs/index.php?99993
  if (store()->storeID() == StoreID::EVENT_STORE) {
    ISvcManager* pISM(dynamic_cast<ISvcManager*>(serviceLocator().get()));
    if (!pISM)
      return StatusCode::FAILURE;
    pISM->setPriority(name(), pISM->getPriority(name())+1).ignore();
    msg() << MSG::VERBOSE << "stop: setting service priority to " << pISM->getPriority(name()) 
          << " so that event stores get finalized and cleared before other stores" <<endmsg;
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////
IIOVSvc* SGImplSvc::getIIOVSvc() {
  // Get hold of the IOVSvc
  if (0 == m_pIOVSvc && !(service("IOVSvc", m_pIOVSvc)).isSuccess()) {
    msg() << MSG::WARNING
          << "Could not locate IOVSvc "
          << endmsg;
  }
  return m_pIOVSvc;
}

//////////////////////////////////////////////////////////////
void SGImplSvc::handle(const Incident &inc) {

  if (inc.type() == "EndEvent") { 
    if (m_DumpStore) {
      SG_MSG_DEBUG("Dumping StoreGate Contents");
      msg() << MSG::INFO 
            << '\n' << dump() << endl 
            << endmsg;
    }
  }
}

StatusCode SGImplSvc::loadEventProxies() {
  lock_t lock (m_mutex);
  StatusCode sc(StatusCode::SUCCESS);
  //FIXME this should probably be dealt with by the providers
  if (0 != m_pPPS && !m_storeLoaded) {
    m_storeLoaded = true;
    sc=m_pPPS->loadProxies(*m_pStore);
  } 
  return sc;
}

///////////////////////////////////////////////////////////////////
// Create a key for a type (used if the client has not specified a key)
string SGImplSvc::createKey(const CLID& id)
{
  ostringstream o;
  o << m_pStore->typeCount(id)+1 << std::ends;
  string ret(o.str());
  return ret;
}
//////////////////////////////////////////////////////////////
// clear store
StatusCode SGImplSvc::clearStore(bool forceRemove)
{
  {
    lock_t lock (m_mutex);
    emptyTrash();
    for (auto& p : m_newBoundHandles)
      p.second.clear();
    assert(m_pStore);
    MsgStream* pmlog( msgLvl(MSG::VERBOSE) ? &msg() : 0);
    msg() << MSG::DEBUG << "Clearing store with forceRemove="
          << forceRemove << endmsg;
    bool hard_reset = (m_numSlots > 1);
    m_pStore->clearStore(forceRemove, hard_reset, pmlog);
    m_storeLoaded=false;  //FIXME hack needed by loadEventProxies
  }
  {
    lock_t remap_lock (m_remapMutex);
    m_remap_impl->m_remaps.clear();
    m_arena.reset();
  }

  return StatusCode::SUCCESS;
}
//////////////////////////////////////////////////////////////
/// Service finalization
StatusCode SGImplSvc::finalize()    {
  msg() << MSG::VERBOSE << "Finalizing " << name() 
        << " - package version " << PACKAGE_VERSION << endmsg ;
  
  // Incident service may not work in finalize.
  // Clear this, so that we won't try to send an incident from clearStore.
  (m_pIncSvc.release()).ignore();
  
  const bool FORCEREMOVE(true);
  clearStore(FORCEREMOVE).ignore();
  
  //protect against double release
  if (m_pHistorySvc) {
    m_pHistorySvc->release();
    m_pHistorySvc = 0;
  }
  
  m_stringpool.clear();
  delete m_pStore;
  m_pStore = nullptr;
  delete m_remap_impl;
  m_remap_impl = 0;
  m_arena.erase();

  return Service::finalize();
}
//////////////////////////////////////////////////////////////
/// Service reinitialization
StatusCode SGImplSvc::reinitialize()    {
  msg() << MSG::VERBOSE << "Reinitializing " << name() 
        << " - package version " << PACKAGE_VERSION << endmsg ;
  const bool FORCEREMOVE(true);
  clearStore(FORCEREMOVE).ignore();
  //not in v20r2p2! return Service::reinitialize();
  return StatusCode::SUCCESS;
}

const InterfaceID& 
SGImplSvc::interfaceID() { 
  static const InterfaceID IID("SGImplSvc", 1, 0);
  return IID; 
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode SGImplSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IProxyDict::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyDict*)this;
  }
  else if ( IProxyDict::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyDict*)this;
  }
  else if ( IHiveStoreMgr::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IHiveStoreMgr*)this;
  }
  else if ( interfaceID().versionMatch(riid) )    {
    // In principle this should be cast to ISGImplSvc*. However, there
    // is an anomaly in that existing clients are using the concrete StoreGate
    // interface instead of an abstract ISGImplSvc interface.
    *ppvInterface = (SGImplSvc*)this;
  } else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// add proxy (with IOpaqueAddress that will later be retrieved from P)
//////////////////////////////////////////////////////////////////////
StatusCode SGImplSvc::recordAddress(const std::string& skey,
                                    IOpaqueAddress* pAddress, 
                                    bool clearAddressFlag)
{
  lock_t lock (m_mutex);
  assert(0 != pAddress);
  CLID dataID = pAddress->clID();

  if (dataID == 0)
    {
      msg() << MSG::WARNING
            << "recordAddress: Invalid Class ID found in IOpaqueAddress @" 
            << pAddress << ". IOA will not be recorded"
            << endmsg;
      return StatusCode::FAILURE;
    }

  //do not overwrite a persistent object
  if (m_pPPS) {
    DataProxy* dp = m_pStore->proxy (dataID, skey);
    if (!dp) {
      dp = m_pPPS->retrieveProxy(dataID, skey, *m_pStore);
    }
    if (dp && dp->provider()) {
      std::string clidTypeName; 
      m_pCLIDSvc->getTypeNameOfID(dataID, clidTypeName).ignore();
      msg() << MSG::WARNING
            << "recordAddress: failed for key="<< skey << ", type "  << clidTypeName
            << " (CLID " << dataID << ')' 
            << "\n there is already a persistent version of this object. Will not record a duplicate! "
            << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Check if a key already exists
  DataProxy* dp = m_pStore->proxy_exact(dataID, skey);
  if (0 == dp && 0 != m_pPPS) {
    dp = m_pPPS->retrieveProxy(dataID, skey, *m_pStore);
  }

  // Now treat the various cases:
  if (0 == dp) 
    {
      // create the proxy object and register it
      dp = new DataProxy (TransientAddress (dataID, skey,
                                            pAddress, clearAddressFlag),
                          m_pDataLoader, true, true);
      m_pStore->addToStore(dataID, dp).ignore();

      addAutoSymLinks (skey, dataID, dp, 0, false);
    }
  else if ((0 != dp) && (0 == dp->address()))
    // Note: intentionally not checking dp->isValidAddress()
    {
      // Update proxy with IOpaqueAddress
      dp->setAddress(pAddress);
    }
  else
    {
      string errType;
      m_pCLIDSvc->getTypeNameOfID(dataID, errType).ignore();
      msg() << MSG::WARNING
            << "recordAddress: preexisting proxy @" << dp
            << " with non-NULL IOA found for key " 
            << skey << " type " << errType << " (" << dataID << "). \n"
            << "Cannot record IOpaqueAddress @" << pAddress
            << endmsg;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;

}    

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// add proxy (with IOpaqueAddress that will later be retrieved from P)
//////////////////////////////////////////////////////////////////////
StatusCode SGImplSvc::recordAddress(IOpaqueAddress* pAddress, bool clearAddressFlag)
{
  lock_t lock (m_mutex);
  assert(0 != pAddress);

  CLID dataID = pAddress->clID();

  string gK = (pAddress->par())[1];   // transient name by convention
  if (gK.empty()) gK = (pAddress->par())[0];   // FIXME backward compatibility
  if (gK.empty()) gK = createKey(dataID);

  return this->recordAddress(gK, pAddress, clearAddressFlag);
}    

DataProxy* SGImplSvc::setupProxy(const CLID& dataID, 
                                 const string& gK, 
                                 DataObject* pDObj,
                                 bool allowMods, 
                                 bool resetOnly) {
  // locate the proxy           
  DataProxy* dp = m_pStore->proxy_exact(dataID, gK);
  
  if (0 != dp) { //proxy found  
    if (0 != dp->object())
      {
        // Case 0: duplicated proxy               
        msg() << MSG::WARNING << " setupProxy:: error setting up proxy for key " 
              << gK << " and clid " << dataID
              << "\n Pre-existing valid DataProxy @"<< dp 
              << " found in Store for key " <<  dp->object()->name()
              << " with clid " << dp->object()->clID()
              << endmsg;
        recycle(pDObj);      // commit this object to trash
        dp = 0;
      } else {
      // Case 1: Proxy found... if not valid, update it:
      dp->setObject(pDObj);
      if (!allowMods) dp->setConst();
    } 
  } else {
    // Case 2: No Proxy found:
    dp = new DataProxy(pDObj,
                       TransientAddress(dataID, gK),
                       !allowMods, resetOnly);
    if (!(m_pStore->addToStore(dataID, dp).isSuccess())) {
      msg() << MSG::WARNING
            << " setupProxy:: could not addToStore proxy @" << dp
            << endmsg;
      recycle(pDObj);      // commit this object to trash
      delete dp;
      dp = 0;
    }
  }
  return dp;
}

/// set store id in DataStore:
void SGImplSvc::setStoreID(StoreID::type id)
{
  lock_t lock (m_mutex);
  store()->setStoreID(id);
}
/// get store id from DataStore:
StoreID::type SGImplSvc::storeID() const
{
  lock_t lock (m_mutex);
  return store()->storeID();
}

void
SGImplSvc::keys(const CLID& id, std::vector<std::string>& vkeys, 
                bool includeAlias, bool onlyValid) 

{ 
  lock_t lock (m_mutex);
  return store()->keys(id, vkeys, includeAlias, onlyValid);
} 

bool SGImplSvc::isSymLinked(const CLID& linkID, DataProxy* dp)   
{        
  return (0 != dp) ? dp->transientID(linkID) : false;        
}

//////////////////////////////////////////////////////////////////
// Dump Contents in store:
string SGImplSvc::dump() const
{ 
  lock_t lock (m_mutex);
  ostringstream ost;
  ost << "<<<<<<<<<<<<<<<<< Data Store Dump >>>>>>>>>>>>>>> \n";
  ost << "SGImplSvc(" + name() + ")::dump():\n";

  DataStore::ConstStoreIterator s_iter, s_end;
  store()->tRange(s_iter, s_end).ignore();

  for (; s_iter != s_end; ++s_iter) 
    {

      CLID id = s_iter->first;
      int nProxy = store()->typeCount(id);
      string tname;
      m_pCLIDSvc->getTypeNameOfID(id, tname).ignore();
      ost << "Found " << nProxy << ((nProxy == 1) ? " proxy" : " proxies") 
          << " for ClassID " << id <<" ("<< tname << "): \n";

      // loop over each type:
      SG::ConstProxyIterator p_iter = (s_iter->second).begin();
      SG::ConstProxyIterator p_end =  (s_iter->second).end();
  
      while (p_iter != p_end) {
        const DataProxy& dp(*p_iter->second);
        //      ost << " proxy@" << &dp;
        ost << " flags: (" 
            << setw(7) << (dp.isValid() ? "valid" : "INVALID") << ", "
            << setw(8) << (dp.isConst() ? "locked" : "UNLOCKED") << ", "
            << setw(6) << (dp.isResetOnly() ? "reset" : "DELETE")
            << ") --- data: " << hex << setw(10) << dp.object() << dec
            << " --- key: " << p_iter->first << '\n';
        ++p_iter;
      }
    }
  ost << "<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>> \n";
  string ret(ost.str());
  return ret;

}
 
DataStore* 
SGImplSvc::store() 
{ 
  return m_pStore; 
}

const DataStore* 
SGImplSvc::store() const 
{ 
  return m_pStore; 
}

 
StatusCode
SGImplSvc::addSymLink(const CLID& linkid, DataProxy* dp)
{ 
  if (0 == dp) {
    msg() << MSG::WARNING
          << "addSymLink: no target DataProxy found. Sorry, can't link to a non-existing data object"
          << endmsg;
    return StatusCode::FAILURE;
  } 
  StatusCode sc = m_pStore->addSymLink(linkid, dp); 

  // If the symlink is a derived->base conversion, then we may have
  // a different transient pointer for the symlink.
  if (sc.isSuccess() && dp->object()) {
    void* baseptr = SG::DataProxy_cast (dp, linkid);
    if (baseptr)
      this->t2pRegister (baseptr, dp).ignore();
  }

  addedNewTransObject (linkid, dp->name());
  return sc;
}

 
StatusCode
SGImplSvc::addAlias(const std::string& aliasKey, DataProxy* proxy)
{
  if (0 == proxy) {
    msg() << MSG::WARNING
          << "addAlias: no target DataProxy given, Cannot alias to a non-existing object" 
          << endmsg;
    return StatusCode::FAILURE;
  }

  addedNewTransObject (proxy->clID(), aliasKey);

  // add key to proxy and to ProxyStore
  return m_pStore->addAlias(aliasKey, proxy);
}

int SGImplSvc::typeCount(const CLID& id) const
{
  lock_t lock (m_mutex);
  return m_pStore->typeCount(id);
}  

DataProxy* 
SGImplSvc::proxy(const void* const pTransient) const
{ 
  lock_t lock (m_mutex);
  return m_pStore->locatePersistent(pTransient); 
}

DataProxy* 
SGImplSvc::proxy(const CLID& id) const
{ 
  return proxy(id, false);
}

DataProxy* 
SGImplSvc::proxy(const CLID& id, bool checkValid) const
{ 
  lock_t lock (m_mutex);
  DataProxy* dp = m_pStore->proxy(id);
  if (0 == dp && 0 != m_pPPS) {
    dp = m_pPPS->retrieveProxy(id, string("DEFAULT"), *m_pStore);
  }
  /// Check if it is valid
  if (checkValid && 0 != dp) {
    // FIXME: For keyless retrieve, this checks only the first instance
    // of the CLID in store. If that happens to be invalid, but the second
    // is valid - this does not work (when checkValid is requested).
    return dp->isValid() ? dp : 0;
  }
  return dp;
}

DataProxy* 
SGImplSvc::proxy(const CLID& id, const string& key) const
{
  return proxy(id, key, false);
}

DataProxy*
SGImplSvc::proxy(const CLID& id, const string& key, bool checkValid) const
{ 
  lock_t lock (m_mutex);
  DataProxy* dp = m_pStore->proxy(id, key);
  if (0 == dp && 0 != m_pPPS) {
    dp = m_pPPS->retrieveProxy(id, key, *m_pStore);
  }
  if (checkValid && 0 != dp && !(dp->isValid())) dp = 0;
  return dp;
}


/**
 * @brief Raw addition of a proxy to the store.
 * @param id CLID of object being added.
 * @param proxy proxy to add.
 */
StatusCode SGImplSvc::addToStore (CLID id, SG::DataProxy* proxy)
{
  lock_t lock (m_mutex);
  return m_pStore->addToStore (id, proxy);
}


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
SG::DataProxy* SGImplSvc::recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                        const std::string& key,
                                        bool allowMods,
                                        bool returnExisting)
{
  lock_t lock (m_mutex);
  const void* raw_ptr = obj.get();
  const std::type_info* tinfo = nullptr;
  
  if (DataBucketBase* bucket = dynamic_cast<DataBucketBase*> (obj.get())) {
    raw_ptr = bucket->object();
    tinfo = &bucket->tinfo();
  }

  if (returnExisting) {
    SG::DataProxy* proxy = this->proxy (obj->clID(), key);
    if (proxy && proxy->isValid()) return proxy;

    // Look for the same object recorded under a different key/clid.
    proxy = this->proxy (raw_ptr);
    if (proxy && proxy->isValid()) {
      if (proxy->transientID (obj->clID())) {
        // CLID matches.  Make an alias.
        if (addAlias (key, proxy).isFailure()) {
          CLID clid = proxy->clID();
          std::string clidTypeName; 
          m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
          msg() << MSG::WARNING
                << "SGImplSvc::recordObject: addAlias fails for object "
                << clid << "[" << clidTypeName << "] " << proxy->name()
                << " and new key " << key
                << endmsg;
          
          proxy = nullptr;
        }
      }

      else if (key == proxy->name() ||
               proxy->alias().count (key) > 0)
      {
        // key matches.  Make a symlink.
        if (addSymLink (obj->clID(), proxy).isFailure()) {
          CLID clid = proxy->clID();
          std::string clidTypeName; 
          m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
          CLID newclid = obj->clID();
          std::string newclidTypeName; 
          m_pCLIDSvc->getTypeNameOfID(newclid, newclidTypeName).ignore();
          msg() << MSG::ERROR
                << "SGImplSvc::recordObject: addSymLink fails for object "
                << clid << "[" << clidTypeName << "] " << proxy->name()
                << " and new clid " << newclid << "[" << newclidTypeName << "]"
                << endmsg;
          proxy = nullptr;
        }
      }

      else {
        CLID clid = proxy->clID();
        std::string clidTypeName; 
        m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
        CLID newclid = obj->clID();
        std::string newclidTypeName; 
        m_pCLIDSvc->getTypeNameOfID(newclid, newclidTypeName).ignore();
        msg() << MSG::ERROR
              << "SGImplSvc::recordObject: existing object found with "
              << clid << "[" << clidTypeName << "] " << proxy->name()
              << " but neither clid " << newclid << "[" << newclidTypeName << "]"
              << " nor key " << key << " match."
              << endmsg;
        proxy = nullptr;
      }

      return proxy;
    }
  }

  const bool resetOnly = true;
  const bool noHist = false;
  SG::DataProxy* proxy = nullptr;
  if (this->typeless_record (obj.get(), key, raw_ptr,
                             allowMods, resetOnly, noHist, tinfo,
                             &proxy, true).isFailure())
    {
      return nullptr;
    }
  return proxy;
}


/**
 * @brief Inform HIVE that an object has been updated.
 * @param id The CLID of the object.
 * @param key The key of the object.
 */
StatusCode SGImplSvc::updatedObject (CLID id, const std::string& key)
{
  lock_t lock (m_mutex);
  addedNewTransObject (id, key);
  return StatusCode::SUCCESS;
}


/**
 * @brief Set the Hive slot number for this store.
 * @param slot The slot number.  -1 means that this isn't a Hive store.
 * @param numSlots The total number of slots.  Should be 1 for the
 *                 non-Hive case.
 */
void SGImplSvc::setSlotNumber (int slot, int numSlots)
{
  m_slotNumber = slot;
  m_numSlots = numSlots;

  SG::ArenaHeader* header = SG::ArenaHeader::defaultHeader();
  header->setArenaForSlot (slot, &m_arena);
}


std::vector<const SG::DataProxy*> 
SGImplSvc::proxies() const
{
  lock_t lock (m_mutex);
  using std::distance;
  DataStore::ConstStoreIterator s_iter, s_end;
  store()->tRange(s_iter, s_end).ignore();

  std::vector<const SG::DataProxy*> proxies;
  proxies.reserve( distance( s_iter, s_end ) );

  for (; s_iter != s_end; ++s_iter ) {

    const CLID id = s_iter->first;
    proxies.reserve( proxies.size() + store()->typeCount(id) );

    // loop over each type:
    SG::ConstProxyIterator p_iter = (s_iter->second).begin();
    SG::ConstProxyIterator p_end =  (s_iter->second).end();

    for ( ; p_iter != p_end; ++p_iter ) {
      proxies.push_back( p_iter->second );
    }
  }

  return proxies;
}


std::vector<CLID>
SGImplSvc::clids() const
{
  lock_t lock (m_mutex);

  using std::distance;
  DataStore::ConstStoreIterator s_iter, s_end;
  store()->tRange(s_iter, s_end).ignore();

  std::vector<CLID> clids;
  clids.reserve( distance( s_iter, s_end ) );

  for (; s_iter != s_end; ++s_iter ) {
    const CLID id = s_iter->first;
    clids.push_back (id);
  }

  return clids;
}


DataProxy*
SGImplSvc::transientProxy(const CLID& id, const string& key) const
{ 
  lock_t lock (m_mutex);
  DataProxy* dp(m_pStore->proxy(id, key));
  return ( (0 != dp && dp->isValidObject()) ? dp : 0 );
}

DataObject* 
SGImplSvc::accessData(const CLID& id) const
{ 
  lock_t lock (m_mutex);
  DataProxy* theProxy(proxy(id, true));
  return (0 == theProxy) ? 0 : theProxy->accessData();
}

DataObject* 
SGImplSvc::accessData(const CLID& id, const string& key) const
{ 
  lock_t lock (m_mutex);
  DataProxy* theProxy(proxy(id, key, true));
  return (0 == theProxy) ? 0 : theProxy->accessData();
}

bool
SGImplSvc::transientSwap( const CLID& id,
                          const std::string& keyA, const std::string& keyB )
{
  lock_t lock (m_mutex);
  const bool checkValid = true;
  DataProxy* a = proxy( id, keyA, checkValid );
  DataProxy* b = proxy( id, keyB, checkValid );
  if ( 0 == a || 0 == b ) { return false; }
  DataObject* objA = a->accessData();
  DataObject* objB = b->accessData();

  if ( 0 == objA || 0 == objB ) { return false; }
  // prevent 'accidental' release of DataObjects...
  const unsigned int refCntA = objA->addRef(); 
  const unsigned int refCntB = objB->addRef();
  // in case swap is being specialized for DataObjects 
  using std::swap;
  swap( objA, objB );
  a->setObject( objA );
  b->setObject( objB );
  // and then restore old ref-count;
  return ( (refCntA-1) == objA->release() && 
           (refCntB-1) == objB->release() );
}

StatusCode
SGImplSvc::typeless_record( DataObject* obj, const std::string& key,
                            const void* const raw_ptr,
                            bool allowMods, bool resetOnly, bool noHist)
{
  return typeless_record (obj, key, raw_ptr, allowMods, resetOnly, noHist, 0,
                          nullptr, true);
}


StatusCode
SGImplSvc::typeless_record( DataObject* obj, const std::string& key,
                            const void* const raw_ptr,
                            bool allowMods, bool resetOnly, bool noHist,
                            const std::type_info* tinfo)
{
  return typeless_record (obj, key, raw_ptr, allowMods, resetOnly, noHist,tinfo,
                          nullptr, true);
}


StatusCode
SGImplSvc::typeless_record( DataObject* obj, const std::string& key,
                            const void* const raw_ptr,
                            bool allowMods, bool resetOnly, bool noHist,
                            const std::type_info* tinfo,
                            SG::DataProxy** proxy_ret,
                            bool noOverwrite)
{
  lock_t lock (m_mutex);
  SG::DataProxy* proxy =
    record_impl( obj, key, raw_ptr, allowMods, resetOnly, !noOverwrite, tinfo);
  if ( proxy == nullptr )
    return StatusCode::FAILURE;
  if (proxy_ret)
    *proxy_ret = proxy;

  if ( !m_ActivateHistory || noHist ) {
    return StatusCode::SUCCESS;
  }

  if ( store()->storeID() != StoreID::EVENT_STORE ) {
    return StatusCode::SUCCESS;
  } else {
    return record_HistObj( obj->clID(), key, name(), allowMods, resetOnly );
  }
}

StatusCode
SGImplSvc::typeless_overwrite( const CLID& clid, 
                               DataObject* obj, 
                               const std::string& key,
                               const void* const raw_ptr,
                               bool allowMods,
                               bool noHist,
                               const std::type_info* tinfo)
{
  lock_t lock (m_mutex);
  StatusCode sc(StatusCode::SUCCESS);
  SG::DataProxy* toRemove(proxy(clid, key, false));
  if (0 != toRemove) {
    toRemove->addRef();
    const bool FORCEREMOVE(true);        
    sc =removeProxy(toRemove, (void*)0, FORCEREMOVE);
  }
  if (sc.isSuccess()) {
    const bool ALLOWOVERWRITE(true);
    const bool NORESET(false);
    if (record_impl( obj, key, raw_ptr, allowMods, NORESET, ALLOWOVERWRITE, tinfo) == nullptr)
      sc = StatusCode::FAILURE;
    else if ( m_ActivateHistory && noHist && store()->storeID() == StoreID::EVENT_STORE ) {
      sc = record_HistObj( obj->clID(), key, name(), allowMods, NORESET );
    }
  }
  //for detector store objects managed by IIOVSvc, replace the old proxy with the new one (#104311)
  if (toRemove && sc.isSuccess() && store()->storeID() == StoreID::DETECTOR_STORE) {
    sc = getIIOVSvc()->replaceProxy(toRemove, proxy(clid, key));
  }
  if (toRemove)
    toRemove->release();
  return sc;
}

SG::DataProxy*
SGImplSvc::record_impl( DataObject* pDObj, const std::string& key,
                        const void* const raw_ptr,
                        bool allowMods, bool resetOnly, bool allowOverwrite,
                        const std::type_info* tinfo)
{
  CLID clid = pDObj->clID();
  std::string rawKey(key);
  bool isVKey(SG::VersionedKey::isVersionedKey(key));
  if (isVKey) {
    //FIXME VersionedKeys will need to be handled more efficiently
    SG::VersionedKey vk(rawKey);
    DataProxy *dp(proxy(clid, vk.key()));
    if (dp) {
      //proxies primary key
      const std::string& pTAName(dp->name());
      //original key as versioned
      SG::VersionedKey primaryVK(pTAName);
      
      //if the existing matching object has no version
      //create a versioned alias for the original unversioned key
      //so it will remain accessible
      if (!SG::VersionedKey::isVersionedKey(pTAName)) {
        if (!(this->addAlias(primaryVK.rawVersionKey(), dp)).isSuccess()) {
          msg() << MSG::WARNING
                << "record_impl: Could not setup alias key " 
                << primaryVK.rawVersionKey() 
                << " for unversioned object " << pTAName
                << endmsg;      
          return nullptr;
        }
      }
      if (vk.isAuto()) {
        //make a new versioned key incrementing the existing version
        SG::VersionedKey newVK(primaryVK.key(), primaryVK.version()+1);
        //FIXME this will fail in a confusing way if version+1 is in use
        //FIXME need a better error message below, probably looking at all
        //FIXME aliases
        rawKey = newVK.rawVersionKey();
      }
    }
  }
  if (!allowOverwrite && m_pPPS) {
    //do not overwrite a persistent object
    DataProxy* dp = m_pStore->proxy (clid, rawKey);
    if (!dp) {
      dp = m_pPPS->retrieveProxy(clid, rawKey, *m_pStore);
    }
    if (dp && dp->provider()) {
      std::string clidTypeName; 
      m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
      msg() << MSG::WARNING
            << "record_impl: you are recording an object with key "
            << rawKey << ", type "  << clidTypeName
            << " (CLID " << clid << ')' 
            << "\n There is already a persistent version of this object. Recording a duplicate may lead to unreproducible results and it is deprecated."
            << endmsg;
    }
  }
  //now check whether raw_ptr has already been recorded
  //We need to do this before we create the bucket, the proxy etc
  SG::DataProxy* dp(proxy(raw_ptr));
  if (0 != dp) {
    std::string clidTypeName; 
    m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
    msg() << MSG::WARNING
          << "record_impl: failed for key="<< rawKey << ", type "  << clidTypeName
          << " (CLID " << clid << ')' 
          << "\n object @" << raw_ptr 
          << " already in store with key="<< dp->name()
          << ". Will not record a duplicate! "
          << endmsg;
    if (pDObj != dp->object()) {
      DataBucketBase* pDBB(dynamic_cast<DataBucketBase*>(pDObj));
      if (!pDBB) std::abort();
      pDBB->relinquish(); //don't own the data obj already recorded!
    }
    this->recycle(pDObj);
    return nullptr;
  }


  // setup the proxy
  dp = setupProxy( clid, rawKey, pDObj, allowMods, resetOnly );
  if ( 0 == dp ) {
    std::string clidTypeName; 
    m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
    msg() << MSG::WARNING
          << "record_impl: Problem setting up the proxy for object @" <<raw_ptr 
          << "\n recorded with key " << rawKey 
          << " of type "  << clidTypeName
          << " (CLID " << clid << ") in DataObject @" << pDObj
          << endmsg;

    return nullptr;
  }

  // record in t2p:
  if ( !(this->t2pRegister( raw_ptr, dp )).isSuccess() ) {
    std::string clidTypeName; 
    m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
    msg() << MSG::WARNING
          << "record_impl: can not add to t2p map object @" <<raw_ptr 
          << "\n with key " << rawKey 
          << " of type "  << clidTypeName
          << " (CLID " << clid << ')' 
          << endmsg;
    return nullptr;
  }

  addAutoSymLinks (rawKey, clid, dp, tinfo);

  //handle versionedKeys: we register an alias with the "true" key
  //unless an object as already been recorded with that key.
  //Notice that addAlias overwrites any existing alias, so a generic
  //retrieve will always return the last version added 
  //FIXME not the one with the highest version
  if (isVKey) {
    SG::VersionedKey vk(rawKey);
    if (!(this->addAlias(vk.key(), dp)).isSuccess()) {
      msg() << MSG::WARNING
            << "record_impl: Could not setup alias key " << vk.key() 
            << " for VersionedKey " << rawKey
            << ". Generic access to this object with clid" << clid 
            << " will not work"
            << endmsg;      
    }
  }

  addedNewTransObject(clid, rawKey);

  return dp;
  
}

DataProxy*
SGImplSvc::locatePersistent(const TransientAddress* tAddr, 
                            bool checkValid) const
{ 
  DataProxy* dp = m_pStore->proxy(tAddr);
  
  if (checkValid && 0 != dp) {
    return dp->isValid() ? dp : 0;
  } else {
    return dp;
  }
}

StatusCode
SGImplSvc::removeProxy(DataProxy* proxy, const void* pTrans, 
                       bool forceRemove)
{
  lock_t lock (m_mutex);
  // check if valid proxy
  if (0 == proxy) return StatusCode::FAILURE;

  if (0 == pTrans) {
    DataBucketBase* bucket = dynamic_cast<DataBucketBase*>(proxy->object());
    if (bucket) pTrans = bucket->object();
  }

  // remove all entries from t2p map
  this->t2pRemove(pTrans);
  SG::DataProxy::CLIDCont_t clids = proxy->transientID();
  for (SG::DataProxy::CLIDCont_t::const_iterator i = clids.begin();
       i != clids.end();
       ++i)
    {
      void* ptr = SG::DataProxy_cast (proxy, *i);
      this->t2pRemove(ptr);
    }

  // remove from store
  return m_pStore->removeProxy(proxy, forceRemove, true);
}

StatusCode
SGImplSvc::t2pRegister(const void* const pTrans, DataProxy* const pPers)
{ 
  return m_pStore->t2pRegister(pTrans, pPers);
}


void
SGImplSvc::t2pRemove(const void* const pTrans)
{
  m_pStore->t2pRemove(pTrans); 
}

void
SGImplSvc::msg_update_handler(Property& /*outputLevel*/)
{
  setUpMessaging();
  msg().setLevel (outputLevel());
  msgSvc()->setOutputLevel(name(), outputLevel());
}

StatusCode 
SGImplSvc::proxyRange(const CLID& id,
                      SG::ConstProxyIterator& begin,
                      SG::ConstProxyIterator& end) const {
  lock_t lock (m_mutex);
  return m_pStore->pRange(id,begin,end);
}

StatusCode SGImplSvc::setConst(const void* pObject)
{
  lock_t lock (m_mutex);
  // Check if DataProxy does not exist
  DataProxy * dp = proxy(pObject); 

  if (0 == dp)
    {
      msg() << MSG::WARNING
            << "setConst: NO Proxy for the dobj you want to set const"
            << endmsg;
      return StatusCode::FAILURE;
    }

  dp->setConst();
  return StatusCode::SUCCESS;
}

//put a bad (unrecordable) dobj away
void SGImplSvc::recycle(DataObject* pBadDObj) {
  assert(pBadDObj);
  pBadDObj->addRef(); 
  m_trash.push_back(pBadDObj);
}

//throw away bad objects
void SGImplSvc::emptyTrash() {
  lock_t lock (m_mutex);
  while (!m_trash.empty()) {
    m_trash.front()->release();  //delete the bad data object
    m_trash.pop_front();     //remove pointer from list
  }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool SGImplSvc::bindHandleToProxy(const CLID& id, const string& key,
                                  IResetable* ir, DataProxy *&dp) 
{

  dp = m_pStore->proxy (id, key);
  if (dp == nullptr && m_pPPS != nullptr) {
    dp = m_pPPS->retrieveProxy(id, key, *m_pStore);
  }

  if (0 == dp) return false;

  if (! dp->bindHandle(ir) ) {
    msg() << MSG::FATAL << "DataHandle at " << hex << ir << dec 
          << " already bound to DataProxy with key " << ir->key() 
          << ". Cannot bind to proxy " << dp->name() << " as well\n"
          << "        You have probably registered multiple callbacks via regFcn with the same DataHandle using different keys (DataProxies)\n"
          << endmsg;
    return false;
  }
    
  //already done in DataHandleBase::setState  dp->addRef();

#ifndef NDEBUG
  SG_MSG_DEBUG(" Bound handle " << MSG::hex << ir << " to proxy " 
               << dp << MSG::dec); 
#endif
  return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
SGImplSvc::record_HistObj(const CLID& id, const std::string& key,
                          const std::string& store, 
                          bool allowMods, bool resetOnly) {

  assert(m_pHistorySvc);

  DataHistory *dho;
  dho = m_pHistorySvc->createDataHistoryObj( id, key, store );

  std::string idname;
  StatusCode sc = m_pCLIDSvc->getTypeNameOfID(id, idname);
  if (sc.isFailure() || idname == "" ) { 
    std::ostringstream ost;
    ost << id;
    idname = ost.str();
  }
  idname = idname + "/" + key;

  DataObject* obj = asStorable<DataHistory>(dho);
  
  const bool ALLOWOVERWRITE(false);
  if (record_impl(obj, idname, dho, allowMods, resetOnly, ALLOWOVERWRITE,
                  &typeid(DataHistory)) == nullptr)
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


/**
 * @brief Find the key for a string/CLID pair.
 * @param str The string to look up.
 * @param clid The CLID associated with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
SGImplSvc::sgkey_t
SGImplSvc::stringToKey (const std::string& str, CLID clid)
{
  lock_t lock (m_stringPoolMutex);
  return m_stringpool.stringToKey (str, clid);
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* SGImplSvc::keyToString (sgkey_t key) const
{
  lock_t lock (m_stringPoolMutex);
  return m_stringpool.keyToString (key);
}


/**
 * @brief Find the string and CLID corresponding to a given key.
 * @param key The key to look up.
 * @param clid[out] The found CLID.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string*
SGImplSvc::keyToString (sgkey_t key, CLID& clid) const
{
  lock_t lock (m_stringPoolMutex);
  return m_stringpool.keyToString (key, clid);
}


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
void SGImplSvc::registerKey (sgkey_t key,
                             const std::string& str,
                             CLID clid)
{
  lock_t lock (m_stringPoolMutex);
  if (!m_stringpool.registerKey (key, str, clid)) {
    CLID clid2;
    const std::string* str2 = m_stringpool.keyToString (key, clid2);
    REPORT_MESSAGE (MSG::WARNING) << "The numeric key " << key
                                  << " maps to multiple string key/CLID pairs: "
                                  << *str2 << "/" << clid2 << " and "
                                  << str << "/" << clid;
  }
}

void
SGImplSvc::releaseObject(const CLID& id, const std::string& key) {
  lock_t lock (m_mutex);
  DataProxy *pP(0);
  if (0 != (pP = proxy(id, key))) {
    // remove all entries from t2p map
    SG::DataProxy::CLIDCont_t clids = pP->transientID();
    SG::DataProxy::CLIDCont_t::const_iterator i(clids.begin()), e(clids.end());
    while (i != e) t2pRemove(SG::DataProxy_cast (pP, *i++));
    DataBucketBase *pDBB(dynamic_cast<DataBucketBase*>(pP->object()));
    //tell the bucket to let go of the data object
    if (0 != pDBB) pDBB->relinquish(); //somebody else better took ownership
    bool hard_reset = (m_numSlots > 1);
    pP->reset (hard_reset);
  }
}

void
SGImplSvc::clearProxyPayload(SG::DataProxy* dp) {
  lock_t lock (m_mutex);
  SG::DataProxy::CLIDCont_t clids = dp->transientID();
  SG::DataProxy::CLIDCont_t::const_iterator i(clids.begin()), e(clids.end());
  while (i != e) {
    t2pRemove(SG::DataProxy_cast (dp, *i++));
  }
  bool hard_reset = (m_numSlots > 1);
  dp->reset (hard_reset);
}


/**
 * @brief Declare a remapping.
 * @brief source Key hash of the container being remapped.
 * @brief target Key hash of the container being remapped to.
 * @brief index_offset Amount by which the index should be adjusted
 *        between the two containers.
 */
void SGImplSvc::remap_impl (sgkey_t source,
                            sgkey_t target,
                            off_t index_offset)
{
  lock_t lock (m_remapMutex);
  SG::RemapImpl::remap_t payload;
  payload.target = target;
  payload.index_offset = index_offset;
  m_remap_impl->m_remaps[source] = payload;
}


/**
 * @brief Test to see if the target of an ElementLink has moved.
 * @param sgkey_in Original hashed key of the EL.
 * @param index_in Original index of the EL.
 * @param sgkey_out[out] New hashed key for the EL.
 * @param index_out[out] New index for the EL.
 * @return True if there is a remapping; false otherwise.
 */
bool SGImplSvc::tryELRemap (sgkey_t sgkey_in, size_t index_in,
                            sgkey_t& sgkey_out, size_t& index_out)
{
  lock_t lock (m_remapMutex);
  SG::RemapImpl::remap_map_t::iterator i =
    m_remap_impl->m_remaps.find (sgkey_in);
  if (i == m_remap_impl->m_remaps.end())
    return false;
  const SG::RemapImpl::remap_t& payload = i->second;
  sgkey_out = payload.target;
  index_out = index_in + payload.index_offset;
  return true;
}

DataObject* SGImplSvc::typeless_readPrivateCopy(const CLID& clid,
                                                const std::string& key) {
  lock_t lock (m_mutex);
  DataObject *pObj(0);
  DataProxy *p(this->proxy(clid, key));
  if (p) {
    if (p->object()) { //this looks in transient mem only
      //if there is a dobj in transient memory we take ownership with addRef
      p->addRef();
      //and make the store forget about the proxy for a moment
      const bool FORCEREMOVE(true);      
      bool hard_reset = (m_numSlots > 1);
      store()->removeProxy(p, FORCEREMOVE, hard_reset).ignore();
      //now we try to read the object from disk. Relies on PPS to reload proxy
      DataProxy *pDisk(this->proxy(clid, key));
      if (pDisk) {
        //We are managing the pObj so we addRef it
        if ( (pObj = pDisk->accessData()) ) pObj->addRef();
        //don't need this guy anymore, notice we use the SGImplSvc version
        //to remove the t2p entry as well
        removeProxy(pDisk, SG::DataProxy_cast(pDisk,clid), FORCEREMOVE).ignore();
      }
      //replace the "transient" proxy where it was
      if (store()->addToStore(clid, p).isFailure()) {
        msg() << MSG::ERROR << "SGImplSvc::typeless_readPrivateCopy: "
              << "could not re-record proxy in store: "
              << clid << "/" << key << endmsg;
      }
    } else if ( (pObj = p->accessData()) ) { //try reading from disk
      //We are managing the pObj so we addRef it
      pObj->addRef();
      //and make the proxy forget about it
      bool hard_reset = (m_numSlots > 1);
      p->reset (hard_reset);
    }
  }
  if (0 == pObj) {
    string errType;
    m_pCLIDSvc->getTypeNameOfID(clid, errType).ignore();
    msg() << MSG::WARNING << "SGImplSvc::typeless_readPrivateCopy: "
          << "did not find object of type "
          << errType << " with key " << key << endmsg;
  }
  return pObj;
}

/// Add automatically-made symlinks for DP.
void SGImplSvc::addAutoSymLinks (const std::string& key,
                                 CLID clid,
                                 DataProxy* dp,
                                 const std::type_info* tinfo,
                                 bool warn_nobib /*= true*/)
{
  // Automatically make all legal base class symlinks
  if (!tinfo) {
    tinfo = CLIDRegistry::CLIDToTypeinfo (clid);
  }
  const SG::BaseInfoBase* bib = nullptr;
  if (tinfo) {
    bib = SG::BaseInfoBase::find (*tinfo);
  }
  if (!bib) {
    // Could succeed where the previous fails if clid for DataVector<T>
    // but tinfo is for ConstDataVector<DataVector<T> >.
    bib = SG::BaseInfoBase::find (clid);
  }
  if ( bib ) {
    std::vector<CLID> bases = bib->get_bases();
    for ( std::size_t i = 0, iMax = bases.size(); i < iMax; ++i ) {
      if ( bases[i] != clid ) {
        if ( addSymLink( bases[i], dp ).isSuccess() ) {
          // register with t2p
          if (dp->object())
            this->t2pRegister( SG::DataProxy_cast( dp, bases[i] ), dp ).ignore();
        }
        else {
          msg() << MSG::WARNING
                << "record_impl: Doing auto-symlinks for object with CLID " << clid
                << " and SG key " << key 
                << ": Proxy already set for base CLID " << bases[i]
                << "; not making auto-symlink." << endmsg;
        }
      }
    }

    // Handle copy conversions.
    {
      BOOST_FOREACH(CLID copy_clid, bib->get_copy_conversions()) {
        if (m_pStore->addSymLink (copy_clid, dp).isFailure()) {
          msg() << MSG::WARNING
                << "record_impl: Doing auto-symlinks for object with CLID " << clid
                << " and SG key " << key 
                << ": Proxy already set for copy-conversion CLID " << copy_clid
                << "; not making auto-symlink." << endmsg;
        }
      }
    }
  }
  else {
    if (warn_nobib) {
      msg() << MSG::WARNING
            << "record_impl: Could not find suitable SG::BaseInfoBase for CLID ["
            << clid << "] (" << key << ") !\t"
            << "No auto-symlink established !"
            << endmsg;
    }
  }
}

StatusCode SGImplSvc::getNewDataObjects(DataObjIDColl& products) {
  lock_t lock (m_mutex);
  tbb::spin_rw_mutex::scoped_lock lock2(m_newDataLock,true);
  products.swap(m_newDataObjects);
  m_newDataObjects.clear();
  return StatusCode::SUCCESS;
}

bool SGImplSvc::newDataObjectsPresent() /*const*/ {
  lock_t lock (m_mutex);
  tbb::spin_rw_mutex::scoped_lock lock2(m_newDataLock,false);
  return !m_newDataObjects.empty();
} 

void
SGImplSvc::commitNewDataObjects() {
  lock_t lock (m_mutex);
  tbb::spin_rw_mutex::scoped_lock lock2(m_newDataLock,true);
  for (auto obj : s_newObjs) {
    if (msg().level() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE
            << "committing dataObj \"" << obj << "\""
            << endmsg;
    }
    m_newDataObjects.insert( obj );
  }
  s_newObjs.clear();

  // Reset handles added since the last call to commit.
  bool hard_reset = (m_numSlots > 1);
  std::vector<IResetable*> handles;
  m_newBoundHandles[std::this_thread::get_id()].swap (handles);
  for (IResetable* h : handles)
    h->reset (hard_reset);
}

#ifndef SLOW_NEWDATAOBJECTS
void SGImplSvc::addedNewPersObject(CLID, DataProxy*) {}
void SGImplSvc::addedNewTransObject(CLID, const std::string&) {}
#else
void SGImplSvc::addedNewPersObject(CLID clid, DataProxy* dp) {
  lock_t lock (m_mutex);
  //if proxy is loading from persistency
  //add key of object to list of "newly recorded" objects
  if (0 != dp->provider()) {
    // The object itself.
    s_newObjs.insert(DataObjID(clid,dp->name()));

    // Aliases.
    for (const std::string& alias : dp->alias()) {
      s_newObjs.insert(DataObjID(clid,alias));
    }

    // Symlinks.
    for (CLID clid2 : dp->transientID()) {
      if (clid2 != clid)
        s_newObjs.insert(DataObjID(clid2,dp->name()));
    }
  }
}
void SGImplSvc::addedNewTransObject(CLID clid, const std::string& key) {
  lock_t lock (m_mutex);
  s_newObjs.insert(DataObjID(clid,key));
}
#endif


/**
 * @brief Tell the store that a proxy has been bound to a handle.
 * @param proxy The proxy that was bound.
 * The default implementation does nothing.
 */
void
SGImplSvc::boundHandle (IResetable* handle)
{
  m_newBoundHandles[std::this_thread::get_id()].push_back (handle);
}


/**
 * @brief Tell the store that a handle has been unbound from a proxy.
 * @param handle The handle that was unbound.
 * The default implementation does nothing.
 */
void
SGImplSvc::unboundHandle (IResetable* handle)
{
  std::vector<IResetable*>& v = m_newBoundHandles[std::this_thread::get_id()];
  std::vector<IResetable*>::iterator it =
    std::find (v.begin(), v.end(), handle);
  if (it != v.end())
    v.erase (it);
}


/// The current store is becoming the active store.  Switch the
/// allocation arena, if needed.
// Only intended to be called by ActiveStoreSvc.
void SGImplSvc::makeCurrent()
{
  lock_t lock (m_mutex);
  m_arena.makeCurrent();
  SG::CurrentEventStore::setStore (this);
}


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
StatusCode
SGImplSvc::createObj (IConverter* cvt,
                      IOpaqueAddress* addr,
                      DataObject*& refpObject)
{
  lock_t lock (m_mutex);
  return cvt->createObj (addr, refpObject);
}


// This is intended to be called from the debugger.
void SG_dump (SGImplSvc* sg)
{
  std::cout << sg->dump() << "\n";
}
void SG_dump (SGImplSvc* sg, const char* fname)
{
  std::ofstream f (fname);
  f << sg->dump() << "\n";
  f.close();
}


