/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_TdaqEnabledTestAlg.h
*
* @brief Header file for the SCT_TdaqEnabledTestAlg class 
*  in package SCT_ConditionsServices
*
* @author Shaun Roe
**/

#ifndef SCT_TdaqEnabledTestAlg_H
#define SCT_TdaqEnabledTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Local
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"


//Forward declarations
class ISvcLocator;

///Example algorithm to show calling the SCT_ModuleVeto to exclude bad components
class SCT_TdaqEnabledTestAlg : public AthAlgorithm {
 public:
  SCT_TdaqEnabledTestAlg(const std::string& name, ISvcLocator *pSvcLocator);
 ~SCT_TdaqEnabledTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
   ServiceHandle<ISCT_ConditionsSvc> m_pTdaqEnabledSvc;
}; //end of class

#endif // SCT_TdaqEnabledTestAlg_H
