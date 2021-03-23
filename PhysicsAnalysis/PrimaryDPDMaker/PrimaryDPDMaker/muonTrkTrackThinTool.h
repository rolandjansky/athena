/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTRKTRACKTHINTOOL_H
#define MUONTRKTRACKTHINTOOL_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"


class muonTrkTrackThinTool : public AthReentrantAlgorithm {
  
 public: 
  /** Constructor with parameters */
  muonTrkTrackThinTool( const std::string& name, ISvcLocator* pSvcLocator );
  
  /** Destructor */
  virtual ~muonTrkTrackThinTool()=default;
  
  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& ctx) const  override;
  virtual StatusCode  finalize() override;
    
private:
  mutable std::atomic<int> m_All{0};
  mutable std::atomic<int> m_pass{0};
  mutable std::atomic<int> m_trackAll{0};
  mutable std::atomic<int> m_trackpass{0};
  
  StringProperty m_streamName
    { this, "StreamName", "", "Name of the stream being thinned" };
  SG::ThinningHandleKey<TrackCollection> m_trackCollKey
    { this, "TrackCollectionKey", "MuonSpectrometerTracks", "" };
  
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonReadKey
    {this, "MuonReadKey", "Muons", "Key to retrieve the xAOD::MuonContainer"};
}; 

#endif
