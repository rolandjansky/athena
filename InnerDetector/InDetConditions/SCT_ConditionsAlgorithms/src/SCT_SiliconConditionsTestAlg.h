/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SiliconConditionsTestAlg.h
 *
 * @brief Header file for the SCT_SiliconConditionsTestAlg class 
 *  in package SCT_ConditionsAlgorithms
 *
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 **/

#ifndef SCT_SiliconConditionsTestAlg_H
#define SCT_SiliconConditionsTestAlg_H 

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

///Example class to show calling the SCT_SiliconConditionsSvc
class SCT_SiliconConditionsTestAlg : public AthAlgorithm {
 public:
  SCT_SiliconConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_SiliconConditionsTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISiliconConditionsTool> m_siliconTool{this, "SCT_SiliconConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};
}; //end of class

#endif // SCT_SiliconConditionsTestAlg_H
