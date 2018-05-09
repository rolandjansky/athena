/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILICONcONDITIONSTESTALG_H
#define PIXELSILICONcONDITIONSTESTALG_H

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

class PixelSiliconConditionsTestAlg : public AthAlgorithm {
  public:
    PixelSiliconConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
    virtual ~PixelSiliconConditionsTestAlg() = default;

    StatusCode initialize() override;
    StatusCode execute() override;
    StatusCode finalize() override;

  private:
    ToolHandle<ISiliconConditionsTool> m_siliconTool{this, "PixelSiliconConditionsTool", "PixelSiliconConditionsTool", "Tool to retrieve Pixel information"};
//    ToolHandle<ISiliconConditionsTool> m_siliconTool{this, "SCT_SiliconConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};
};

#endif // PIXELSILICONcONDITIONSTESTALG_H
