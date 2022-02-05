/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTSLEWCORFUNC0_H
#define MUONCALIB_MDTSLEWCORFUNC0_H

#include "MdtCalibData/IMdtSlewCorFunc.h"

namespace MuonCalib {

    /** implementation of a slewing correction function */

    class MdtSlewCorFunc0 : public IMdtSlewCorFunc {
    public:
        explicit MdtSlewCorFunc0(const CalibFunc::ParVec& vec) : IMdtSlewCorFunc(vec){};
        virtual std::string name() const { return "MdtSlewCorFunc0"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double correction(double /*t*/, double /*adc*/) const { return 0.0; }
    };

}  // namespace MuonCalib

#endif
