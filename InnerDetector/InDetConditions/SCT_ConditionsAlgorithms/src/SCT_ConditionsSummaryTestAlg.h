/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTestAlg.h
 *
 * @brief Header file for the SCT_ConditionsSummaryTestAlg class 
 *  in package SCT_ConditionsAlgorithms
 *
 * @author Shaun Roe
 **/

#ifndef SCT_ConditionsSummaryTestAlg_H
#define SCT_ConditionsSummaryTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

///Example class to show calling the SCT_ConditionsSummarySvc
class SCT_ConditionsSummaryTestAlg : public AthAlgorithm {
 public:
  SCT_ConditionsSummaryTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_ConditionsSummaryTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ServiceHandle<IInDetConditionsSvc> m_pSummarySvc;
}; //end of class

#endif // SCT_ConditionsSummaryTestAlg_H
