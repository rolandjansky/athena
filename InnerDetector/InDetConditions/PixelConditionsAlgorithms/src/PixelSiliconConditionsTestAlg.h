/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsAlgorithms/PixelSiliconConditionsTestAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Test algorithm to check the various contents from the Data.
 */

#ifndef PIXELSILICONcONDITIONSTESTALG_H
#define PIXELSILICONcONDITIONSTESTALG_H

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "PixelConditionsData/PixelDCSHVData.h"
#include "PixelConditionsData/PixelDCSTempData.h"
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

    SG::ReadCondHandleKey<PixelDCSTempData> m_readKeyTemp
    {this, "ReadKeyeTemp", "PixelDCSTempCondData", "Key of input sensor temperature conditions folder"};

    SG::ReadCondHandleKey<PixelDCSHVData> m_readKeyHV
    {this, "ReadKeyHV", "PixelDCSHVCondData", "Key of input bias voltage conditions folder"};

    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool
    {this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};

    SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
    {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Output key"};

    SG::ReadCondHandleKey<PixelDistortionData> m_distortionKey
    {this, "PixelDistortionData", "PixelDistortionData", "Output key"};

};

#endif
