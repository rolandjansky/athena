/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCREATORALG_H
#define MUONCOMBINEDALGS_MUONCREATORALG_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"
#include "MuonSegment/MuonSegment.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "xAODTracking/TrackParticleContainer.h"

class MuonCreatorAlg : public AthReentrantAlgorithm {
public:
    MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator);

    ~MuonCreatorAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

private:
    ToolHandle<MuonCombined::IMuonCreatorTool> m_muonCreatorTool{this, "MuonCreatorTool", "MuonCombined::MuonCreatorTool/MuonCreatorTool",
                                                                 "Muon creator tool"};
    ToolHandle<xAODMaker::IMuonSegmentConverterTool> m_muonSegmentConverterTool{this, "MuonSegmentConverterTool",
                                                                                "Muon::MuonSegmentConverterTool/MuonSegmentConverterTool"};
    SG::WriteHandleKey<xAOD::MuonContainer> m_muonCollectionName{this, "MuonContainerLocation", "Muons", "Muon Container"};
    SG::WriteHandleKey<xAOD::SlowMuonContainer> m_slowMuonCollectionName{this, "SlowMuonContainerLocation", "SlowMuons",
                                                                         "Slow Muon Container"};
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_combinedCollectionName{this, "CombinedLocation", "CombinedMuon", "Combined muons"};
    SG::WriteHandleKey<TrackCollection> m_combinedTrkCollectionName{this, "CombinedTrackLocation", "CombinedMuon", "Combined muon tracks"};
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_extrapolatedCollectionName{this, "ExtrapolatedLocation", "ExtrapolatedMuon",
                                                                                  "Extrapolated muons"};
    SG::WriteHandleKey<TrackCollection> m_extrapolatedTrkCollectionName{this, "ExtrapolatedTrackLocation", "ExtrapolatedMuon",
                                                                        "Extrapolated muon tracks"};
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_msOnlyExtrapolatedCollectionName{this, "MSOnlyExtrapolatedLocation",
                                                                                        "MSOnlyExtrapolatedMuon", "MS Extrapolated muons"};
    SG::WriteHandleKey<TrackCollection> m_msOnlyExtrapolatedTrkCollectionName{
        this, "MSOnlyExtrapolatedTrackLocation", "MSOnlyExtrapolatedMuonTracks", "MS extrapolated muon tracks"};
    SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName{this, "InDetCandidateLocation", "InDetCandidates",
                                                                               "ID candidates"};

    SG::ReadHandleKeyArray<MuonCandidateCollection> m_muonCandidateKeys{
        this, "MuonCandidateLocation", {"MuonCandidates"}, "Muon candidates"};
    SG::ReadHandleKeyArray<MuonCombined::InDetCandidateToTagMap> m_tagMaps{
        this, "TagMaps", {"muidcoTagMap", "stacoTagMap", "muGirlTagMap", "caloTagMap", "segmentTagMap"}, "ID candidate to tag maps"};
    SG::ReadHandleKey<Trk::SegmentCollection> m_inputSegContainerName{this, "InputSegmentContainerName", "TrackMuonSegments",
                                                                      "Input track segments"};
    SG::WriteHandleKey<xAOD::MuonSegmentContainer> m_segContainerName{this, "SegmentContainerName", "MuonSegments", "Segments"};
    SG::WriteHandleKey<Trk::SegmentCollection> m_segTrkContainerName{this, "TrackSegmentContainerName", "TrkMuonSegments",
                                                                     "Track segments"};

    SG::WriteHandleKey<xAOD::CaloClusterContainer> m_clusterContainerName{this, "ClusterContainerName", "MuonClusterCollection",
                                                                          "Clusters"};
    SG::WriteHandleKey<CaloClusterCellLinkContainer> m_clusterContainerLinkName{this, "CaloClusterCellLinkName", "MuonClusterCollection",
                                                                                "Cluster links"};

    Gaudi::Property<bool> m_buildSlowMuon{this, "BuildSlowMuon", false};
    Gaudi::Property<bool> m_doSA{this, "CreateSAmuons", false};
    Gaudi::Property<bool> m_makeClusters{this, "MakeClusters", true};
    Gaudi::Property<bool> m_copySegs{this, "CopySegments", true};

    // Monitoring tool
    ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
};

#endif
