/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigCOOLUpdateHelper.h
 * @brief  Helper tool for COOL updates
 * @author Frank Winklmeier
 *
 * $Id: TrigCOOLUpdateHelper.cxx 666321 2015-05-11 17:32:34Z fwinkl $
 */

#include "TrigCOOLUpdateHelper.h"

#include "TrigServices/TrigHLTIssues.h"

#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CoralBase/AttributeListException.h"

#include "TH1F.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;


//=========================================================================
// Standard methods
//=========================================================================
TrigCOOLUpdateHelper::TrigCOOLUpdateHelper(const std::string &type,
                                           const std::string &name,
                                           const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_iovSvc(0),
    m_iovDbSvc(0),
    m_incidentSvc("IncidentSvc", name),
    m_hist_timeCondUpdate(0)
{
  // No abstract interface
  declareInterface<TrigCOOLUpdateHelper>(this);

  declareProperty("coolFolder", m_coolFolderName = "", "Name of COOL folder containing folder map");
  declareProperty("foldersToResetOnBeginRun", m_beginRunFolders, "COOL folders to reset at beginning of run");
}


TrigCOOLUpdateHelper::~TrigCOOLUpdateHelper()
{}


StatusCode TrigCOOLUpdateHelper::initialize()
{
  // Do not create these services if they are not available already
  service("IOVSvc", m_iovSvc, /*createIf=*/false).ignore();
  service("IOVDbSvc", m_iovDbSvc, /*createIf=*/false).ignore();

  CHECK( m_incidentSvc.retrieve() );

  ATH_MSG_INFO(" ---> foldersToResetOnBeginRun = ["
               << boost::algorithm::join(m_beginRunFolders, ",") << "]");
  
  // Register DataHandle with COOLUPDATE folder
  if (!m_coolFolderName.empty()) {
    CHECK( detStore()->regHandle(m_folderMapHandle, m_coolFolderName) );
  }

  // Register incident listener
  m_incidentSvc->addListener(this, "EndOfBeginRun");
  
  return StatusCode::SUCCESS;
}


StatusCode TrigCOOLUpdateHelper::start()
{
  m_folderInfo.clear();

  // Book histograms
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  if ( histSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Histogram service can not be retrieved");
    return StatusCode::FAILURE;
  }
  
  m_hist_timeCondUpdate = new TH1F("CoolFolderUpdateTime",
                                   "Time for conditions update;time [ms]",
                                   100, 0, 200);

  // monitoring information root directory
  string histPath = string("/EXPERT/");

  const INamedInterface* p_parent = dynamic_cast<const INamedInterface*>(parent());
  if (p_parent != 0) {
    histPath = histPath + p_parent->name() + string("/");
  } else {
    histPath = histPath + name() + string("/");
  }
  if ( (histSvc->regHist(histPath + m_hist_timeCondUpdate->GetName(), m_hist_timeCondUpdate)).isFailure() ) {
    ATH_MSG_ERROR("Registration of histogram '" << (histPath + m_hist_timeCondUpdate->GetName()) << "' failed.");
    return StatusCode::FAILURE;
  }

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

void TrigCOOLUpdateHelper::handle(const Incident&)
{
  m_folderUpdates.clear();
    
  // Triggered on EndOfBeginRun, i.e. when all folders are available
  readFolderInfo();
  if (m_coolFolderName.empty()) return;
  
  if (!m_folderMapHandle.isInitialized()) {
    ATH_MSG_ERROR("DataHandle for '" << m_coolFolderName << "' not initialized.");
  }
  else {
    ATH_MSG_INFO("COOL folder map in " << m_coolFolderName << ":");
    for (CondAttrListCollection::const_iterator c = m_folderMapHandle->begin();
         c != m_folderMapHandle->end(); ++c) {
      ATH_MSG_INFO("   (" << c->first << ") " << c->second["FolderName"].data<string>());
    }
  }
}


//=========================================================================
// Read COOL folder info
// Loop over all registered keys and get folder name and CLID
//=========================================================================
void TrigCOOLUpdateHelper::readFolderInfo()
{
  if (m_iovDbSvc==0) return;
  
  // Loop over all keys registered with IOVDbSvc
  vector<string> keyList(m_iovDbSvc->getKeyList());
  BOOST_FOREACH(string key, keyList) {
    string foldername, tag;
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
}


//=========================================================================
// Reset COOL folders
//=========================================================================
StatusCode TrigCOOLUpdateHelper::resetBeginRunFolders(EventID::number_type currentRun)
{
  readFolderInfo();   // Folders might be only partially avaiable
  return resetFolders(m_beginRunFolders, currentRun, false);
}

StatusCode TrigCOOLUpdateHelper::resetFolders(const std::vector<std::string>& folders,
                                              EventID::number_type currentRun,
                                              bool dropObject)
{
  if (folders.empty()) return StatusCode::SUCCESS;
  
  StatusCode sc(StatusCode::SUCCESS);
  BOOST_FOREACH(string folder, folders) {
    if ( resetFolder(folder, currentRun, dropObject).isFailure() ) sc = StatusCode::FAILURE;
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
  const string& key = f->second.key;
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
    if (not m_iovDbSvc->dropObject(key, /*resetCache=*/true)) {
      ATH_MSG_WARNING("IOVDbSvc could not drop " << folder);
      return StatusCode::FAILURE;
    }
  }
    
  // All OK
  ATH_MSG_INFO("Invalidated IOV" << (dropObject ? " and dropped payload " : " ")
               << "of folder " << folder << (key!=folder ? " (key="+key+")" : "") );
  
  return StatusCode::SUCCESS;
}


//=========================================================================
// Folder update during the run
//=========================================================================

StatusCode TrigCOOLUpdateHelper::hltCoolUpdate(EventID::number_type currentLB,
                                               EventID::number_type currentRun,
                                               EventID::number_type sor_time_sec,
                                               EventID::number_type sor_time_nsec)
{
  bool checkIOV_done = false;

  // Loop over folders to be updated
  std::map<CTPfragment::FolderIndex, FolderUpdate>::iterator f = m_folderUpdates.begin();
  for (; f!=m_folderUpdates.end(); ++f) {

    if (f->second.needsUpdate) {
      string folderName;
      if (getFolderName(f->second.folderIndex, folderName).isFailure()) {
        continue;  // On purpose not a failure
      }

      const EventInfo* currentEvent(0);
      CHECK(evtStore()->retrieve(currentEvent));
      ATH_MSG_INFO("Reload of COOL folder " << folderName << " for IOV change in lumiblock " << f->second.lumiBlock
                   << ". Current event: "  << *currentEvent->event_ID());

      if ( hltCoolUpdate(folderName, currentLB, currentRun,
                         sor_time_sec, sor_time_nsec).isFailure() ) {
        ATH_MSG_FATAL("COOL update failed for " << folderName << ". Aborting.");
        throw ers::HLTAbort(ERS_HERE, name()+": Failure during COOL update of "+folderName);
      }
      // All OK
      checkIOV_done = true;
      f->second.needsUpdate = false;
    }
  }

  // Needs to be done on every LB change
  if (!checkIOV_done) checkIOV(currentLB, currentRun, sor_time_sec, sor_time_nsec);

  return StatusCode::SUCCESS;
}


StatusCode TrigCOOLUpdateHelper::hltCoolUpdate(const std::string& folderName,
                                               EventID::number_type currentLB,
                                               EventID::number_type currentRun,
                                               EventID::number_type sor_time_sec,
                                               EventID::number_type sor_time_nsec)
{
  if ( m_iovSvc==0 || m_coolFolderName.empty() ) {
    ATH_MSG_DEBUG("Passive mode. Not updating COOL folders");
    return StatusCode::SUCCESS;
  }

  // Start timer
  longlong t1_ms = System::currentTime(System::milliSec);

  // Reset folder and make IOVDbSvc drop objects

  if (resetFolder(folderName, currentRun, true).isFailure()) {
    ATH_MSG_ERROR("Reset of " << folderName << " failed");
    return StatusCode::FAILURE;
  }

  checkIOV(currentLB, currentRun, sor_time_sec, sor_time_nsec);

  // Stop timer and fill histogram
  longlong t2_ms = System::currentTime(System::milliSec);
  if (m_hist_timeCondUpdate) m_hist_timeCondUpdate->Fill(t2_ms-t1_ms);

  return StatusCode::SUCCESS;
}

void TrigCOOLUpdateHelper::checkIOV(EventID::number_type currentLB,
                                    EventID::number_type currentRun,
                                    EventID::number_type sor_time_sec,
                                    EventID::number_type sor_time_nsec)
{
  // Force IOVSvc to check IOV ranges
  EventInfo eventInfo(new EventID(currentRun, /*event*/ 0,
                                  sor_time_sec, sor_time_nsec,
                                  currentLB, /*bcid*/ 0),
                      new EventType());
}


StatusCode TrigCOOLUpdateHelper::getFolderName(CTPfragment::FolderIndex idx, std::string& folderName)
{
  if (m_coolFolderName.empty()) return StatusCode::SUCCESS;
  
  if (!m_folderMapHandle.isInitialized()) {
    ATH_MSG_ERROR("DataHandle for '" << m_coolFolderName << "' not initialized.");
    return StatusCode::FAILURE;
  }
  try {
    folderName = m_folderMapHandle->attributeList(idx)["FolderName"].data<string>();      
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
// Schedule COOL folder updates (called on every event by EventLoopMgr)
//=========================================================================
void TrigCOOLUpdateHelper::setFolderUpdates(const std::map<CTPfragment::FolderIndex, CTPfragment::FolderEntry>& folderUpdates)
{
  // Loop over potential new folder updates
  std::map<CTPfragment::FolderIndex, CTPfragment::FolderEntry>::const_iterator i = folderUpdates.begin();
  for (; i!=folderUpdates.end(); ++i) {

    // Check if we already have an update for this folder
    std::map<CTPfragment::FolderIndex, FolderUpdate>::const_iterator f = m_folderUpdates.find(i->first);

    // No updates yet or this update superseds the previous one
    if (f==m_folderUpdates.end() || (f->second.lumiBlock != i->second.lumiBlock)) {
      m_folderUpdates[i->first] = FolderUpdate(i->second);   // new folder update
      
      ATH_MSG_DEBUG("Conditions update for folder " << i->second.folderIndex
                   << " on lumiblock " << i->second.lumiBlock);
    }
    /* The above could be optmized by handling events from a previous lumiblock in a special way
       and not forcing a COOL update. However, this would complicate the code and in the merged HLT
       system events from already seen LBs will be very rare (or even impossible).
    */
  }
}
