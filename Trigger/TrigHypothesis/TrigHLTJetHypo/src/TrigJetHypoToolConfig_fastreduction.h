/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLCONFIG_FASTREDUCTION_H
#define TRIGJETHYPOTOOLCONFIG_FASTREDUCTION_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_fastreduction.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolNoGrouperConfig.h"
#include "./RepeatedConditionsDefs.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "./IJetsMatcher.h"
#include "./ConditionsDefs.h"
#include "./ITrigJetRepeatedConditionConfig.h"
#include "./ITrigHypoJetVectorFilterConfig.h"


class TrigJetHypoToolConfig_fastreduction:
public extends<AthAlgTool, ITrigJetHypoToolNoGrouperConfig> {

 public:
  
  TrigJetHypoToolConfig_fastreduction(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_fastreduction();

  virtual StatusCode initialize() override;
  virtual std::unique_ptr<IJetsMatcher> getMatcher() const override;

  virtual std::size_t requiresNJets() const override;
  virtual StatusCode checkVals() const override;


 private:
  ToolHandleArray<ITrigJetRepeatedConditionConfig> m_conditionMakers{
    this, "conditionMakers", {}, "hypo tree Condition builder AlgTools"};
  
  ToolHandleArray<ITrigHypoJetVectorFilterConfig> m_filterMakers{
    this, "filterMakers", {}, "AlgTools that construct Condition filters"
  };

  Gaudi::Property<std::vector<std::size_t>> m_treeVec{
    this, "treeVector", {}, "integer sequence representation of jet hypo tree"};

  Gaudi::Property<std::vector<int>> m_leafNodes{
    this, "leafVector", {}, "node ids for leaf nodes"};

  ConditionPtrs getRepeatedConditions() const;

  // function to obtain the filters associated with the leaf conditions
  std::vector<FilterPtr> getFilters() const;


};
#endif
