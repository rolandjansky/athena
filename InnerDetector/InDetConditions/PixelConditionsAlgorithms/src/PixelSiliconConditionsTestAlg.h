/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILICONcONDITIONSTESTALG_H
#define PIXELSILICONcONDITIONSTESTALG_H

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "PixelConditionsTools/IPixelDCSConditionsTool.h"

class PixelSiliconConditionsTestAlg : public AthAlgorithm {
  public:
    PixelSiliconConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
    virtual ~PixelSiliconConditionsTestAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    ToolHandle<IPixelDCSConditionsTool> m_siliconTool{this, "PixelDCSConditionsTool", "PixelDCSConditionsTool", "Tool to retrieve Pixel information"};
};

#endif // PIXELSILICONcONDITIONSTESTALG_H
