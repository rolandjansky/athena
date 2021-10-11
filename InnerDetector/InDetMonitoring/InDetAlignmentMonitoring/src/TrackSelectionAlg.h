/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRACKSELECTIONALG_H
#define TRACKSELECTIONALG_H

// **********************************************************************
// TrackSelectionAlg.h
// AUTHORS: Ben Cooper
// **********************************************************************

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"

#include "GaudiKernel/ToolHandle.h"

#include <vector>

namespace Trk {
  class Track;
}

class TrackSelectionAlg: public AthAlgorithm
{
public:
  TrackSelectionAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackSelectionAlg ();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();
private:
  bool makeTrackCuts(const Trk::Track*, float);

  PublicToolHandle< Trk::ITrackSummaryTool > m_trackSumTool
  {
    this, "TrackSummaryTool", "Trk::TrackSummaryTool/InDetTrackSummaryTool", ""
  }; //!<  Pointer to Trk::ITrackSummaryTool

  float m_trackMinPt;
  float m_trackMinEta;
  float m_trackMaxEta;
  float m_trackMaxVtxZ0;
  float m_trackMaxD0;
  int m_minPixelHits;
  int m_minSCTPixHits;
  int m_minBLayerHits;
  int m_minTRTHits;
  int m_minTRTHitsHT;

  //these member variables only play a role if use selectTracks() zero argument method above
  SG::ReadHandleKey<TrackCollection> m_inputTrackCol {
    this, "InputTrackColName", "ExtendedTracks"
  };
  SG::WriteHandleKey<TrackCollection> m_outputTrackCol {
    this, "OutputTrackColName", "MySelectedTracks"
  };
  SG::ReadHandleKey<VxContainer> m_vertices {
    this, "PrimaryVertexName", "VxPrimaryCandidate"
  };
};



#endif
