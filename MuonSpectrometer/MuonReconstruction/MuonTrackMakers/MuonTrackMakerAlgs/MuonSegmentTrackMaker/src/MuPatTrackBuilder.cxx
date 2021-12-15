/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuPatTrackBuilder.h"

#include <vector>

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonSegment/MuonSegment.h"
#include "Particle/TrackParticleContainer.h"
#include "StoreGate/DataHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"

using namespace Muon;
MuPatTrackBuilder::MuPatTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator) {

    }
StatusCode MuPatTrackBuilder::initialize() {
    ATH_CHECK(m_trackMaker.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_MSG_DEBUG("Retrieved " << m_trackMaker);

    ATH_CHECK(m_segmentKey.initialize());
    ATH_CHECK(m_spectroTrackKey.initialize());

    if (!m_monTool.name().empty()) { ATH_CHECK(m_monTool.retrieve()); }

    return StatusCode::SUCCESS;
}

StatusCode MuPatTrackBuilder::execute(const EventContext& ctx) const {
    typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

    SG::ReadHandle<Trk::SegmentCollection> segmentColl(m_segmentKey, ctx);
    if (!segmentColl.isValid()) {
        ATH_MSG_WARNING("Could not find MuonSegmentCollection at " << segmentColl.name());
        return StatusCode::FAILURE;
    }

    if (!segmentColl.cptr()) {
        ATH_MSG_WARNING("Obtained zero pointer for MuonSegmentCollection at " << segmentColl.name());
        return StatusCode::FAILURE;
        ;
    }

    ATH_MSG_DEBUG("Retrieved MuonSegmentCollection " << segmentColl->size());

    MuonSegmentCollection msc;
    msc.reserve(segmentColl->size());
    for (const Trk::Segment* seg : *segmentColl) {
        if (!seg) continue;
        const Muon::MuonSegment* ms = dynamic_cast<const Muon::MuonSegment*>(seg);
        if (ms) msc.push_back(ms);
    }

    if (msc.size() != segmentColl->size()) {
        ATH_MSG_WARNING("Input segment collection (size " << segmentColl->size() << ") and translated MuonSegment collection (size "
                                                          << msc.size() << ") are not the same size.");
    }

    std::unique_ptr<TrackCollection> newtracks{m_trackMaker->find(ctx, msc)};
    if (!newtracks) newtracks = std::make_unique<TrackCollection>();
    const TrackCollection* track_raw_ptr = newtracks.get();
    SG::WriteHandle<TrackCollection> spectroTracks(m_spectroTrackKey, ctx);
    ATH_CHECK(spectroTracks.record(std::move(newtracks)));
    ATH_MSG_DEBUG("TrackCollection '" << m_spectroTrackKey.key() << "' recorded in storegate, ntracks: " << track_raw_ptr->size());

    //---------------------------------------------------------------------------------------------------------------------//
    //------------                Monitoring of muon segments and tracks inside the trigger algs               ------------//
    //------------ Author:  Laurynas Mince                                                                     ------------//
    //------------ Created: 03.10.2019                                                                         ------------//
    //---------------------------------------------------------------------------------------------------------------------//

    // Only run monitoring for online algorithms
    if (not m_monTool.name().empty()) {
        auto mstrks_n = Monitored::Scalar<int>("mstrks_n", track_raw_ptr->size());
        auto mstrks_pt = Monitored::Collection("mstrks_pt", *track_raw_ptr, [](auto const& mstrk) {
            return mstrk->perigeeParameters()->momentum().perp() / 1000.0;
        });  // pT converted to GeV
        auto mstrks_eta = Monitored::Collection("mstrks_eta", *track_raw_ptr, [](auto const& mstrk) {
            return -log(tan(mstrk->perigeeParameters()->parameters()[Trk::theta] * 0.5));
        });
        auto mstrks_phi = Monitored::Collection("mstrks_phi", *track_raw_ptr,
                                                [](auto const& mstrk) { return mstrk->perigeeParameters()->parameters()[Trk::phi0]; });
        auto mssegs_n = Monitored::Scalar<int>("mssegs_n", msc.size());
        auto mssegs_eta = Monitored::Collection("mssegs_eta", msc, [](auto const& seg) { return seg->globalPosition().eta(); });
        auto mssegs_phi = Monitored::Collection("mssegs_phi", msc, [](auto const& seg) { return seg->globalPosition().phi(); });

        auto monitorIt = Monitored::Group(m_monTool, mstrks_n, mstrks_pt, mstrks_eta, mstrks_phi, mssegs_n, mssegs_eta, mssegs_phi);
    }

    return StatusCode::SUCCESS;
}  // execute
