/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RTRESOLUTION0_H
#define RTRESOLUTION0_H

#include "MdtCalibData/IRtResolution.h"

namespace MuonCalib {

    /** implementation of a resolution function */

    class RtResolution0 : public IRtResolution {
        explicit RtResolution0(const CalibFunc::ParVec& vec) : IRtResolution(vec){};
        virtual std::string name() const { return "RtResolution0"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double resolution(double t, double bgRate = 0.0) const { return 1.0; }
    };

}  // namespace MuonCalib

#endif
