/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTTRACKBUILDER_H
#define MUON_IMUONSEGMENTTRACKBUILDER_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
    class Track;
    class PrepRawData;
}  // namespace Trk

namespace Muon {
    class MuonSegment;
}

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonSegmentTrackBuilder : virtual public IAlgTool {
    public:
        typedef std::vector<const Trk::PrepRawData*> PrepVec;
        typedef PrepVec::iterator PrepIt;
        typedef PrepVec::const_iterator PrepCit;

    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentTrackBuilder("Muon::IMuonSegmentTrackBuilder", 1, 0);
            return IID_IMuonSegmentTrackBuilder;
        }

        /** @brief refit track
            @param track the track
            @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
        */
        virtual std::unique_ptr<Trk::Track> refit(const EventContext& ctx, Trk::Track& track) const = 0;

        /** recalibrate hits on track, does not refit
            @param track the track
            @param doMdts flag to indicate whether to recalibration the MDTs
            @param doCompetingClusters flag to indicate whether to redo competing ROTs
            @return a pointer to the resulting track, will return zero if calibration failed. Ownership passed to user.
        */
        virtual std::unique_ptr<Trk::Track> recalibrateHitsOnTrack(const EventContext& ctx, const Trk::Track& track, bool doMdts,
                                                                   bool doCompetingClusters) const = 0;

        /** @brief combine two segments to a super segment
            @param seg1 the first segment
            @param seg2 the second segment
            @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
            @return a pointer to the combined segment, will return zero if combination failed. Ownership passed to user.
        */
        virtual MuonSegment* combineToSegment(const EventContext& ctx, const MuonSegment& seg1, const MuonSegment& seg2,
                                              const PrepVec* patternPhiHits) const = 0;

        /** @brief combine two segments to a track
            @param seg1 the first segment
            @param seg2 the second segment
            @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
            @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
        */
        virtual std::unique_ptr<Trk::Track> combine(const EventContext& ctx, const MuonSegment& seg1, const MuonSegment& seg2,
                                                    const PrepVec* patternPhiHits) const = 0;

        /** @brief combine a track with a segment
            @param track a track
            @param seg a segment
            @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
            @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
        */
        virtual std::unique_ptr<Trk::Track> combine(const EventContext& ctx, const Trk::Track& track, const MuonSegment& seg,
                                                    const PrepVec* patternPhiHits) const = 0;

        /** @brief find closest TrackParameters to the position. Closest is defined as closest in z in the endcap and
             closest in r in the barrel.
             @param track a reference to a Track
             @param pos a reference to a GlobalPosition
             @return a pointer to TrackParameters, the ownership of the parameters is passed to the client calling the tool.
          */
        virtual Trk::TrackParameters* findClosestParameters(const Trk::Track& track, const Amg::Vector3D& pos) const = 0;

        /** @brief find closest TrackParameters to the surface. The distance is calculated along the track
            @param track a reference to a Track
            @param pos a reference to a Surface
            @return a pointer to TrackParameters, the ownership of the parameters is passed to the client calling the tool.
         */
        virtual Trk::TrackParameters* getClosestParameters(const Trk::Track& track, const Trk::Surface& surf) const = 0;

        virtual ~IMuonSegmentTrackBuilder() = default;
    };

}  // namespace Muon

#endif
