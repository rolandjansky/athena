/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena
#include "SCT_ConditionsServices/ISCT_DetectorLevelConditionsSvc.h"

//Forward declarations
class ISvcLocator;
class StatusCode;
class ISCT_ConditionsSvc;

///Example class to show calling the SCT_MajorityConditionsSvc
class SCT_MajorityConditionsTestAlg : public AthAlgorithm {
 public:
  SCT_MajorityConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~SCT_MajorityConditionsTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  ServiceHandle<ISCT_DetectorLevelConditionsSvc> m_majoritySvc;
}; 

#endif
