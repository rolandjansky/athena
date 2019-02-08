/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigCOOLUpdateHelper.h
 * @brief  Helper tool for COOL updates
 * @author Frank Winklmeier
 */

#include "TrigCOOLUpdateHelper.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListException.h"

// TDAQ includes
#include "CTPfragment/CTPfragment.h"
#include "CTPfragment/CTPExtraWordsFormat.h"
#include "CTPfragment/Issue.h"

//=========================================================================
// Standard methods
//=========================================================================
TrigCOOLUpdateHelper::TrigCOOLUpdateHelper(const std::string &type,
                                           const std::string &name,
                                           const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_robDataProviderSvc("ROBDataProviderSvc", name)
{}

TrigCOOLUpdateHelper::~TrigCOOLUpdateHelper()
{}

StatusCode TrigCOOLUpdateHelper::initialize()
{
  // Do not create these services if they are not available already
  service("IOVSvc", m_iovSvc, /*createIf=*/false).ignore();
  service("IOVDbSvc", m_iovDbSvc, /*createIf=*/false).ignore();

  // Register DataHandle with COOLUPDATE folder
  if (!m_coolFolderName.empty()) {
    ATH_CHECK( detStore()->regHandle(m_folderMapHandle, m_coolFolderName) );
  }

  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}


StatusCode TrigCOOLUpdateHelper::start()
{
  m_folderUpdates.clear();
  return StatusCode::SUCCESS;
}

StatusCode TrigCOOLUpdateHelper::stop()
{
  std::ostringstream pending;
  // Loop over folder updates
  for (auto f : m_folderUpdates) {
    if (f.second.needsUpdate) pending << "[" << f.second.lumiBlock << "," << f.second.folderIndex << "]";
  }
  
  if (pending.str().empty()) pending << "NONE";
  ATH_MSG_INFO("[LB,Folder] with pending COOL updates: " << pending.str());

  return StatusCode::SUCCESS;
}

//=========================================================================
// Read COOL folder info
// Loop over all registered keys and get folder name and CLID
//=========================================================================
void TrigCOOLUpdateHelper::readFolderInfo()
{
  if (m_iovDbSvc==0) return;

  m_folderInfo.clear();
  // Loop over all keys registered with IOVDbSvc
  for (const std::string& key : m_iovDbSvc->getKeyList()) {
    std::string foldername, tag;
    IOVRange range;
    bool retrieved;
    unsigned long long bytesRead;
    float readTime;

    // Get folder name and CLID for each key
    bool found = m_iovDbSvc->getKeyInfo(key, foldername, tag, range,
                                        retrieved, bytesRead, readTime);

    if ( !found || m_folderInfo.find(foldername)!=m_folderInfo.end() )
      continue;
    
    CLID clid = detStore()->clid(key);
    if (clid!=CLID_NULL)
      m_folderInfo.insert(FolderInfoMap::value_type(foldername,FolderInfo(clid, key)));
    else
      ATH_MSG_ERROR("Cannot find CLID for " << key);
  }

  if (!m_coolFolderName.empty()) {
    ATH_MSG_INFO("COOL folder map in " << m_coolFolderName << ":");
    for (CondAttrListCollection::const_iterator c = m_folderMapHandle->begin();
         c != m_folderMapHandle->end(); ++c) {
      ATH_MSG_INFO("   (" << c->first << ") " << c->second["FolderName"].data<std::string>());
    }
  }
}

StatusCode TrigCOOLUpdateHelper::resetFolders(const std::vector<std::string>& folders,
                                              EventID::number_type currentRun,
                                              bool dropObject)
{
  if (folders.empty()) return StatusCode::SUCCESS;
  
  StatusCode sc(StatusCode::SUCCESS);
  for (const std::string& f : folders) {
    if ( resetFolder(f, currentRun, dropObject).isFailure() ) {
      sc = StatusCode::FAILURE;
    }
  }
  return sc;
}

StatusCode TrigCOOLUpdateHelper::resetFolder(const std::string& folder,
                                             EventID::number_type currentRun,
                                             bool dropObject)
{
  // Force a reset of folders by setting an IOVRange in the past

  if (m_iovSvc==0 || m_iovDbSvc==0) return StatusCode::SUCCESS;
  
  // Set IOV in the past to trigger reload in IOVSvc
  IOVRange iov_range(IOVTime(currentRun-2, 0),
                     IOVTime(currentRun-1, 0));

  FolderInfoMap::const_iterator f = m_folderInfo.find(folder);
  if ( f==m_folderInfo.end() ) {
    ATH_MSG_DEBUG("Folder " << folder << " not registered with IOVDbSvc");
    return StatusCode::SUCCESS;
  }
    
  const CLID& clid = f->second.clid;
  const std::string& key = f->second.key;
  // Check if proxy exists
  if ( detStore()->proxy(clid, key)==0 ) {
    ATH_MSG_DEBUG("No proxy for " << key << " found");
    return StatusCode::SUCCESS;  // on purpose not a failure
  }

  // Make sure we are not trying to reset a time-based folder    
  IOVRange iov;
  if ( m_iovSvc->getRange(clid, key, iov).isSuccess() ) {         
    if ( iov.start().isTimestamp() || iov.start().isBoth() ) {
      ATH_MSG_ERROR(folder << " is not a Run/LB based folder. Cannot perform COOL update.");
      return StatusCode::FAILURE;
    }
  }
       
  // Invalidate IOV range
  ATH_MSG_DEBUG("Setting IOV range of " << folder << " to " << iov_range);
    
  if ((m_iovSvc->setRange(clid, key, iov_range, "StoreGateSvc")).isFailure()) {
    ATH_MSG_WARNING("Could not set IOV range for " << folder << " to " << iov_range);
    return StatusCode::FAILURE;        
  }
    
  // Drop object and reset cache
  if (dropObject) {
    ATH_CHECK( m_iovDbSvc->dropObject(key, /*resetCache=*/true), StatusCode::FAILURE );
  }
    
  // All OK
  ATH_MSG_INFO("Invalidated IOV" << (dropObject ? " and dropped payload " : " ")
               << "of folder " << folder << (key!=folder ? " (key="+key+")" : "") );
  
  return StatusCode::SUCCESS;
}


//=========================================================================
// Folder update during the run
//=========================================================================
StatusCode TrigCOOLUpdateHelper::hltCoolUpdate(const EventContext& ctx)
{
  // Loop over folders to be updated
  std::map<CTPfragment::FolderIndex, FolderUpdate>::iterator f = m_folderUpdates.begin();
  for (; f!=m_folderUpdates.end(); ++f) {
          
    if (f->second.needsUpdate) {
      std::string folderName;
      if (getFolderName(f->second.folderIndex, folderName).isFailure()) {
        continue;  // On purpose not a failure
      }

      ATH_MSG_INFO("Reload of COOL folder " << folderName << " for IOV change in lumiblock " << f->second.lumiBlock
                   << ". Current event: "  << ctx.eventID());
              
      if ( hltCoolUpdate(folderName, ctx).isFailure() ) {
        ATH_MSG_FATAL("COOL update failed for " << folderName << ". Aborting.");
        return StatusCode::FAILURE;
      }
      // All OK
      f->second.needsUpdate = false;
    }      
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigCOOLUpdateHelper::hltCoolUpdate(const std::string& folderName, const EventContext& ctx)
{
  if ( m_iovSvc==0 || m_coolFolderName.empty() ) {
    ATH_MSG_DEBUG("Passive mode. Not updating COOL folders");
    return StatusCode::SUCCESS;
  }

  auto mon_t = Monitored::Timer("TIME_CoolFolderUpdate");

  // Reset folder and make IOVDbSvc drop objects
  if (resetFolder(folderName, ctx.eventID().run_number(), true).isFailure()) {
    ATH_MSG_ERROR("Reset of " << folderName << " failed");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigCOOLUpdateHelper::getFolderName(CTPfragment::FolderIndex idx, std::string& folderName)
{
  if (m_coolFolderName.empty()) return StatusCode::SUCCESS;
  
  if (!m_folderMapHandle.isInitialized()) {
    ATH_MSG_ERROR("DataHandle for '" << m_coolFolderName << "' not initialized.");
    return StatusCode::FAILURE;
  }
  try {
    folderName = m_folderMapHandle->attributeList(idx)["FolderName"].data<std::string>();
  }
  catch (coral::AttributeListException &) {
    ATH_MSG_ERROR(m_coolFolderName << " does not contain a folder for index/channel " << idx
                  << ". Existing folders are:");
    m_folderMapHandle->dump();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//=========================================================================
// Schedule COOL folder updates (called on every event by HltEventLoopMgr)
//=========================================================================
StatusCode TrigCOOLUpdateHelper::scheduleFolderUpdates(const EventContext& ctx)
{
  using CTPfragment::FolderIndex;
  using CTPfragment::FolderEntry;

  // Fetch CTP fragment ROB
  const std::vector<uint32_t> robs{m_ctpRobId};
  IROBDataProviderSvc::VROBFRAG ctpRobs;
  m_robDataProviderSvc->addROBData(ctx, robs, name());
  m_robDataProviderSvc->getROBData(ctx, robs, ctpRobs, name());

  if ( ctpRobs.empty() ) {
    ATH_MSG_FATAL("Cannot retrieve CTP ROB " << m_ctpRobId.value());
    return StatusCode::FAILURE;
  }

  // Decode CTP extra payload words
  std::vector<uint32_t> l1_extraPayload = CTPfragment::extraPayloadWords( ctpRobs[0] );
  CTPfragment::ExtraPayload ctp_payload;
  try {
    ctp_payload = CTPfragment::ExtraPayload(l1_extraPayload);
  }
  catch (CTPfragment::ExtraPayloadTooLong& ex) {
    ATH_MSG_FATAL("Invalid CTP fragment. Exception = " << ex.what());
    return StatusCode::FAILURE;
  }

  if ( msgLevel(MSG::DEBUG) ) {
    msg() << MSG::DEBUG << "CTP extra payload (" << l1_extraPayload.size() << " words): ";
    for (std::size_t i=0; i<l1_extraPayload.size(); ++i) {
      msg() << " " << l1_extraPayload[i];
    }
    msg() << ctp_payload << endmsg;
  }

  // Loop over potential new folder updates
  for (const std::pair<FolderIndex, FolderEntry>& kv : ctp_payload.getFolderUpdates()) {

    // Check if we already have an update for this folder
    std::map<FolderIndex, FolderUpdate>::const_iterator f = m_folderUpdates.find(kv.first);

    // No updates yet or this update superseds the previous one
    if (f==m_folderUpdates.end() || (f->second.lumiBlock != kv.second.lumiBlock)) {
      m_folderUpdates[kv.first] = FolderUpdate(kv.second);   // new folder update
      
      ATH_MSG_DEBUG("Conditions update for folder " << kv.second.folderIndex
                   << " on lumiblock " << kv.second.lumiBlock);
    }
  }
  return StatusCode::SUCCESS;
}
