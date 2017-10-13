/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 27.10.2007, AUTHOR: OLIVER KORTNER
// Modified: 26.11.2007 by O. Kortner, fix for segment refinement.
//           13.12.2007 by O. Kortner, time-out added.
//           07.08.2008 by O. Kortner, bug fix when hits are disabled.
//				 19.11.2008 by I. Potrap: 1) several bugs have been fixed;
//												  2) external fitter(DCLS) replaced by
//													  internal code of linear-least-chi2-fit
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef StraightPatRecH
#define StraightPatRecH

//::::::::::::::::::::::::::
//:: CLASS StraightPatRec ::
//::::::::::::::::::::::::::

/// \class StraightPatRec
/// This class searches for the best hit pattern to be fitted with the
/// DCSLFitter.
/// The user can set a time-out for the track finding. It is set to 10 seconds
/// by default.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 27.10.2007, 13.12.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// CLHEP //
//#include "CLHEP/config/CLHEP.h"
//#include "CLHEP/Units/SystemOfUnits.h"
//#include "CLHEP/Units/PhysicalConstants.h"
#include "GeoPrimitives/GeoPrimitives.h"

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibInterfaces/IMdtPatRecFitter.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/MTStraightLine.h"
#include "MdtCalibFitters/DCSLFitter.h"
//#include "MdtCalibFitters/FourCasesFitter.h"


namespace MuonCalib {

class StraightPatRec : public IMdtPatRecFitter {

public:
// Constructors //
	StraightPatRec(void) {
		init();
		}
	///< Default constructor: road width for pattern recognition = 0.5 mm.

	StraightPatRec(const double & r_road_width) {
		init(r_road_width);
		}
	///< Constructor: user-defined road width for pattern recognition.

// Methods //
// get-methods //
	double roadWidth(void) const;
	                                ///< get the road width used in the 
	                                ///< pattern recognition
	unsigned int numberOfTrackHits(void) const;
	                                ///< get the number of track hits in the
	                                ///< final track,
	                                ///< the method returns 0, if no track 
	                                ///< has been reconstructed
	const std::vector<const MdtCalibHitBase*> & trackHits(void) const;
	                                ///< get a vector with a pointer to the
	                                ///< hits used in the track
	                                ///< reconstruction
	double chi2(void) const;
	                                ///< get the chi^2 of the final track,
	                                ///< the method returns -1, if no track
	                                ///< has been reconstructed
	double chi2PerDegreesOfFreedom(void) const;
	                                ///< get the chi^2 per degrees of
	                                ///< freedom for the final track,
	                                ///< the method returns -1, if no track
	                                ///< has been reconstructed
	MTStraightLine track(void) const;
	                                ///< get the final track in the local
					///< co-ordinate frame, i.e.
					///<                      z 
					///<                      ^
					///<     o o o o o o      |
					///<  ... o o o o o ...   o--> y
					///<     o o o o o o     x

// set-method //
	void setRoadWidth(const double & r_road_width);
	                                ///< set the road width for the pattern
	                                ///< recognition = r_road_width
	void setTimeOut(const double & time_out);
					///< set the time-out for the track
					///< finding to time_out (in seconds)

	void setFixSelection(bool fix_sel);

// methods required by the base class "IMdtSegmentFitter" //
	bool fit(MuonCalibSegment & r_segment) const;
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
	bool fit(MuonCalibSegment & r_segment, HitSelection r_selection) const;
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

bool fitCallByReference(MuonCalibSegment & r_segment, HitSelection &r_selection) const;

	void printLevel(int level);
	                                ///< set the print level, this call
					///< has no effect

private:
// internal co-ordinate definition //
//	                    x3 
//	                    ^
//	   o o o o o o      |
//	... o o o o o ...   o--> x2
//	   o o o o o o     x1
//

// final track parameters //
	mutable std::vector<const MdtCalibHitBase*> m_track_hits;
	                                // pointer to hits used by the final
	                                // track
	mutable int m_nb_track_hits; // number of track hits in the final 
	                                // track
	mutable double m_chi2; // chi^2 of the final track
	double m_a_x1, m_b_x1; // slope and intercept in the x1-x3 plane
	double m_a_x2, m_b_x2; // slope and intercept in the x2-x3 plane
	double m_a_x1_err, m_b_x1_err; // errors on a_x1 and b_x1
	double m_a_x2_err, m_b_x2_err; // errors on a_x2 and b_x2
	mutable MTStraightLine m_track; // final track

	bool m_fix_selection;

// parameters for the adjustment of the track reconstruction //
	double m_r_max; // maximum radius
	double m_road_width; // road width for pattern recognition
	double m_time_out; // time-out for track finding

// chi^2 refitter //
//	DCSLFitter m_nfitter; // NIKHEF straight line reconstruction
//	FourCasesFitter m_nfitter;

// initialization methods //
	void init(void);
	                        // default initialization:  road width = 0.5 CLHEP::mm
	void init(const double & r_road_width);
	                        // initialization with user-defined road width

// auxiliary methods //
	MTStraightLine tangent(const Amg::Vector3D & r_w1,
	        const double & r_r1, const double & r_sigma12,
	        const Amg::Vector3D & r_w2, const double & r_r2,
	        const double & r_sigma22, const int & r_case) const;
	      // method for the calculation of tangents with errors;
	      // r_w1: wire position for the first hit,
	      // r_r1: drift radius of the first hit,
	      // r_sigma12: sigma(r_r1)^2,
	      // r_w2: wire position for the second hit,
	      // r_r2: drift radius of the second hit,
	      // r_sigma22: sigma(r_r2)^2,
	      // r_case = 1, 2, 3, 4: select one of the four cases of a tangent
	MTStraightLine fitCandidate(MuonCalibSegment & r_segment,
				const std::vector<unsigned int> & r_selection,
				const MTStraightLine & cand_line) const;
//				const std::vector<unsigned int> & index) const;
		// refit the candidate "cand_line" using the hits specified by
		// the r_selection and index vectors

};

}

#endif
