/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTRANTUPLEANALYSIS_MUONCALIBSELECTOR_H
#define MUONCALIBEXTRANTUPLEANALYSIS_MUONCALIBSELECTOR_H

#include <float.h>

#include <vector>

#include "CxxUtils/checker_macros.h"
#include "MuonCalibExtraTreeEvent/EventHandler.h"
namespace MuonCalib {

    class MuonCalibExtendedSegment;
    class MuonCalibExtendedTrack;
    class MuonCalibTrackSummary;

    class MuonCalibSelector {
    public:
        using SegVec = EventHandler::SegmentVec;
        using TrackVec = EventHandler::TrackVec;

        struct Selection {
            Selection() = default;
            virtual ~Selection() = default;

            unsigned int nminMdtHits{3};        //!< minimum number of MDT hits
            unsigned int nminMdtMl1{0};         //!< minimum number of MDT hits in ml 1
            unsigned int nminMdtMl2{0};         //!< minimum number of MDT hits in ml 1
            unsigned int nminMdtCh{0};          //!< minimum number of chambers MDT
            unsigned int nmaxMdtCh{1000};       //!< maximum number of chambers MDT
            unsigned int nminPhiTrigHits{0};    //!< minimum number of phi trigger hits
            unsigned int nminEtaTrigHits{0};    //!< minimum number of eta trigger hits
            unsigned int nmaxOutliers{100000};  //!< maximum number of outliers
            unsigned int nmaxPseudo{1000};      //!< maximum number of pseudo-measurements
            double maxChi2Ndof{DBL_MAX};        //!< maximum chi2/ndof
        };

        struct SegmentSelection : public Selection {
            SegmentSelection() = default;

            bool hasFittedT0{false};
            bool hasAssociatedTrack{false};
            bool singleML{false};
            bool twoML{false};
            double minAdcGoodFraction{0.};
        };

        struct TrackSelection : public Selection {
            TrackSelection() = default;
            bool barrelTrack{false};
            bool endcapTrackA{false};
            bool endcapTrackC{false};
            double maxr0{DBL_MAX};  //!< maximum transverse impact parameter wrt I.P.
            double maxz0{DBL_MAX};  //!< maximum longitudinal impact parameter at I.P.
        };

    public:
        // default constructor
        MuonCalibSelector();

        /** select segments satisfying selection. If a selection is passed it will be used, else default */
        SegVec select(const SegVec& segments, const Selection* selection = nullptr) const;

        /** select tracks satisfying selection. If a selection is passed it will be used, else default */
        TrackVec select(const TrackVec& tracks, const Selection* selection = nullptr) const;

        /** check whether segment passes selection. If a selection is passed it will be used, else default */
        bool select(const MuonCalibExtendedSegment& segment, const Selection* selection = nullptr) const;

        /** check whether track passes selection. If a selection is passed it will be used, else default */
        bool select(const MuonCalibExtendedTrack& track, const Selection* selection = nullptr) const;

        /** check whether summary passes selection. If a selection is passed it will be used, else default */
        bool select(const MuonCalibTrackSummary& summary, const Selection* selection = nullptr) const;

    private:
        Selection m_selection{};
    };

}  // namespace MuonCalib
#endif
