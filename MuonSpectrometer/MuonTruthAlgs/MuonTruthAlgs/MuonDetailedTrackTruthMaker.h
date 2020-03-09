/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDETAILEDTRACKTRUTHMAKER_H
#define MUONDETAILEDTRACKTRUTHMAKER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
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
  SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_PRD_TruthNames{this,"PRD_TruthNames",{"CSC_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"},"PRD truth"};

  // Track input
  SG::ReadHandleKeyArray<TrackCollection> m_trackCollectionNames{this,"TrackCollectionNames",{"MuonSpectrometerTracks"},"track collections"};

  // DetailedTrackTruthCollection output
  SG::WriteHandleKeyArray<DetailedTrackTruthCollection> m_detailedTrackTruthNames{this,"DetailedTrackTruthNames",{""},"output collections"};

  // Tool Handle for truth tool
  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool;

  bool m_hasCSC;
  bool m_hasSTgc;
  bool m_hasMM;

};

#endif/*MUONDETAILEDTRACKTRUTHMAKER_H*/
