/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.03.2006, AUTHOR: OLIVER KORTNER
// Modified: 15.07.2006 by O. Kortner, error calculation corrected,
//                                     chi^2 refit functionality added.
//           13.01.2007 by O. Kortner, bug fix in candidate treatment, some
//                                     candidates were considered to be
//                                     identical although different;
//                                     modifications to improve the
//                                     reconstruction efficiency at very high
//                                     background count rates.
//           27.03.2007 by O. Kortner, distances with signs filled into
//                                     MuonCalibSegment.
//           23.03.2007 by O. Kortner, isnan check for chi^2.
//           08.06.2007 by O. Kortner, final track segment has rphi position and
//                                     direction of the initial segment.
//           23.06.2006 by O. Kortner, add convention for rphi track if the
//                                     pattern recognition has failed to
//                                     provide it.
//           26.11.2007 by O. Kortner, fix for segment refinement.
//           13.12.2007 by O. Kortner, time-out added.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QuasianalyticLineReconstructionH
#define QuasianalyticLineReconstructionH

//:::::::::::::::::::::::::::::::::::::::::::
//:: CLASS QuasianalyticLineReconstruction ::
//:::::::::::::::::::::::::::::::::::::::::::

/// \class QuasianalyticLineReconstruction
/// This class performs the reconstruction of straight line in MDT chambers by
/// calculating tangents to all pairs of two tubes on a straight-line track and
/// averaging over the calculated tangents to get the parameters of the
/// straight-line track. The class redoes the pattern recognition using the
/// hits provided to it. The hits which were used in the track fit can be
/// retrieved from the class. A refit of the result of the quasianalytic
/// reconstruction with the DCSLFitter can requested.
/// The user can set a time-out for the track finding. It is set to 10 seconds
/// by default.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 01.03.2006, 16.07.2006, 13.12.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// CLHEP //
//#include "CLHEP/config/CLHEP.h"

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibInterfaces/IMdtPatRecFitter.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/MTStraightLine.h"
#include "MdtCalibFitters/IndexSet.h"
#include "MdtCalibFitters/DCSLFitter.h"


namespace MuonCalib {

class QuasianalyticLineReconstruction : public IMdtPatRecFitter {

public:
// Constructors //
	QuasianalyticLineReconstruction(void) {
		init();
		}
	///< Default constructor: road width for pattern recognition = 0.5 mm.

	QuasianalyticLineReconstruction(const double & r_road_width) {
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
	                                ///< radii are only approximate
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
	mutable MTStraightLine m_track; // final track

// parameters for the adjustment of the track reconstruction //
	double m_r_max; // maximum radius
	double m_road_width; // road width for pattern recognition
	double m_time_out; // time-out for track finding

// chi^2 refitter //
	DCSLFitter m_nfitter; // NIKHEF straight line reconstruction

// internal storage vectors //
	std::vector<MTStraightLine> m_tangent;
	                                    // vector of tangents (for 1 track)
	std::vector<int> m_nb_cand_hits; // number of hits on track candidate
	std::vector<MTStraightLine> m_candidate; // track candidates

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

	MTStraightLine track_candidate(
				const IndexSet & r_index_set,
				const int & r_k_cand,
				const int & r_l_cand,
				const int & r_cand_case,
				std::vector<Amg::Vector3D> r_w,
				std::vector<double> r_r,
				std::vector<double> r_sigma2,
				double & r_chi2) const;
	 // method for the calculatation of a straight line from tangents;
	 // r_nb_selected_hits: number of selected hits,
	 // r_k_cand: index of the 1st wire of the candidate tangent
	 //           (starting from 0),
	 // r_l_cand: index of the 2nd wire of the candidate tangent
	 //           (starting from 0),
	 // r_cand_case: configuration of the tangent to be used (1, 2, 3, or 4)
	 // r_w: wire positions,
	 // r_r: drift radii,
	 // r_sigma2: sigma(r)^2,
	 // r_chi2: chi^2 for the reconstructed track

};

}

#endif
