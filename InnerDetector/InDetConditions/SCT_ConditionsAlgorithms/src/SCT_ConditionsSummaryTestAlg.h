// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

///Example class to show calling the SCT_ConditionsSummaryTool
class SCT_ConditionsSummaryTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_ConditionsSummaryTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_ConditionsSummaryTestAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };
   
 private:
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "SCT_ConditionsSummaryTool", "SCT_ConditionsSummaryTool", "ConditionsSummaryTool for SCT"};
}; //end of class

#endif // SCT_ConditionsSummaryTestAlg_H
