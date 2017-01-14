///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CondInputLoader.cxx 
// Implementation file for class CondInputLoader
/////////////////////////////////////////////////////////////////// 

#include "CondInputLoader.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"

#include "xAODEventInfo/EventInfo.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CondInputLoader::CondInputLoader( const std::string& name, 
                                  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_dump(false),
  m_condStore("StoreGateSvc/ConditionStore", name),
  m_condSvc("CondSvc",name),
  m_IOVSvc("IOVSvc",name)

{
  //
  // Property declaration
  // 
  declareProperty( "Load", m_load); 
  //->declareUpdateHandler(&CondInputLoader::loader, this);
  declareProperty( "ShowEventDump", m_dump=false);

}

// Destructor
///////////////
CondInputLoader::~CondInputLoader()
{}

//-----------------------------------------------------------------------------

// Athena Algorithm's Hooks
////////////////////////////
StatusCode 
CondInputLoader::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_condSvc.isValid()) {
    ATH_MSG_ERROR("could not get the CondSvc");
    return StatusCode::FAILURE;
  }

  if (!m_condStore.isValid()) {
    ATH_MSG_ERROR("could not get the ConditionStore");
    return StatusCode::FAILURE;
  }


  // Trigger read of IOV database
  ServiceHandle<IIOVSvc> ivs("IOVSvc",name());
  if (!ivs.isValid()) {
    ATH_MSG_FATAL("unable to retrieve IOVSvc");
    return StatusCode::FAILURE;
  }

  // Update the SG keys if different from Folder Names
  ServiceHandle<IIOVDbSvc> idb("IOVDbSvc",name());
  if (!idb.isValid()) {
    ATH_MSG_FATAL("unable to retrieve IOVDbSvc");
    return StatusCode::FAILURE;
  }

  std::vector<std::string> keys = idb->getKeyList();
  std::string folderName, tg;
  IOVRange range;
  bool retrieved;
  unsigned long long br;
  float rt;
  DataObjIDColl m_loadCopy;

  std::map<std::string,std::string> folderKeyMap;
  for (auto key : keys) {
    if (idb->getKeyInfo(key, folderName, tg, range, retrieved, br, rt)) {
      folderKeyMap[folderName] = key;
      m_keyFolderMap[key] = folderName;
    } else {
      ATH_MSG_WARNING("unable to retrieve keyInfo for " << key );
    }
  }

  std::map<std::string,std::string>::const_iterator itr;
  for (auto id : m_load) {
    itr = folderKeyMap.find(id.key());
    if (itr != folderKeyMap.end() && id.key() != itr->second) {
      ATH_MSG_DEBUG(" mapping folder " << id.key() << " to SGkey " 
                    << itr->second);
      id.updateKey( itr->second );
    // } else {
    //   ATH_MSG_DEBUG(" not remapping folder " << id.key());
    }
    m_loadCopy.insert(id);
  }

  // for (auto key : keys) {
  //   if (idb->getKeyInfo(key, folderName, tg, range, retrieved, br, rt)) {
  //     ATH_MSG_VERBOSE("folder: " << folderName << "  key: " << key);
  //     for (auto id : m_load) {
  //       if (id.key() == folderName) {
  //         ATH_MSG_DEBUG("  mapping folder " << folderName << "  to SGkey " << key );
  //         id.updateKey( key );
  //         m_loadCopy.insert(id);
  //         m_keyFolderMap[key] = folderName;
  //         break;
  //       }
  //     }
  //   } else {
  //     ATH_MSG_WARNING("unable to retrieve keyInfo for " << key );
  //   }
  // }

  m_load = m_loadCopy;

  // Update the properties, set the ExtraOutputs for Alg deps
  const Property &p = getProperty("Load");

  ATH_MSG_DEBUG("setting prop ExtraOutputs to " <<  p.toString());
  if (!setProperty("ExtraOutputs", p).isSuccess()) {
    ATH_MSG_ERROR("failed setting property ExtraOutputs");
    return StatusCode::FAILURE;
  }

  StatusCode sc(StatusCode::SUCCESS);
  std::ostringstream str;
  str << "Will create WriteCondHandle dependencies for the following DataObjects:";
  for (auto &e : m_load) {
    str << "\n    + " << e;
    if (e.key() == "") {
      sc = StatusCode::FAILURE;
      str << "   ERROR: empty key is not allowed!";
    } else {
      Gaudi::DataHandle dh(e, Gaudi::DataHandle::Writer, this);
      if (m_condSvc->regHandle(this, dh, e.key()).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle " << dh.fullKey());
        sc = StatusCode::FAILURE;
      }
      m_IOVSvc->ignoreProxy(dh.fullKey().clid(), e.key());
    }
  }

  ATH_MSG_INFO(str.str());

  return sc;
}

//-----------------------------------------------------------------------------

StatusCode 
CondInputLoader::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------

StatusCode 
CondInputLoader::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if (m_first) {
    DataObjIDColl::iterator itr;
    for (itr = m_load.begin(); itr != m_load.end(); ) {
      //      if ( ! m_condStore->contains(itr->clid(), itr->key() ) ){
      if ( ! m_condStore->contains<CondContBase>( itr->key() ) ){
        ATH_MSG_INFO("ConditionStore does not contain a CondCont<> of "
                     << *itr
                     << ". Either a ReadCondHandle was not initailized or "
                     << "no other Algorithm is using this Handle");
        itr = m_load.erase(itr);
      } else {
        ++itr;
      }
    }
    m_first = false;
  }

  EventIDBase now;
  if(getContext()==nullptr) {
    const xAOD::EventInfo* thisEventInfo;
    if(evtStore()->retrieve(thisEventInfo)!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Unable to get Event Info");
      return StatusCode::FAILURE;
    }
    now.set_run_number(thisEventInfo->runNumber());
    now.set_event_number(thisEventInfo->eventNumber());
    now.set_time_stamp(thisEventInfo->timeStamp());
  }
  else {
    now.set_run_number(getContext()->eventID().run_number());
    now.set_event_number(getContext()->eventID().event_number());
    now.set_time_stamp(getContext()->eventID().time_stamp());
  }
  IOVTime t(now.run_number(), now.event_number(), now.time_stamp());

  EventIDRange r;
  std::string tag;
  //  for (auto &obj: extraOutputDeps()) {
  for (auto &obj: m_load) {

    ATH_MSG_DEBUG( "handling id: " << obj );

    CondContBase* ccb(0);
    if (! m_condStore->retrieve(ccb, obj.key()).isSuccess()) {
      ATH_MSG_ERROR( "unable to get CondContBase* for " << obj 
                     << " from ConditionStore" );
      continue;
    }
   

    if (ccb->valid(now)) {
      ATH_MSG_INFO( "  CondObj " << obj << " is still valid at " << now );
      evtStore()->addedNewTransObject(obj.clid(), obj.key());
      continue;
    }


    std::string dbKey = m_keyFolderMap[obj.key()];
    if (m_IOVSvc->createCondObj( ccb, obj, now ).isFailure()) {
      ATH_MSG_ERROR("unable to create Cond object for " << obj << " dbKey: " 
                    << dbKey);
      return StatusCode::FAILURE;
    } else {
      evtStore()->addedNewTransObject(obj.clid(), obj.key());
    }

  }

  if (m_dump) {
    ATH_MSG_DEBUG(evtStore()->dump()); 
  }
  

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------

