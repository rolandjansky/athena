/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDETAILEDTRACKTRUTHMAKER_H
#define MUONDETAILEDTRACKTRUTHMAKER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/WriteHandleKeyArray.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"

#include <string>

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
  
private:
  // PRD truth maps in any order
  SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_PRD_TruthNames{this,"PRD_TruthNames",{"CSC_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"},"PRD truth"};

  // Track input
  SG::ReadHandleKeyArray<TrackCollection> m_trackCollectionNames{this,"TrackCollectionNames",{"MuonSpectrometerTracks"},"track collections"};

  // DetailedTrackTruthCollection output
  SG::WriteHandleKeyArray<DetailedTrackTruthCollection> m_detailedTrackTruthNames{this,"DetailedTrackTruthNames",{""},"output collections"};

  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool{this,"TruthTool","Trk::DetailedTrackTruthBuilder"};

  Gaudi::Property<bool> m_hasCSC {this, "HasCSC", true};
  Gaudi::Property<bool> m_hasSTgc {this, "HasSTgc", true};
  Gaudi::Property<bool> m_hasMM {this, "HasMM", true};

};

#endif/*MUONDETAILEDTRACKTRUTHMAKER_H*/
