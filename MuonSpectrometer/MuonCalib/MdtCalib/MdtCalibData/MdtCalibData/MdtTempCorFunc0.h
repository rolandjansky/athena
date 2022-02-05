/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTTEMPCORFUNC0_H
#define MUONCALIB_MDTTEMPCORFUNC0_H

#include "MdtCalibData/IMdtTempCorFunc.h"

namespace MuonCalib {

    /** implementation of a temperature correction function */

    class MdtTempCorFunc0 : public IMdtTempCorFunc {
    public:
        explicit MdtTempCorFunc0(const CalibFunc::ParVec& vec) : IMdtTempCorFunc(vec){};
        virtual std::string name() const { return "MdtTempCorFunc0"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double correction(double /*t*/, double /*temp*/) const { return 0.0; }
    };

}  // namespace MuonCalib

#endif
