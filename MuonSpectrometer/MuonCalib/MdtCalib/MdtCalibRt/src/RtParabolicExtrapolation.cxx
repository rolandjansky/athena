/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibRt/RtParabolicExtrapolation.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/LegendrePolynomial.h"

using namespace MuonCalib;

RtParabolicExtrapolation::RtParabolicExtrapolation(void) {}

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::::::::::::::::
//:: METHOD getRtWithParabolicExtrapolation(.,.,.) ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::

RtRelationLookUp RtParabolicExtrapolation::getRtWithParabolicExtrapolation(const IRtRelation& in_rt, const double& r_min,
                                                                           const double& r_max) const {
    ///////////////
    // VARIABLES //
    ///////////////

    double t_min(t_from_r(r_min, in_rt));
    double t_max(t_from_r(r_max, in_rt));
    std::vector<SamplePoint> t_r(10);
    BaseFunctionFitter fitter(3);
    LegendrePolynomial pol;
    std::vector<double> rt_param(102);  // parameters of the output r-t relationship
    double r, t;                        // drift radius

    ////////////////////////
    // FILL SAMPLE POINTS //
    ////////////////////////

    double step_size((t_max - t_min) / static_cast<double>(t_r.size() - 1));
    for (unsigned int k = 0; k < t_r.size(); k++) {
        t_r[k].set_x1(t_min + k * step_size);
        t_r[k].set_x2(in_rt.radius(t_min + k * step_size));
        t_r[k].set_error(1.0);
    }
    fitter.fit_parameters(t_r, 1, t_r.size(), &pol);

    rt_param[0] = in_rt.tLower();
    rt_param[1] = (in_rt.tUpper() - in_rt.tLower()) / 99.0;
    for (unsigned int k = 0; k < 100; k++) {
        t = rt_param[0] + rt_param[1] * k;
        r = in_rt.radius(t);
        if (r < r_min) {
            rt_param[k + 2] = r;
        } else {
            rt_param[k + 2] = 0.0;
            for (unsigned int l = 0; l < 3; l++) { rt_param[k + 2] = rt_param[k + 2] + fitter.coefficients()[l] * pol.value(l, t); }
        }
    }

    return RtRelationLookUp(rt_param);
}

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: METHOD getRtWithParabolicExtrapolation(.,.,.,.,.) ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::

RtRelationLookUp RtParabolicExtrapolation::getRtWithParabolicExtrapolation(const IRtRelation& in_rt, const double& r_min,
                                                                           const double& r_max, const double& r_ext,
                                                                           const std::vector<SamplePoint>& add_fit_points) const {
    ///////////////
    // VARIABLES //
    ///////////////

    //    double t_min(t_from_r(r_min, in_rt));
    double t_min(get_max_t_at_r(r_min, in_rt));
    double t_max(t_from_r(r_max, in_rt));
    std::vector<SamplePoint> t_r(10);
    BaseFunctionFitter fitter(3);
    LegendrePolynomial pol;
    std::vector<double> rt_param(102);  // parameters of the output r-t relationship
    double r, t;                        // drift radius, drift time

    ////////////////////////
    // FILL SAMPLE POINTS //
    ////////////////////////

    double step_size((t_max - t_min) / static_cast<double>(t_r.size() - 1));

    // r-t-points in fit region
    for (unsigned int k = 0; k < t_r.size(); k++) {
        t_r[k].set_x1(t_min + k * step_size);
        t_r[k].set_x2(in_rt.radius(t_min + k * step_size));
        t_r[k].set_error(1.0);
    }

    // addtional points for the fit
    for (unsigned int k = 0; k < add_fit_points.size(); k++) { t_r.push_back(add_fit_points[k]); }

    // perform fit //
    fitter.fit_parameters(t_r, 1, t_r.size(), &pol);

    // bring r-t-points in the right format for RtRelationLookUp and fill non
    // modified and extrapolated points in the r-t
    rt_param[0] = in_rt.tLower();
    rt_param[1] = (in_rt.tUpper() - in_rt.tLower()) / 99.0;
    for (unsigned int k = 0; k < 100; k++) {
        t = rt_param[0] + rt_param[1] * k;
        r = in_rt.radius(t);

        // distinguish if extrapolation area is right or left from fit region
        if (r_ext < r_min) {
            if (r > r_min && t > t_min) {
                rt_param[k + 2] = r;  // fill original values
            } else {
                rt_param[k + 2] = 0.0;
                for (unsigned int l = 0; l < 3; l++) {  // fill extrapolated values
                    rt_param[k + 2] = rt_param[k + 2] + fitter.coefficients()[l] * pol.value(l, t);
                }
                if (rt_param[k + 2] < 0.0) { rt_param[k + 2] = 0.0; }
            }
        } else if (r_ext > r_max) {
            if (r < r_max) {
                rt_param[k + 2] = r;  // fill original values
            } else {
                rt_param[k + 2] = 0.0;
                for (unsigned int l = 0; l < 3; l++) {  // fill extrapolated values
                    rt_param[k + 2] = rt_param[k + 2] + fitter.coefficients()[l] * pol.value(l, t);
                }
            }
        }
        // fill only original values if the radius where we want to extrapolate to is
        // within the fit region
        else if (r_ext <= r_max && r_ext >= r_min) {
            rt_param[k + 2] = r;
            MsgStream log(Athena::getMessageSvc(), "RtParabolicExtrapolation");
            log << MSG::WARNING << "getRtWithParabolicExtrapolation() - Extrapolated radius withing fit region - Nothing to be done."
                << endmsg;
        }
    }

    return RtRelationLookUp(rt_param);
}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD t_from_r ::
//:::::::::::::::::::::

double RtParabolicExtrapolation::t_from_r(const double& r, const IRtRelation& in_rt) const {
    ///////////////
    // VARIABLES //
    ///////////////

    double precision(0.010);       // spatial precision of the inversion
    double t_max(in_rt.tUpper());  // upper time search limit
    double t_min(in_rt.tLower());  // lower time search limit

    /////////////////////////////////////////////
    // SEARCH FOR THE CORRESPONDING DRIFT TIME //
    /////////////////////////////////////////////

    while (t_max - t_min > 0.1 && std::abs(in_rt.radius(0.5 * (t_min + t_max)) - r) > precision) {
        if (in_rt.radius(0.5 * (t_min + t_max)) > r) {
            t_max = 0.5 * (t_min + t_max);
        } else {
            t_min = 0.5 * (t_min + t_max);
        }
    }

    return 0.5 * (t_min + t_max);
}

//*****************************************************************************

//:::::::::::::::::::::::::::
//:: METHOD get_max_t_at_r ::
//:::::::::::::::::::::::::::

double RtParabolicExtrapolation::get_max_t_at_r(const double& r, const IRtRelation& in_rt) const {
    for (double t = in_rt.tUpper(); t >= in_rt.tLower(); t = t - 1.0) {
        if (in_rt.radius(t) < r) { return t + 1.0; }
    }
    return in_rt.tLower();
}
