/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledTestAlg.h
 *
 * @brief Header file for the SCT_TdaqEnabledTestAlg class 
 *  in package SCT_ConditionsAlgorithms*
 * @author Shaun Roe
 **/

#ifndef SCT_TdaqEnabledTestAlg_H
#define SCT_TdaqEnabledTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//Athena
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

///Example algorithm to show calling the SCT_ModuleVeto to exclude bad components
class SCT_TdaqEnabledTestAlg : public AthAlgorithm {
 public:
  SCT_TdaqEnabledTestAlg(const std::string& name, ISvcLocator *pSvcLocator);
  virtual ~SCT_TdaqEnabledTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_ConditionsTool> m_pTdaqEnabledTool{this, "SCT_TdaqEnabledTool", "SCT_TdaqEnabledTool", "Tool to retrieve active/inactive SCT ROD information"};
}; //end of class

#endif // SCT_TdaqEnabledTestAlg_H
