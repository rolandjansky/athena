/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILICONcONDITIONSTESTALG_H
#define PIXELSILICONcONDITIONSTESTALG_H

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "PixelConditionsData/PixelModuleData.h"
#include "StoreGate/ReadCondHandleKey.h"

class PixelSiliconConditionsTestAlg : public AthAlgorithm {
  public:
    PixelSiliconConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
    virtual ~PixelSiliconConditionsTestAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<PixelModuleData> m_readKeyTemp{this, "ReadKeyeTemp", "PixelDCSTempCondData",         "Key of input sensor temperature conditions folder"};
    SG::ReadCondHandleKey<PixelModuleData> m_readKeyHV  {this, "ReadKeyHV",    "PixelDCSHVCondData",           "Key of input bias voltage conditions folder"};
    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey{this, "PixelModuleData", "PixelModuleData", "Output key"};
};

#endif
