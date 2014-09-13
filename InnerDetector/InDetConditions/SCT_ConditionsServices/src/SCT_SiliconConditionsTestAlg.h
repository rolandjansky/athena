/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_SiliconConditionsTestAlg.h
*
* @brief Header file for the SCT_SiliconConditionsTestAlg class 
*  in package SCT_ConditionsServices
*
* @author Carl Gwilliam <gwilliam@mail.cern.ch>
**/

#ifndef SCT_SiliconConditionsTestAlg_H
#define SCT_SiliconConditionsTestAlg_H 

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"

//Forward declarations
class ISvcLocator;
class StatusCode;

///Example class to show calling the SCT_SiliconConditionsSvc
class SCT_SiliconConditionsTestAlg : public AthAlgorithm {
 public:
  SCT_SiliconConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~SCT_SiliconConditionsTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  ServiceHandle<ISiliconConditionsSvc> m_siliconSvc;
}; //end of class

#endif
