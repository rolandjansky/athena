/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 19.06.2006, AUTHOR: OLIVER KORTNER
// Modified: 16.01.2008 by O. Kortner, RtSpline allowed as input; faster, but
//                      less accurate implementation of correction funtion can
//                      be requested; bug fix in integral calculation.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "MdtCalibData/BFieldCorFunc.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/IRtRelation.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/LegendrePolynomial.h"
#include "cmath"

using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::
void BFieldCorFunc::init(const std::string &quality, const CalibFunc::ParVec &params, const IRtRelation *rt) {
    ////////////////
    // PARAMETERS //
    ////////////////
    m_quality = quality;
    m_param = params;

    ///////////////////////
    // CONSISTENCY CHECK //
    ///////////////////////
    if (m_param.size() != 2) {
        MsgStream log(Athena::getMessageSvc(), "BFieldCorFunc");
        log << MSG::ERROR << "Wrong number of parameters!" << endmsg;
        m_Legendre = nullptr;
        return;
    }

    ///////////////
    // VARIABLES //
    ///////////////
    unsigned int nb_points(31);    // number of sample points for the integral
                                   // in the correction function
    double step;                   // r step size
    double time;                   // auxiliary time variable
    BaseFunctionFitter fitter(6);  // 6 fit parameters for the integral by
                                   // default ("medium quality")
    LegendrePolynomial legendre;

    /////////////////////
    // QUALITY SETTING //
    /////////////////////
    if (m_quality == "high") {
        fitter.set_number_of_coefficients(8);
        nb_points = 31;
        m_step_size = 0.02;
    }
    if (m_quality == "medium") {
        fitter.set_number_of_coefficients(8);
        nb_points = 31;
        m_step_size = 0.06;
    }
    if (m_quality == "low") {
        fitter.set_number_of_coefficients(8);
        nb_points = 31;
        m_step_size = 0.12;
    }
    // sample points for the integral factor in the correction function
    std::vector<SamplePoint> sample_points(nb_points);

    ////////////////////////////////////////////////////////////
    // CALCULATE THE INTEGRAL PART OF THE CORRECTION FUNCTION //
    ////////////////////////////////////////////////////////////
    m_t_min = (rt)->tLower();
    m_t_max = (rt)->tUpper();

    // minimum and maximum radius //
    m_r_min = 0.025 * CLHEP::mm;    // minimum radius
    m_r_max = rt->radius(m_t_max);  // maximum radius
    if (m_r_max > 17.0 || m_r_max < m_r_min) {
        MsgStream log(Athena::getMessageSvc(), "BFieldCorFunc");
        log << MSG::INFO << "UNPHYSICAL MAXIMUM DRIFT RADIUS OF " << m_r_max << ", WILL BE SET TO 17.0!" << endmsg;
        m_r_max = 17.0;
    }
    step = ((m_r_max - m_r_min) / static_cast<double>(nb_points - 1));

    // set the sample points //
    double prev_r = 0;
    double prev_integral = 0;
    for (unsigned int k = 0; k < nb_points; k++) {
        time = t_from_r(m_r_min + k * step, rt);
        sample_points[k].set_x1(2 * (time - 0.5 * (m_t_min + m_t_max)) / (m_t_max - m_t_min));
        double new_r = rt->radius(time);
        double new_integral = 1.0e9 * integral(prev_r, new_r, rt) + prev_integral;
        sample_points[k].set_x2(new_integral);
        sample_points[k].set_error(1.0);
        prev_r = new_r;
        prev_integral = new_integral;
    }

    // perform the fit //
    if (fitter.fit_parameters(sample_points, 1, nb_points, &legendre)) {
        MsgStream log(Athena::getMessageSvc(), "BFieldCorFunc");
        log << MSG::WARNING << "Unable to fit the integral in the correction!" << endmsg;
        m_Legendre = nullptr;
        return;
    }
    m_alpha = fitter.coefficients();

    //////////////////////////////////////////////
    // SET A POINTER TO THE LEGENDRE POLYNOMIAL //
    //////////////////////////////////////////////
    m_Legendre = Legendre_polynomial::get_Legendre_polynomial();

    return;
}  // end BFieldCorFunc::init

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD t_from_r ::
//:::::::::::::::::::::
double BFieldCorFunc::t_from_r(const double &r, const IRtRelation *rt) const {
    ///////////////
    // VARIABLES //
    ///////////////
    double precision(0.010);  // spatial precision of the inversion
    double t_max(m_t_max);    // upper time search limit
    double t_min(m_t_min);    // lower time search limit
    double r_max(m_r_max);    // upper radius search limit
    double r_min(m_r_min);    // lower radius search limit
                              /////////////////////////////////////////////
                              // SEARCH FOR THE CORRESPONDING DRIFT TIME //
                              /////////////////////////////////////////////
    double t_guess, r_guess;

    do {
        t_guess = t_min + (t_max - t_min) / (r_max - r_min) * (r - r_min);
        r_guess = rt->radius(t_guess);
        if (r_guess > r) {
            r_max = r_guess;
            t_max = t_guess;
        } else {
            r_min = r_guess;
            t_min = t_guess;
        }
    } while (t_max - t_min > 0.1 && std::abs(r_guess - r) > precision);
    return t_guess;
}  // end BFieldCorFunc::t_from_r

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD integral ::
//:::::::::::::::::::::
double BFieldCorFunc::integral(const double &r_min, const double &r_max, const IRtRelation *rt) const {
    // catch fp exceptions//
    if (m_r_min < 1e-10 || r_min < m_r_min) return 0.0;

    ///////////////
    // VARIABLES //
    ///////////////
    double E0(m_param[0] / std::log(m_r_max / m_r_min));  // E(r)=E0/r
    double radius(r_max), rp(r_min);                      // auxiliary radius variables
    double integ(0.0);                                    // current value of the integral
                                                          // 	double step(0.010); // integration step size [mm]
    double step(m_step_size);                             // integration step size [mm]
    double time;                                          // drift time

    //////////////////////////////
    // r IN [m_r_min, m_r_max]? //
    //////////////////////////////
    if (r_max < r_min) { return 0.0; }
    if (r_max > m_r_max) { radius = m_r_max; }

    /////////////////
    // INTEGRATION //
    /////////////////
    double delta = step;
    while (rp < radius) {
        time = t_from_r(rp, rt);
        if (rp + step > radius) delta = radius - rp;
        integ = integ + 1.0e-3 * delta * std::pow(std::abs(rt->driftvelocity(time)) * 1.0e6, 1.0 - m_param[1]) /
                            std::pow(E0 / (rp * 1.0e-3), 2.0 - m_param[1]);
        rp = rp + step;
    }

    return integ;
}  // end BFieldCorFunc::integral

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD epsilon ::
//::::::::::::::::::::
double BFieldCorFunc::epsilon(void) const { return m_param[1]; }

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD setEpsilon ::
//:::::::::::::::::::::::
/*void BFieldCorFunc::setEpsilon(const double & eps) {
  m_param[1] = eps;
  init(m_quality, m_param);
  return;
}*/

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD setRtRelationship ::
//::::::::::::::::::::::::::::::
void BFieldCorFunc::setRtRelationship(const IRtRelation &rt) {
    init(m_quality, m_param, &rt);
    return;
}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD name ::
//:::::::::::::::::
std::string BFieldCorFunc::name() const { return std::string("BFieldCorFunc"); }

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD correction ::
//:::::::::::::::::::::::
double BFieldCorFunc::correction(double t, double B_wire, double B_mu) const {
    if (m_Legendre == nullptr) { return 0.0; }

    ///////////////
    // VARIABLES //
    ///////////////
    double B_perp(std::hypot(B_wire, B_mu));  // B orthogonal to the
                                              // electron drift path
    double B_factor(std::pow(B_perp, 2.0 - m_param[1]));
    double precision(0.1);                                             // precision of the correction in ns
    double t_max(t);                                                   // upper time search limit
    double t_min(t - 2 * correction_to_B(t, B_wire, B_mu, B_factor));  // lower time search limit
    if (t_min < m_t_min) t_min = m_t_min;
    double time(t);                           // auxiliary time variable
    double integ(0.0);                        // integral
    double tmean(0.5 * (m_t_min + m_t_max));  // mean time
    double tlength(m_t_max - m_t_min);        // length of drift-time interval

    //////////////////////
    // DRIFT TIME CHECK //
    //////////////////////
    if (t <= m_t_min) { return 0.0; }
    if (t > m_t_max) {
        t_max = m_t_max;
        time = m_t_max;
    }

    /////////////////////////////////////////
    // SEARCH FOR THE CORRECTED DRIFT TIME //
    /////////////////////////////////////////
    while (t_max - t_min > precision) {
        integ = 0.0;
        for (int k = 0; k < m_alpha.rows(); k++) {
            integ = integ + m_alpha[k] * m_Legendre->value(k, 2 * (0.5 * (t_min + t_max) - tmean) / tlength);
        }
        if (0.5 * (t_min + t_max) + B_factor * integ > time) {
            t_max = 0.5 * (t_min + t_max);
        } else {
            t_min = 0.5 * (t_min + t_max);
        }
    }

    return B_factor * integ;
}  // end BFieldCorFunc::correction

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD correction_to_B ::
//::::::::::::::::::::::::::::
double BFieldCorFunc::correction_to_B(double t, double B_wire, double B_mu, double B_factor) const {
    if (m_Legendre == nullptr) { return 0.0; }
    ///////////////
    // VARIABLES //
    ///////////////
    if (B_factor < 0) {
        double B_perp(std::hypot(B_wire, B_mu));
        // B orthogonal to the electron drift path
        B_factor = std::pow(B_perp, 2.0 - m_param[1]);
    }
    double time(t);
    double integ(0.0);                        // integral
    double tmean(0.5 * (m_t_min + m_t_max));  // mean time
    double tlength(m_t_max - m_t_min);        // length of drift-time interval

    //////////////////////
    // DRIFT TIME CHECK //
    //////////////////////
    if (t <= m_t_min) { return 0.0; }
    if (t > m_t_max) { time = m_t_max; }

    //////////////////////////////
    // CALCULATE THE CORRECTION //
    //////////////////////////////
    integ = 0.0;
    for (int k = 0; k < m_alpha.rows(); k++) { integ = integ + m_alpha[k] * m_Legendre->value(k, 2 * (time - tmean) / tlength); }

    return B_factor * integ;
}  // end BFieldCorFunc::correction_to_B
