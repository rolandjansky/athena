/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCreatorAlg.h"

#include <vector>

#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/SlowMuonAuxContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
namespace {
    constexpr const double MeVtoGeV = 1 / Gaudi::Units::GeV;
}

MuonCreatorAlg::MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonCreatorAlg::initialize() {
    ATH_CHECK(m_muonCreatorTool.retrieve());
    if (m_copySegs)
        ATH_CHECK(m_muonSegmentConverterTool.retrieve());
    else
        m_muonSegmentConverterTool.disable();
    ATH_CHECK(m_muonCollectionName.initialize());
    ATH_CHECK(m_slowMuonCollectionName.initialize(m_buildSlowMuon));
    ATH_CHECK(m_indetCandidateCollectionName.initialize(!m_doSA));
    ATH_CHECK(m_muonCandidateKeys.initialize(!m_buildSlowMuon));
    // Can't use a flag in intialize for an array of keys
    if (!m_doSA) ATH_CHECK(m_tagMaps.initialize());
    ATH_CHECK(m_inputSegContainerName.initialize(m_copySegs));
    ATH_CHECK(m_segContainerName.initialize());
    ATH_CHECK(m_segTrkContainerName.initialize());
    m_combinedTrkCollectionName = m_combinedCollectionName.key() + "Tracks";
    m_combinedCollectionName = m_combinedCollectionName.key() + "TrackParticles";
    ATH_CHECK(m_combinedCollectionName.initialize());
    ATH_CHECK(m_combinedTrkCollectionName.initialize());
    m_extrapolatedTrkCollectionName = m_extrapolatedCollectionName.key() + "Tracks";
    m_extrapolatedCollectionName = m_extrapolatedCollectionName.key() + "TrackParticles";
    ATH_CHECK(m_extrapolatedCollectionName.initialize());
    ATH_CHECK(m_extrapolatedTrkCollectionName.initialize());
    m_msOnlyExtrapolatedCollectionName = m_msOnlyExtrapolatedCollectionName.key() + "TrackParticles";
    m_msOnlyExtrapolatedTrkCollectionName = m_msOnlyExtrapolatedCollectionName.key() + "Tracks";
    ATH_CHECK(m_msOnlyExtrapolatedCollectionName.initialize());
    ATH_CHECK(m_msOnlyExtrapolatedTrkCollectionName.initialize());
    ATH_CHECK(m_clusterContainerName.initialize(m_makeClusters));
    m_clusterContainerLinkName = m_clusterContainerName.key() + "_links";
    ATH_CHECK(m_clusterContainerLinkName.initialize(m_makeClusters));

    if (not m_monTool.name().empty()) { ATH_CHECK(m_monTool.retrieve()); }
    ATH_MSG_DEBUG("MuonCreatorAlg has been setup  successfully");
    ATH_MSG_DEBUG("    *** SegmentTrackContainer:            " << m_segTrkContainerName);
    ATH_MSG_DEBUG("    *** SegmentContainer:                 " << m_segContainerName);
    ATH_MSG_DEBUG("    *** CombinedTrackContainer:           " << m_combinedTrkCollectionName);
    ATH_MSG_DEBUG("    *** xAOD::CombinedTrackContainer:     " << m_combinedCollectionName);

    ATH_MSG_DEBUG("    *** xAOD::ExtrpolatedTrackContainer:  " << m_extrapolatedCollectionName);
    ATH_MSG_DEBUG("    *** ExtrpolatedTrackContainer:        " << m_extrapolatedTrkCollectionName);
    ATH_MSG_DEBUG("    *** xAOD::MSOnlyTrackContainer:       " << m_msOnlyExtrapolatedCollectionName);
    ATH_MSG_DEBUG("    *** ExtrpolatedTackContainer:         " << m_msOnlyExtrapolatedTrkCollectionName);

    return StatusCode::SUCCESS;
}

StatusCode MuonCreatorAlg::execute(const EventContext& ctx) const {
    
    std::vector<const MuonCombined::InDetCandidateToTagMap*> tagMaps;
    tagMaps.reserve(m_tagMaps.size());
    if (!m_doSA) {
        for (SG::ReadHandle<MuonCombined::InDetCandidateToTagMap>& h : m_tagMaps.makeHandles(ctx)) tagMaps.push_back(h.cptr());
    }

    // Create the xAOD container and its auxiliary store:
    SG::WriteHandle<xAOD::MuonContainer> wh_muons(m_muonCollectionName, ctx);
    ATH_CHECK(wh_muons.record(std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>()));
    ATH_MSG_DEBUG("Recorded Muons with key: " << m_muonCollectionName.key());
    MuonCombined::IMuonCreatorTool::OutputData output(*(wh_muons.ptr()));

    // Create and record track particles:
    // combined tracks
    SG::WriteHandle<xAOD::TrackParticleContainer> wh_combtp(m_combinedCollectionName, ctx);
    SG::WriteHandle<TrackCollection> wh_combtrk(m_combinedTrkCollectionName, ctx);
    ATH_CHECK(wh_combtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
    ATH_CHECK(wh_combtrk.record(std::make_unique<TrackCollection>()));
    output.combinedTrackParticleContainer = wh_combtp.ptr();
    output.combinedTrackCollection = wh_combtrk.ptr();

    // extrapolated tracks
    SG::WriteHandle<xAOD::TrackParticleContainer> wh_extrtp(m_extrapolatedCollectionName, ctx);
    SG::WriteHandle<TrackCollection> wh_extrtrk(m_extrapolatedTrkCollectionName, ctx);
    ATH_CHECK(wh_extrtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
    ATH_CHECK(wh_extrtrk.record(std::make_unique<TrackCollection>()));
    output.extrapolatedTrackParticleContainer = wh_extrtp.ptr();
    output.extrapolatedTrackCollection = wh_extrtrk.ptr();

    // msonly tracks
    SG::WriteHandle<xAOD::TrackParticleContainer> wh_msextrtp(m_msOnlyExtrapolatedCollectionName, ctx);
    SG::WriteHandle<TrackCollection> wh_msextrtrk(m_msOnlyExtrapolatedTrkCollectionName, ctx);
    ATH_CHECK(wh_msextrtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
    ATH_CHECK(wh_msextrtrk.record(std::make_unique<TrackCollection>()));
    output.msOnlyExtrapolatedTrackParticleContainer = wh_msextrtp.ptr();
    output.msOnlyExtrapolatedTrackCollection = wh_msextrtrk.ptr();

    // segments
    SG::WriteHandle<xAOD::MuonSegmentContainer> wh_segment(m_segContainerName, ctx);
    ATH_CHECK(wh_segment.record(std::make_unique<xAOD::MuonSegmentContainer>(), std::make_unique<xAOD::MuonSegmentAuxContainer>()));
    output.xaodSegmentContainer = wh_segment.ptr();
    SG::WriteHandle<Trk::SegmentCollection> wh_segmentTrk(m_segTrkContainerName, ctx);
    ATH_CHECK(wh_segmentTrk.record(std::make_unique<Trk::SegmentCollection>()));
    output.muonSegmentCollection = wh_segmentTrk.ptr();
    if (m_copySegs) {
        // now copy the input segments to the final container
        SG::ReadHandle<Trk::SegmentCollection> inputSegs(m_inputSegContainerName, ctx);
        if (inputSegs.isValid()) {
            for (const Trk::Segment* seg : *inputSegs) {
                // have to cast because the collection stores Trk::Segments
                const Muon::MuonSegment* muonSegment = dynamic_cast<const Muon::MuonSegment*>(seg);
                if (muonSegment) wh_segmentTrk->push_back(new Muon::MuonSegment(*muonSegment));
            }
        }
        // now convert
        unsigned int index = 0;
        for (const Trk::Segment* seg : *wh_segmentTrk) {
            // have to cast because the collection stores Trk::Segments
            const Muon::MuonSegment* muonSegment = dynamic_cast<const Muon::MuonSegment*>(seg);
            if (!muonSegment) {
                ++index;
                continue;
            }
            ElementLink<Trk::SegmentCollection> link(*wh_segmentTrk, index);
            ++index;
            m_muonSegmentConverterTool->convert(link, wh_segment.ptr());
        }
    }

    // calo clusters
    std::unique_ptr<xAOD::CaloClusterContainer> caloclusters{nullptr};
    std::unique_ptr<xAOD::CaloClusterAuxContainer> caloclustersaux{nullptr};

    if (m_makeClusters) {
        caloclusters = std::make_unique<xAOD::CaloClusterContainer>();
        caloclustersaux = std::make_unique<xAOD::CaloClusterAuxContainer>();
        caloclusters->setStore(caloclustersaux.get());
        output.clusterContainer = caloclusters.get();
    }

    SG::WriteHandle<xAOD::SlowMuonContainer> wh_slowmuon;
    /// Use the ConstDataVector pipe all MuonCandidate collections into a single vector
    ConstDataVector<MuonCandidateCollection> muon_candidates{SG::VIEW_ELEMENTS};

    if (m_buildSlowMuon) {
        wh_slowmuon = SG::WriteHandle<xAOD::SlowMuonContainer>(m_slowMuonCollectionName, ctx);
        ATH_CHECK(wh_slowmuon.record(std::make_unique<xAOD::SlowMuonContainer>(), std::make_unique<xAOD::SlowMuonAuxContainer>()));
        output.slowMuonContainer = wh_slowmuon.ptr();
    } else {
        for (SG::ReadHandle<MuonCandidateCollection>& muonCandidateRH : m_muonCandidateKeys.makeHandles(ctx)) {
            if (!muonCandidateRH.isValid()) {
                ATH_MSG_ERROR("Could not read " << muonCandidateRH.fullKey());
                return StatusCode::FAILURE;
            }
            muon_candidates.insert(muon_candidates.end(), muonCandidateRH->begin(), muonCandidateRH->end());
        }
    }
    m_muonCreatorTool->create(ctx, muon_candidates.asDataVector(), tagMaps, output);

    if (m_makeClusters) {
        SG::WriteHandle<CaloClusterCellLinkContainer> wh_clusterslink{m_clusterContainerLinkName, ctx};
        SG::WriteHandle<xAOD::CaloClusterContainer> wh_clusters{m_clusterContainerName, ctx};
        std::unique_ptr<CaloClusterCellLinkContainer> clusterlinks = std::make_unique<CaloClusterCellLinkContainer>();
        auto* sg = wh_clusters.storeHandle().get();
        for (xAOD::CaloCluster* cl : *output.clusterContainer) { cl->setLink(clusterlinks.get(), sg); }
        ATH_CHECK(wh_clusterslink.record(std::move(clusterlinks)));
        ATH_CHECK(wh_clusters.record(std::move(caloclusters), std::move(caloclustersaux)));
    }

    //---------------------------------------------------------------------------------------------------------------------//
    //------------                Monitoring of the reconstructed muons inside the trigger algs                ------------//
    //------------ Author:        Laurynas Mince                                                               ------------//
    //------------ Created:       26.07.2019                                                                   ------------//
    //---------------------------------------------------------------------------------------------------------------------//

    // Only run monitoring for online algorithms
    if (!m_monTool.name().empty()) {
        // Monitoring histograms and variables
        auto muon_n = Monitored::Scalar<int>("muon_n", wh_muons->size());
        auto muon_pt =
            Monitored::Collection("muon_pt", *(wh_muons.ptr()), [](auto const& mu) { return mu->pt() * MeVtoGeV; });  // converted to GeV
        auto muon_eta = Monitored::Collection("muon_eta", *(wh_muons.ptr()), &xAOD::Muon_v1::eta);
        auto muon_phi = Monitored::Collection("muon_phi", *(wh_muons.ptr()), &xAOD::Muon_v1::phi);

        auto satrks_n = Monitored::Scalar<int>("satrks_n", wh_extrtp->size());
        auto satrks_pt = Monitored::Collection("satrks_pt", *(wh_extrtp.ptr()),
                                               [](auto const& satrk) { return satrk->pt() * MeVtoGeV; });  // converted to GeV
        auto satrks_eta = Monitored::Collection("satrks_eta", *(wh_extrtp.ptr()), &xAOD::TrackParticle_v1::eta);
        auto satrks_phi = Monitored::Collection("satrks_phi", *(wh_extrtp.ptr()), &xAOD::TrackParticle_v1::phi);

        auto cbtrks_n = Monitored::Scalar<int>("cbtrks_n", wh_combtp->size());
        auto cbtrks_pt = Monitored::Collection("cbtrks_pt", *(wh_combtp.ptr()),
                                               [](auto const& cbtrk) { return cbtrk->pt() * MeVtoGeV; });  // converted to GeV
        auto cbtrks_eta = Monitored::Collection("cbtrks_eta", *(wh_combtp.ptr()), &xAOD::TrackParticle_v1::eta);
        auto cbtrks_phi = Monitored::Collection("cbtrks_phi", *(wh_combtp.ptr()), &xAOD::TrackParticle_v1::phi);

        if (!m_doSA) {
            SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection{m_indetCandidateCollectionName, ctx};

            auto idtrks_n = Monitored::Scalar<int>("idtrks_n", indetCandidateCollection->size());
            auto idtrks_pt = Monitored::Collection("idtrks_pt", *indetCandidateCollection,
                                                   [](auto const& idtrk) { return idtrk->indetTrackParticle().pt() * MeVtoGeV; });
            auto idtrks_eta = Monitored::Collection("idtrks_eta", *indetCandidateCollection,
                                                    [](auto const& idtrk) { return idtrk->indetTrackParticle().eta(); });
            auto idtrks_phi = Monitored::Collection("idtrks_phi", *indetCandidateCollection,
                                                    [](auto const& idtrk) { return idtrk->indetTrackParticle().phi(); });
            auto monitorIt = Monitored::Group(m_monTool, muon_n, muon_pt, muon_eta, muon_phi, satrks_n, satrks_pt, satrks_eta, satrks_phi,
                                              cbtrks_n, cbtrks_pt, cbtrks_eta, cbtrks_phi, idtrks_n, idtrks_pt, idtrks_eta, idtrks_phi);
        } else
            auto monitorIt = Monitored::Group(m_monTool, muon_n, muon_pt, muon_eta, muon_phi, satrks_n, satrks_pt, satrks_eta, satrks_phi,
                                              cbtrks_n, cbtrks_pt, cbtrks_eta, cbtrks_phi);
    }

    return StatusCode::SUCCESS;
}
