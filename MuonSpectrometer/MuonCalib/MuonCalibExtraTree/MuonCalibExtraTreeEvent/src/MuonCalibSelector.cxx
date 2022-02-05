/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibSelector.h"

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrackSummary.h"

namespace MuonCalib {

    MuonCalibSelector::MuonCalibSelector() = default;

    MuonCalibSelector::SegVec MuonCalibSelector::select(const MuonCalibSelector::SegVec& segments,
                                                        const MuonCalibSelector::Selection* selection) const {
        SegVec selectedSegments;

        // loop over segments and check whether they pass selection, if they do add to output vector
        for (const EventHandler::ExtendedSegmentPtr& it : segments) {
            if (select(*it, selection)) selectedSegments.emplace_back(it);
        }
        return selectedSegments;
    }

    bool MuonCalibSelector::select(const MuonCalibExtendedSegment& segment, const Selection* selection) const {
        // select selection
        const Selection& sel = selection ? *selection : m_selection;

        // perform tests
        const MuonCalibTrackSummary& summary = segment.summary();
        if (!select(summary, &sel)) { return false; }
        double chi2Ndof = segment.chi2();
        int ndof = 1;
        // hack! could go wrong if: fit included trigger hits or 4D CSC segments
        if (!segment.mdtHOT().empty()) ndof = segment.mdtHOT().size() - 2;
        if (!segment.cscHOT().empty()) ndof = segment.cscHOT().size() - 2;
        if (ndof != 0.) chi2Ndof /= ndof;
        if (chi2Ndof > sel.maxChi2Ndof) { return false; }
        const SegmentSelection* segmentSelection = dynamic_cast<const SegmentSelection*>(&sel);
        if (segmentSelection) {
            if (segmentSelection->hasFittedT0 && (!segment.hasFittedT0() || segment.fittedT0() == 0.)) { return false; }
            // if (segmentSelection->hasAssociatedTrack && segment.associatedTracks().empty()) { return false; }

            if (segmentSelection->singleML && summary.nmdtHitsPerMl(1) > 0 && summary.nmdtHitsPerMl(2) > 0) return false;

            if (segmentSelection->twoML && (summary.nmdtHitsPerMl(1) == 0 || summary.nmdtHitsPerMl(2) == 0)) return false;

            float adcGoodHitFraction = (summary.nmdtHits - summary.nmdtHitsBelowADCCut) / summary.nmdtHits;
            if (adcGoodHitFraction < segmentSelection->minAdcGoodFraction) return false;
        }

        return true;
    }

    bool MuonCalibSelector::select(const MuonCalibTrackSummary& summary, const MuonCalibSelector::Selection* selection) const {
        // select selection
        const Selection& sel = selection ? *selection : m_selection;

        if (summary.nmdtHits < sel.nminMdtHits) return false;

        if (summary.nphiTrigHits() < sel.nminPhiTrigHits) return false;

        if (summary.netaTrigHits() < sel.nminEtaTrigHits) return false;

        if (summary.noutliers > sel.nmaxOutliers) return false;

        if (summary.npseudo > sel.nmaxPseudo) return false;

        if (summary.nmdtHitsPerMl(1) < sel.nminMdtMl1) return false;
        if (summary.nmdtHitsPerMl(2) < sel.nminMdtMl2) return false;
        unsigned int nmdtCh = summary.nmdtCh();
        if (nmdtCh < sel.nminMdtCh) return false;
        if (nmdtCh > sel.nmaxMdtCh) return false;

        return true;
    }

    MuonCalibSelector::TrackVec MuonCalibSelector::select(const MuonCalibSelector::TrackVec& tracks,
                                                          const MuonCalibSelector::Selection* selection) const {
        TrackVec selectedTracks;

        // loop over segments and check whether they pass selection, if they do add to output vector
        for (const EventHandler::ExtendedTrackPtr& it : tracks) {
            if (select(*it, selection)) selectedTracks.emplace_back(it);
        }
        return selectedTracks;
    }

    bool MuonCalibSelector::select(const MuonCalibExtendedTrack& track, const Selection* selection) const {
        // select selection
        const Selection& sel = selection ? *selection : m_selection;

        // perform tests
        const MuonCalibTrackSummary& summary = track.summary();
        if (!select(summary, &sel)) return false;

        double chi2Ndof = track.chi2();
        int ndof = track.ndof();
        if (ndof != 0.) chi2Ndof /= ndof;
        if (chi2Ndof > sel.maxChi2Ndof) return false;

        const TrackSelection* trackSelection = dynamic_cast<const TrackSelection*>(&sel);
        if (trackSelection) {
            // select track with barrel hits
            if (trackSelection->barrelTrack) {
                if (!summary.hasBarrel) return false;

                // if not requiring endcap hits but track has endcap hits, reject
                if (!trackSelection->endcapTrackA && summary.hasEndcapA) return false;
                if (!trackSelection->endcapTrackC && summary.hasEndcapC) return false;
            }

            // select track with endcap hits
            if (trackSelection->endcapTrackA) {
                if (!summary.hasEndcapA) return false;

                // if not requiring barrel hits but track has barrel hits, reject
                if (!trackSelection->barrelTrack && summary.hasBarrel) return false;
                if (!trackSelection->endcapTrackC && summary.hasEndcapC) return false;
            }

            // select track with endcap hits
            if (trackSelection->endcapTrackC) {
                if (!summary.hasEndcapC) return false;

                // if not requiring barrel hits but track has barrel hits, reject
                if (!trackSelection->barrelTrack && summary.hasBarrel) return false;
                if (!trackSelection->endcapTrackA && summary.hasEndcapA) return false;
            }

            if (std::abs(track.d0()) > trackSelection->maxr0) return false;
            if (std::abs(track.z0ip()) > trackSelection->maxz0) return false;
        }

        return true;
    }

}  // namespace MuonCalib
