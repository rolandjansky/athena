/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTBFIELDCORFUNC0_H
#define MUONCALIB_MDTBFIELDCORFUNC0_H

#include "MdtCalibData/IMdtBFieldCorFunc.h"

namespace MuonCalib {

    /** implementation of a b-field correction function */

    class MdtBFieldCorFunc0 : public IMdtBFieldCorFunc {
    public:
        explicit MdtBFieldCorFunc0(const CalibFunc::ParVec& vec) : IMdtBFieldCorFunc(vec){};
        virtual std::string name() const { return "MdtBFieldCorFunc0"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double correction(double /*t*/, double /*Bpar*/, double /*Bperp*/) const { return 0.0; }
    };

}  // namespace MuonCalib

#endif
