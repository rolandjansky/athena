/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CachedSummaryTestAlg.h
 *
 * @brief Header file for the SCT_CachedSummaryTestAlg class 
 *  in package SCT_ConditionsAlgorithms
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

///Example class to show calling the SCT_CachedSummarySvc
class SCT_CachedSummaryTestAlg : public AthAlgorithm {
 public:
  SCT_CachedSummaryTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_CachedSummaryTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ServiceHandle<IInDetConditionsSvc> m_pSummarySvc;
}; //end of class

#endif // SCT_CachedSummaryTestAlg_H
