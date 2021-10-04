/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  MDT Timeslew correction for AMT digitization chip.
  Timeslew correction parmeterized as function of ADC:  timeslew = 109/q(ADC) [ns]
   where q = integrated charge of MDT signal, derived from AMT parameters, see ATL-MUON-2002-003
   constant 109 determined by empirical fit of data, see
   https://indico.cern.ch/event/115774/contributions/69612/attachments/2260534/3836764/110912_rauscher_timeslewing.pdf

   Note: Timeslew correction should be updated for Run4 when AMT is replaced by a new MDT digitization chip
   Author:  Felix Rauscher (2011)
   Revision: Edward Diehl (2021)
     1. Move initialization of LUT from correction function to an initialize function for AthenaMT compatibility.
     2. Change to LUT from a map to vector which is more accurate and has lower access time.
*/

#include "MdtCalibData/MdtSlewCorFuncHardcoded.h"

#include <cmath>

namespace MuonCalib {

    double MdtSlewCorFuncHardcoded::correction(double /*t*/, double adc) const {
        // Timeslew correction is negligible (<0.05 ns) for ADC>400 so do not bother computing it.
        // In addition there are very few hits with ADC>400
        // Constant 109 is from an optimization of the timeslew correction
        // calibrated_p(i) is the integrated charge as a function of ADC
        if (adc > 400. || adc < 0.) return 0.;
        static const double A = 109. * std::exp(-1.11925e+00);
        constexpr double adc_chan_conversion = 25. / 32.;
        constexpr double Lambda = -2.08708e-02 * adc_chan_conversion;  // ADC to ns
        // Convert ADC to integrated charge for AMT chip, see ATL-MUON-2002-003
        return A * std::exp(adc * Lambda);
    }

}  // namespace MuonCalib
