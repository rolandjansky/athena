/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTBACKGROUNDCORFUNC0_H
#define MUONCALIB_MDTBACKGROUNDCORFUNC0_H

#include "MdtCalibData/IMdtBackgroundCorFunc.h"

namespace MuonCalib {

    /** implementation of a background correction function */

    class MdtBackgroundCorFunc0 : public IMdtBackgroundCorFunc {
    public:
        explicit MdtBackgroundCorFunc0(const CalibFunc::ParVec& vec) : IMdtBackgroundCorFunc(vec){};
        virtual std::string name() const { return "MdtBackgroundCorFunc0"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double correction(double /*t*/, double /*bgRate*/) const { return 0.0; }
    };

}  // namespace MuonCalib

#endif
