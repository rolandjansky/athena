/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_TRIGCOOLUPDATEHELPER_H
#define TRIGSERVICES_TRIGCOOLUPDATEHELPER_H
/**
 * @file   TrigCOOLUpdateHelper.h
 * @brief  Helper tool for COOL updates
 * @author Frank Winklmeier
 *
 * $Id: $
 */

// STL includes
#include <string>
#include <vector>
#include <map>

// Framework includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/EventID.h"

#include "CTPfragment/CTPExtraWordsFormat.h"

class TH1F;
class IIOVSvc;
class IIOVDbSvc;
class IIncidentSvc;
class CondAttrListCollection;

/**
 * Struct to hold CLID <-> folder name mapping
 */
struct FolderInfo {
  FolderInfo() : clid(CLID_NULL) {}
  FolderInfo(const CLID& cl, const std::string& k) : clid(cl), key(k) {}
  CLID clid;
  std::string key;
};

/**
 * Folder update entry
 */
struct FolderUpdate {
  FolderUpdate() : lumiBlock(0), folderIndex(0), needsUpdate(true) {}
  explicit FolderUpdate(const CTPfragment::FolderEntry& entry) :
    lumiBlock(entry.lumiBlock),
    folderIndex(entry.folderIndex),
    needsUpdate(true) {}
  
  EventID::number_type lumiBlock;
  CTPfragment::FolderIndex folderIndex;
  bool needsUpdate;
};

/**
 * @brief Tool to perform COOL updates during the run
 *
 */
class TrigCOOLUpdateHelper : public AthAlgTool,
                             public IIncidentListener
{
public:
  
  TrigCOOLUpdateHelper(const std::string &type, const std::string &name, const IInterface *parent);    
  virtual ~TrigCOOLUpdateHelper();

  static const InterfaceID& interfaceID();  
  virtual StatusCode initialize();
  virtual StatusCode start();
  virtual StatusCode stop();
  virtual void handle (const Incident&);
  
  /**
   * @brief Reset folders configured to be reset on BeginRun
   * @param currentRun  Current run number
   */
  StatusCode resetBeginRunFolders(EventID::number_type currentRun);

  /**
   * @brief Perform COOL udpates if needed
   * @param currentLB      Current lumiblock
   * @param currentRun     Current run number
   * @param sor_time_sec   Start-Of-Run time in seconds
   * @param sor_time_nsec  Start-Of-Run time offset in nanoseconds
   */
  StatusCode hltCoolUpdate(EventID::number_type currentLB,
                           EventID::number_type currentRun,
                           EventID::number_type sor_time_sec,
                           EventID::number_type sor_time_nsec);

  /**
   * @brief Update specified COOL folder
   * @param folder         COOL folder to reset
   * @param currentLB      Current lumiblock
   * @param currentRun     Current run number
   * @param sor_time_sec   Start-Of-Run time in seconds
   * @param sor_time_nsec  Start-Of-Run time offset in nanoseconds
   * @param doProxyReset   Drop object and reset cache
   */
  StatusCode hltCoolUpdate(const std::string& folder,
                           EventID::number_type currentLB,
                           EventID::number_type currentRun,
                           EventID::number_type sor_time_sec,
                           EventID::number_type sor_time_nsec);

  /**
   * @brief Force IOVSvc to recheck all IOVs (needs to be called on every LB change)
   * @param currentLB      Current lumiblock
   * @param currentRun     Current run number
   * @param sor_time_sec   Start-Of-Run time in seconds
   * @param sor_time_nsec  Start-Of-Run time offset in nanoseconds
   */
  void checkIOV(EventID::number_type currentLB,
                EventID::number_type currentRun,
                EventID::number_type sor_time_sec,
                EventID::number_type sor_time_nsec);

  /**
   * @brief Return folder name to index
   * @param idx         Folder index
   * @param folderName  Returns folder name
   */
  StatusCode getFolderName(CTPfragment::FolderIndex idx, std::string& folderName);
  
  /**
   * @brief Set the folder updates as retrieved from the CTP fragment
   * @param map returned by CTPfragment::ExtraPayload::getFolderUpdates()
   */
  void setFolderUpdates(const std::map<CTPfragment::FolderIndex, CTPfragment::FolderEntry>& folderUpdates);
  
private:

  /**
   * @brief Reset COOL folder
   * @param folder COOL folder name
   * @param currentRun Current run number
   * @param dropObject Drop object and reset cache
   */
  StatusCode resetFolder(const std::string& folder,
                         EventID::number_type currentRun,
                         bool dropObject = false);
  
  /**
   * @brief Reset list of COOL folders
   * @param folders List of folder names
   * @param currentRun Current run number
   * @param dropObject Drop object and reset cache
   */
  StatusCode resetFolders(const std::vector<std::string>& folders,
                          EventID::number_type currentRun,
                          bool dropObject = false);

  /**
   * Read information about existing COOL folders
   */
  void readFolderInfo();

  typedef std::map<std::string, FolderInfo> FolderInfoMap;
  FolderInfoMap m_folderInfo;  //!< COOL folder info
  
  const DataHandle<CondAttrListCollection> m_folderMapHandle;
  std::map<CTPfragment::FolderIndex, FolderUpdate> m_folderUpdates;
  
  // Services and Tools
  IIOVSvc*                    m_iovSvc;
  IIOVDbSvc*                  m_iovDbSvc;
  ServiceHandle<IIncidentSvc> m_incidentSvc;

  // Histograms
  TH1F*        m_hist_timeCondUpdate;   //!< Timing histogram for conditions updates
  
  // Properties
  std::string               m_coolFolderName;        //!< COOL folder name
  std::vector<std::string>  m_beginRunFolders;  //!< Folders to reset before BeginRun
};


inline const InterfaceID& TrigCOOLUpdateHelper::interfaceID()
{
  static const InterfaceID IID("TrigCOOLUpdateHelper", 1, 0);
  return IID;
}

#endif
