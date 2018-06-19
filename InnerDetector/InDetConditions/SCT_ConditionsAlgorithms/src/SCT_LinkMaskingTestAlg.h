/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_LinkMaskingTestAlg.h
*
* @brief 
*  
*
* @author gwilliam@mail.cern.ch
**/

#ifndef SCT_LinkMaskingTestAlg_H
#define SCT_LinkMaskingTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//Athena
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

///Example class to show calling the SCT_LinkMaskingTool
class SCT_LinkMaskingTestAlg : public AthAlgorithm {
 public:
  SCT_LinkMaskingTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_LinkMaskingTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_ConditionsTool> m_linkMaskingTool{this, "LinkMaskingTool", "SCT_LinkMaskingTool", "Tool to retrieve masked links"};
}; 

#endif // SCT_LinkMaskingTestAlg_H
