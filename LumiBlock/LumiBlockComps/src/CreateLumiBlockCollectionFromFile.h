/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CREATELUMIBLOCKCOLLECTIONFROMFILE_H
#define CREATELUMIBLOCKCOLLECTIONFROMFILE_H

/**
 * @file CreateLumiBlockCollectionFromFile.hh
 *
 * @author Marjorie Shapiro <mdshapiro@lbl.gov>
 *
 * @brief This algorithm is used to store the list of processed lumiblocks
 *    in the metadata store
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVTime.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"

#include <vector>
#include <map>
#include <utility>
#include <string>
#include <stdint.h>

class StoreGateSvc;

class CreateLumiBlockCollectionFromFile : public AthAlgorithm
  , virtual public IIncidentListener
  , virtual public IIoComponent
{
 public:
  CreateLumiBlockCollectionFromFile (const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  /// Incident service handle listening for MetaDataStop
  virtual void handle(const Incident& incident) override;

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
  virtual StatusCode io_reinit() override;

 private:
  /// Fill metaDataStore and ntuples
  StatusCode fillLumiBlockCollection();

  uint32_t getNEventsFromDb();

  // __________________ Properties ______________________
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey {this
      ,"EventInfoKey"
      ,"EventInfo"
      ,"RHK for EventInfo"};

  SG::ReadCondHandleKey<CondAttrListCollection> m_rchk {this
      ,"GlobalFileCountKey"
      ,"/GLOBAL/FILECOUNT/PROMPT"
      ,"RCHK for condition data" };

  Gaudi::Property<std::string> m_streamName {this
      ,"streamName"
      ,""
      ,"Stream name"};

  Gaudi::Property<std::string> m_LBColl_name {this
      ,"LBCollName"
      ,"LumiBlocks"
      ,"Complete LumiBlock collection name"};

  Gaudi::Property<std::string> m_unfinishedLBColl_name {this
      ,"unfinishedLBCollName"
      ,"IncompleteLumiBlocks"
      ,"Incomplete LumiBlock collection name"};

  Gaudi::Property<std::string> m_suspectLBColl_name {this
      ,"suspectLBCollName"
      ,"SuspectLumiBlocks"
      ,"Suspicious LumiBlock collection name"};

  Gaudi::Property<bool>        m_checkEventsExpected {this
      ,"checkEventsExpected"
      ,true
      ,"Should we read the database?"};
  // __________________ Properties ______________________

  // Here is the transient storage of the LB metadata
  typedef std::pair<uint32_t,uint32_t> inOut;
  typedef std::map<IOVTime,inOut> RLBMap;
  RLBMap m_LumiBlockInfo;

  uint32_t m_lastRun;        // remember RunNumber from last event
  uint32_t m_lastLumiBlock;  // remember LumiBlock from last event
  IOVTime m_lastIOVTime;     // could remake from m_lastRun and m_lastLumiBlock, but for efficiency save it

  ServiceHandle<StoreGateSvc> m_metaStore;    //cache the StoreGateSvc ptr for efficiency
};

#endif
