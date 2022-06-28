/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMomentumCorrections/CalibInitializer.h"

#include <cmath>
#include <stdexcept>

namespace MCP {
    namespace CalibInitializer {

        std::map<SagittaCorrection, std::shared_ptr<CalibContainer>> createSagittaCorrMap(DataYear dataYear, TrackType type, const std::string& recommendationPath, const std::string& correctionType)
        {
            std::map<SagittaCorrection, std::shared_ptr<CalibContainer>> calibMap;

            std::string path = "MuonMomentumCorrections/" + recommendationPath + "/sagittaBias/" + MCP::toString(dataYear) + "/";

            std::string trackType = MCP::toString(type);

            if(correctionType == "data")
            {
                path += trackType + "_data.root";
                calibMap[SagittaCorrection::Nominal]        = std::make_shared<CalibContainer>(path, "p" + trackType + "_0");
                calibMap[SagittaCorrection::Datastat__1up]  = std::make_shared<CalibContainer>(path, "p" + trackType + "_statError");
                calibMap[SagittaCorrection::Residual__1up]  = std::make_shared<CalibContainer>(path, "p" + trackType + "_1");
            }
            else if(correctionType == "mc")
            {
                path += trackType + "_mc.root";
                calibMap[SagittaCorrection::Nominal]        = std::make_shared<CalibContainer>(path, "p" + trackType + "_0");
            }
            else
            {
                throw std::invalid_argument("Cannot recognize saggita correction type: " + correctionType);
            }

            return calibMap;
        }
        std::map<ScaleResCorrection, std::shared_ptr<CalibContainer>> createScaleResCorrMap(DataYear dataYear, TrackType type, const std::string& recommendationPath, ScaleSmearParam param)
        {
            std::map<ScaleResCorrection, std::shared_ptr<CalibContainer>> calibMap;


            std::string path = "MuonMomentumCorrections/" + recommendationPath + "/ScaleAndSmear/" + MCP::toString(dataYear) + "/";

            std::string trackType = MCP::toString(type);
            path += trackType + ".root";

            std::string correctionType = MCP::toString(param);

            calibMap[ScaleResCorrection::Nominal]         = std::make_shared<CalibContainer>(path, correctionType + "_nom");
            calibMap[ScaleResCorrection::SystErr__1up]    = std::make_shared<CalibContainer>(path, correctionType + "_upErr");
            calibMap[ScaleResCorrection::SystErr__1down]  = std::make_shared<CalibContainer>(path, correctionType + "_downErr");
            
            return calibMap;
        }

        std::map<MST_Categories, std::shared_ptr<CalibContainer>> createHighpTCorrMap(DataYear dataYear, const std::string& type, const std::string& recommendationPath, const std::string& param)
        {
            std::map<MST_Categories, std::shared_ptr<CalibContainer>> calibMap;
            std::string path = "MuonMomentumCorrections/" + recommendationPath + "/ScaleAndSmear/" + MCP::toString(dataYear) + "/ExtraHighPt_"+type+"_muons.root";

            calibMap[MST_Categories::Zero]  = std::make_shared<CalibContainer>(path, param + "_cat0");
            calibMap[MST_Categories::One]   = std::make_shared<CalibContainer>(path, param + "-cat1");
            calibMap[MST_Categories::Two]   = std::make_shared<CalibContainer>(path, param + "-cat2");
            calibMap[MST_Categories::Three] = std::make_shared<CalibContainer>(path, param + "-cat3");
            calibMap[MST_Categories::Four]  = std::make_shared<CalibContainer>(path, param + "-cat4");

            return calibMap;
        }

        std::map<ExpectedResParam, std::shared_ptr<CalibContainer>> createExpectedResMap(DataYear dataYear, TrackType type, const std::string& recommendationPath)
        {
            std::map<ExpectedResParam, std::shared_ptr<CalibContainer>> calibMap;
            std::string path = "MuonMomentumCorrections/" + recommendationPath + "/ScaleAndSmear/" + MCP::toString(dataYear) + "/MC_values_muons.root";

            if(type == TrackType::ID)
            {
                calibMap[ExpectedResParam::r1] = std::make_shared<CalibContainer>(path, "r1_ID_MC");
                calibMap[ExpectedResParam::r2] = std::make_shared<CalibContainer>(path, "r2_ID_MC");
                calibMap[ExpectedResParam::r2tan2] = std::make_shared<CalibContainer>(path, "r2_ID_TAN2_MC");
            }
            else if(type == TrackType::ME)
            {
                calibMap[ExpectedResParam::r0] = std::make_shared<CalibContainer>(path, "r0_MS_MC");
                calibMap[ExpectedResParam::r1] = std::make_shared<CalibContainer>(path, "r1_MS_MC");
                calibMap[ExpectedResParam::r2] = std::make_shared<CalibContainer>(path, "r2_MS_MC");
            }

            return calibMap;
        }




    } 
}
