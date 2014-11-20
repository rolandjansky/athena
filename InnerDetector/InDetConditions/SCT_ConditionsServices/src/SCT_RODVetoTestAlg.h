/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_RODVetoTestAlg.h
*
* @brief Header file for the SCT_RODVetoTestAlg class 
*  in package SCT_ConditionsServices
*
* @author Daiki Hayakawa
**/

#ifndef SCT_RODVetoTestAlg_H
#define SCT_RODVetoTestAlg_H 
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

///Example algorithm to show calling the SCT_RODVeto to exclude bad components
class SCT_RODVetoTestAlg : public AthAlgorithm {
 public:
  SCT_RODVetoTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~SCT_RODVetoTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
   ServiceHandle<ISCT_ConditionsSvc> m_pRODVetoSvc;
}; //end of class

#endif
