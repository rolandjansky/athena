/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTCALIBRATIONFACTORY_H
#define MUONCALIB_MDTCALIBRATIONFACTORY_H

#include <string>
#include <vector>

#include "MdtCalibData/CalibFunc.h"

namespace MuonCalib {

    class IMdtBackgroundCorFunc;
    class IMdtBFieldCorFunc;
    class IMdtSlewCorFunc;
    class IMdtTempCorFunc;
    class IMdtWireSagCorFunc;
    class IRtRelation;
    class IRtResolution;

    /** @class MdtCalibrationFactory
    Factory to create instances of correction functions
    */
    class MdtCalibrationFactory {
        typedef CalibFunc::ParVec ParVec;

    public:
        static IMdtBackgroundCorFunc* createBackgroundCorFunc(const std::string& name, const ParVec& pars);

        static IMdtBFieldCorFunc* createBFieldCorFunc(const std::string& name, const ParVec& pars);

        static IMdtSlewCorFunc* createMdtSlewCorFunc(const std::string& name, const ParVec& pars);

        static IMdtTempCorFunc* createMdtTempCorFunc(const std::string& name, const ParVec& pars);

        static IMdtWireSagCorFunc* createMdtWireSagCorFunc(const std::string& name, const ParVec& pars);

        static IRtRelation* createRtRelation(const std::string& name, const ParVec& pars);

        static IRtResolution* createRtResolution(const std::string& name, const ParVec& pars);
    };

}  // namespace MuonCalib

#endif
