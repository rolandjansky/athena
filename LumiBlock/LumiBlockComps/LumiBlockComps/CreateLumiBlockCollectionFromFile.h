/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CREATELUMIBLOCKCOLLECTIONFROMFILE_H
# define CREATELUMIBLOCKCOLLECTIONFROMFILE_H

// *****************************************************************
//
// Algorithm:  CreateLumiBlockCollectionFromFile.hh
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
// Created: March 2007
// Description:
//     This algorithm is used to store the list of processed lumiblocks
//     in the metadata store
//
// *********************************************************************

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVTime.h"
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <stdint.h>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "xAODEventInfo/EventInfo.h"

class StoreGateSvc;

class CreateLumiBlockCollectionFromFile:public AthAlgorithm, virtual public IIncidentListener,
		virtual public IIoComponent {
public:
  CreateLumiBlockCollectionFromFile (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode fillLumiBlockCollection();

  /// Incident service handle listening for BeginFile and EndFile.
  void handle(const Incident& incident);
  ///  Callback for database access
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit();

protected:
 
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK for EventInfo"};

  /// Fill metaDataStore and ntuples
  void finishUp();

  // Here is the transient storage of the LB metadata
  typedef std::pair<uint32_t,uint32_t> inOut;
  typedef std::map<IOVTime,inOut> RLBMap;
  RLBMap m_LumiBlockInfo;

  uint32_t m_lastRun;        // remember RunNumber from last event
  uint32_t m_lastLumiBlock;  // remember LumiBlock from last event
  uint32_t m_channel;        // database channel holding number of events expected
  std::string m_streamName;  // Stream name 
  IOVTime m_lastIOVTime;     // could remake from m_lastRun and m_lastLumiBlock, but for efficiency save it
  uint32_t m_numExpected;    // number of expected events for this LumiBlock
  bool m_checkEventsExpected;   // should we read the database?

  ServiceHandle<StoreGateSvc> m_metaStore;    //cache the StoreGateSvc ptr for efficiency
 
  std::string m_LBColl_name;
  std::string m_unfinishedLBColl_name;
  std::string m_suspectLBColl_name;
  std::string m_folderName;
};


#endif // end of CREATELUMIBLOCKCOLLECTIONFROMFILE_H
