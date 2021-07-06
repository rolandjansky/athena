/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLCONFIG_CONDITIONFILTER_H
#define TRIGJETHYPOTOOLCONFIG_CONDITIONFILTER_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_conditionfilter.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * produces a ConditionsFilter from an array of RepeatedCondition makers
 *********************************************************************/


#include "./RepeatedConditionsDefs.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "ITrigHypoJetVectorFilterConfig.h"
#include "./ITrigJetRepeatedConditionConfig.h"
#include "./ConditionFilter.h"

class TrigJetHypoToolConfig_conditionfilter:
  public extends<AthAlgTool, ITrigHypoJetVectorFilterConfig> {

  
 public:
  
  TrigJetHypoToolConfig_conditionfilter(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_conditionfilter();

  virtual StatusCode initialize() override;
  virtual FilterPtr getHypoJetVectorFilter() const override;


 private:
  ToolHandleArray<ITrigJetRepeatedConditionConfig> m_conditionMakers{
    this, "conditionMakers", {},  "Condition builder AlgTools"};

  ConditionPtrs getRepeatedConditions() const;


};
#endif
