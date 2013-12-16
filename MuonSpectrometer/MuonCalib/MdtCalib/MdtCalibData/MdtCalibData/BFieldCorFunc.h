/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 19.07.2006, AUTHOR: OLIVER KORTNER
// Modified: 16.01.2008 by O. Kortner, RtSpline allowed as input; faster, but
//                      less accuracte implementation of correction funtion can
//                      be requested; bug fix in integral calculation.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_BFieldCorFuncH
#define MuonCalib_BFieldCorFuncH

//:::::::::::::::::::::::::
//:: CLASS BFieldCorFunc ::
//:::::::::::::::::::::::::

namespace MuonCalib {
/// \class BFieldCorFunc
/// This class allows the user to get the difference between the drift time 
/// measured by a tube operated in a magnetic field $\vec{B}$ and the drift time
/// which would be measured by this tube if $\vec{B}$ vanished.
///
/// Correction:
///
/// $t(r,\vec{B}) = t(r,\vec{B}=0) + B_\perp^{2-\epsilon}\cdot
///                                  \int\limits_{25\ \mu m}^{r}
///					\frac{v_{B=0}^{1-\epsilon}(r')}
///						{E^{2-\epsilon}(r')}\,dr'$.
///
/// $B_\perp = |\vec{B}_\perp|$; $\vec{B}_\perp = \vec{B}_{wire}+\vec{B}_\mu$;
///
/// $\vec{B}_{wire}$: magnetic field parallel to the anode wire of the given
///                   tube;
///
/// $\vec{B}_\mu$: magnetic field magnetic field perpendicular to wire and
///                parallel to the muon trajectory in the given tube.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 19.07.2006
}

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>
#include <string>

// CLHEP //
#include "EventPrimitives/EventPrimitives.h"

// MuonCalib //
#include "MdtCalibData/IMdtBFieldCorFunc.h"
#include "MdtCalibData/IRtRelation.h"
#include "MuonCalibMath/Legendre_polynomial.h"

namespace MuonCalib {

class IRtRelation;


class BFieldCorFunc : public IMdtBFieldCorFunc {

public:
// Constructors //
	explicit BFieldCorFunc(const std::string & quality,
		const CalibFunc::ParVec & parameters, const IRtRelation *rt) : 
					IMdtBFieldCorFunc(parameters) {
		init(quality, parameters, rt);
		}
	///< Constructor:
	///< quality = "high", slow but accurate initialization initialization
	///< of the correction function,
	///< quality = "medium", compromise between speed and accuracy of the
	///< initialization of the correction function (default),
	///< quality = "low", fast initialization of the correction function
	///< at the price of lower quality.
	///< parameters[0] = high voltage [V],
	///< parameters[1] = $\epsilon$,
	explicit BFieldCorFunc(const CalibFunc::ParVec & parameters, const IRtRelation *rt) : 
					IMdtBFieldCorFunc(parameters) {
		init(std::string("medium"), parameters, rt);
		}
	///< Constructor:
	///< parameters[0] = high voltage [V],
	///< parameters[1] = $\epsilon$,

// Methods //
// get-methods //
	double epsilon(void) const;
	                                ///< get the $\epsilon$ parameter of the
	                                ///< B-field correction function

// set-methods //
//	void setEpsilon(const double & eps);
	                                ///< set the $\epsilon$ parameter of the
	                                ///< B-field correction function = eps
	void setRtRelationship(const IRtRelation & rt);
	                                ///< set the r-t relationship used to
	                                ///< calculate the B field correction
	                                ///< to the measured drift time = rt

// methods required by the base class "IMdtBFieldCorFunc" //
	std::string name() const;
	                                ///< get the class name
	double correction(double t, double B_wire, double B_mu) const;
	                                ///< get t(r,\vec{B}!=0)-t(r,\vec{B}=0);
	                                ///< t = measured drift time t [ns];
	                                ///< B_wire = magnetic field parallel
	                                ///<          to the anode wire of the
	                                ///<          given tube,
	                                ///< B_mu = magnetic field orthogonal
	                                ///<        to the wire and  parallel
	                                ///<        to the muon trajectory in
	                                ///<        the given tube
					///< [B] = Tesla
	double correction_to_B(double t, double B_wire, double B_mu, double B_factor=-1.0) const;
	                                ///< get t(r,\vec{B}!=0)-t(r,\vec{B}=0);
	                                ///< t = drift time t [ns] for B=0;
	                                ///< B_wire = magnetic field parallel
	                                ///<          to the anode wire of the
	                                ///<          given tube,
	                                ///< B_mu = magnetic field orthogonal
	                                ///<        to the wire and  parallel
	                                ///<        to the muon trajectory in
	                                ///<        the given tube
					///< [B] = Tesla

private:
// data //
	std::vector<double> m_param; // parameters (copy of m_parameter in the
	                             // base class, needed to enable overwriting)

// quality setting //
	std::string m_quality; // quality string ("high", "medium", "low")
	double m_step_size; // integration step size steering the quality

// correction function //
        Amg::VectorX m_alpha; // parameter of the fit function describing the
	                   // integral part of the correction function
	Legendre_polynomial *m_Legendre; // pointer to the Legendre polynomial
	                                 // singleton (chosen in order to limit
					 // the amount of memory used)

// information about r(t) used to calculate the correction function //
	double m_t_min, m_t_max; // minimum and maximum drift times allowed by
	                         // the r-t relationship
	double m_r_min, m_r_max; // minimum and maximum drift radius

// private methods //
	void init(const std::string & quality, const CalibFunc::ParVec & params, const IRtRelation *rt);
	                        // initialization method,
				// parameters are the parameters passed to
				// constructor of this class
	double t_from_r(const double & r, const IRtRelation *rt) const;
	                        // get t(r) for the relationship "rt",
				// the method is auxiliary and not optimized;
				// it will disappear when the t(r) will be
				// available in the MuonCalib framework
	double integral(const double &r_min, const double & r_max, const IRtRelation *rt) const;
				// get the integral in the correction function
	                        // from r_min to r_max; rt contains the drift
	                        // velocity needed to calculate the integral

};

}

#endif
