/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTWIRESAGCORFUNC0_H
#define MUONCALIB_MDTWIRESAGCORFUNC0_H

#include "MdtCalibData/IMdtWireSagCorFunc.h"

namespace MuonCalib {

    /** implementation of a wire sag correction function */

    class MdtWireSagCorFunc0 : public IMdtWireSagCorFunc {
    public:
        explicit MdtWireSagCorFunc0(const CalibFunc::ParVec& params) : IMdtWireSagCorFunc(params){};
        virtual std::string name() const { return "MdtWireSagCorFunc0"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double correction(double /*signedDriftRadius*/, double /*effectiveSag*/) const { return 0.0; }
    };

}  // namespace MuonCalib

#endif
