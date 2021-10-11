/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IMDTBFIELDCORFUNC_H
#define MUONCALIB_IMDTBFIELDCORFUNC_H

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    /** generic interface for b-field correction functions */

    class IMdtBFieldCorFunc : public CalibFunc {
    public:
        explicit IMdtBFieldCorFunc(const CalibFunc::ParVec& vec) : CalibFunc(vec){};
        virtual ~IMdtBFieldCorFunc() = default;
        virtual std::string typeName() const { return "IMdtBFieldCorFunc"; }
        /** Return time correction (ns). Input: t = drift-time (ns), Bpar = magnetic field parallel to wire (Tesla),
            Bperp = magnetic field perpendicular to wire and parallel to track. */
        virtual double correction(double t, double Bpar, double Bperp) const = 0;
    };

}  // namespace MuonCalib

#endif
