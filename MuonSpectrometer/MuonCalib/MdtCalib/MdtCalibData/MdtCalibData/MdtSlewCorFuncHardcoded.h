/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MdtSlewCorFuncHardcoded_H
#define MUONCALIB_MdtSlewCorFuncHardcoded_H

#include <vector>

#include "MdtCalibData/IMdtSlewCorFunc.h"

namespace MuonCalib {

    /** An implementation of a time slew correction function for AMT digitization chip */
    class MdtSlewCorFuncHardcoded : public IMdtSlewCorFunc {
    public:
        explicit MdtSlewCorFuncHardcoded(const CalibFunc::ParVec& vec) : IMdtSlewCorFunc(vec) {}
        inline virtual std::string name() const override { return "MdtSlewCorFuncHardCoded"; }
        static unsigned int nUsedPar() { return 0; }
        virtual double correction(double t, double adc) const override;
    };

}  // namespace MuonCalib

#endif
