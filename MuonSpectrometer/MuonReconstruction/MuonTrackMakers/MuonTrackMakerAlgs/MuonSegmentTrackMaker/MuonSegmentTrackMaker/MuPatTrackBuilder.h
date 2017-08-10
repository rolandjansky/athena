/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATTRACKBUILDER_H
#define MUPATTRACKBUILDER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"

namespace Muon {
  class IMuonTrackFinder;
  class MuonEDMHelperTool;
}
//class IMuonboyToParticleTool;
//namespace Muon {
//  class IMuonBackTracker;
//}

class MuPatTrackBuilder : public AthAlgorithm
{
 public:
  MuPatTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator);

 public:
  virtual ~MuPatTrackBuilder();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  SG::ReadHandleKey<Trk::SegmentCollection>  m_segmentKey;       //!< Key of input MuonSegmentCombination collection
  SG::WriteHandleKey<TrackCollection>        m_spectroTrackKey;  //!< Track output Key for tracks strictly in MS
  SG::WriteHandleKey<TrackCollection>        m_spectroPartiKey;  //!< Track output Key for track particles (strictly in MS)
  SG::WriteHandleKey<TrackCollection>        m_extrapTrackKey;  //!< Output Key for back-extrapolated tracks
  SG::WriteHandleKey<TrackCollection>        m_extrapPartiKey;  //!< Output Key for back-extrapolated particles

  ToolHandle<Muon::IMuonTrackFinder> m_trackMaker;  //!< Actual tool to do the track finding
//  ToolHandle<IMuonboyToParticleTool> m_convTool;    //!< Tool for converting from tracks to track particles (acts as a flag)
//  ToolHandle<Muon::IMuonBackTracker> p_IMuonBackTracker;    //!< Tool for extrapolating tracks
  ToolHandle<Muon::MuonEDMHelperTool> m_helper;    //!< helper Tool 
};

#endif 
