/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOCANDIDATEMATCHINGTOOL_H
#define MOOCANDIDATEMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
//
#include "CxxUtils/checker_macros.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MuPatCandidateTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTrackSegmentMatchingTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "TrkExInterfaces/IExtrapolator.h"
//
#include "MuonTrackSegmentMatchResult.h"
//
#include <array>
#include <atomic>
#include <set>
#include <string>

namespace Trk {
    class Track;
    class MeasurementBase;
}  // namespace Trk

namespace Muon {

    class MuonSegment;
    class MuPatTrack;
    class MuPatSegment;
    class MuPatCandidateBase;

    /** class to manipulate MuPatCandidateBase objects */
    class MooCandidateMatchingTool : virtual public IMuonTrackSegmentMatchingTool, public AthAlgTool {
    public:
        /** Class with matching return information */
        struct MooTrackSegmentMatchResult : public TrackSegmentMatchResult {
            const MuPatTrack* MCTBTrack{nullptr};
            const MuPatSegment* MCTBSegment{nullptr};

            MooTrackSegmentMatchResult();

            virtual ~MooTrackSegmentMatchResult() = default;

            virtual void clear();
        };
        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_MooCandidateMatchingTool("Muon::MooCandidateMatchingTool", 1, 0);
            return IID_MooCandidateMatchingTool;
        }

        /** default AlgTool constructor */
        MooCandidateMatchingTool(const std::string&, const std::string&, const IInterface*);

        /** destructor */
        virtual ~MooCandidateMatchingTool();

        /** initialize method, method taken from bass-class AlgTool */
        virtual StatusCode initialize() override;

        /** finialize method, method taken from bass-class AlgTool */
        virtual StatusCode finalize() override;

        /** @brief match two segment entries */
        bool match(const EventContext& ctx, const MuPatSegment& entry1, const MuPatSegment& entry2, bool useTightCuts ) const;

        /** @brief match a segment entry with a track entry */
        bool match(const EventContext& ctx, const MuPatTrack& entry1, const MuPatSegment& entry2, bool useTightCuts) const;

        /** @brief match a segment entry with a base entry */
        bool match(const EventContext& ctx, const MuPatCandidateBase& entry1, const MuPatSegment& entry2, bool useTightCuts) const;

        /** @brief match a track with a segment */        
        virtual bool match(const EventContext& ctx, const Trk::Track& track, const MuonSegment& segment, bool useTightCuts) const override;

        /** @brief calculate the info needed for the matching decision */
        void calculateTrackSegmentMatchResult(const EventContext& ctx, const MuPatTrack& entry1, const MuPatSegment& entry2, MooTrackSegmentMatchResult& info) const;

        TrackSegmentMatchCuts getMatchingCuts(const MuPatTrack& entry1, const MuPatSegment& entry2, bool useTightCuts) const;

        bool applyTrackSegmentCuts(const EventContext& ctx, MooTrackSegmentMatchResult& info, const TrackSegmentMatchCuts& cuts) const;

        /** @brief check whether two segments are on the same side of the point of
           closest approach to the perigee of the connecting track, or on the same
           side of the calorimeter (i.e. not crossing calo).

            @c requireSameSideOfPerigee : True  : require the segments to be on same
           side of perigee False : require the segments to be on same side of
           calorimeter
        */
        bool sameSide(const MuPatSegment& entry1, const MuPatSegment& entry2, bool requireSameSideOfPerigee) const;

        /** @brief Check whether the track and segment are on the same side of the
           point of closest approach to the perigee of the connecting track, or on the
           same side of the calorimeter (i.e. not crossing calo).

            @c requireSameSideOfPerigee : True  : require the segments to be on same
           side of perigee False : require the segments to be on same side of
           calorimeter
        */
        bool sameSide(const MuPatTrack& entry1, const MuPatSegment& entry2, bool requireSameSideOfPerigee) const;

        /** check whether the two tracks are on the same side of the point of closest
           approach to the perigee of the connecting track, or on the same side of the
           calorimeter (i.e. not crossing calo).

            @c requireSameSideOfPerigee : True  : require the segments to be on same
           side of perigee False : require the segments to be on same side of
           calorimeter
        */
        bool sameSide(const MuPatTrack& entry1, const MuPatTrack& entry2, bool requireSameSideOfPerigee) const;

        /** check whether two positions are on the same side of the point of closest
           approach to the perigee of the connecting track, or on the same side of the
           calorimeter (i.e. not crossing calo).

            @c requireSameSideOfPerigee : True  : require the segments to be on same
           side of perigee False : require the segments to be on same side of
           calorimeter
        */
        bool sameSide(const Amg::Vector3D& pos1, const Amg::Vector3D& pos2, bool requireSameSideOfPerigee) const;

        /** check whether two positions are on the same side of the point of closest
           approach to the perigee of the connecting track, or on the same side of the
           calorimeter (i.e. not crossing calo).

            @c requireSameSideOfPerigee : True  : require the segments to be on same
           side of perigee False : require the segments to be on same side of
           calorimeter
        */
        bool sameSide(const Amg::Vector3D& dir, const Amg::Vector3D& pos1, const Amg::Vector3D& pos2, bool requireSameSideOfPerigee) const;

        /** @brief extract Idenfitiers from a vector of measurements and copy them
         * into a set */
        void getIdentifierSet(const std::vector<const Trk::MeasurementBase*>& measurements, std::set<Identifier>& ids) const;

        /** @brief evaluate overlap between phi hits of two entries.
            returns true if the entries share all phi hits or the phi hits of one of
           the entries are subset of the other  */
        bool checkPhiHitConsistency(const MuPatCandidateBase& entry1, const MuPatCandidateBase& entry2) const;

        /** @brief evaluate distance between two segments, if too large return false
         * (cut at 3000.).  */
        bool checkSegmentDistance(const MuPatSegment& entry1, const MuPatSegment& entry2) const;

        /** @brief return whether the 2 segments are in neighbouring chambers */
        bool areInNeighbouringChambers(const MuPatSegment& seg1, const MuPatSegment& seg2) const;

    private:
        double m_minimumSideMatchRadius;
        double m_matchAngXCut;
        double m_matchAngYCut;
        double m_matchAngXPullCut;
        double m_matchAngYPullCut;
        double m_matchPosXCut;
        double m_matchPosYCut;
        double m_matchPosPullCut;
        double m_alignErrorPosX;
        double m_alignErrorPosY;
        double m_alignErrorAngleX;
        double m_alignErrorAngleY;
        double m_matchChiSquaredCut;
        double m_matchChiSquaredCutTight;

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                        "Handle to the service providing the IMuonEDMHelperSvc interface"};

        ToolHandle<MuonEDMPrinterTool> m_printer{this, "MuonPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
                                                 "tool to print EDM objects"};
        ToolHandle<Trk::IExtrapolator> m_atlasExtrapolator{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator",
                                                           "curved extrapolator"};
        ToolHandle<IMuonSegmentMatchingTool> m_segmentMatchingTool{this, "SegmentMatchingTool",
                                                                   "Muon::MuonSegmentMatchingTool/MuonSegmentMatchingTool"};
        ToolHandle<IMuonSegmentMatchingTool> m_segmentMatchingToolTight{this, "SegmentMatchingToolTight",
                                                                        "Muon::MuonSegmentMatchingTool/MuonSegmentMatchingToolTight"};
        ToolHandle<MuPatCandidateTool> m_candidateTool{this, "MuPatCandidateTool", "Muon::MuPatCandidateTool/MuPatCandidateTool"};

        // Read handle for conditions object to get the field cache
        SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                                  "Name of the Magnetic Field conditions object key"};

        Gaudi::Property<int> m_trackSegmentPreMatchingStrategy{this, "TrackSegmentPreMatching", 0,
                                                               "0=no segments match,1=any segment match,2=all segment match"};
        Gaudi::Property<bool> m_doTrackSegmentMatching{this, "DoTrackSegmentMatching", false, "Apply dedicated track-segment matching"};

        /** matching counters */
        mutable std::atomic_ulong m_goodSegmentMatches{0};
        mutable std::atomic_ulong m_goodSegmentMatchesTight{0};
        mutable std::atomic_ulong m_segmentMatches{0};
        mutable std::atomic_ulong m_segmentMatchesTight{0};
        mutable std::atomic_ulong m_goodSegmentTrackMatches{0};
        mutable std::atomic_ulong m_goodSegmentTrackMatchesTight{0};
        mutable std::atomic_ulong m_sameSideOfPerigee{0};
        mutable std::atomic_ulong m_otherSideOfPerigee{0};
        mutable std::atomic_ulong m_sameSideOfPerigeeTrk{0};
        mutable std::atomic_ulong m_otherSideOfPerigeeTrk{0};
        mutable std::atomic_ulong m_segmentTrackMatches{0};
        mutable std::atomic_ulong m_segmentTrackMatchesTight{0};
        mutable std::atomic_ulong m_reasonsForMatchOk[TrackSegmentMatchResult::NumberOfReasons] {0};
        mutable std::atomic_ulong m_reasonsForMatchNotOk[TrackSegmentMatchResult::NumberOfReasons] {0};

        double m_caloMatchZ;  //!< Z position of calo end-cap disks. Used to determine
                              //!< if segments are on same side of Calo

        bool m_requireSameSide;  //!< require entries to be on the same side of the
                                 //!< Perigee or Calorimeter
    };

}  // namespace Muon

#endif
