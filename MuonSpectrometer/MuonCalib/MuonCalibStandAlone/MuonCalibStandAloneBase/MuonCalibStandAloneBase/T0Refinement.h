/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.06.2007, AUTHOR: OLIVER KORTNER
// Modified: 23.11.2007 by O. Kortner, fix for problems with CLHEP vectors on
//                                     some platforms.
//           18.07.2008 by O. Kortner, switch from the quasianalytic fitter
//                                     to StraightPatRec for better performance.
//           18.08.2008 by O. Kortner, curved segment fitting enabled; time-out
//                                     option added.
//           04.11.2008 by O. Kortner, road width adjustable by the user.
//				 19.11.2008 by I. Potrap,  non-limited number of steps,
//                                     only 3 last points are used for t0-fit.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_T0RefinementH
#define MuonCalib_T0RefinementH

//::::::::::::::::::::::::
//:: CLASS T0Refinement ::
//::::::::::::::::::::::::

/// \class T0Refinement
///
/// This class allows the user to get a refined t0 which improves the fit to
/// a given segment.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 23.06.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// MuonCalib //
#include "MdtCalibData/IRtRelation.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
//#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MdtCalibFitters/StraightPatRec.h"
#include "MdtCalibFitters/CurvedPatRec.h"


namespace MuonCalib {

class T0Refinement {

public:
// Constructors //
	T0Refinement(void);
	///< Default constructor.

	~T0Refinement(void) {}
	///< Destructor.

// Methods //
	double getDeltaT0(MuonCalibSegment * segment,
				const IRtRelation *rt, bool overwrite,
				double & error, bool & failed, bool curved = false);
					///< determine a t0 correction for the
					///< given segment; the algorithm
					///< choses the correction such that the
					///< chi^2 of the segment fit becomes
					///< maximum; the algorithm has to
					///< get a pointer to r-t relationship
					///< to be used for the fitting;
					///< if overwrite is set to true, the
					///< segment is refitted with the
					///< refined t0 and overwritten;
					///< in case of failure failed=true;
					///< error is the estimated error of
					///< the time correction,
					///< if curved is set to true, a curved
					///< fit is performed
	inline void SetDeltaT0(const double &dt0)
		{
		m_delta_t0=dt0;
		}
					///< Set the scan point distance
	void setTimeOut(const double & time_out);
					///< set the time-out for pattern finding
					///< to time_out (s)
    void setRoadWidth(const double & road_width);
                    ///< set the road with to road_width (mm) (default: 1mm)

private:
//	QuasianalyticLineReconstruction *m_qfitter; // straight-line fitter
	StraightPatRec *m_qfitter; // straight-line fitter
	CurvedPatRec *m_cfitter; // curved-segment fitter
	double m_delta_t0;
	double m_time_out; // time-out for pattern finding

};

}

#endif
