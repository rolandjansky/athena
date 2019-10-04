/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATTRACKBUILDER_H
#define MUPATTRACKBUILDER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

// Monitoring
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/Monitored.h"

namespace Muon {
  class IMuonTrackFinder;
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
  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };    //!< helper Tool 

  // Monitoring tool
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };


};

#endif 
