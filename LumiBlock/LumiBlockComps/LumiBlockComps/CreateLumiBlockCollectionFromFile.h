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
  StatusCode fillLumiBlockCollection(std::vector<IOVTime>* coll, std::string key);

  /// Incident service handle listening for BeginFile and EndFile.
  void handle(const Incident& incident);

protected:
 
  /// Fill metaDataStore and ntuples
  void finishUp();

  std::vector<IOVTime> m_LumiBlockColl;  //collection of (run,lumiblock) pairs
  std::vector<IOVTime> m_cacheLBColl;  //collection of (run,lumiblock) pairs
                                       //for files that have closed properly 
 
  uint32_t m_lastRun;           // remember run from last event
  uint32_t m_lastLumiBlock;     // remember lumiBlock from last event
  StoreGateSvc* m_storeGate;    //cache the StoreGateSvc ptr for efficiency
  StoreGateSvc* m_metaStore;    //cache the StoreGateSvc ptr for efficiency
 
  StringProperty  m_LBColl_name;
  StringProperty  m_unfinishedLBColl_name;
};

#endif // end of CREATELUMIBLOCKCOLLECTIONFROMFILE_H
