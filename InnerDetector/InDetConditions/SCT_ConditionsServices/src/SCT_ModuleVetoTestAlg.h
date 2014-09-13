/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_ModuleVetoTestAlg.h
*
* @brief Header file for the SCT_ModuleVetoTestAlg class 
*  in package SCT_ConditionsServices
*
* @author Shaun Roe
**/

#ifndef SCT_ModuleVetoTestAlg_H
#define SCT_ModuleVetoTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Local
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"


//Forward declarations
class ISvcLocator;
class StatusCode;

///Example algorithm to show calling the SCT_ModuleVeto to exclude bad components
class SCT_ModuleVetoTestAlg : public AthAlgorithm {
 public:
  SCT_ModuleVetoTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~SCT_ModuleVetoTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
   ServiceHandle<ISCT_ConditionsSvc> m_pModuleVetoSvc;
}; //end of class

#endif
