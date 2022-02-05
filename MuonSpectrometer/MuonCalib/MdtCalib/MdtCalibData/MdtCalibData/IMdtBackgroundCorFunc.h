/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IMDTBACKGROUNDCORFUNC_H
#define MUONCALIB_IMDTBACKGROUNDCORFUNC_H

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    /** generic interface for cavern background correction functions */

    class IMdtBackgroundCorFunc : public CalibFunc {
    public:
        explicit IMdtBackgroundCorFunc(const CalibFunc::ParVec& vec) : CalibFunc(vec){};
        virtual ~IMdtBackgroundCorFunc() {}
        virtual std::string typeName() const { return "IMdtBackgroundCorFunc"; }
        /** Return time correction (ns). Input: t = drift-time (ns), bgRate = background current (nA) as measured by DCS in a chamber. */
        virtual double correction(double t, double bgRate) const = 0;
    };

}  // namespace MuonCalib

#endif
