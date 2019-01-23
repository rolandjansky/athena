/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

///Example class to show calling the SCT_LinkMaskingTool
class SCT_LinkMaskingTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_LinkMaskingTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_LinkMaskingTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_ConditionsTool> m_linkMaskingTool{this, "LinkMaskingTool", "SCT_LinkMaskingTool", "Tool to retrieve masked links"};
}; 

#endif // SCT_LinkMaskingTestAlg_H
