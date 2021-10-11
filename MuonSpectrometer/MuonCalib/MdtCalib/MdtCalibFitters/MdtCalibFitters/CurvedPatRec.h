/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_CurvedPatRecH
#define MuonCalib_CurvedPatRecH

//::::::::::::::::::::::::
//:: CLASS CurvedPatRec ::
//::::::::::::::::::::::::

/// \class CurvedPatRec
///
/// This class searches for the best hit combination for a parabolic curved
/// segment fit and fits a parabola in the precision plane to the hits found
/// before. The algorithm overwrites the segment position nor the
/// segment direction as well as the chi^2 in the latest versions.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 18.08.2008

#include <vector>

#include "CxxUtils/checker_macros.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MdtCalibFitters/CurvedLine.h"
#include "MdtCalibFitters/MTStraightLine.h"
#include "MdtCalibFitters/StraightPatRec.h"
#include "MdtCalibInterfaces/IMdtPatRecFitter.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

namespace MuonCalib {

    class CurvedPatRec : public IMdtPatRecFitter {
    public:
        using MdtHitVec = MuonCalibSegment::MdtHitVec;
        using MdtHitPtr = MuonCalibSegment::MdtHitPtr;

        // Constructors //
        CurvedPatRec();
        ///< Default constructor: road width of 0.5 mm is used.

        CurvedPatRec(const double& road_width);
        ///< Constructor: user defined road width [mm] for pattern recognition.

        // Methods //
        double roadWidth() const;
        ///< get the road width used in the
        ///< pattern recognition [mm]

        // set-method //
        void setRoadWidth(const double& r_road_width);
        ///< set the road width [mm] for the pattern
        ///< recognition = r_road_width
        void setTimeOut(const double& time_out);
        ///< set the time-out for the track
        ///< finding to time_out (in seconds)

        // methods required by the base class "IMdtSegmentFitter" //
        bool fit(MuonCalibSegment& r_segment) const;
        ///< reconstruction of the track using
        ///< all hits in the segment
        ///< "r_segment", returns true in case
        ///< of success;
        ///< the algorithm overwrites
        ///< the track radii, the track
        ///< position, track direction, and
        ///< chi^2 per degrees of freedom;
        ///< warning: the errors of the track
        ///< radii are only approximate
        bool fit(MuonCalibSegment& r_segment, HitSelection r_selection) const;
        ///< reconstruction of the track using
        ///< only those hits in r_segment for
        ///< which the r_selection[.] is 0,
        ///< return true in case of success;
        ///< the algorithm overwrites
        ///< the track position, direction,
        ///< and the chi^2 in r_segment; it
        ///< updates the distances of all hits
        ///< from the track, i.e. also of those
        ///< hits which were rejected from the
        ///< track reconstruction;
        ///< warning: the errors of the track
        ///< radii are only approximate
        bool fit(MuonCalibSegment& r_segment, HitSelection r_selection, CurvedLine& curved_track) const;
        void printLevel(int /*level*/){};

    private:
        // internal co-ordinate definition //
        //	                    x3
        //	                    ^
        //	   o o o o o o      |
        //	... o o o o o ...   o--> x2
        //	   o o o o o o     x1
        //

        // settings //
        double m_road_width{0.5};  // road width to be used in pattern finding
        double m_time_out{10};     // time out for pattern recognition.

        // straight pattern recognition to get the approximate incidence angle //

        // auxiliary methods //
        Amg::Vector3D getHitPoint(const MdtHitPtr& hit, const MTStraightLine& straight_track) const;
        // transform the given hit into a hit point in
        // 3 dimensions using the given straight line
        // as helper
        std::vector<Amg::Vector3D> getHitPoints(const MdtHitVec& track_hits, const MTStraightLine& straight_track) const;
        // transform the track hits into hit points in
        // 3 dimensions using the given straight line
        // as helper
        std::vector<Amg::Vector3D> getHitPoints(const MdtHitVec& track_hits, const CurvedLine& curved_track) const;
        // transform the track hits into hit points in
        // 3 dimensions using the given curved line
        // as helper
    };

}  // namespace MuonCalib

#endif
