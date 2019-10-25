/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILICONcONDITIONSTESTALG_H
#define PIXELSILICONcONDITIONSTESTALG_H

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "PixelConditionsData/PixelDistortionData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

class PixelSiliconConditionsTestAlg : public AthAlgorithm {
  public:
    PixelSiliconConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
    virtual ~PixelSiliconConditionsTestAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Output key"};

    SG::ReadCondHandleKey<PixelModuleData> m_readKeyTemp
    {this, "ReadKeyeTemp", "PixelDCSTempCondData", "Key of input sensor temperature conditions folder"};

    SG::ReadCondHandleKey<PixelModuleData> m_readKeyHV
    {this, "ReadKeyHV", "PixelDCSHVCondData", "Key of input bias voltage conditions folder"};

    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool
    {this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};

    SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
    {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Output key"};

    SG::ReadCondHandleKey<PixelDistortionData> m_distortionKey
    {this, "PixelDistortionData", "PixelDistortionData", "Output key"};

};

#endif
