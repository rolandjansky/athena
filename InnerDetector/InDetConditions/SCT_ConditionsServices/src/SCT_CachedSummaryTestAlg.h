/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_CachedSummaryTestAlg.h
*
* @brief Header file for the SCT_CachedSummaryTestAlg class 
*  in package SCT_ConditionsServices
*
* @author Shaun Roe
**/

#ifndef SCT_CachedSummaryTestAlg_H
#define SCT_CachedSummaryTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"



//Forward declarations
class ISvcLocator;
class StatusCode;

///Example class to show calling the SCT_ConditionsSummarySvc
class SCT_CachedSummaryTestAlg : public AthAlgorithm {
 public:
  SCT_CachedSummaryTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~SCT_CachedSummaryTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  ServiceHandle<IInDetConditionsSvc> m_pSummarySvc;
}; //end of class

#endif
