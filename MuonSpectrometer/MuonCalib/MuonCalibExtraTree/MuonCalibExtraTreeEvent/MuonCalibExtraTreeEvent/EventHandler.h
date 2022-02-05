/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibExtrTreeEvent_EventHandler_h
#define MuonCalibExtrTreeEvent_EventHandler_h

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "CxxUtils/checker_macros.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedRawHits.h"

namespace MuonCalib {

    class MuonCalibExtendedTrack;
    class MuonCalibExtendedSegment;

    class EventHandler {
    public:
        struct AuthorIndices {
            AuthorIndices(int s, int t) : segment{s}, track{t} {}
            int segment;
            int track;
        };

    public:
        using ExtendedTrackPtr = MuonCalibExtendedRawHits::ExtendedTrkPtr;
        using ExtendedSegmentPtr = MuonCalibExtendedRawHits::ExtendedSegPtr;

        using TruthPtr = MuonCalibTruthCollection::TruthPtr;
        using TruthMdtPtr = MuonCalibTruthCollection::TruthMdtPtr;
        using TruthRpcPtr = MuonCalibTruthCollection::TruthRpcPtr;
        using TruthTgcPtr = MuonCalibTruthCollection::TruthTgcPtr;
        using TruthCscPtr = MuonCalibTruthCollection::TruthCscPtr;

        using TrackVec = std::vector<ExtendedTrackPtr>;
        using AuthorTrackVecMap = std::map<int, TrackVec>;

        using SegmentVec = std::vector<ExtendedSegmentPtr>;
        using AuthorSegmentVecMap = std::map<int, SegmentVec>;
        using AuthorSegmentVecIt = AuthorSegmentVecMap::iterator;
        using AuthorSegmentVecCit = AuthorSegmentVecMap::const_iterator;

    public:
        /** default constructor */
        EventHandler();

        /** default destructor */
        ~EventHandler();

        /** clear all buffers */
        void clear();

        /** set a new event (will call clear on the previous) */
        void setEvent(std::shared_ptr<const MuonCalibEvent_E> event);

        /** get extended tracks */
        const TrackVec& extendedTracks(int author) const;

        /** get extended tracks */
        const SegmentVec& extendedSegments(int author) const;

        /** get extended raw hits */
        const MuonCalibExtendedRawHits& extendedRawHits() const;

        /** get event */
        const MuonCalibEvent_E& event() const;

        /** get author indices for Moore */
        const AuthorIndices& mooreIndices() const;

        /** get author indices for Muonboy */
        const AuthorIndices& muonboyIndices() const;

        /** dump routines */
        std::string dumpRawHits() const;
        std::string dumpTracks() const;
        std::string dumpTracks(const TrackVec& tracks) const;
        std::string dumpSegments(const SegmentVec& segments) const;
        std::string dumpSegments() const;
        std::string dumpEvent() const;
        void setdbg(const bool dbg_on);

    private:
        /** create extended tracks for event */
        void createExtendedTracks();

        /** create extended tracks for event */
        void createExtendedTruthTracks();

        /** create extended segments for event */
        void createExtendedSegments();

        /** create extend raw hits */
        void createExtendedRawHits();

        /** create association between tracks and segments */
        void associateSegmentsWithTracks(SegmentVec& segments, TrackVec& tracks);

        /** check whether segment and track share the same hits */
        int associateSegmentWithTrack(const MuonCalibExtendedSegment& segment, const MuonCalibExtendedTrack& track);

        /** check whether segments  share the same hits */
        int segmentOverlap(const MuonCalibExtendedSegment& segment1, const MuonCalibExtendedSegment& segment2);

        /** link tracks to the reference track using the hits on the track */
        void linkTracks(int referenceIndex);

        /** find best match of tracks with reference */
        void match(const MuonCalibExtendedTrack& reference, TrackVec& tracks);

        // data members
        std::shared_ptr<const MuonCalibEvent_E> m_event;
        AuthorTrackVecMap m_extendedTracks;
        AuthorSegmentVecMap m_extendedSegments;
        MuonCalibExtendedRawHits m_extendedRawHits;

        // author indices of segment and tracks in ntuple
        AuthorIndices m_mooreIndices;
        AuthorIndices m_muonboyIndices;
        bool m_debug;

        /// Spltis the truth hit container into a map having the barcode of the particle as key
        template <typename T> void split_along_barcode(const std::vector<T>& container, std::map<int, std::set<T>>& barcode_map);

        std::shared_ptr<MuonCalibTrack_E> transform_to_trk(const TruthPtr& truth_part, const std::map<int, std::set<TruthMdtPtr>>& mdt_hits,
                                                           const std::map<int, std::set<TruthCscPtr>>& csc_hits,
                                                           const std::map<int, std::set<TruthRpcPtr>>& rpc_hits,
                                                           const std::map<int, std::set<TruthTgcPtr>>& tgc_hits);
    };

}  // namespace MuonCalib
#endif
