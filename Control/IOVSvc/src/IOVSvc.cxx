/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  IOVSvc.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *
 *  Provides automatic updating and callbacks for time dependent data
 *
 *****************************************************************************/

#include "IOVSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/DataBucketBase.h"

#include "IOVSvc/IIOVSvcTool.h"
#include "GaudiKernel/IConversionSvc.h"

using SG::DataProxy;
using SG::TransientAddress;

const std::string defaultStore = "StoreGateSvc";


namespace {


/**
 * @brief Helper to check two EventIDBase objects for equality.
 *        Check either/both of run+lbn or timestamp.
 *        Don't use operator== for EventIDBase; that compares run+event+lbn only.
 */
bool eventIDMatch (const EventIDBase& e1, const EventIDBase& e2)
{
  if (e1.isRunLumi()) {
    if (e1.run_number() != e2.run_number()) return false;
    if (e1.lumi_block() != e2.lumi_block()) return false;
  }
  if (e1.isTimeStamp()) {
    if (e1.time_stamp() != e2.time_stamp()) return false;
    if (e1.time_stamp_ns_offset() != e2.time_stamp_ns_offset()) return false;
  }
  return true;
}


} // anonymous namespace



//
///////////////////////////////////////////////////////////////////////////
//

IOVSvc::IOVSvc( const std::string& name, ISvcLocator* svc )
  : base_class( name, svc ), 
    p_toolSvc("ToolSvc",name),
    p_CLIDSvc("ClassIDSvc",name),
    p_sgs("StoreGateSvc",name),
    p_detStore("StoreGateSvc/DetectorStore",name),
    p_condSvc("CondSvc",name)
{

  declareProperty("preLoadRanges",m_preLoadRanges=false);
  declareProperty("preLoadData",m_preLoadData=false);
  declareProperty("partialPreLoadData",m_partialPreLoadData=true);
  declareProperty("preLoadExtensibleFolders", m_preLoadExtensibleFolders=true);
  declareProperty("updateInterval", m_updateInterval="Event");
  declareProperty("sortKeys",m_sortKeys=true);
  declareProperty("forceResetAtBeginRun", m_forceReset=false);

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IOVSvc::~IOVSvc() {

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVSvc::initialize() {
  StatusCode status = AthService::initialize();
  msg().setLevel( m_outputLevel.value() );

  ATH_MSG_DEBUG( "Initializing IOVSvc version " << PACKAGE_VERSION  );
  ATH_MSG_DEBUG( "AthService initialized"  );

  if (!p_sgs.isValid()) {
    ATH_MSG_ERROR("could not get the Event Store");
    status = StatusCode::FAILURE;
  }

  if (!p_detStore.isValid()) {
    ATH_MSG_ERROR("could not get the Detector Store");
    status = StatusCode::FAILURE;
  }

  if (!p_condSvc.isValid()) {
    ATH_MSG_ERROR("could not get the ConditionSvc");
    status = StatusCode::FAILURE;
  }

  return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVSvc::finalize()
{
  ATH_CHECK( AthService::finalize() );
  ATH_MSG_DEBUG( "Service finalised successfully" );
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Register a DataProxy with the service
///
StatusCode 
IOVSvc::regProxy( DataProxy *proxy, const std::string& key,
                  const std::string& storeName ) {

  std::lock_guard<std::recursive_mutex> lock(m_lock);
  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    ATH_MSG_ERROR( "regProxy: no IOVSvcTool associated with store \"" 
                   << storeName << "\" and failed to create one"  );
    return (StatusCode::FAILURE);
  }

  IIOVSvcTool *ist2 = getTool( proxy );
  if (ist2 != 0) {
    if (ist2 != ist) {
      ATH_MSG_ERROR( "regProxy: when registering proxy for " 
                     << fullProxyName(proxy) << " with store \"" << storeName
                     << "\", it is already registered with store \"" 
                     << ist2->getStoreName() << "\""  );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "regProxy: proxy for " << fullProxyName(proxy)
                     << " already registered with store \"" << storeName << "\""
                     );
      return StatusCode::SUCCESS;
    }
  }

  return ist->regProxy( proxy, key );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Register a DataProxy with the service
///
StatusCode 
IOVSvc::regProxy( const CLID& clid, const std::string& key,
                  const std::string& storeName ) {

  std::lock_guard<std::recursive_mutex> lock(m_lock);
  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    ATH_MSG_ERROR( "regProxy: no IOVSvcTool associated with store \"" 
                   << storeName << "\" and failed to create one."  );
    return (StatusCode::FAILURE);
  }

  IIOVSvcTool *ist2 = getTool( clid, key );
  if (ist2 != 0) {
    if (ist2 != ist) {
      ATH_MSG_ERROR( "regProxy: when registering proxy for "
                     << fullProxyName(clid,key)
                     << " with store " << storeName
                     << ", it is already registered with store \"" 
                     << ist2->getStoreName() << "\""  );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "regProxy: proxy for " << fullProxyName(clid,key)
                     << " already registered with store \"" << storeName << "\""
                     );
      return StatusCode::SUCCESS;
    }
  }

  return ist->regProxy( clid, key );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Deregister a DataProxy with the service
///
StatusCode 
IOVSvc::deregProxy( DataProxy *proxy ) {


  std::lock_guard<std::recursive_mutex> lock(m_lock);
  IIOVSvcTool *ist = getTool( proxy );
  if (ist == 0) {
    ATH_MSG_ERROR( "deregProxy: no IOVSvcTool found for proxy "
          << fullProxyName( proxy ) );
    return (StatusCode::FAILURE);
  }

  return ist->deregProxy( proxy );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Deregister a DataProxy with the service
///
StatusCode 
IOVSvc::deregProxy( const CLID& clid, const std::string& key ) {


  std::lock_guard<std::recursive_mutex> lock(m_lock);
  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    ATH_MSG_ERROR( "deregProxy: no IOVSvcTool found for proxy " 
                   << fullProxyName(clid,key) );
    return StatusCode::FAILURE; 
  }

  return ist->deregProxy( clid, key );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// ignore proxy
///
void
IOVSvc::ignoreProxy( const CLID& clid, const std::string& key, 
                     const std::string& storeName ) {


  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    ATH_MSG_ERROR( "ignoreProxy: no IOVSvcTool found for store " 
                   << storeName << " and failed to create one" );
    return;
  }

  return ist->ignoreProxy( clid, key );
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Replace a registered DataProxy with a new version
///
StatusCode 
IOVSvc::replaceProxy( DataProxy* pOld, DataProxy* pNew, 
                      const std::string& storeName ) {

  StatusCode sc(StatusCode::FAILURE);
  IIOVSvcTool *ist = getTool( storeName );
  if (0 != ist) {
    sc = ist->replaceProxy(pOld, pNew);
  } else {
    ATH_MSG_ERROR( "regProxy: no IOVSvcTool associated with store \"" 
                   << storeName << "\" and failed to create one."  );
  } 
  return sc;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// add to a set of TADs that will be registered at start of first event
///

StatusCode 
IOVSvc::preLoadTAD( const TransientAddress *tad, 
                    const std::string& storeName ) {


  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    ATH_MSG_ERROR( "preLoadTAD: no IOVSvcTool associated with store \""
                   << storeName << "\" and failed to create one."  );
    return StatusCode::FAILURE;
  } else {
    return ist->preLoadTAD( tad );
  }
 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// add to a set of TADs that who's data will be preLoaded
///
StatusCode 
IOVSvc::preLoadDataTAD( const TransientAddress *tad,
                        const std::string& storeName ) {

  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    ATH_MSG_ERROR( "preLoadDataTAD: no IOVSvcTool associated with store \""
                   << storeName << "\" and failed to create one."  );
    return StatusCode::FAILURE;
  } else {
    return ist->preLoadDataTAD( tad );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::setRange(const CLID& clid, const std::string& key,
                 IOVRange& iovr) {

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {

    // FIXME - this should be eliminated once the IOVDbSvc is set up to 
    // use store names. There should be no default store for setRange

    ATH_MSG_WARNING( "setRange(CLID,key,range) for unregistered proxies "
                     << "is deprecated - you need to specify a store! "
                     << "This will be an ERROR soon!" );

    return setRange(clid,key,iovr, defaultStore );

  } else {
    return ist->setRange( clid, key, iovr );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::setRange(const CLID& clid, const std::string& key,
                 IOVRange& iovr, const std::string& storeName) {

  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    ATH_MSG_ERROR( "setRange: no IOVSvcTool assocaited with store \"" 
                   << storeName << "\" and failed to create one."  );
    return StatusCode::FAILURE;
  }

  IIOVSvcTool *ist2 = getTool( clid, key );
  if (ist2 == 0) {
    ATH_MSG_INFO( "setRange: proxy for " << fullProxyName(clid,key)
                  << " not registered with store \"" << storeName << "\". Doing it now"
                  );
    if (ist->regProxy(clid, key).isFailure()) {
      return StatusCode::FAILURE;
    }
  } else if (ist2 != ist) {
    ATH_MSG_INFO( "setRange: when registering proxy (clid: " 
                  << clid << " key: " << key << ") with store \"" << storeName
                  << "\" --> already registered with store \"" << ist2->getStoreName()
                  << "\""  );
    return StatusCode::FAILURE;
  }

  return ist->setRange( clid, key, iovr );

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::getRange(const CLID& clid, const std::string& key, 
                 IOVRange& iov) const {


  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    ATH_MSG_ERROR( "getRange: proxy for " << fullProxyName(clid,key)
                   << " not registered"  );
    return StatusCode::FAILURE;
  } else {
    return ist->getRange( clid, key, iov );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::getRangeFromDB(const CLID& clid, const std::string& key, 
                       IOVRange& range, std::string& tag,
                       std::unique_ptr<IOpaqueAddress>& ioa) const { 

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    ATH_MSG_ERROR( "getRangeFromDB: proxy for " 
                   << fullProxyName(clid,key) << " not registered"  );
    return StatusCode::FAILURE;
  } else {
    return ist->getRangeFromDB( clid, key, range, tag, ioa );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::getRangeFromDB(const CLID& clid, const std::string& key,
                       const IOVTime& time, IOVRange& range, 
                       std::string& tag,
                       std::unique_ptr<IOpaqueAddress>& ioa) const {

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    ATH_MSG_ERROR( "getRangeFromDB: proxy for "
                   << fullProxyName(clid, key) << " not registered"  );
    return StatusCode::FAILURE;
  } else {
    return ist->getRangeFromDB( clid, key, time, range, tag, ioa );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::setRangeInDB(const CLID& clid, const std::string& key, 
                     const IOVRange& range, const std::string &tag) {
  
  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    ATH_MSG_ERROR( "setRangeInDB: proxy for "
                   << fullProxyName(clid,key) << " not registered"  );
    return StatusCode::FAILURE;
  } else {
    return ist->setRangeInDB( clid, key, range, tag );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::regFcn(SG::DataProxy* dp, 
               const CallBackID& c, 
               const IOVSvcCallBackFcn& fcn,
               bool trigger) {

  IIOVSvcTool *ist = getTool( dp );
  if (ist == 0) {
    ATH_MSG_ERROR( "regFcn: no IOVSvcTool found containing DataProxy "
                   << fullProxyName( dp )
                   << "-> Need to bind DataHandle first"  );
    return StatusCode::FAILURE;
  } else {
    return ist->regFcn( dp, c, fcn, trigger );
  }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::regFcn(const CallBackID& c1,
               const CallBackID& c2, const IOVSvcCallBackFcn& fcn2, 
               bool trigger) {


  if (c1 == c2) {
    ATH_MSG_ERROR( "Cannot register 2nd callback function and assocaited"
                   << " object with itself"  );
    return StatusCode::FAILURE;
  }


  IIOVSvcTool *ist = getTool( c1 );
  if (ist == 0) {
    ATH_MSG_ERROR( "CallBack function \"" << c2.name()
                   << "\" cannot be registered since function \"" << c1.name()
                   << "\" has not been registered first"  );
    return StatusCode::FAILURE;
  } else {
    return ist->regFcn(c1, c2, fcn2, trigger);
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::regFcn(const std::string& toolName,
               const CallBackID& c2, const IOVSvcCallBackFcn& fcn2, 
               bool trigger) {

  IAlgTool *ia;
  if ( p_toolSvc->retrieveTool(toolName, ia, 0, false).isFailure() ) {
    ATH_MSG_ERROR( "AlgTool " << toolName << " has not yet been created"
                   << " and thus cannot be registered" );
    return StatusCode::FAILURE;
  }

  IIOVSvcTool *ist = getTool( ia );
  if (ist == 0) {
    ATH_MSG_ERROR( "No callback registered with AlgTool " << toolName );
    return StatusCode::FAILURE;
  } else {
    return ist->regFcn(ia, c2, fcn2, trigger);
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
IOVSvc::getTriggeredTools(const std::string& key, 
                          std::set<std::string>& tools, 
                          const std::string& storeName) {

  IIOVSvcTool *ist = getTool( storeName, false );
  if (ist == 0) {
    ATH_MSG_ERROR( "getTriggeredTools: no store \"" << storeName
                   << "\" associated with any IOVSvcTool" );
    return StatusCode::FAILURE;
  } else {
    return ist->getTriggeredTools(key, tools);
  }

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::reinitialize()
{
  // Set flag to reset all proxies 
  StatusCode sc;
  toolMap::iterator itr = m_toolMap.begin();
  for ( ; itr!=m_toolMap.end(); ++itr) {
    sc &= itr->second->reinitialize();
  }
  return sc;

}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
IOVSvc::createIOVTool( const std::string& storeName, IIOVSvcTool*& ist ) {

  std::string store(storeName);
  std::string toolName("IOVSvcTool");
  if (storeName == "default") store = defaultStore;

  // Append the store name if not default
  if (store != defaultStore) toolName += ("_" + store);

  ATH_MSG_DEBUG( "Creating " << toolName << " associated with store \"" << store
                 << "\""  );

  toolMap::iterator itr = m_toolMap.find( store );
  if ( itr == m_toolMap.end() ) {
    ist = nullptr;
    if (p_toolSvc->retrieveTool( "IOVSvcTool/" + toolName, ist, this ).isFailure()) {
      ATH_MSG_ERROR( "Unable to create IOVSvcTool associated with store \"" 
                     << store << "\""  );
      return StatusCode::FAILURE;
    } else {
      m_toolMap[ store ] = ist;
      ist->setStoreName( store );
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_INFO( "an IOVSvcTool already exists associated with store \"" 
                  << store << "\""  );
    ist = itr->second;
    return StatusCode::SUCCESS;
  }

}
 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
StatusCode
IOVSvc::createIOVTool( const std::string& storeName )
{
  IIOVSvcTool* ist = nullptr;
  return createIOVTool (storeName, ist);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( const std::string& storeName, bool createIF ) {

  std::string store(storeName);
  if (storeName == "default") {
    store = defaultStore;
  }

  toolMap::const_iterator itr = m_toolMap.find( store );
  IIOVSvcTool *ist(0);
  if ( itr == m_toolMap.end() ) {
    ATH_MSG_INFO( "No IOVSvcTool associated with store \"" << store
                  << "\""  );

    if (createIF) {
      createIOVTool(store, ist).ignore();
    }

  } else {
    ist = itr->second;
  }

  return ist;
}
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( DataProxy* proxy ) const {


  IIOVSvcTool *ist(0);
  toolMap::const_iterator itr = m_toolMap.begin();
  for (; itr != m_toolMap.end(); ++itr) {
    if (itr->second->holdsProxy( proxy )) {
      ist = itr->second;
      return ist;
    }
  }

  return ist;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( const CLID& clid, const std::string& key ) const {

  IIOVSvcTool *ist(0);
  toolMap::const_iterator itr = m_toolMap.begin();
  for (; itr != m_toolMap.end(); ++itr) {
    if (itr->second->holdsProxy( clid, key )) {
      ist = itr->second;
      return ist;
    }
  }

  return ist;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( const CallBackID& c1 ) const {

  IIOVSvcTool *ist(0);
  toolMap::const_iterator itr = m_toolMap.begin();
  for (; itr != m_toolMap.end(); ++itr) {
    if (itr->second->holdsCallback( c1 )) {
      ist = itr->second;
      return ist;
    }
  }

  return ist;

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( const IAlgTool* ia ) const {

  IIOVSvcTool *ist(0);
  toolMap::const_iterator itr = m_toolMap.begin();
  for (; itr != m_toolMap.end(); ++itr) {
    if (itr->second->holdsAlgTool( ia )) {
      ist = itr->second;
      return ist;
    }
  }

  return ist;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::string
IOVSvc::fullProxyName( const DataProxy* dp ) const {

  return fullProxyName(dp->clID(), dp->name());

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::string
IOVSvc::fullProxyName( const CLID& clid, const std::string& key ) const {

  std::string fullname, tname;
  std::ostringstream ost;
  ost << clid;
  if (p_CLIDSvc->getTypeNameOfID( clid, tname ).isFailure()) {
    fullname = "[" + ost.str() + "/" + key + "]";
  } else {
    fullname = "[" + tname + ":" + ost.str() + "/" + key + "]";
  }

  return fullname;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::vector<std::string>
IOVSvc::getStoreNames() const {

  std::vector<std::string> stores;

  toolMap::const_iterator itr = m_toolMap.begin();
  for( ; itr!=m_toolMap.end(); ++itr) {
    stores.push_back( itr->first );
  }

  return stores;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IOVSvc::resetAllProxies() {

  toolMap::iterator itr = m_toolMap.begin();
  for (; itr!= m_toolMap.end(); ++itr) {
    IIOVSvcTool* ist = itr->second;
    ATH_MSG_DEBUG( "resetting all proxies for store \""
                   << ist->getStoreName() << "\""  );
    ist->resetAllProxies();
  }

}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::createCondObj(CondContBase* ccb, const DataObjID& id, 
                      const EventIDBase& now) {
  
  ATH_MSG_DEBUG("createCondObj:  id: " << id << "  t: " << now << "  valid: "
                << ccb->valid(now));

  if (ccb->valid(now)) {
    if (msgLvl(MSG::DEBUG)) {
      EventIDRange r;
      ccb->range(now,r);
      ATH_MSG_DEBUG( "  range " << r << " for " << id 
                     << " is still valid at " << now );
    }
    return StatusCode::SUCCESS;
  }

  IOVTime t(now.run_number(), now.lumi_block(), (long long)now.time_stamp()*1000000000+now.time_stamp_ns_offset());
  IOVRange range;
  std::string tag;
  // remove storename from key
  std::string sgKey = id.key();
  auto sep = sgKey.find("+");
  if (sep != std::string::npos) {
    sgKey.erase(0,sep+1);
  }
  
  std::unique_ptr<IOpaqueAddress> ioa;
  if (getRangeFromDB(id.clid(), sgKey, t, range, tag, ioa).isFailure()) {
    ATH_MSG_ERROR( "unable to get range from db for " 
                   << id.clid() << " " << sgKey );
    return StatusCode::FAILURE;
  }
     
  ATH_MSG_DEBUG( " new range for ID " << id << " : " << range 
                 << " IOA: " << ioa.get());

  // If the start of the new range matches the start of the last range, then
  // extend the last range rather than trying to insert a new range.
  // This can happen when a folder is tagged as `extensible' in IOVDbSvc.
  EventIDRange r;
  if (ccb->range (range.start(), r) &&
      eventIDMatch (r.start(), range.start()))
  {
    if (ccb->extendLastRange (range).isSuccess()) {
      return StatusCode::SUCCESS;
    }
  }

  if (ccb->proxy() == nullptr) {
    // nb. We don't want to be holding the IOVSvc lock here,
    // as SGImplSvc can call to IOVSvc with the store lock held.
    SG::DataProxy* dp = p_detStore->proxy (id.clid(), sgKey);
    ATH_MSG_DEBUG( " found DataProxy " << dp << " for " << id );
    ccb->setProxy(dp);
  }

  // this will talk to the IOVDbSvc, get current run/event from EventInfo 
  // object, load
  SG::DataProxy* dp = ccb->proxy();
  DataObject* dobj(0);
  void* v(0);

  if (dp->store()->createObj(dp->loader(), ioa.get(), dobj).isFailure()) {
    ATH_MSG_ERROR(" could not create a new DataObject ");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(" created new obj at " << dobj );

    v = SG::Storable_cast(dobj, id.clid());
  }

  // Free the DataBucket that was created along with the object we read.
  if (DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (dobj)) {
    dbb->relinquish();
    delete dobj;
    dobj = 0;
  }

  // Some data objects may be reference counted by the address.
  // CondCont will take ownership of the object, but doesn't
  // do refcounting.  We'll have gotten a reference via the Storable_cast
  // above, so it should be ok ... unless CondCont deletes
  // the new object immediately instead of inserting.
  // In that case, when we delete the address, it will
  // follow an invalid pointer.  So be sure to delete
  // the address before the object is added to CondCont.
  ioa.release();

  // DataObject *d2 = static_cast<DataObject*>(v);
  
  ATH_MSG_DEBUG( " SG::Storable_cast to obj: " << v );
  
  EventIDRange r2 = range;

  StatusCode sc = ccb->typelessInsert (r2, v);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("unable to insert Object at " << v << " into CondCont " 
                  << ccb->id() << " for range " << r2 );
    return StatusCode::FAILURE;
  }
  else if (CondContBase::Category::isOverlap (sc)) {
    // Disable this for now... IOVDbSvc can sometimes produce overlapping
    // ranges.
    //ATH_MSG_ERROR ("IOV range overlap.");
    //return StatusCode::FAILURE;
  }
 
  return sc;

}
