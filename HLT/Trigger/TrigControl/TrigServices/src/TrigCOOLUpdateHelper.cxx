/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/DataHandle.h"

// TDAQ includes
#include "CTPfragment/CTPfragment.h"
#include "CTPfragment/CTPExtraWordsFormat.h"
#include "CTPfragment/Issue.h"

#include <algorithm>
#include <sstream>

//=========================================================================
// Standard methods
//=========================================================================
TrigCOOLUpdateHelper::TrigCOOLUpdateHelper(const std::string &type,
                                           const std::string &name,
                                           const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_robDataProviderSvc("ROBDataProviderSvc", name)
{}


StatusCode TrigCOOLUpdateHelper::initialize()
{
  // Do not create these services if they are not available already
  service("IOVSvc", m_iovSvc, /*createIf=*/false).ignore();
  service("IOVDbSvc", m_iovDbSvc, /*createIf=*/false).ignore();

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
StatusCode TrigCOOLUpdateHelper::readFolderInfo()
{
  if (m_iovDbSvc==nullptr) return StatusCode::SUCCESS;

  m_folderInfo.clear();
  // Loop over all keys registered with IOVDbSvc
  for (const std::string& key : m_iovDbSvc->getKeyList()) {

    // Get folder name and CLID for each key
    IIOVDbSvc::KeyInfo info;
    if ( !m_iovDbSvc->getKeyInfo(key, info) ||
         m_folderInfo.find(info.folderName)!=m_folderInfo.end() )
      continue;
    
    CLID clid = detStore()->clid(key);
    if (clid!=CLID_NULL)
      m_folderInfo.insert({info.folderName, FolderInfo{clid, key}});
    else
      ATH_MSG_ERROR("Cannot find CLID for " << key);

    // If the folder is in the allowed list, make sure it is marked "extensible"
    if (std::find(m_folders.begin(), m_folders.end(), info.folderName)!=m_folders.end() &&
        not info.extensible) {
      ATH_MSG_ERROR("IOVDBSvc folder " << info.folderName << " is not marked as </extensible>. "
                    "Remove it from the allowed 'Folders' property or mark it as extensible.");
      return StatusCode::FAILURE;
    }
  }

  if (!m_coolFolderName.empty()) {
    // Read COOL folder map
    const DataHandle<CondAttrListCollection> folderMapHandle;
    ATH_CHECK( detStore()->regHandle(folderMapHandle, m_coolFolderName) );

    ATH_MSG_INFO("COOL folder map in " << m_coolFolderName << ":");
    for (auto const& [idx, attr] : *folderMapHandle) {
      m_folderNames[idx] = attr["FolderName"].data<std::string>();
      ATH_MSG_INFO("   (" << idx << ") " << m_folderNames[idx]);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigCOOLUpdateHelper::resetFolders(const std::vector<std::string>& folders,
                                              EventIDBase::number_type currentRun,
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
                                             EventIDBase::number_type currentRun,
                                             bool dropObject)
{
  // Force a reset of folders by setting an IOVRange in the past

  if (m_iovSvc==nullptr || m_iovDbSvc==nullptr) return StatusCode::SUCCESS;
  
  // Set IOV in the past to trigger reload in IOVSvc
  IOVRange iov_range(IOVTime(currentRun-2, 0),
                     IOVTime(currentRun-1, 0));

  const auto& f = m_folderInfo.find(folder);
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
  ATH_MSG_DEBUG("Invalidating IOV range of " << folder << " by setting it to " << iov_range);

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
  for (auto& [idx, f] : m_folderUpdates) {
          
    if (f.needsUpdate) {
      std::string folderName;
      if (getFolderName(f.folderIndex, folderName).isFailure()) {
        continue;  // On purpose not a failure
      }

      ATH_MSG_INFO("Reload of COOL folder " << folderName << " for IOV change in lumiblock " << f.lumiBlock
                   << ". Current event: "  << ctx.eventID());
              
      if ( hltCoolUpdate(folderName, ctx).isFailure() ) {
        ATH_MSG_ERROR("COOL update failed for " << folderName << ". Aborting.");
        return StatusCode::FAILURE;
      }
      // All OK
      f.needsUpdate = false;
    }      
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigCOOLUpdateHelper::hltCoolUpdate(const std::string& folderName, const EventContext& ctx)
{
  if ( m_iovSvc==nullptr || m_coolFolderName.empty() ) {
    ATH_MSG_DEBUG("Passive mode. Not updating COOL folders");
    return StatusCode::SUCCESS;
  }

  if (std::find(m_folders.begin(), m_folders.end(), folderName)==m_folders.end()) {
    ATH_MSG_ERROR("Received request to update COOL folder '" << folderName
                  << "' but this folder is not in the allowed list:" << m_folders);
    return StatusCode::FAILURE;
  }

  auto mon_t = Monitored::Timer("TIME_CoolFolderUpdate");
  Monitored::Group mon(m_monTool, mon_t);

  // Reset folder and make IOVDbSvc drop objects
  if (resetFolder(folderName, ctx.eventID().run_number(), true).isFailure()) {
    ATH_MSG_ERROR("Reset of " << folderName << " failed");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigCOOLUpdateHelper::getFolderName(CTPfragment::FolderIndex idx, std::string& folderName) const
{
  if (m_coolFolderName.empty()) return StatusCode::SUCCESS;

  const auto& itr = m_folderNames.find(idx);
  if (itr==m_folderNames.end()) {
    ATH_MSG_ERROR(m_coolFolderName << " does not contain a folder for index/channel " << idx
                  << ". Existing folders are:");
    for (auto const& [idx, name] : m_folderNames) {
      ATH_MSG_INFO("   (" << idx << ") " << name);
    }
    return StatusCode::FAILURE;
  }

  folderName = itr->second;
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
  try {
    m_robDataProviderSvc->addROBData(ctx, robs, name());
    m_robDataProviderSvc->getROBData(ctx, robs, ctpRobs, name());
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("Cannot retrieve CTP ROB 0x" << MSG::hex << m_ctpRobId.value() << MSG::dec << " due to exception: " << ex.what());
    return StatusCode::FAILURE;
  }

  if ( ctpRobs.empty() ) {
    ATH_MSG_ERROR("Cannot retrieve CTP ROB 0x" << MSG::hex << m_ctpRobId.value() << MSG::dec);
    return StatusCode::FAILURE;
  }

  // Decode CTP extra payload words
  std::vector<uint32_t> l1_extraPayload = CTPfragment::extraPayloadWords( ctpRobs[0] );
  CTPfragment::ExtraPayload ctp_payload;
  try {
    ctp_payload = CTPfragment::ExtraPayload(l1_extraPayload);
  }
  catch (CTPfragment::ExtraPayloadTooLong& ex) {
    ATH_MSG_ERROR("Invalid CTP fragment. Exception = " << ex.what());
    return StatusCode::FAILURE;
  }

  if ( msgLevel(MSG::DEBUG) ) {
    msg() << MSG::DEBUG << "CTP extra payload (" << l1_extraPayload.size() << " words): ";
    for (std::size_t i=0; i<l1_extraPayload.size(); ++i) {
      msg() << " " << l1_extraPayload[i];
    }
    std::ostringstream out;
    out << ctp_payload;
    msg() << out.str() << endmsg;
  }

  // Loop over potential new folder updates
  for (const std::pair<FolderIndex, FolderEntry>& kv : ctp_payload.getFolderUpdates()) {

    // Check if we already have an update for this folder
    std::map<FolderIndex, FolderUpdate>::const_iterator f = m_folderUpdates.find(kv.first);

    // No updates yet or this update supersedes the previous one
    if (f==m_folderUpdates.end() || (f->second.lumiBlock != kv.second.lumiBlock)) {
      m_folderUpdates[kv.first] = FolderUpdate(kv.second);   // new folder update
      
      ATH_MSG_DEBUG("Conditions update for folder " << kv.second.folderIndex
                   << " on lumiblock " << kv.second.lumiBlock);
    }
  }
  return StatusCode::SUCCESS;
}
