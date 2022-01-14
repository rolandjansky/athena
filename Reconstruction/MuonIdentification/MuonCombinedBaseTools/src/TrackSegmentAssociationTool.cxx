/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackSegmentAssociationTool.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"

namespace {
    using SegLink_t = ElementLink<xAOD::MuonSegmentContainer>;
    
    struct MatchResult {
        SegLink_t link;
        Muon::MuonSegmentKey key;
        unsigned int numberOfMatchedMeasurements;
        bool isSelected{true};
        MatchResult(const SegLink_t& link_, const Muon::MuonSegmentKey& key_,
                    unsigned int numberOfMatchedMeasurements_) :
            link(link_), key(key_), numberOfMatchedMeasurements(numberOfMatchedMeasurements_) {}
    };
}

namespace MuonCombined {
    
    TrackSegmentAssociationTool::TrackSegmentAssociationTool(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t, n, p) {
        declareInterface<IMuonTrackToSegmentAssociationTool>(this);
    }

    StatusCode TrackSegmentAssociationTool::initialize() {
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_printer.retrieve());
        return StatusCode::SUCCESS;
    }

    /** Returns a list of segments that match with the input Muon. */
    bool TrackSegmentAssociationTool::associatedSegments(const xAOD::Muon& muon, const xAOD::MuonSegmentContainer* segments,
                                                         std::vector<SegLink_t >& assoc_segments) const {

        /// only fill if the primary track particle is not equal to the ID track particle and
        /// it has an associated track with track states
        const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
        if (!tp || !tp->track() || !tp->track()->measurementsOnTrack() || tp == muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle))
            return false;

        return associatedSegments(*tp->track(), segments, assoc_segments);
    }

    
    bool TrackSegmentAssociationTool::associatedSegments(const Trk::Track& track, const xAOD::MuonSegmentContainer* segments,
                                std::vector<SegLink_t >& assoc_segments) const  {
        if (!segments) {
            ATH_MSG_DEBUG("no segment container passed, returning");
            return false;
        }
        if (segments->empty()) {
            ATH_MSG_DEBUG("no segments in container, returning");
            return false;
        }
        std::vector<MatchResult> matched_segs;
        ATH_MSG_DEBUG("track " << m_printer->print(track) << std::endl << m_printer->printStations(track));

        Muon::MuonSegmentKey trackKeys(track.measurementsOnTrack()->stdcont());

        Muon::CompareMuonSegmentKeys compareKeys{};

        for (unsigned int i = 0; i < segments->size(); ++i) {
            const xAOD::MuonSegment* curr_seg = (*segments)[i];
            if (!curr_seg) continue;

            // check element link to Muon::MuonSegment
            ElementLink<Trk::SegmentCollection> muonSegmentLink = curr_seg->muonSegment();
            if (!muonSegmentLink.isValid() || !*muonSegmentLink) {
                ATH_MSG_DEBUG("Segment without valid link");
                continue;
            }
            const Muon::MuonSegment* muonSegment = dynamic_cast<const Muon::MuonSegment*>(*muonSegmentLink);
            if (!muonSegment) continue;
            Muon::MuonSegmentKey segmentKeys(*muonSegment);
            Muon::CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(trackKeys, segmentKeys, true);
            if (overlapResult == Muon::CompareMuonSegmentKeys::NoOverlap) continue;
            MatchResult matchResult(SegLink_t(*segments, i), segmentKeys, compareKeys.intersectionSize);
            matched_segs.push_back(matchResult);
            ATH_MSG_DEBUG("numberOfMatchedMeasurements = " << matchResult.numberOfMatchedMeasurements);
        }
        ATH_MSG_DEBUG("Number of matched space-time points = " << matched_segs.size());

        // refined selection
        for (unsigned int i = 0; i < matched_segs.size(); i++) {
            
            MatchResult& result_i = matched_segs[i];
            if (!result_i.isSelected) continue;

            const xAOD::MuonSegment* seg_i = *result_i.link;
            int nMatched_i = result_i.numberOfMatchedMeasurements;
            float chi2PerDof_i = seg_i->chiSquared() / seg_i->numberDoF();

            for (unsigned int j = i + 1; j < matched_segs.size(); j++) {
                MatchResult& result_j = matched_segs[j];
                if (!result_j.isSelected) continue;

                const xAOD::MuonSegment* seg_j = *result_j.link;
                int nMatched_j = result_j.numberOfMatchedMeasurements;
                float chi2PerDof_j = seg_j->chiSquared() / seg_j->numberDoF();

                // In case the two segments have common hits:
                // 1) choose the one with higher number of matched hits to the track.
                // 2) if the numbers of matched hits are the same, choose the one with smaller chi2/DoF.
                Muon::CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(result_i.key, result_j.key, true);
                if (overlapResult == Muon::CompareMuonSegmentKeys::NoOverlap) continue;
                if (nMatched_j > nMatched_i) {
                    result_i.isSelected = false;
                } else if (nMatched_j < nMatched_i) {
                    result_j.isSelected = false;
                } else {
                    if (chi2PerDof_j < chi2PerDof_i)
                        result_i.isSelected = false;
                    else
                        result_j.isSelected = false;
                }

                ATH_MSG_VERBOSE("Segments " << i << " and " << j << " have " << compareKeys.intersectionSize << "hit(s) in common.");
                if (result_i.isSelected)
                    ATH_MSG_VERBOSE("Keeping segment " << i);
                else
                    ATH_MSG_VERBOSE("Keeping segment " << j);
            }
        }

        for (MatchResult& matches  : matched_segs) {
            if (matches.isSelected) { assoc_segments.push_back(std::move(matches.link)); }
        }
        std::sort(assoc_segments.begin(),assoc_segments.end(),[](const SegLink_t& a, const SegLink_t& b) ->bool{
            using chamIdx = Muon::MuonStationIndex::ChIndex;
            chamIdx ch_a = (*a)->chamberIndex();
            chamIdx ch_b = (*b)->chamberIndex();
            Muon::MuonStationIndex::StIndex st_a = Muon::MuonStationIndex::toStationIndex(ch_a);  
            Muon::MuonStationIndex::StIndex st_b = Muon::MuonStationIndex::toStationIndex(ch_b);
            if (st_a != st_b) return st_a < st_b;
            /// Sort the CSC segments at the first giving priority to the small sectors
            if (ch_a == chamIdx::CSL || ch_a == chamIdx::CSS || ch_b == chamIdx::CSS || ch_b == chamIdx::CSL) 
                return (ch_a == chamIdx::CSL) + 2*(ch_a == chamIdx::CSS) > (ch_b == chamIdx::CSL) + 2*(ch_b == chamIdx::CSS);
            return ch_a < ch_b;
        });
        if (msgLevel(MSG::DEBUG)){
            std::stringstream sstr;
            for (const SegLink_t& seg : assoc_segments){
                sstr<<Muon::MuonStationIndex::chName( (*seg)->chamberIndex())
                    <<"  ("<<Muon::MuonStationIndex::technologyName( (*seg)->technology())<<"), ";
            }
            ATH_MSG_DEBUG("Selected segments " << assoc_segments.size()<<" "<<sstr.str());  
        }
        return true;
    }

}  // namespace Muon
