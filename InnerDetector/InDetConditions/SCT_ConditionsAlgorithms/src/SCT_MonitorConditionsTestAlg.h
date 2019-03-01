/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_MonitorConditionsTestAlg.h
*
* @brief Header file for the SCT_MonitorConditionsTestAlg class 
*  in package SCT_ConditionsAlgorithms
*
* @author Kazu Hanagaki
**/

#ifndef SCT_MonitorConditionsTestAlg_H
#define SCT_MonitorConditionsTestAlg_H

// Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SCT_ConditionsTools/ISCT_MonitorConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//Forward declarations
class SCT_ID;

///Example class to show calling the SCT_MonitorConditions
class SCT_MonitorConditionsTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_MonitorConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_MonitorConditionsTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_MonitorConditionsTool> m_pMonitorConditionsTool{this, "SCT_MonitorConditionsTool", "SCT_MonitorConditionsTool", "Tool to retrieve noisy strip information"};
  const SCT_ID* m_sctId;
}; // end of class

#endif // SCT_MonitorConditionsTestAlg_H
