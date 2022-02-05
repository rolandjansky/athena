/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IRTRESOLUTION_H
#define MUONCALIB_IRTRESOLUTION_H

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    /** generic interface for a resolution function */

    class IRtResolution : public CalibFunc {
    public:
        explicit IRtResolution(const CalibFunc::ParVec& vec) : CalibFunc(vec){};
        virtual ~IRtResolution(){};
        virtual std::string typeName() const { return "IRtResolution"; }

        /** returns resolution for a give time and background rate */
        virtual double resolution(double t, double bgRate = 0.0) const = 0;
    };

}  // namespace MuonCalib

#endif
