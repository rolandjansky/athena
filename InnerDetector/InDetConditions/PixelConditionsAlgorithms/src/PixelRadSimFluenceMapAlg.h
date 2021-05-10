/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelRadSimFluenceMapAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date April, 2021
 * @brief Store pixel radiation fluence map parameters in PixelRadiationDamageFluenceMapData.
 */

#ifndef PIXELRADSIMFLUENCEMAPALG
#define PIXELRADSIMFLUENCEMAPALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelRadiationDamageFluenceMapData.h"

class PixelRadSimFluenceMapAlg : public AthReentrantAlgorithm {
  public:
    PixelRadSimFluenceMapAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::WriteCondHandleKey<PixelRadiationDamageFluenceMapData> m_writeFluenceMapKey
    {this, "WriteRadiationFluenceMapKey", "PixelRadiationDamageFluenceMapData", "Output key for radiation damage fluence map"};
};

#endif
