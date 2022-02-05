/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IMDTTEMPCORFUNC_H
#define MUONCALIB_IMDTTEMPCORFUNC_H

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    /** generic interface for temperature correction functions */

    class IMdtTempCorFunc : public CalibFunc {
    public:
        explicit IMdtTempCorFunc(const CalibFunc::ParVec& vec) : CalibFunc(vec){};
        virtual ~IMdtTempCorFunc() {}
        virtual std::string typeName() const { return "IMdtTempCorFunc"; }
        /** Return time correction (ns). Input: t = drift-time (ns), temp = temperature (degrees Celcius) */
        virtual double correction(double t, double temp) const = 0;
    };

}  // namespace MuonCalib

#endif
