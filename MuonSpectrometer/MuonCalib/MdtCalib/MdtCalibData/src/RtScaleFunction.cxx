/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/RtScaleFunction.h"

#include "MdtCalibData/IRtRelation.h"

namespace MuonCalib {

    static const double RtScaleFunction_p[] = {-0.00102327, 0.00582117, -0.00676815, 0.00167504, -8.67371e-05, 1.66509e-06};

    float RtScalePolynomial(const float &r) {
        // calculate polynomial
        float p = 0;
        for (int i = 5; i >= 0; i--) { p = RtScaleFunction_p[i] + r * p; }
        return p;
    }

    float RtScaleFunction(const float &t, const bool ml2, const IRtRelation &rt) {
        if (t < rt.tLower() || !rt.HasTmaxDiff()) return 0.0;
        // apply linear scale and calculate r as input to the polinomial
        float rt_length = rt.tUpper() - rt.tLower();
        float corr_val = (t / rt_length) * rt.GetTmaxDiff() * (ml2 ? -0.5 : +0.5);
        float newt = t - corr_val;
        // keep linear scale for t>t_max
        if (newt > rt.tUpper()) return corr_val;

        // calculate polynomial
        float p = 0;
        float r = rt.radius(newt);
        for (int i = 5; i >= 0; i--) { p = RtScaleFunction_p[i] + r * p; }
        return p * rt.GetTmaxDiff() * (ml2 ? -0.5 : +0.5);
    }

}  // namespace MuonCalib
