/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATTRACKBUILDER_H
#define MUPATTRACKBUILDER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonTrackFinder.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"

class MuPatTrackBuilder : public AthAlgorithm {
public:
    using AthAlgorithm::AthAlgorithm;

public:
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

private:
    SG::ReadHandleKey<Trk::SegmentCollection> m_segmentKey{this, "MuonSegmentCollection", "MooreSegments",
                                                           "Input segment location"};  //!< Key of input MuonSegmentCombination collection
    SG::WriteHandleKey<TrackCollection> m_spectroTrackKey{this, "SpectrometerTrackOutputLocation",
                                                          "MuonSpectrometerTracks"};  //!< Track output Key for tracks strictly in MS
    ToolHandle<Muon::IMuonTrackFinder> m_trackMaker{this, "TrackSteering", "Muon::MuonTrackFinder/MuonTrackSteering",
                                                    "Actual tool to do the track finding"};  //!< Actual tool to do the track finding
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
        this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface"};  //!< helper Tool

    // Monitoring tool
    ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
};

#endif
