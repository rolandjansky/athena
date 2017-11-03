/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreClearedIncident.h"
#include "AthAllocators/ArenaHeader.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/tools/SGImplSvc.h"

#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include <fstream>

using namespace SG;
using namespace std;

__thread HiveEventSlot* s_pSlot(0);


/// Standard Constructor
StoreGateSvc::StoreGateSvc(const std::string& name,ISvcLocator* svc) : 
  Service(name,svc), 
  m_defaultStore(0),
  m_pPPSHandle("ProxyProviderSvc", name),
  m_incSvc("IncidentSvc", name),
  m_activeStoreSvc("ActiveStoreSvc", name),
  m_pIOVSvc(nullptr)
{
  //our properties
  //properties of SGImplSvc
  declareProperty("Dump", m_DumpStore=false, "Dump contents at EndEvent");
  declareProperty("ActivateHistory", m_ActivateHistory=false, "record DataObjects history");
  declareProperty("ProxyProviderSvc", m_pPPSHandle);
  declareProperty("IncidentSvc", m_incSvc);

  //add handler for Service base class property
  //FIXME m_outputLevel.declareUpdateHandler(&SGImplSvc::msg_update_handler, this);
}
 
/// Standard Destructor
StoreGateSvc::~StoreGateSvc()  
{}

void 
StoreGateSvc::setDefaultStore(SGImplSvc* pStore) {
  if (m_defaultStore) m_defaultStore->release();
  m_defaultStore = pStore;
  if (m_defaultStore) m_defaultStore->addRef();  
}

bool
StoreGateSvc::isHiveStore() const {
  return ((m_defaultStore->store()->storeID() == StoreID::EVENT_STORE) &&  (0 != s_pSlot));
}

SGImplSvc* 
StoreGateSvc::currentStore() const {
  return isHiveStore() ? s_pSlot->pEvtStore : m_defaultStore;
}


void 
StoreGateSvc::setSlot(SG::HiveEventSlot* pSlot) { 
  s_pSlot=pSlot;
  if ( 0 != s_pSlot) {
    s_pSlot->pEvtStore->makeCurrent();
  }
}

SG::HiveEventSlot*
StoreGateSvc::currentSlot() { 
  return s_pSlot; 
}

/////////////////////////////////////////////////////////////////


bool 
StoreGateSvc::newDataObjectsPresent() {
  _SGXCALL(newDataObjectsPresent, (), false);
}

StatusCode 
StoreGateSvc::getNewDataObjects(DataObjIDColl& products) {
  _SGXCALL(getNewDataObjects, (products), StatusCode::FAILURE);
}

void
StoreGateSvc::commitNewDataObjects() {
  _SGVOIDCALL(commitNewDataObjects, ());
}

///a new object transient object has been recorded
void 
StoreGateSvc::addedNewTransObject(CLID clid, const std::string& key) {
  _SGVOIDCALL(addedNewTransObject, (clid, key));
}

///a new object persistent object has been recorded
void 
StoreGateSvc::addedNewPersObject(CLID clid, SG::DataProxy* dp) {
  _SGVOIDCALL(addedNewPersObject, (clid, dp));
}

/// Create a proxy object using an IOpaqueAddress and a transient key
StatusCode 
StoreGateSvc::recordAddress(const std::string& skey,
                            IOpaqueAddress* pAddress, bool clearAddressFlag) {
  _SGXCALL(recordAddress, (skey, pAddress, clearAddressFlag), StatusCode::FAILURE);
}
/// Create a proxy object using an IOpaqueAddress
StatusCode 
StoreGateSvc::recordAddress(IOpaqueAddress* pAddress, bool clearAddressFlag) {
  _SGXCALL(recordAddress, (pAddress, clearAddressFlag), StatusCode::FAILURE);
}


StatusCode 
StoreGateSvc::setConst(const void* pObject) {
  _SGXCALL(setConst, (pObject), StatusCode::FAILURE);
}

/// DEPRECATED, use version taking ref to vector
std::vector<std::string> //FIXME inefficient. Should take ref to vector
StoreGateSvc::keys(const CLID& id, bool allKeys){
  std::vector<std::string> nullV;
  _SGXCALL( keys, (id, allKeys), nullV );
}




/////////////////////////////////////////////////////////////
/// Service initialization
StatusCode StoreGateSvc::initialize()    {

  // Initialize service:
  CHECK( Service::initialize() );

  msg() << MSG::VERBOSE << "Initializing " << name() 
        << " - package version " << PACKAGE_VERSION << endmsg;

  // lifted from AlwaysPrivateToolSvc (see Wim comment about lack of global jo svc accessor
  // retrieve the job options svc (TODO: the code below relies heavily on
  // internals; figure out if there's no global getJobOptionsSvc() ... )
  IAppMgrUI* appmgr = Gaudi::createApplicationMgr();
  IProperty* appmgrprop = 0;
  appmgr->queryInterface( IProperty::interfaceID(), (void**)&appmgrprop ).ignore();
  //all of the above to get the jo svc type
  const Property& prop = appmgrprop->getProperty( "JobOptionsSvcType" );
  IJobOptionsSvc* pJOSvc(0);
  if ( serviceLocator()->service( prop.toString(), "JobOptionsSvc", pJOSvc ).isFailure() ) {
    error() << "Failed to retrieve JobOptionsSvc" << endmsg;
  }
  //copy our properties to the prototype (default) SGImplSvc
  std::string implStoreName = name() + "_Impl";
  const std::vector<const Property*>* props = pJOSvc->getProperties( name() );
  if ( props ) {
    std::vector<const Property*>::const_iterator prop(props->begin());
    std::vector<const Property*>::const_iterator pEnd(props->end());
    while (prop != pEnd) {
      pJOSvc->addPropertyToCatalogue( implStoreName, **prop ).ignore();
      ++prop;
    }    
  } 
  pJOSvc->release();
  pJOSvc=0;

  //HACK ALERT: using createService rather then the customary service(...,CREATEIF=true) we set our pointer
  // to SGImplSvc early (even before it is initialized). This should help take care of some initialize loops
  // for example when we try to record an address from one of the address providers initialize methods

  std::string implStoreFullName = "SGImplSvc/" + implStoreName;
  debug() << "trying to create store " << implStoreFullName << endmsg;
  
  ISvcManager* pSM(dynamic_cast<ISvcManager*>(&*serviceLocator()));
  if (!pSM) std::abort();
  m_defaultStore = dynamic_cast<SGImplSvc*>( (pSM->createService(implStoreFullName)).get() );

  if (!m_defaultStore) {
    error() << "Could not create store " << implStoreFullName << endmsg;
    return StatusCode::FAILURE;
  }
  
  if ( m_defaultStore->sysInitialize().isSuccess() ) {
    // createService returns to us a reference to the service; we shouldn't
    // increment it again.
    //m_defaultStore->addRef();

    // If this is the default event store (StoreGateSvc), then declare
    // our arena as the default for memory allocations.
    if (name()  == "StoreGateSvc") {
      m_defaultStore->makeCurrent();
    }
  } else {
    error() << "Could not initialize default store " << implStoreFullName 
            << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_incSvc.retrieve().isSuccess() ) {
    error() << "Could not locate IncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  // Don't retrieve m_activeStoreSvc here to prevent a possible
  // initialization loop.

  const int PRIORITY=100;
  m_incSvc->addListener(this, "EndEvent",PRIORITY);
  m_incSvc->addListener(this, "BeginEvent", PRIORITY);

  return StatusCode::SUCCESS;
}

/// Service start
StatusCode StoreGateSvc::stop()    {
  msg() << MSG::VERBOSE << "Stop " << name() << endmsg;
  //HACK ALERT: ID event store objects refer to det store objects
  //by setting an ad-hoc priority for event store(s) we make sure they are finalized and hence cleared first
  // see e.g. https://savannah.cern.ch/bugs/index.php?99993
  if (m_defaultStore->store()->storeID() == StoreID::EVENT_STORE) {
    ISvcManager* pISM(dynamic_cast<ISvcManager*>(serviceLocator().get()));
    if (!pISM)
      return StatusCode::FAILURE;
    pISM->setPriority(name(), pISM->getPriority(name())+1).ignore();
    msg() << MSG::VERBOSE << "stop: setting service priority to " << pISM->getPriority(name()) 
          << " so that event stores get finalized and cleared before other stores" <<endmsg;
  }
  return StatusCode::SUCCESS;
}

void StoreGateSvc::handle(const Incident &inc) {
  m_defaultStore->handle(inc);
}

//////////////////////////////////////////////////////////////
IIOVSvc* StoreGateSvc::getIIOVSvc() {
  // Get hold of the IOVSvc
  if (0 == m_pIOVSvc && !(service("IOVSvc", m_pIOVSvc)).isSuccess()) {
    msg() << MSG::WARNING
          << "Could not locate IOVSvc "
          << endmsg;
  }
  return m_pIOVSvc;
}

StatusCode
StoreGateSvc::finalize() {
  CHECK( Service::finalize() );
  msg() << MSG::VERBOSE << "Finalizing " << name() 
        << " - package version " << PACKAGE_VERSION << endmsg;
  if (m_defaultStore) {
    // m_defaultStore is not active, so ServiceManager won't finalize it!
    CHECK( m_defaultStore->finalize());
    m_defaultStore->release();
  }
  return StatusCode::SUCCESS;
}


/// get proxy for a given data object address in memory
DataProxy* 
StoreGateSvc::proxy(const void* const pTransient) const {
  _SGXCALL(proxy, (pTransient), 0);
}

/// get default proxy with given id. Returns 0 to flag failure
DataProxy* 
StoreGateSvc::proxy(const CLID& id) const {
  _SGXCALL(proxy, (id), 0);
}

/// get proxy with given id and key. Returns 0 to flag failure
DataProxy* 
StoreGateSvc::proxy(const CLID& id, const std::string& key) const { 
  _SGXCALL(proxy, (id, key), 0);
}


/// get default proxy with given id, optionally checking validity.
///  @returns 0 to flag failure
SG::DataProxy* 
StoreGateSvc::proxy(const CLID& id, bool checkValid) const {
  _SGXCALL(proxy, (id, checkValid), 0);
}

/// get proxy with given id and key, optionally checking validity.
///  @returns 0 to flag failure
SG::DataProxy* 
StoreGateSvc::proxy(const CLID& id, const std::string& key, bool checkValid) const {
  _SGXCALL(proxy, (id, key, checkValid), 0);
}


/**
 * @brief Raw addition of a proxy to the store.
 * @param id CLID of object being added.
 * @param proxy proxy to add.
 */
StatusCode StoreGateSvc::addToStore (CLID id, SG::DataProxy* proxy)
{
  _SGXCALL(addToStore,  (id, proxy), StatusCode::FAILURE);
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
SG::DataProxy*
StoreGateSvc::recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                            const std::string& key,
                            bool allowMods,
                            bool returnExisting)
{
  _SGXCALL(recordObject, (std::move(obj), key, allowMods, returnExisting), nullptr);
}


/**
 * @brief Inform HIVE that an object has been updated.
 * @param id The CLID of the object.
 * @param key The key of the object.
 */
StatusCode StoreGateSvc::updatedObject (CLID id, const std::string& key)
{
  _SGXCALL(updatedObject, (id, key), StatusCode::FAILURE);
}


/// return the list of all current proxies in store
vector<const SG::DataProxy*> 
StoreGateSvc::proxies() const {
  vector<const SG::DataProxy*> nullV;
  _SGXCALL(proxies, (), nullV);
}

/// Return all CLIDs in the store.
vector<CLID> 
StoreGateSvc::clids() const
{
  vector<CLID> nullV;
  _SGXCALL(clids, (), nullV);
}

/// get proxy with given id and key. Does not query ProxyProviderSvc.
///  @returns 0 to flag failure
SG::DataProxy* 
StoreGateSvc::transientProxy(const CLID& id, const std::string& key) const {
  _SGXCALL(transientProxy, (id, key), 0);
}


/// find proxy and access its data. Returns 0 to flag failure
DataObject* 
StoreGateSvc::accessData(const CLID& id) const {
  _SGXCALL(accessData, (id), 0);
}

/// find proxy and access its data. Returns 0 to flag failure
DataObject* 
StoreGateSvc::accessData(const CLID& id, const std::string& key) const {
  _SGXCALL(accessData, (id, key), 0);
}

bool
StoreGateSvc::transientSwap( const CLID& id,
                             const std::string& keyA, const std::string& keyB ) {
  _SGXCALL(transientSwap, (id, keyA, keyB), false);
}

DataObject*
StoreGateSvc::typeless_readPrivateCopy(const CLID& clid, const std::string& key) {
  _SGXCALL(typeless_readPrivateCopy, (clid, key), 0);
}
/// type-less recording of an object with a key, allow possibility of
/// specifying const-access and history record
StatusCode 
StoreGateSvc::typeless_record( DataObject* obj, const std::string& key,
                               const void* const raw_ptr,
                               bool allowMods, bool resetOnly,
                               bool noHist ) {
  _SGXCALL(typeless_record, (obj, key, raw_ptr, allowMods, resetOnly, noHist), StatusCode::FAILURE);
}
/// same as typeless_record, allows to overwrite an object in memory or on disk
StatusCode 
StoreGateSvc::typeless_overwrite( const CLID& id,
                                  DataObject* obj, const std::string& key,
                                  const void* const raw_ptr,
                                  bool allowMods,
                                  bool noHist,
                                  const std::type_info* tinfo) {
  _SGXCALL(typeless_overwrite, (id, obj, key, raw_ptr, allowMods, noHist, tinfo), StatusCode::FAILURE);
}

/// set store id in DataStore:
void 
StoreGateSvc::setStoreID(StoreID::type id)
{
  _SGVOIDCALL(setStoreID,(id));
}
StoreID::type 
StoreGateSvc::storeID() const
{
  _SGXCALL(storeID,(),StoreID::UNKNOWN);
}

void
StoreGateSvc::keys(const CLID& id, std::vector<std::string>& vkeys, 
                   bool includeAlias, bool onlyValid) 
{ 
  _SGVOIDCALL(keys,(id, vkeys, includeAlias, onlyValid));
} 



std::string 
StoreGateSvc::dump() const {
  std::string nullS;
  _SGXCALL(dump, (), nullS);  
}

int
StoreGateSvc::typeCount(const CLID& clid) const{
  _SGXCALL(typeCount, (clid), -1);
}



typename StoreGateSvc::sgkey_t 
StoreGateSvc::stringToKey (const std::string& str, CLID clid) {
  _SGXCALL( stringToKey, (str, clid), 0 );
}

const std::string* 
StoreGateSvc::keyToString (sgkey_t key) const {
  _SGXCALL( keyToString, (key), 0 );
}

const std::string* 
StoreGateSvc::keyToString (sgkey_t key, CLID& clid) const {
  _SGXCALL( keyToString, (key, clid), 0 );
}

void 
StoreGateSvc::registerKey (sgkey_t key,
                           const std::string& str,
                           CLID clidid) {
  _SGVOIDCALL( registerKey, (key, str, clidid) );
}
void 
StoreGateSvc::remap_impl (sgkey_t source,
                          sgkey_t target,
                          off_t index_offset) {
  _SGVOIDCALL( remap_impl, (source, target, index_offset) );
}

bool 
StoreGateSvc::tryELRemap (sgkey_t sgkey_in, size_t index_in,
                          sgkey_t& sgkey_out, size_t& index_out) {
  _SGXCALL( tryELRemap, (sgkey_in, index_in, sgkey_out, index_out), false );
}

StatusCode 
StoreGateSvc::proxyRange(const CLID& id,
                         SG::ConstProxyIterator& beg,
                         SG::ConstProxyIterator& end) const {
  _SGXCALL( proxyRange, (id, beg, end), StatusCode::FAILURE );
}


void 
StoreGateSvc::releaseObject(const CLID& id, const std::string& key) {
  _SGVOIDCALL( releaseObject, (id, key) );
}

void 
StoreGateSvc::clearProxyPayload(SG::DataProxy* proxy) {
  _SGVOIDCALL( clearProxyPayload, (proxy) );
}


StatusCode 
StoreGateSvc::loadEventProxies() {
  CHECK( m_activeStoreSvc.retrieve() );
  m_activeStoreSvc->setStore(this);
  _SGXCALL(loadEventProxies, (), StatusCode::FAILURE);
}

//////////////////////////////////////////////////////////////
// clear store
StatusCode 
StoreGateSvc::clearStore(bool forceRemove)
{
  StatusCode sc;
  if (isHiveStore()) { 
    if (0 != s_pSlot->pEvtStore) {
      sc = s_pSlot->pEvtStore->clearStore(forceRemove);
    }
  } else sc = m_defaultStore->clearStore(forceRemove);

  // Send a notification that the store was cleared.
  if (sc.isSuccess()) {
    m_incSvc->fireIncident(StoreClearedIncident (this, name()));
  }
  return sc;
}

void
StoreGateSvc::emptyTrash() {
  _SGVOIDCALL( emptyTrash, () );
}

const InterfaceID& 
StoreGateSvc::interfaceID() { 
  static const InterfaceID IDStoreGateSvc("StoreGateSvc", 1, 0);
  return IDStoreGateSvc; 
}
StatusCode StoreGateSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( interfaceID().versionMatch(riid) )    {
    *ppvInterface = (StoreGateSvc*)this;
  }
  else if ( IProxyDict::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyDict*)this;
  }
  else if ( IHiveStore::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IHiveStore*)this;
  }
  else if ( IHiveStoreMgr::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IHiveStoreMgr*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


/// The current store is becoming the active store.  Switch the
/// allocation arena, if needed.
// Only intended to be called by ActiveStoreSvc.
void StoreGateSvc::makeCurrent()
{
  _SGVOIDCALL (makeCurrent, ());
}

StatusCode StoreGateSvc::removeProxy(SG::DataProxy* proxy, const void* pTrans, 
                                     bool forceRemove) {
  _SGXCALL(removeProxy, (proxy, pTrans, forceRemove), StatusCode::FAILURE);
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
StoreGateSvc::createObj (IConverter* cvt,
                         IOpaqueAddress* addr,
                         DataObject*& refpObject)
{
  _SGXCALL( createObj, (cvt, addr, refpObject), StatusCode::FAILURE );
}


void SG_dump (StoreGateSvc* sg)
{
  std::cout << sg->dump() << "\n";
}

void SG_dump (StoreGateSvc* sg, const char* fname)
{
  std::ofstream f (fname);
  f << sg->dump() << "\n";
  f.close();
}

