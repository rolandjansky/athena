/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityConditionsTestAlg.h
 *
 * @brief 
 *  
 *
 * @author gwilliam@mail.cern.ch
 **/

#ifndef SCT_MajorityConditionsTestAlg_H
#define SCT_MajorityConditionsTestAlg_H 

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SCT_ConditionsTools/ISCT_DetectorLevelConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

///Example class to show calling the SCT_MajorityConditionsSvc
class SCT_MajorityConditionsTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_MajorityConditionsTestAlg(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~SCT_MajorityConditionsTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_DetectorLevelConditionsTool> m_majorityTool{this, "MajorityTool", "InDetSCT_MajorityConditionsTool", "Tool to retrieve the majority detector status"};
}; 

#endif // SCT_MajorityConditionsTestAlg_H
