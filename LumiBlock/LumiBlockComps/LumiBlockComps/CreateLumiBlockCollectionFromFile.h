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

#include "GaudiKernel/Algorithm.h"
#include "AthenaKernel/IOVTime.h"
#include <vector>
#include <map>
#include <utility>
#include <stdint.h>

#include "LumiBlockData/LumiBlockCollection.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

class StoreGateSvc;

class CreateLumiBlockCollectionFromFile:public Algorithm, virtual public IIncidentListener {
public:
  CreateLumiBlockCollectionFromFile (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode fillLumiBlockCollection();

  /// Incident service handle listening for BeginFile and EndFile.
  void handle(const Incident& incident);

protected:
 
  /// Fill metaDataStore and ntuples
  void finishUp();

  // Here is the transient storage of the LB metadata
  typedef std::pair<uint32_t,uint32_t> inOut;
  typedef std::map<IOVTime,inOut> RLBMap;
  RLBMap m_LumiBlockInfo;

  uint32_t m_lastRun;           // remember run from last event
  uint32_t m_lastLumiBlock;     // remember lumiBlock from last event
  IOVTime m_lastIOVTime;        // could remake from the previous 2, but for efficiency save it
  bool m_checkEventsExpected;   

  StoreGateSvc* m_storeGate;    //cache the StoreGateSvc ptr for efficiency
  StoreGateSvc* m_metaStore;    //cache the StoreGateSvc ptr for efficiency
 
  StringProperty  m_LBColl_name;
  StringProperty  m_unfinishedLBColl_name;
  StringProperty  m_suspectLBColl_name;
};


#endif // end of CREATELUMIBLOCKCOLLECTIONFROMFILE_H
