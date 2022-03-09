/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCandidateTool
//  AlgTool performing pre-selection on MS tracks, extrapolation and creation
//  of MuonCandidate collection.
//
//////////////////////////////////////////////////////////////////////////////

#include "MuonCandidateTool.h"

#include "TrkTrackSummary/MuonTrackSummary.h"

namespace {
    // Temporary collection for extrapolated tracks and links with correspondent MS tracks
    struct track_link {
        std::unique_ptr<Trk::Track> track;
        unsigned int container_index;
        bool extp_succeed;
        track_link(std::unique_ptr<Trk::Track> _trk, unsigned int _idx, bool _succeed) :
            track{std::move(_trk)}, container_index{_idx}, extp_succeed{_succeed} {}
    };
}

namespace MuonCombined {

    MuonCandidateTool::MuonCandidateTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonCandidateTool>(this);
    }

    StatusCode MuonCandidateTool::initialize() {
        ATH_CHECK(m_printer.retrieve());
        if (!m_trackBuilder.empty())
            ATH_CHECK(m_trackBuilder.retrieve());
        else
            m_trackBuilder.disable();
        if (!m_trackExtrapolationTool.empty())
            ATH_CHECK(m_trackExtrapolationTool.retrieve());
        else
            m_trackExtrapolationTool.disable();
        ATH_CHECK(m_ambiguityProcessor.retrieve());
        ATH_CHECK(m_trackSummaryTool.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_beamSpotKey.initialize());

        ATH_CHECK(m_segmentKey.initialize(!m_segmentKey.empty()));
        if (!m_segmentKey.empty()) {
            ATH_CHECK(m_trackSegmentAssociationTool.retrieve());
        }
        return StatusCode::SUCCESS;
    }
    void MuonCandidateTool::create(const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection,
                                   TrackCollection& outputTracks, const EventContext& ctx) const {
        ATH_MSG_DEBUG("Producing MuonCandidates for " << tracks.size());
        unsigned int ntracks = 0;

        SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{m_beamSpotKey, ctx};
        if (!beamSpotHandle.isValid()) {
            ATH_MSG_ERROR("Could not retrieve the BeamSpot data from key " << m_beamSpotKey.objKey());
            return;
        }
        float beamSpotX = beamSpotHandle->beamPos()[Amg::x];
        float beamSpotY = beamSpotHandle->beamPos()[Amg::y];
        float beamSpotZ = beamSpotHandle->beamPos()[Amg::z];

        ATH_MSG_DEBUG("Beamspot position bs_x=" << beamSpotX << ", bs_y=" << beamSpotY << ", bs_z=" << beamSpotZ);
        
        
        std::vector<track_link> trackLinks;

        unsigned int index = -1;
        // Loop over MS tracks
        for (const auto* track : tracks) {
            ++index;

            if (!track->trackLink().isValid() || !track->track()) {
                ATH_MSG_WARNING("MuonStandalone track particle without Trk::Track");
                continue;
            }
            const Trk::Track& msTrack = *track->track();

            ATH_MSG_VERBOSE("Re-Fitting track " << std::endl
                                                << m_printer->print(msTrack) << std::endl
                                                << m_printer->printStations(msTrack));
            std::unique_ptr<Trk::Track> standaloneTrack;
            if (m_extrapolationStrategy == 0u) {
                standaloneTrack = m_trackBuilder->standaloneFit(msTrack, ctx, nullptr, beamSpotX, beamSpotY, beamSpotZ);
            } else {
                standaloneTrack = m_trackExtrapolationTool->extrapolate(msTrack, ctx);
            }
            if (standaloneTrack) {
                // Reject the track if its fit quality is much (much much) worse than that of the non-extrapolated track
                if (standaloneTrack->fitQuality()->doubleNumberDoF() == 0) {
                    standaloneTrack.reset();
                    ATH_MSG_DEBUG("extrapolated track has no DOF, don't use it");
                } else {
                    double mschi2 = 2.5;  // a default we should hopefully never have to use (taken from CombinedMuonTrackBuilder)
                    if (msTrack.fitQuality()->doubleNumberDoF() > 0)
                        mschi2 = msTrack.fitQuality()->chiSquared() / msTrack.fitQuality()->doubleNumberDoF();
                    // choice of 1000 is slightly arbitrary, the point is that the fit should be really be terrible
                    if (standaloneTrack->fitQuality()->chiSquared() / standaloneTrack->fitQuality()->doubleNumberDoF() > 1000 * mschi2) {
                        standaloneTrack.reset();
                        ATH_MSG_DEBUG("extrapolated track has a degraded fit, don't use it");
                    }
                }
            }
            if (standaloneTrack) {
                standaloneTrack->info().setParticleHypothesis(Trk::muon);
                standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandAlone);
                ATH_MSG_VERBOSE("Extrapolated track " << std::endl
                                                      << m_printer->print(*standaloneTrack) << std::endl
                                                      << m_printer->printStations(*standaloneTrack));
                ++ntracks;
                if (!standaloneTrack->perigeeParameters())
                    ATH_MSG_WARNING(" Track without perigee " << (*standaloneTrack));
                else if (!standaloneTrack->perigeeParameters()->covariance())
                    ATH_MSG_WARNING(" Track with perigee without covariance " << (*standaloneTrack));
                trackLinks.emplace_back(std::move(standaloneTrack), index, true);
            } else {
                // We can create tracks from EM segments+TGC hits
                // If these are not successfully extrapolated, they are too low quality to be useful
                // So only make candidates from un-extrapolated tracks if they are not EM-only
                bool skipTrack = true;
                const Trk::MuonTrackSummary* msMuonTrackSummary = nullptr;
                std::unique_ptr<Trk::TrackSummary> msTrackSummary;
                // If reading from an ESD, the track will not have a track summary yet
                if (!msTrack.trackSummary()) {
                    msTrackSummary = m_trackSummaryTool->summary(msTrack, nullptr);
                    msMuonTrackSummary = msTrackSummary->muonTrackSummary();
                } else
                    msMuonTrackSummary = msTrack.trackSummary()->muonTrackSummary();
                for (const auto& chs : msMuonTrackSummary->chamberHitSummary()) {
                    if ((chs.isMdt() && m_idHelperSvc->stationIndex(chs.chamberId()) != Muon::MuonStationIndex::EM) ||
                        m_idHelperSvc->isCsc(chs.chamberId())) {
                        skipTrack = false;
                        break;
                    }
                }
                if (!skipTrack) { trackLinks.emplace_back(std::make_unique<Trk::Track>(msTrack), index, false); }
            }
        }
        ///
        std::unique_ptr<TrackCollection> extrapTracks = std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS);
        extrapTracks->reserve(trackLinks.size());
        for (const track_link& link : trackLinks) extrapTracks->push_back(link.track.get());
        ATH_MSG_DEBUG("Finished back-tracking, total number of successfull fits " << ntracks);

        // Resolve ambiguity between extrapolated tracks (where available)
        std::unique_ptr<const TrackCollection> resolvedTracks(m_ambiguityProcessor->process(extrapTracks.get()));

        ATH_MSG_DEBUG("Finished ambiguity solving: " << extrapTracks->size() << " track(s) in -> " << resolvedTracks->size()
                                                     << " track(s) out");

        const xAOD::MuonSegmentContainer* segments{nullptr};
        if (!m_segmentKey.empty()) {
            SG::ReadHandle<xAOD::MuonSegmentContainer> readHandle{m_segmentKey,ctx};
            if (!readHandle.isValid()) {
                ATH_MSG_WARNING("Failed to retrieve the segment container "<<m_segmentKey.fullKey());
            } else segments = readHandle.cptr();
        }

        // Loop over resolved tracks and build MuonCondidate collection
        for (const Trk::Track* track : *resolvedTracks) {
            std::vector<track_link>::iterator tLink =
                std::find_if(trackLinks.begin(), trackLinks.end(), [&track](const track_link& link) { return link.track.get() == track; });

            if (tLink == trackLinks.end()) {
                ATH_MSG_WARNING("Unable to find internal link between MS and SA tracks!");
                continue;
            }

            std::unique_ptr<MuonCandidate> muon_candidate;
            ElementLink<xAOD::TrackParticleContainer> MS_TrkLink{tracks, tLink->container_index, ctx};
            if (tLink->extp_succeed) {
                outputTracks.push_back(std::move(tLink->track));
                ElementLink<TrackCollection> saLink(outputTracks, outputTracks.size() - 1, ctx);
                muon_candidate =
                    std::make_unique<MuonCandidate>(MS_TrkLink, saLink, outputTracks.size() - 1);
                // remove track from set so it is not deleted
            } else {
                // in this case the extrapolation failed
                muon_candidate = std::make_unique<MuonCandidate>(MS_TrkLink);
            }
            muon_candidate->setCommissioning(m_commissioning);
            /// Last but not least set the segments
            if (segments) {
                std::vector<ElementLink<xAOD::MuonSegmentContainer>> assoc_segs;
                m_trackSegmentAssociationTool->associatedSegments(*muon_candidate->primaryTrack(), segments, assoc_segs);
                muon_candidate->setSegments(std::move(assoc_segs));
            }

            outputCollection.push_back(std::move(muon_candidate));
        }
    }

}  // namespace MuonCombined
