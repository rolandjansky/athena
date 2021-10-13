/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MDTCALIBFITTERS_StraightPatRecH
#define MDTCALIBFITTERS_StraightPatRecH

/// \class StraightPatRec
/// This class searches for the best hit pattern to be fitted with the
/// DCSLFitter.
/// The user can set a time-out for the track finding. It is set to 10 seconds
/// by default.

#include <vector>

#include "CxxUtils/checker_macros.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MdtCalibFitters/DCSLFitter.h"
#include "MdtCalibFitters/MTStraightLine.h"
#include "MdtCalibInterfaces/IMdtPatRecFitter.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

namespace MuonCalib {

    class StraightPatRec : public IMdtPatRecFitter {
    public:
        // Constructors //
        StraightPatRec() { init(); }
        ///< Default constructor: road width for pattern recognition = 0.5 mm.

        StraightPatRec(const double &r_road_width) { init(r_road_width); }
        ///< Constructor: user-defined road width for pattern recognition.

        // Methods //
        // get-methods //
        double roadWidth() const;
        ///< get the final track in the local
        ///< co-ordinate frame, i.e.
        ///<                      z
        ///<                      ^
        ///<     o o o o o o      |
        ///<  ... o o o o o ...   o--> y
        ///<     o o o o o o     x

        // set-method //
        void setRoadWidth(const double &r_road_width);
        ///< set the road width for the pattern
        ///< recognition = r_road_width
        void setTimeOut(const double &time_out);
        ///< set the time-out for the track
        ///< finding to time_out (in seconds)

        void setFixSelection(bool fix_sel);

        // methods required by the base class "IMdtSegmentFitter" //
        bool fit(MuonCalibSegment &r_segment) const;
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
        ///< warning : the errors of the track
        ///< CLHEP::radii are only approximate
        bool fit(MuonCalibSegment &r_segment, HitSelection r_selection) const;
        /// the CurvedPatRec class relies on the first place on information cached in the MSStraightLine
        bool fit(MuonCalibSegment &r_segment, HitSelection r_selection, MTStraightLine &line_track) const;

        void printLevel(int) {}

        bool fitCallByReference(MuonCalibSegment &r_segment, HitSelection &r_selection, MTStraightLine &line_track) const;
        bool fitCallByReference(MuonCalibSegment &r_segment, HitSelection &r_selection) const;

    private:
        // internal co-ordinate definition //
        //	                    x3
        //	                    ^
        //	   o o o o o o      |
        //	... o o o o o ...   o--> x2
        //	   o o o o o o     x1
        //

        bool m_fix_selection;

        // parameters for the adjustment of the track reconstruction //
        double m_r_max;       // maximum radius
        double m_road_width;  // road width for pattern recognition
        double m_time_out;    // time-out for track finding

        // initialization methods //
        void init();
        // default initialization:  road width = 0.5 CLHEP::mm
        void init(const double &r_road_width);
        // initialization with user-defined road width

        // auxiliary methods //
        MTStraightLine tangent(const Amg::Vector3D &r_w1, const double &r_r1, const double &r_sigma12, const Amg::Vector3D &r_w2,
                               const double &r_r2, const double &r_sigma22, const int &r_case) const;
        // method for the calculation of tangents with errors;
        // r_w1: wire position for the first hit,
        // r_r1: drift radius of the first hit,
        // r_sigma12: sigma(r_r1)^2,
        // r_w2: wire position for the second hit,
        // r_r2: drift radius of the second hit,
        // r_sigma22: sigma(r_r2)^2,
        // r_case = 1, 2, 3, 4: select one of the four cases of a tangent
        MTStraightLine fitCandidate(MuonCalibSegment &r_segment, const std::vector<unsigned int> &r_selection,
                                    const MTStraightLine &cand_line) const;

        // refit the candidate "cand_line" using the hits specified by
        // the r_selection and index vectors
    };

}  // namespace MuonCalib

#endif
