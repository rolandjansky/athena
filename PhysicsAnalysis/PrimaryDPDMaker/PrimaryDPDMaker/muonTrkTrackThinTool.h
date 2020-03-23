/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// muonTrkTrackThinTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRKTRACKTHINTOOL_H
#define MUONTRKTRACKTHINTOOL_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"
#include "TrkTrack/TrackCollection.h"


class muonTrkTrackThinTool : public AthAlgorithm {
  
 public: 
  /** Constructor with parameters */
  muonTrkTrackThinTool( const std::string& name, ISvcLocator* pSvcLocator );
  
  /** Destructor */
  virtual ~muonTrkTrackThinTool();
  
  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;
    
private:
  int m_All;
  int m_pass;
  int m_trackAll;
  int m_trackpass;
  
  StringProperty m_streamName
    { this, "StreamName", "", "Name of the stream being thinned" };
  SG::ThinningHandleKey<TrackCollection> m_trackCollKey
    { this, "TrackCollectionKey", "MuonSpectrometerTracks", "" };
}; 

#endif
