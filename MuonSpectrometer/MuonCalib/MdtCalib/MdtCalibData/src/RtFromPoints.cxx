/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/RtFromPoints.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/RtSpline.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/ChebyshevPolynomial.h"
#include "MuonCalibMath/PolygonBase.h"

using namespace MuonCalib;

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD get_min_max ::
//::::::::::::::::::::::::
void RtFromPoints::get_min_max(const std::vector<SamplePoint> &sample_points, double &x_min, double &x_max) {
    for (unsigned int k = 0; k < sample_points.size(); k++) {
        if (k == 0) {
            x_min = sample_points[k].x1();
            x_max = x_min;
        }
        if (x_min > sample_points[k].x1()) { x_min = sample_points[k].x1(); }
        if (x_max < sample_points[k].x1()) { x_max = sample_points[k].x1(); }
    }

    return;
}

//*****************************************************************************

//:::::::::::::::::::::::::::
//:: METHOD getRtChebyshev ::
//:::::::::::::::::::::::::::
RtChebyshev RtFromPoints::getRtChebyshev(const std::vector<SamplePoint> &sample_points, const unsigned int &order) {
    ///////////////
    // VARIABLES //
    ///////////////
    std::vector<double> rt_param(order + 3);            // input parameters of RtChebyshev
    BaseFunctionFitter fitter(order + 1);               // Chebyshev fitter
    ChebyshevPolynomial chebyshev;                      // Chebyshev polynomial
    std::vector<SamplePoint> my_points(sample_points);  // copy of the sample to add reduced times

    ///////////////////////////////////////////////////////////////////
    // GET THE MINIMUM AND MAXIMUM TIMES AND CALCULATE REDUCED TIMES //
    ///////////////////////////////////////////////////////////////////
    get_min_max(sample_points, rt_param[0], rt_param[1]);

    double mean(0.5 * (rt_param[1] + rt_param[0]));
    double length(rt_param[1] - rt_param[0]);
    for (unsigned int k = 0; k < my_points.size(); k++) { my_points[k].set_x1(2 * (sample_points[k].x1() - mean) / length); }

    //////////////////////////////////////////////////
    // PERFORM A CHEBYSHEV FIT TO THE SAMPLE POINTS //
    //////////////////////////////////////////////////
    if (fitter.fit_parameters(my_points, 1, sample_points.size(), &chebyshev)) {
        MsgStream log(Athena::getMessageSvc(), "RtFromPoints");
        log << MSG::WARNING << "Class RtFromPoints, method getRtChebyshev: Could not determine Chebyshev coefficients." << endmsg;
    }
    for (unsigned int k = 0; k < order + 1; k++) { rt_param[k + 2] = fitter.coefficients()[k]; }

    //////////////////////////////////////////////////////////////
    // CREATE AN RtChebyshev OBJECT WITH THE CORRECT PARAMETERS //
    //////////////////////////////////////////////////////////////
    RtChebyshev rt_chebyshev(rt_param);
    return rt_chebyshev;
}  // end RtFromPoints::getRtChebyshev

//*****************************************************************************

//::::::::::::::::::::::::::::::::
//:: METHOD getRtRelationLookUp ::
//::::::::::::::::::::::::::::::::
RtRelationLookUp RtFromPoints::getRtRelationLookUp(const std::vector<SamplePoint> &sample_points) {
    // create spline rt relation
    CalibFunc ::ParVec pars(2 * sample_points.size());
    for (unsigned int i = 0; i < sample_points.size(); i++) {
        pars[2 * i] = sample_points[i].x1();
        pars[2 * i + 1] = sample_points[i].x2();
    }
    RtSpline rt(pars);

    // variables
    unsigned int nb_points(100);                                                         // number of (r, t) points
    double bin_width((rt.tUpper() - rt.tLower()) / static_cast<double>(nb_points - 1));  // step size
    std::vector<double> rt_param(nb_points + 2);                                         // r-t parameters

    ///////////////////////////////////////////////////////////////////
    // CREATE AN RtRelationLookUp OBJECT WITH THE CORRECT PARAMETERS //
    ///////////////////////////////////////////////////////////////////
    rt_param[0] = rt.tLower();
    rt_param[1] = bin_width;
    for (unsigned int k = 0; k < nb_points; k++) { rt_param[k + 2] = rt.radius(rt.tLower() + k * bin_width); }
    RtRelationLookUp rt_relation_look_up(rt_param);

    return rt_relation_look_up;
}
