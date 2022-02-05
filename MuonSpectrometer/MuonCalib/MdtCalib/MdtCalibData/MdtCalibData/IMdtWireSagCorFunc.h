/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IMDTWIRESAGCORFUNC_H
#define MUONCALIB_IMDTWIRESAGCORFUNC_H

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    /** generic interface for a wire sag correction function */

    class IMdtWireSagCorFunc : public CalibFunc {
    public:
        explicit IMdtWireSagCorFunc(const CalibFunc::ParVec& vec) : CalibFunc(vec){};
        virtual ~IMdtWireSagCorFunc() {}
        virtual std::string typeName() const { return "IMdtWireSagCorFunc"; }

        /** Return time correction (ns).
            Input: signedDriftRadius, effectiveSag (corrected for eta, phi)
        */
        virtual double correction(double signedDriftRadius, double effectiveSag) const = 0;
    };

}  // namespace MuonCalib

#endif
