/*
  Copyright (C) 2002-202 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H

#include <map>
#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRecToolInterfaces/HoughDataPerSec.h"
#include "MuonRecToolInterfaces/IMuonSystemExtensionTool.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"

class MuonCombinedInDetCandidateAlg : public AthReentrantAlgorithm {
public:
    MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonCombinedInDetCandidateAlg() = default;

    StatusCode initialize() override;
    StatusCode execute(const EventContext& ctx) const override;

private:
    SG::ReadHandleKeyArray<xAOD::TrackParticleContainer> m_indetTrackParticleLocation{
        this, "TrackParticleLocation", {"InDetTrackParticles"}};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_indetForwardTrackParticleLocation{this, "ForwardParticleLocation",
                                                                                        "InDetForwardTrackParticles"};

    SG::WriteHandleKey<InDetCandidateCollection> m_candidateCollectionName{this, "InDetCandidateLocation", "InDetCandidates"};

    SG::ReadHandleKey<Muon::HoughDataPerSectorVec> m_houghDataPerSectorVecKey{this, "Key_MuonLayerHoughToolHoughDataPerSectorVec",
                                                                              "HoughDataPerSectorVec", "HoughDataPerSectorVec key"};

    ToolHandle<Trk::ITrackSelectorTool> m_trackSelector{
        this, "TrackSelector", "InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool", "Track selector tool"};
    ToolHandle<Trk::ITrackSelectorTool> m_forwardTrackSelector{
        this, "InDetForwardTrackSelector", "InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedForwardTrackSelectorTool",
        "Forward track selector tool"};
    ToolHandle<Muon::IMuonSystemExtensionTool> m_muonSystemExtensionTool{
        this, "MuonSystemExtensionTool", "Muon::MuonSystemExtensionTool/MuonSystemExtensionTool", "Muon system extension tool"};

    struct InDetCandidateCache {
        /// Track particle container
        const xAOD::TrackParticleContainer* inDetContainer{nullptr};
        /// Flag the candidates as sillicion forward associated
        bool flagAsSAF{false};
        /// Output container for the StoreGate
        std::unique_ptr<InDetCandidateCollection> outputContainer = std::make_unique<InDetCandidateCollection>(SG::OWN_ELEMENTS);

        const Trk::ITrackSelectorTool* trackSelector{nullptr};

        std::map<Muon::MuonStationIndex::DetectorRegionIndex, std::set<int>> hitted_sectors;
    };

    StatusCode create(const EventContext& ctx, InDetCandidateCache& output_cache) const;

    bool isValidTrackParticle(const Trk::ITrackSelectorTool* currentTrackSelector, const xAOD::TrackParticle* const tp) const;
    void printTrackParticleInfo(const xAOD::TrackParticle* const tp, const std::string& what) const;

    int getCount(const xAOD::TrackParticle& tp, xAOD::SummaryType type) const;

    Gaudi::Property<bool> m_doSiliconForwardMuons{this, "DoSiliconAssocForwardMuons", false};

    Gaudi::Property<float> m_extThreshold{this, "ExtensionPtThreshold", 2500};

    Gaudi::Property<bool> m_restrictExtension{this, "UseOnlyHittedSectors", false};

    StatusCode findHittedSectors(const EventContext& ctx, const SG::ReadHandleKey<Muon::HoughDataPerSectorVec>& key,
                                 InDetCandidateCache& output_cache) const;
};

#endif
