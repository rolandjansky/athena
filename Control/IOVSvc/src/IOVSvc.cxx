/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  IOVSvc.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVSvc.cxx,v 1.66 2008-06-04 23:35:03 leggett Exp $
 *
 *  Provides automatic updating and callbacks for time dependent data
 *
 *****************************************************************************/

#include "IOVSvc/IOVSvc.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/IClassIDSvc.h"

#include "IOVSvc/IIOVSvcTool.h"
#include "GaudiKernel/IConversionSvc.h"

using SG::DataProxy;
using SG::TransientAddress;
using namespace std;

const std::string defaultStore = "StoreGateSvc";

//
///////////////////////////////////////////////////////////////////////////
//

IOVSvc::IOVSvc( const std::string& name, ISvcLocator* svc )
  : AthService( name, svc ), 
    p_toolSvc("ToolSvc",name),
    p_CLIDSvc("ClassIDSvc",name),
    p_sgs("StoreGateSvc",name),
    p_detStore("StoreGateSvc/DetectorStore",name),
    p_condSvc("CondSvc",name)
{

  declareProperty("preLoadRanges",m_preLoadRanges=false);
  declareProperty("preLoadData",m_preLoadData=false);
  declareProperty("partialPreLoadData",m_partialPreLoadData=true);
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

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG << "Initializing IOVSvc version " 
          << PACKAGE_VERSION << endmsg;
  }
#endif

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG << "AthService initialized" << endmsg;
  }
#endif

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

StatusCode IOVSvc::finalize() {

  // cleanup

  toolMap::iterator itr = m_toolMap.begin();
  for (; itr!= m_toolMap.end(); ++itr) {
    IIOVSvcTool* ist = itr->second;
    p_toolSvc->releaseTool( ist ).ignore();
  }

  StatusCode status = AthService::finalize();

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG) && status.isSuccess() ) {
    msg() << MSG::DEBUG << "Service finalised successfully" << endmsg;
  }
#endif

  return status;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IIOVSvc::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IIOVSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Register a DataProxy with the service
///
StatusCode 
IOVSvc::regProxy( const DataProxy *proxy, const std::string& key,
                  const std::string& storeName ) {

  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    msg() << MSG::ERROR << "regProxy: no IOVSvcTool associated with store \"" 
          << storeName << "\" and failed to create one" << endmsg;
    return (StatusCode::FAILURE);
  }

  IIOVSvcTool *ist2 = getTool( proxy );
  if (ist2 != 0) {
    if (ist2 != ist) {
      msg() << MSG::ERROR << "regProxy: when registering proxy for " 
            << fullProxyName(proxy) << " with store \"" << storeName
            << "\", it is already registered with store \"" 
            << ist2->getStoreName() << "\"" << endmsg;
      return StatusCode::FAILURE;
    } else {
#ifndef NDEBUG
      if (msgLvl(MSG::DEBUG)) {
        msg() << MSG::DEBUG << "regProxy: proxy for " << fullProxyName(proxy)
              << " already registered with store \"" << storeName << "\""
              << endmsg;
      }
#endif
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

  IIOVSvcTool *ist = getTool( storeName );
  if (ist == 0) {
    msg() << MSG::ERROR << "regProxy: no IOVSvcTool associated with store \"" 
          << storeName << "\" and failed to create one." << endmsg;
    return (StatusCode::FAILURE);
  }

  IIOVSvcTool *ist2 = getTool( clid, key );
  if (ist2 != 0) {
    if (ist2 != ist) {
      msg() << MSG::ERROR << "regProxy: when registering proxy for "
            << fullProxyName(clid,key)
            << " with store " << storeName
            << ", it is already registered with store \"" 
            << ist2->getStoreName() << "\"" << endmsg;
      return StatusCode::FAILURE;
    } else {
#ifndef NDEBUG
      if (msgLvl(MSG::DEBUG)) {
        msg() << MSG::DEBUG << "regProxy: proxy for " << fullProxyName(clid,key)
              << " already registered with store \"" << storeName << "\""
              << endmsg;
      }
#endif
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
IOVSvc::deregProxy( const DataProxy *proxy ) {


  IIOVSvcTool *ist = getTool( proxy );
  if (ist == 0) {
    msg() << MSG::ERROR << "deregProxy: no IOVSvcTool found for proxy "
          << fullProxyName( proxy )
          << endmsg;
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


  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    msg() << MSG::ERROR << "deregProxy: no IOVSvcTool found for proxy " 
          << fullProxyName(clid,key) 
          << endmsg;
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
    msg() << MSG::ERROR << "ignoreProxy: no IOVSvcTool found for store " 
          << storeName << " and failed to create one"
          << endmsg;
    return;
  }

  return ist->ignoreProxy( clid, key );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Replace a registered DataProxy with a new version
///
StatusCode 
IOVSvc::replaceProxy( const DataProxy* pOld, const DataProxy* pNew, 
                      const std::string& storeName ) {

  StatusCode sc(StatusCode::FAILURE);
  IIOVSvcTool *ist = getTool( storeName );
  if (0 != ist) {
    sc = ist->replaceProxy(pOld, pNew);
  } else {
    msg() << MSG::ERROR << "regProxy: no IOVSvcTool associated with store \"" 
          << storeName << "\" and failed to create one." << endmsg;
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
    msg() << MSG::ERROR << "preLoadTAD: no IOVSvcTool associated with store \""
          << storeName << "\" and failed to create one." << endmsg;
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
    msg() << MSG::ERROR 
          << "preLoadDataTAD: no IOVSvcTool associated with store \""
          << storeName << "\" and failed to create one." << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->preLoadDataTAD( tad );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::setRange(const CLID& clid, const std::string& key,
                 IOVRange& iovr) {

  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {

    // FIXME - this should be eliminated once the IOVDbSvc is set up to 
    // use store names. There should be no default store for setRange

    msg() << MSG::WARNING << "setRange(CLID,key,range) for unregistered proxies "
          << "is deprecated - you need to specify a store! "
          << "This will be an ERROR soon!" 
          << endmsg;

    return setRange(clid,key,iovr, defaultStore );

    //     msg() << MSG::ERROR << "setRange: proxy " << fullProxyName(clid, key) 
    //           << " not registered in any store."
    //           << endmsg;
    //     return StatusCode::FAILURE;

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
    msg() << MSG::ERROR << "setRange: no IOVSvcTool assocaited with store \"" 
          << storeName << "\" and failed to create one." << endmsg;
    return StatusCode::FAILURE;
  }

  IIOVSvcTool *ist2 = getTool( clid, key );
  if (ist2 == 0) {
    msg() << MSG::INFO << "setRange: proxy for " << fullProxyName(clid,key)
          << " not registered with store \"" << storeName << "\". Doing it now"
          << endmsg;
    if (ist->regProxy(clid, key).isFailure()) {
      return StatusCode::FAILURE;
    }
  } else if (ist2 != ist) {
    msg() << MSG::INFO << "setRange: when registering proxy (clid: " 
          << clid << " key: " << key << ") with store \"" << storeName
          << "\" --> already registered with store \"" << ist2->getStoreName()
          << "\"" << endmsg;
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
    msg() << MSG::ERROR << "getRange: proxy for " << fullProxyName(clid,key)
          << " not registered" << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->getRange( clid, key, iov );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::getRangeFromDB(const CLID& clid, const std::string& key, 
                       IOVRange& range, std::string& tag, IOpaqueAddress*& ioa) const { 

  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    msg() << MSG::ERROR << "getRangeFromDB: proxy for " 
          << fullProxyName(clid,key) << " not registered" << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->getRangeFromDB( clid, key, range, tag, ioa );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::getRangeFromDB(const CLID& clid, const std::string& key,
                       const IOVTime& time, IOVRange& range, 
                       std::string& tag, IOpaqueAddress*& ioa) const {

  IIOVSvcTool *ist = getTool( clid, key );
  if (ist == 0) {
    msg() << MSG::ERROR << "getRangeFromDB: proxy for "
          << fullProxyName(clid, key) << " not registered" << endmsg;
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
    msg() << MSG::ERROR << "setRangeInDB: proxy for "
          << fullProxyName(clid,key) << " not registered" << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->setRangeInDB( clid, key, range, tag );
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::regFcn(SG::DataProxy* dp, 
               const CallBackID c, 
               const IOVSvcCallBackFcn& fcn,
               bool trigger) {

  IIOVSvcTool *ist = getTool( dp );
  if (ist == 0) {
    msg() << MSG::ERROR << "regFcn: no IOVSvcTool found containing DataProxy "
          << fullProxyName( dp )
          << "-> Need to bind DataHandle first" << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->regFcn( dp, c, fcn, trigger );
  }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::regFcn(const CallBackID c1,
               const CallBackID c2, const IOVSvcCallBackFcn& fcn2, 
               bool trigger) {


  if (c1 == c2) {
    msg() << MSG::ERROR 
          << "Cannot register 2nd callback function and assocaited"
          << " object with itself" << endmsg;
    return StatusCode::FAILURE;
  }


  IIOVSvcTool *ist = getTool( c1 );
  if (ist == 0) {
    msg() << MSG::ERROR << "CallBack function \"" << c2.name()
          << "\" cannot be registered since function \"" << c1.name()
          << "\" has not been registered first" << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->regFcn(c1, c2, fcn2, trigger);
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::regFcn(const std::string& toolName,
               const CallBackID c2, const IOVSvcCallBackFcn& fcn2, 
               bool trigger) {

  IAlgTool *ia;
  if ( p_toolSvc->retrieveTool(toolName, ia, 0, false).isFailure() ) {
    msg() << MSG::ERROR << "AlgTool " << toolName << " has not yet been created"
          << " and thus cannot be registered"
          << endmsg;

    return StatusCode::FAILURE;
  }

  IIOVSvcTool *ist = getTool( ia );
  if (ist == 0) {
    msg() << MSG::ERROR << "No callback registered with AlgTool " << toolName
          << endmsg;
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
    msg() << MSG::ERROR << "getTriggeredTools: no store \"" << storeName
          << "\" associated with any IOVSvcTool"
          << endmsg;
    return StatusCode::FAILURE;
  } else {
    return ist->getTriggeredTools(key, tools);
  }

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::reinitialize()
{
  // Set flad to reset all proxies 

  toolMap::iterator itr = m_toolMap.begin();
  for ( ; itr!=m_toolMap.end(); ++itr) {
    itr->second->reinitialize();
  }
  return (StatusCode::SUCCESS);

}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
IOVSvc::createIOVTool( const std::string& storeName ) {

  std::string store(storeName);
  if (storeName == "default") store = defaultStore;

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG
          << "Creating IOVSvcTool associated with store \"" << store
          << "\"" << endmsg;
  }
#endif

  toolMap::iterator itr = m_toolMap.find( store );
  if ( itr == m_toolMap.end() ) {
    IIOVSvcTool *ist(0);
    if (p_toolSvc->retrieveTool( "IOVSvcTool/" + store, ist, this ).isFailure()) {
      msg() << MSG::ERROR 
            << "Unable to create IOVSvcTool assocaited with store \"" 
            << store << "\"" << endmsg;
      return StatusCode::FAILURE;
    } else {
      m_toolMap[ store ] = ist;
      ist->setStoreName( store );
      return StatusCode::SUCCESS;
    }
  } else {
    msg() << MSG::INFO
          << "an IOVSvcTool already exists assocaited with store \"" 
          << store << "\"" << endmsg;
    return StatusCode::SUCCESS;
  }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool
IOVSvc::createIOVTool( const std::string& storeName, IIOVSvcTool*& ist ) const {

  std::string store(storeName);
  if (storeName == "default") store = defaultStore;

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG
          << "Creating IOVSvcTool associated with store \"" << store
          << "\"" << endmsg;
  }
#endif

  ist = 0;
  toolMap::const_iterator itr = m_toolMap.find( store );
  if ( itr == m_toolMap.end() ) {
    if (p_toolSvc->retrieveTool( "IOVSvcTool/"+store, ist, this ).isFailure()) {
      msg() << MSG::ERROR 
            << "Unable to create IOVSvcTool assocaited with store \"" 
            << store << "\"" << endmsg;
      return false;
    } else {
      m_toolMap[ store ] = ist;
      ist->setStoreName( store );
    }
  } else {
    msg() << MSG::INFO
          << "an IOVSvcTool already exists assocaited with store \"" 
          << store << "\"" << endmsg;
    ist = itr->second;    
  }
  return true;

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( const std::string& storeName, bool createIF ) const {

  std::string store(storeName);
  if (storeName == "default") {
    store = defaultStore;
  }

  toolMap::const_iterator itr = m_toolMap.find( store );
  IIOVSvcTool *ist(0);
  if ( itr == m_toolMap.end() ) {
    msg() << MSG::INFO << "No IOVSvcTool associated with store \"" << store
          << "\"" << endmsg;

    if (createIF) createIOVTool(store, ist);

  } else {
    ist = itr->second;
  }

  return ist;
}
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IIOVSvcTool* 
IOVSvc::getTool( const DataProxy* proxy ) const {


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
  ostringstream ost;
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
#ifndef NDEBUG
    if (msgLvl(MSG::DEBUG)) {
      msg() << MSG::DEBUG << "resetting all proxies for store \""
            << ist->getStoreName() << "\"" << endmsg;
    }
#endif
    ist->resetAllProxies();
  }

}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvc::createCondObj(CondContBase* ccb, const DataObjID& id, 
                      const EventIDBase& now) {
  
  std::lock_guard<std::mutex> lock(m_lock);

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
  IOpaqueAddress* ioa;
  std::string tag;
  // remove storename from key
  std::string sgKey = id.key();
  auto sep = sgKey.find("+");
  if (sep != std::string::npos) {
    sgKey.erase(0,sep+1);
  }
  
  if (getRangeFromDB(id.clid(), sgKey, t, range, tag, ioa).isFailure()) {
    ATH_MSG_ERROR( "unable to get range from db for " 
                   << id.clid() << " " << sgKey );
    return StatusCode::FAILURE;
  }
     
  ATH_MSG_DEBUG( " new range for ID " << id << " : " << range 
                 << " IOA: " << ioa);

  if (ccb->proxy() == nullptr) { 
    SG::DataProxy* dp = p_detStore->proxy (id.clid(), sgKey);
    ATH_MSG_DEBUG( " found DataProxy " << dp << " for " << id );
    ccb->setProxy(dp);
  }

  // this will talk to the IOVDbSvc, get current run/event from EventInfo 
  // object, load
  SG::DataProxy* dp = ccb->proxy();
  DataObject* dobj(0);
  void* v(0);

  if (dp->store()->createObj(dp->loader(), ioa, dobj).isFailure()) {
    ATH_MSG_ERROR(" could not create a new DataObject ");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(" created new obj at " << dobj );

    v = SG::Storable_cast(dobj, id.clid());
  }

  // DataObject *d2 = static_cast<DataObject*>(v);
  
  ATH_MSG_DEBUG( " SG::Storable_cast to obj: " << v );
  
  EventIDRange r2 = range;
  
  if (!ccb->typelessInsert (r2, v)) {
    ATH_MSG_ERROR("unable to insert Object at " << v << " into CondCont " 
                  << ccb->id() << " for range " << r2 );
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;

}
