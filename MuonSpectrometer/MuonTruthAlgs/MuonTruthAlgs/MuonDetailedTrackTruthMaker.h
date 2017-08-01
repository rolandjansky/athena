/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDETAILEDTRACKTRUTHMAKER_H
#define MUONDETAILEDTRACKTRUTHMAKER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandleKeyArray.h"
#include "StoreGate/ReadHandleKeyArray.h"

#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"

/**
 * This algorithm produces track truth data using Muon PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 * 
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

class MuonDetailedTrackTruthMaker : public AthAlgorithm  {
  
public:
  MuonDetailedTrackTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  // PRD truth maps in any order
  SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_PRD_TruthNames;

  // Track input
  SG::ReadHandleKeyArray<TrackCollection> m_trackCollectionNames;

  // DetailedTrackTruthCollection output
  SG::WriteHandleKeyArray<DetailedTrackTruthCollection> m_detailedTrackTruthNames;

  // Tool Handle for truth tool
  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool;

};

#endif/*MUONDETAILEDTRACKTRUTHMAKER_H*/
