/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCPCALIBINITLIALIZE_H
#define MCPCALIBINITLIALIZE_H

#include <MuonMomentumCorrections/CalibContainer.h>
#include <MuonMomentumCorrections/EnumDef.h>

// STL //
#include <map>

namespace MCP {
    namespace CalibInitializer
    {
        std::map<SagittaCorrection, std::shared_ptr<CalibContainer>>  createSagittaCorrMap (DataYear dataYear, TrackType type, const std::string& recommendationPath, const std::string& correctionType);
        std::map<ScaleResCorrection, std::shared_ptr<CalibContainer>> createScaleResCorrMap(DataYear dataYear, TrackType type, const std::string& recommendationPath, ScaleSmearParam param);
        std::map<MST_Categories, std::shared_ptr<CalibContainer>> createHighpTCorrMap(DataYear dataYear, const std::string& type, const std::string& recommendationPath, const std::string& param);
        std::map<ExpectedResParam, std::shared_ptr<CalibContainer>> createExpectedResMap(DataYear dataYear, TrackType type, const std::string& recommendationPath);

    };
} 

#endif
