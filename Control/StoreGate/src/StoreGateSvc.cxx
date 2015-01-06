/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <functional>
#include <string>

#include <sstream>
using std::ostringstream;

#include <iomanip>
using std::setw;

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "AthenaKernel/IIOVSvc.h"
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
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/DataHistory.h"
#include "SGTools/DataBucketBase.h"
#include "SGTools/DataProxy.h"
#include "SGTools/DataStore.h"
#include "SGTools/StringPool.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/SGVersionedKey.h"
#include "CxxUtils/unordered_map.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreClearedIncident.h"

#include "StoreGate/StoreGateSvc.h"
#include "boost/foreach.hpp"

using std::bind1st;
using std::find_if;
using std::mem_fun;
using std::not1;
using std::hex;
using std::dec;
using std::endl;
using std::ends;
using std::setw;
using std::string;

using SG::DataProxy;
using SG::DataStore;
using SG::TransientAddress;

///////////////////////////////////////////////////////////////////////////
// Remapping implementation.

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
  typedef SG::unordered_map<sgkey_t, remap_t, keyhash> remap_map_t;
  remap_map_t m_remaps;
};


} // namespace SG


///////////////////////////////////////////////////////////////////////////
/// Standard Constructor
StoreGateSvc::StoreGateSvc(const string& name,ISvcLocator* svc)
  : Service(name, svc), m_pIncSvc(0), m_pCLIDSvc(0), m_pDataLoader(0), 
    m_pPPS(0), m_pHistorySvc(0), m_pStore(new DataStore (*this)), 
    m_DumpStore(false), m_ActivateHistory(false), m_pIOVSvc(0),
    m_storeLoaded(false),
    m_remap_impl (new SG::RemapImpl),
    m_pacSvc("PageAccessControlSvc", name),
    m_monitorPageAccess(false),
    m_msg(msgSvc(), name)
{
  declareProperty("Dump", m_DumpStore);
  declareProperty("ActivateHistory", m_ActivateHistory);
  declareProperty("MonitorPageAccess", m_monitorPageAccess,
		  "Activate PageAccessControlSvc to get statistics on "
		  "unused data objects (SLOW, debug only)");
  //add handler for Service base class property
  m_outputLevel.declareUpdateHandler(&StoreGateSvc::msg_update_handler, this);
}


/// Standard Destructor
StoreGateSvc::~StoreGateSvc()  {
  delete m_pStore;
  delete m_remap_impl;
}

//////////////////////////////////////////////////////////////
/// Service initialisation
StatusCode StoreGateSvc::initialize()    {

  msg() << MSG::INFO << "Initializing " << name() 
      << " - package version " << PACKAGE_VERSION << endreq ;

  if(!(Service::initialize()).isSuccess()) {
    msg() << MSG::ERROR << "Could not initialize base Service !!" << endreq;
    return StatusCode::FAILURE;
  }
  //properties accessible from now on
  
  // set store ID (ugly!):
  string generic_name = getGaudiThreadGenericName(name()) ;
  if (generic_name == "StoreGateSvc" || generic_name == "EventStore") {
    store()->setStoreID(StoreID::EVENT_STORE);
  } else if (generic_name == "DetectorStore") {
    store()->setStoreID(StoreID::DETECTOR_STORE);
  }  else if (generic_name == "ConditionsStore") {
    store()->setStoreID(StoreID::CONDITION_STORE);
  } else if (generic_name == "InputMetaDataStore" || generic_name == "TagMetaDataStore") {
    store()->setStoreID(StoreID::METADATA_STORE);
  } else if (generic_name == "MetaDataStore") {
    store()->setStoreID(StoreID::SIMPLE_STORE);
  } else if (generic_name == "SpareStore") {
    store()->setStoreID(StoreID::SPARE_STORE);
  } else {
    store()->setStoreID(StoreID::UNKNOWN);
  }

  const bool CREATEIF(true);
  // set up the incident service:
  if (!(service("IncidentSvc", m_pIncSvc, CREATEIF)).isSuccess()) {
    msg() << MSG::ERROR 
	<< "Could not locate IncidentSvc "
	<< endreq;
    return StatusCode::FAILURE;
  }

  //start listening to "EndEvent"
  static const int PRIORITY = 100;
  m_pIncSvc->addListener(this, "EndEvent", PRIORITY);
  m_pIncSvc->addListener(this, "BeginEvent", PRIORITY);

  // cache pointer to Persistency Service
  if (!(service("EventPersistencySvc", m_pDataLoader, CREATEIF)).isSuccess()) {
    m_pDataLoader = 0;
    msg() << MSG::ERROR
	<< "Could not get pointer to Persistency Service"
	<< endreq;
    return StatusCode::FAILURE;;
  }

  if (!(service("ClassIDSvc", m_pCLIDSvc, CREATEIF)).isSuccess()) {
    msg() << MSG::ERROR
	<< "Could not get pointer to ClassID Service"
	<< endreq;
    return StatusCode::FAILURE;;
  }

  if (!(service("ProxyProviderSvc", m_pPPS, CREATEIF)).isSuccess()) {
    msg() << MSG::ERROR
	<< "Could not get pointer to ProxyProvider Service"
	<< endreq;
    return StatusCode::FAILURE;;
  }
 
  if ( 0 == m_pPPS || (m_pPPS->preLoadProxies(*m_pStore)).isFailure() )
  {
    SG_MSG_DEBUG(" Failed to preLoad proxies");
    return StatusCode::FAILURE;
  }

  // Get hold of History Service
  if (m_ActivateHistory &&
      !(service("HistorySvc", m_pHistorySvc, CREATEIF)).isSuccess()) {
    msg() << MSG::ERROR
	<< "Could not locate History Service"
	<< endreq;
    return StatusCode::FAILURE;
  }

  // get IPageAccessControlSvc if requested and set the t2p pointer
  if (m_monitorPageAccess.value() && (m_pacSvc.retrieve()).isSuccess()) {
    m_pStore->setPac(&*m_pacSvc);
  }

  return StatusCode::SUCCESS;
}
/// Service start
StatusCode StoreGateSvc::start()    {

  msg() << MSG::INFO << "Start " << name() << endreq;
/*
// This will need regFcn clients to be updated first.
  if ( 0 == m_pPPS || (m_pPPS->preLoadProxies(*m_pStore)).isFailure() )
  {
      msg() << MSG::DEBUG
 	  << " Failed to preLoad proxies"
 	  << endreq;
      return StatusCode::FAILURE;
  }
*/

  return StatusCode::SUCCESS;
}
/// Service start
StatusCode StoreGateSvc::stop()    {

  msg() << MSG::INFO << "Stop " << name() << endreq;
  //HACK ALERT: ID event store objects refer to det store objects
  //by setting an ad-hoc priority for event store(s) we make sure they are finalized and hence cleared first
  // see e.g. https://savannah.cern.ch/bugs/index.php?99993
  if (store()->storeID() == StoreID::EVENT_STORE) {
    ISvcManager* pISM(dynamic_cast<ISvcManager*>(serviceLocator().get()));
    if (!pISM)
      return StatusCode::FAILURE;
    pISM->setPriority(name(), pISM->getPriority(name())+1).ignore();
    msg() << MSG::INFO << "stop: setting service priority to " << pISM->getPriority(name()) 
	  << " so that event stores get finalized and cleared before other stores" <<endmsg;
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////
IIOVSvc* StoreGateSvc::getIIOVSvc() {
  // Get hold of the IOVSvc
  if (0 == m_pIOVSvc && !(service("IOVSvc", m_pIOVSvc)).isSuccess()) {
    msg() << MSG::WARNING
	<< "Could not locate IOVSvc "
	<< endreq;
  }
  return m_pIOVSvc;
}

//////////////////////////////////////////////////////////////
void StoreGateSvc::handle(const Incident &inc) {

  if (inc.type() == "EndEvent") { 
    if (m_DumpStore) {
      SG_MSG_DEBUG("Dumping StoreGate Contents");
      msg() << MSG::INFO 
	  << '\n' << dump() << endl 
	  << endreq;
    }
    if (m_monitorPageAccess.value()) {
      msg() << MSG::INFO << "handle(EndEvent): looking for transient dobj pointers in " << name() << " that were never accessed after being recorded/read from disk"  << endmsg;
      std::vector<DataProxy*> notAccessed(m_pStore->pacReport());
      //loop over all transient raw_ptr and see if they have been accessed
      std::vector<DataProxy*>::iterator p_iter(notAccessed.begin()),
	p_end(notAccessed.end());
      while (p_iter != p_end) {
	DataProxy& p(**p_iter++);
	DataBucketBase *dbb(dynamic_cast<DataBucketBase*>(p.object()));
	if (0 != dbb) {
	  CLID id = p.clID();
	    string tname;
	    m_pCLIDSvc->getTypeNameOfID(id, tname).ignore();
	    msg() << MSG::WARNING  
		 << "\n object @" << dbb->object()
		 << " recorded in store with key="<< p.name()
		 << ", type "  << tname
		 << " (CLID " << id << ')' 
		 << ". Was never accessed! "
		 << endreq;
	}
      }
    }
  }
  
    
  // NO MORE this is now done by the event loop mgrs
  //  else if ( inc.type() == "BeginEvent") 
  //  {
  //     if ( loadEventProxies().isFailure() )
  //     {
  //       msg() << MSG::DEBUG
  // 	  << " Failed to load proxies at Begin Event "
  // 	  << endreq;
  //     }
  //  }
}

StatusCode StoreGateSvc::loadEventProxies() {
  StatusCode sc(StatusCode::SUCCESS);
  //FIXME this should probably be dealt with by the providers
  if (0 != m_pPPS && !m_storeLoaded) {
    m_storeLoaded = true;
    //this (probably) can't be done in initialize (circular init!)
    ActiveStoreSvc* pActive(0);
    const bool CREATEIF(true);
    if (!(serviceLocator()->service("ActiveStoreSvc", pActive, CREATEIF)).isSuccess()) return StatusCode::FAILURE;
    pActive->setStore(this);
    sc=m_pPPS->loadProxies(*m_pStore);
  } 
  return sc;
}

///////////////////////////////////////////////////////////////////
// Create a key for a type (used if the client has not specified a key)
string StoreGateSvc::createKey(const CLID& id)
{
  ostringstream o;
  o << m_pStore->typeCount(id)+1 << std::ends;
  string ret(o.str());
  return ret;
}
//////////////////////////////////////////////////////////////
// clear store
StatusCode StoreGateSvc::clearStore(bool forceRemove)
{
  emptyTrash();
  assert(m_pStore);
  MsgStream* pmlog( msgLvl(MSG::VERBOSE) ? &msg() : 0);
  msg() << MSG::DEBUG << "Clearing store with forceRemove="
	<< forceRemove << endmsg;
  m_pStore->clearStore(forceRemove, pmlog);
  m_storeLoaded=false;  //FIXME hack needed by loadEventProxies
  m_remap_impl->m_remaps.clear();

  // Send a notification that the store was cleared.
  if (m_pIncSvc)
    m_pIncSvc->fireIncident (StoreClearedIncident (this, name()));

  return StatusCode::SUCCESS;
}
//////////////////////////////////////////////////////////////
/// Service finalisation
StatusCode StoreGateSvc::finalize()    {
    msg() << MSG::INFO << "Finalizing " << name() 
      << " - package version " << PACKAGE_VERSION << endreq ;

  // Incident service may not work in finalize.
  // Clear this, so that we won't try to send an incident from clearStore.
  if (m_pIncSvc) {
    m_pIncSvc->release();
    m_pIncSvc = 0;
  }


  const bool FORCEREMOVE(true);
  clearStore(FORCEREMOVE).ignore();

  //protect against double release
  if (m_pHistorySvc) {
    m_pHistorySvc->release();
    m_pHistorySvc = 0;
  }

  return Service::finalize();
}
//////////////////////////////////////////////////////////////
/// Service reinitialization
StatusCode StoreGateSvc::reinitialize()    {
  msg() << MSG::INFO << "Reinitializing " << name() 
      << " - package version " << PACKAGE_VERSION << endreq ;
  const bool FORCEREMOVE(true);
  clearStore(FORCEREMOVE).ignore();
  //not in v20r2p2! return Service::reinitialize();
  return StatusCode::SUCCESS;
}

const InterfaceID& 
StoreGateSvc::interfaceID() { 
  static const InterfaceID _IID("StoreGateSvc", 1, 0);
  return _IID; 
}

  // Query the interfaces.
  //   Input: riid, Requested interface ID
  //          ppvInterface, Pointer to requested interface
  //   Return: StatusCode indicating SUCCESS or FAILURE.
  // N.B. Don't forget to release the interface after use!!!
StatusCode StoreGateSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IProxyDict::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyDict*)this;
  }
  else if ( IStringPool::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IStringPool*)this;
  }
  else if ( IProxyDictWithPool::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyDictWithPool*)this;
  }
  else if ( interfaceID().versionMatch(riid) )    {
    // In principle this should be cast to IStoreGateSvc*. However, there
    // is an anomaly in that existing clients are using the concrete StoreGate
    // interface instread of an abstract IStoreGateSvc interface.
    *ppvInterface = (StoreGateSvc*)this;
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
StatusCode StoreGateSvc::recordAddress(const std::string& skey,
				       IOpaqueAddress* pAddress, 
				       bool clearAddressFlag)
{
  assert(0 != pAddress);
  CLID dataID = pAddress->clID();

  if (dataID == 0)
  {
    msg() << MSG::WARNING
	<< "recordAddress: Invalid Class ID found in IOpaqueAddress @" 
	<< pAddress << ". IOA will not be recorded"
	<< endreq;
	return StatusCode::FAILURE;
  }

  //do not ovewrite a persistent object
  if (m_pPPS) {
    DataProxy *dp(m_pPPS->retrieveProxy(dataID, skey, *m_pStore));
    if (dp && dp->transientAddress() && dp->transientAddress()->provider()) {
      std::string clidTypeName; 
      m_pCLIDSvc->getTypeNameOfID(dataID, clidTypeName).ignore();
      msg() << MSG::WARNING
	    << "recordAddress: failed for key="<< skey << ", type "  << clidTypeName
	    << " (CLID " << dataID << ')' 
	    << "\n there is already a persistent version of this object. Will not record a duplicate! "
	    << endreq;
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
    TransientAddress* tAddr = new TransientAddress(dataID, skey, 
						   pAddress, clearAddressFlag);
    dp = new DataProxy(tAddr, m_pDataLoader, true, true);
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
	<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}    

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// add proxy (with IOpaqueAddress that will later be retrieved from P)
//////////////////////////////////////////////////////////////////////
StatusCode StoreGateSvc::recordAddress(IOpaqueAddress* pAddress, bool clearAddressFlag)
{
  assert(0 != pAddress);

  CLID dataID = pAddress->clID();

  string gK = (pAddress->par())[1];   // transient name by convention
  if (gK.empty()) gK = (pAddress->par())[0];   // FIXME backward compatibility
  if (gK.empty()) gK = createKey(dataID);

  return this->recordAddress(gK, pAddress, clearAddressFlag);
}    

DataProxy* StoreGateSvc::setupProxy(const CLID& dataID, 
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
	  << endreq;
      recycle(pDObj);      // commit this object to trash
      dp = 0;
    } else {
      // Case 1: Proxy found... if not valid, update it:
      dp->setObject(pDObj);
      if (!allowMods) dp->setConst();
    } 
  } else {
    // Case 2: No Proxy found:
    TransientAddress* tAddr = new TransientAddress(dataID, gK);
    dp = new DataProxy(pDObj, tAddr, !allowMods, resetOnly);
    if (!(m_pStore->addToStore(dataID, dp).isSuccess())) {
      msg() << MSG::WARNING
            << " setupProxy:: could not addToStore proxy @" << dp
            << endreq;
      recycle(pDObj);      // commit this object to trash
      delete dp;
      dp = 0;
    }
  }
  return dp;
}

/// set store id in DataStore:
void StoreGateSvc::setStoreID(StoreID::type id)
{
  store()->setStoreID(id);
}

void
StoreGateSvc::keys(const CLID& id, std::vector<std::string>& vkeys, 
     bool includeAlias, bool onlyValid) 

{ 
  return store()->keys(id, vkeys, includeAlias, onlyValid);
} 

bool StoreGateSvc::isSymLinked(const CLID& linkID, DataProxy* dp) 	 
{ 	 
  return (0 != dp) ? dp->transientAddress()->transientID(linkID) : false; 	 
}

//////////////////////////////////////////////////////////////////
// Dump Contents in store:
string StoreGateSvc::dump() const
{ 
  ostringstream ost;
  ost << "<<<<<<<<<<<<<<<<< Data Store Dump >>>>>>>>>>>>>>> \n";
  ost << "StoreGateSvc(" + name() + ")::dump():\n";

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
StoreGateSvc::store() 
{ 
  return m_pStore; 
}

const DataStore* 
StoreGateSvc::store() const 
{ 
  return m_pStore; 
}

 
StatusCode
StoreGateSvc::addSymLink(const CLID& linkid, DataProxy* dp)
{ 
  if (0 == dp) {
    msg() << MSG::WARNING
        << "addSymLink: no target DataProxy found. Sorry, can't link to a non-existing data object"
	<< endreq;
    return StatusCode::FAILURE;
  } 
  StatusCode sc = m_pStore->addSymLink(linkid, dp); 

  // If the symlink is a derived->base conversion, then we may have
  // a different transient pointer for the symlink.
  if (sc.isSuccess()) {
    void* baseptr = SG::DataProxy_cast (dp, linkid);
    this->t2pRegister (baseptr, dp).ignore();
  }
  return sc;
}

 
StatusCode
StoreGateSvc::addAlias(const std::string& aliasKey, SG::DataProxy* proxy)
{
  if (0 == proxy) {
    msg() << MSG::WARNING
          << "addAlias: no target DataProxy given, Cannot alias to a non-existing object" 
          << endreq;
    return StatusCode::FAILURE;
  }

  // add key to proxy and to ProxyStore
  return m_pStore->addAlias(aliasKey, proxy);
}

int StoreGateSvc::typeCount(const CLID& id) const
{
  return m_pStore->typeCount(id);
}  

DataProxy* 
StoreGateSvc::proxy(const void* const pTransient) const
{ 
  return m_pStore->locatePersistent(pTransient); 
}

DataProxy* 
StoreGateSvc::deep_proxy(const void* const pTransient) const
{ 
  return this->proxy (pTransient);
}

DataProxy* 
StoreGateSvc::proxy(const CLID& id) const
{ 
  return proxy(id, false);
}

DataProxy* 
StoreGateSvc::proxy(const CLID& id, bool checkValid) const
{ 
  DataProxy* dp = m_pStore->proxy(id);

  /// Check if it is valid
  if (checkValid && 0 != dp) {
    // FIXME: For keyless retrieve, this checks only the first instance
    // of the CLID in store. If that happens to be invalid, but the second
    // is valid - this does not work (when checkValid is requested).
    return dp->isValid() ? dp : 0;
  }
  //  } else if (0 == dp) {
  //    dp = m_pPPS->retrieveProxy(id, string("DEFAULT"), *m_pStore);
  //  }
  return dp;
}

DataProxy* 
StoreGateSvc::proxy(const CLID& id, const string& key) const
{ 
  return proxy(id, key, false);
}

DataProxy*
StoreGateSvc::proxy(const CLID& id, const string& key, bool checkValid) const
{ 
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
StatusCode StoreGateSvc::addToStore (CLID id, SG::DataProxy* proxy)
{
  return m_pStore->addToStore (id, proxy);
}

std::vector<const SG::DataProxy*> 
StoreGateSvc::proxies() const
{
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

DataProxy*
StoreGateSvc::transientProxy(const CLID& id, const string& key) const
{ 
  DataProxy* dp(m_pStore->proxy(id, key));
  return ( (0 != dp && dp->isValidObject()) ? dp : 0 );
}

DataObject* 
StoreGateSvc::accessData(const CLID& id) const
{ 
  DataProxy* theProxy(proxy(id, true));
  return (0 == theProxy) ? 0 : theProxy->accessData();
}

DataObject* 
StoreGateSvc::accessData(const CLID& id, const string& key) const
{ 
  DataProxy* theProxy(proxy(id, key, true));
  return (0 == theProxy) ? 0 : theProxy->accessData();
}

bool
StoreGateSvc::transientSwap( const CLID& id,
			     const std::string& keyA, const std::string& keyB )
{
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
StoreGateSvc::typeless_record( DataObject* obj, const std::string& key,
			       const void* const raw_ptr,
			       bool allowMods, bool resetOnly, bool noHist)
{
  return typeless_record (obj, key, raw_ptr, allowMods, resetOnly, noHist, 0);
}


StatusCode
StoreGateSvc::typeless_record( DataObject* obj, const std::string& key,
			       const void* const raw_ptr,
			       bool allowMods, bool resetOnly, bool noHist,
                               const std::type_info* tinfo)
{
  const bool NOOVERWRITE(false);
  if ( record_impl( obj, key, raw_ptr, allowMods, resetOnly, NOOVERWRITE, tinfo).isFailure() ) {
    return StatusCode::FAILURE;
  }

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
StoreGateSvc::typeless_overwrite( const CLID& clid, DataObject* obj, const std::string& key,
				  const void* const raw_ptr,
				  bool allowMods, bool resetOnly, bool noHist,
                                  const std::type_info* tinfo)
{
  StatusCode sc(StatusCode::SUCCESS);
  SG::DataProxy* toRemove(proxy(clid, key, false));
  if (0 != toRemove) {  
    const bool FORCEREMOVE(true); 	 
    sc =removeProxy(toRemove, (void*)0, FORCEREMOVE);
  }
  if (sc.isSuccess()) {
    const bool ALLOWOVERWRITE(true);
    sc = record_impl( obj, key, raw_ptr, allowMods, resetOnly, ALLOWOVERWRITE, tinfo);
    if ( m_ActivateHistory && noHist && store()->storeID() == StoreID::EVENT_STORE ) {
      sc = record_HistObj( obj->clID(), key, name(), allowMods, resetOnly );
    }
  }
  //for detector store objects managed by IIOVSvc, replace the old proxy with the new one (#104311)
  if (toRemove && sc.isSuccess() && store()->storeID() == StoreID::DETECTOR_STORE) {
    sc = getIIOVSvc()->replaceProxy(toRemove, proxy(clid, key));
  }
  return sc;
}

StatusCode
StoreGateSvc::record_impl( DataObject* pDObj, const std::string& key,
			   const void* const raw_ptr,
			   bool allowMods, bool resetOnly, bool allowOverwrite,
                           const std::type_info* tinfo)
{
  CLID clid = pDObj->clID();
  if (!allowOverwrite && m_pPPS) {
    //do not ovewrite a persistent object
    DataProxy *dp(m_pPPS->retrieveProxy(clid, key, *m_pStore));
    if (dp && dp->transientAddress() && dp->transientAddress()->provider()) {
      std::string clidTypeName; 
      m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
      msg() << MSG::WARNING
	    << "record_impl: you are recording an object with key "<< key << ", type "  << clidTypeName
	    << " (CLID " << clid << ')' 
	    << "\n There is already a persistent version of this object. Recording a duplicate may lead to unreproducible results and it is deprecated. Use StoreGateSvc::overwrite method instead"
	    << endreq;
      //NOT YET
      // msg() << MSG::WARNING
      // 	    << "record_impl: failed for key="<< key << ", type "  << clidTypeName
      // 	    << " (CLID " << clid << ')' 
      // 	    << "\n there is already a persistent version of this object. Will not record a duplicate! "
      // 	    << endreq;
      // DataBucketBase* pDBB(dynamic_cast<DataBucketBase*>(pDObj));
      // if (pDBB) pDBB->relinquish(); //don't own the data obj already recorded!
      // this->recycle(pDObj);
      // return StatusCode::FAILURE;
    }
  }
  //now check whether raw_ptr has already been recorded
  //We need to do this before we create the bucket, the proxy etc
  SG::DataProxy* dp(proxy(raw_ptr));
  if (0 != dp) {
    std::string clidTypeName; 
    m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
    msg() << MSG::WARNING
	<< "record_impl: failed for key="<< key << ", type "  << clidTypeName
	<< " (CLID " << clid << ')' 
	<< "\n object @" << raw_ptr 
	<< " already in store with key="<< dp->name()
	<< ". Will not record a duplicate! "
	<< endreq;
    DataBucketBase* pDBB(dynamic_cast<DataBucketBase*>(pDObj));
    if (pDBB) pDBB->relinquish(); //don't own the data obj already recorded!
    this->recycle(pDObj);
    return StatusCode::FAILURE;
  }


  // setup the proxy
  dp = setupProxy( clid, key, pDObj, allowMods, resetOnly );
  if ( 0 == dp ) {
    std::string clidTypeName; 
    m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
    msg() << MSG::WARNING
	<< "record_impl: Problem setting up the proxy for object @" <<raw_ptr 
	<< "\n recorded with key " << key 
	<< " of type "  << clidTypeName
	<< " (CLID " << clid << ") in DataObject @" << pDObj
	<< endreq;

    return StatusCode::FAILURE;
  }

  // record in t2p:
  if ( !(this->t2pRegister( raw_ptr, dp )).isSuccess() ) {
    std::string clidTypeName; 
    m_pCLIDSvc->getTypeNameOfID(clid, clidTypeName).ignore();
    msg() << MSG::WARNING
	<< "record_impl: can not add to t2p map object @" <<raw_ptr 
	<< "\n with key " << key 
	<< " of type "  << clidTypeName
	<< " (CLID " << clid << ')' 
	<< endreq;
    return StatusCode::FAILURE;
  }

  addAutoSymLinks (key, clid, dp, tinfo);

  //handle versionedKeys: we register an alias with the "true" key
  //unless an object as already been recorded with that key.
  //Notice that addAlias overwrites any existing alias, so a generic
  //retrieve will always return the last version added 
  //FIXME not the one with the highest version
  if (SG::VersionedKey::isVersionedKey(key)) {
    SG::VersionedKey vk(key);
    std::string trueKey(vk.key());
    //first check if we have already a proxy for trueKey
    if (0 == this->proxy(clid,trueKey)) {
      //if none found add the alias
      if (!(this->addAlias(trueKey, dp)).isSuccess()) {
        msg() << MSG::WARNING
              << "record_impl: Could not setup alias key " << trueKey 
              << " for VersionedKey " << key
              << ". Generic access to this object with clid" << clid 
              << " will not work"
              << endreq;      
      }
    }
  }

  return StatusCode::SUCCESS;
  
}

DataProxy*
StoreGateSvc::locatePersistent(const TransientAddress* tAddr, 
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
StoreGateSvc::removeProxy(DataProxy* proxy, const void* pTrans, 
			  bool resetOnly)
{
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
  return m_pStore->removeProxy(proxy, resetOnly);
}

StatusCode
StoreGateSvc::t2pRegister(const void* const pTrans, DataProxy* const pPers)
{ 
  return m_pStore->t2pRegister(pTrans, pPers);
}


void
StoreGateSvc::t2pRemove(const void* const pTrans)
{
  m_pStore->t2pRemove(pTrans); 
}

void
StoreGateSvc::msg_update_handler(Property& /*outputLevel*/)
{
  msg().setLevel (outputLevel());
  msgSvc()->setOutputLevel(name(), outputLevel());
}

StatusCode 
StoreGateSvc::proxyRange(const CLID& id,
			 SG::ConstProxyIterator& begin,
			 SG::ConstProxyIterator& end) const {
  return m_pStore->pRange(id,begin,end);
}

StatusCode StoreGateSvc::setConst(const void* pObject)
{
  // Check if dataproxy does not exist
  DataProxy * dp = proxy(pObject); 

  if (0 == dp)
  {
    msg() << MSG::WARNING
          << "setConst: NO Proxy for the dobj you want to set const"
          << endreq;
    return StatusCode::FAILURE;
  }

  dp->setConst();
  return StatusCode::SUCCESS;
}

//put a bad (unrecordable) dobj away
void StoreGateSvc::recycle(DataObject* pBadDObj) {
  assert(pBadDObj);
  //DataObject::release won't delete an object refCount==0
  if (0 == pBadDObj->refCount()) pBadDObj->addRef(); 
    m_trash.push_back(pBadDObj);
}

//throw away bad objects
void StoreGateSvc::emptyTrash() {
  while (!m_trash.empty()) {
    m_trash.front()->release();  //delete the bad data object
    m_trash.pop_front();     //remove pointer from list
  }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool StoreGateSvc::bindHandleToProxy(const CLID& id, const string& key,
				     IResetable* ir, DataProxy *&dp) 
{

  dp = (0 == m_pPPS) ? 0 : m_pPPS->retrieveProxy(id, key, *m_pStore);

  if (0 == dp) return false;

  if (! dp->bindHandle(ir) ) {
    msg() << MSG::FATAL << "DataHandle at " << hex << ir << dec 
	<< " already bound to DataProxy with key " << ir->key() 
	<< ". Cannot bind to proxy " << dp->name() << " as well\n"
	<< "        You have probably registered multiple callbacks via regFcn with the same DataHandle using different keys (DataProxies)\n"
	<< endreq;
    return false;
  }
    
  //already done in DataHandleBase::setState  dp->addRef();

#ifndef NDEBUG
#ifndef HAVE_NEW_IOSTREAMS
  SG_MSG_DEBUG(" Bound handle " << hex << ir << " to proxy " 
		<< dp << dec); 
#else
  SG_MSG_DEBUG(" Bound handle " << MSG::hex << ir << " to proxy " 
		<< dp << MSG::dec); 
#endif
#endif
    return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
StoreGateSvc::record_HistObj(const CLID& id, const std::string& key,
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
  return record_impl(obj, idname, dho, allowMods, resetOnly, ALLOWOVERWRITE,
                     &typeid(DataHistory));
}


/**
 * @brief Find the key for a string/CLID pair.
 * @param str The string to look up.
 * @param clid The CLID associated with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
StoreGateSvc::sgkey_t
StoreGateSvc::stringToKey (const std::string& str, CLID clid)
{
  return m_stringpool.stringToKey (str, clid);
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* StoreGateSvc::keyToString (sgkey_t key) const
{
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
StoreGateSvc::keyToString (sgkey_t key, CLID& clid) const
{
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
void StoreGateSvc::registerKey (sgkey_t key,
                                const std::string& str,
                                CLID clid)
{
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
StoreGateSvc::releaseObject(const CLID& id, const std::string& key) {
  DataProxy *pP(0);
  if (0 != (pP = proxy(id, key))) {
    // remove all entries from t2p map
    SG::DataProxy::CLIDCont_t clids = pP->transientID();
    SG::DataProxy::CLIDCont_t::const_iterator i(clids.begin()), e(clids.end());
    while (i != e) t2pRemove(SG::DataProxy_cast (pP, *i++));
    DataBucketBase *pDBB(dynamic_cast<DataBucketBase*>(pP->object()));
    //tell the bucket to let go of the data object
    if (0 != pDBB) pDBB->relinquish(); //somebody else better took ownership
    pP->reset();
  }
}

void
StoreGateSvc::clearProxyPayload(SG::DataProxy* dp) {
  SG::DataProxy::CLIDCont_t clids = dp->transientID();
  SG::DataProxy::CLIDCont_t::const_iterator i(clids.begin()), e(clids.end());
  while (i != e) {
    t2pRemove(SG::DataProxy_cast (dp, *i++));
  }
  dp->reset();
}


/**
 * @brief Declare a remapping.
 * @brief source Key hash of the container being remapped.
 * @brief target Key hash of the container being remapped to.
 * @brief index_offset Amount by which the index should be adjusted
 *        between the two containers.
 */
void StoreGateSvc::remap_impl (sgkey_t source,
                               sgkey_t target,
                               off_t index_offset)
{
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
bool StoreGateSvc::tryELRemap (sgkey_t sgkey_in, size_t index_in,
                               sgkey_t& sgkey_out, size_t& index_out)
{
  SG::RemapImpl::remap_map_t::iterator i =
    m_remap_impl->m_remaps.find (sgkey_in);
  if (i == m_remap_impl->m_remaps.end())
    return false;
  const SG::RemapImpl::remap_t& payload = i->second;
  sgkey_out = payload.target;
  index_out = index_in + payload.index_offset;
  return true;
}

DataObject* StoreGateSvc::typeless_readPrivateCopy(const CLID& clid,
						   const std::string& key) {
  DataObject *pObj(0);
  DataProxy *p(this->proxy(clid, key));
  if (p) {
    if (p->object()) { //this looks in transient mem only
      //if there is a dobj in transient memory we take ownership with addRef
      p->addRef();
      //and make the store forget about the proxy for a moment
      const bool FORCEREMOVE(true); 	 
      store()->removeProxy(p, FORCEREMOVE).ignore();
      //now we try to read the object from disk. Relies on PPS to reload proxy
      DataProxy *pDisk(this->proxy(clid, key));
      if (pDisk) {
	//We are managing the pObj so we addRef it
	if ( (pObj = pDisk->accessData()) ) pObj->addRef();
	//don't need this guy anymore, notice we use the StoreGateSvc version
	//to remove the t2p entry as well
	removeProxy(pDisk, SG::DataProxy_cast(pDisk,clid), FORCEREMOVE).ignore();
      }
      //replace the "transient" proxy where it was
      if (store()->addToStore(clid, p).isFailure()) {
        msg() << MSG::ERROR << "StoreGateSvc::typeless_readPrivateCopy: "
              << "could not re-record proxy in store: "
              << clid << "/" << key << endmsg;
      }
    } else if ( (pObj = p->accessData()) ) { //try reading from disk
      //We are managing the pObj so we addRef it
      pObj->addRef();
      //and make the proxy forget about it
      p->reset();
    }
  }
  if (0 == pObj) {
    string errType;
    m_pCLIDSvc->getTypeNameOfID(clid, errType).ignore();
    msg() << MSG::WARNING << "StoreGateSvc::typeless_readPrivateCopy: "
          << "did not find object of type "
          << errType << " with key " << key << endmsg;
  }
  return pObj;
}


/// Add automatically-made symlinks for DP.
void StoreGateSvc::addAutoSymLinks (const std::string& key,
                                    CLID clid,
                                    DataProxy* dp,
                                    const std::type_info* tinfo,
                                    bool warn_nobib /*= true*/)
{
  // Automatically make all legal base class symlinks
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find( clid );
  if (!bib && tinfo)
    bib = SG::BaseInfoBase::find (*tinfo);
  if ( bib ) {
    std::vector<CLID> bases = bib->get_bases();
    for ( std::size_t i = 0, iMax = bases.size(); i < iMax; ++i ) {
      if ( bases[i] != clid ) {
	if ( addSymLink( bases[i], dp ).isSuccess() ) {
          // register with t2p
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
            << endreq;
    }
  }
}


// This is intended to be called from the debugger.
void SG_dump (StoreGateSvc* sg)
{
  std::cout << sg->dump() << "\n";
}


