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
#include "./CapacityCheckedConditionsDefs.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "./IJetsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "./ITrigJetCapacityCheckedConditionConfig.h"
#include "./ConditionFilter.h"

class TrigJetHypoToolConfig_fastreduction:
public extends<AthAlgTool, ITrigJetHypoToolNoGrouperConfig> {

 public:
  
  TrigJetHypoToolConfig_fastreduction(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_fastreduction();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetsMatcherMT> getMatcher() const override;
  virtual std::optional<ConditionsMT> getConditions() const override;

  virtual std::vector<std::unique_ptr<ConditionFilter>>
  getConditionFilters() const override;

  virtual std::size_t requiresNJets() const override;
  virtual StatusCode checkVals() const override;


 private:
  ToolHandleArray<ITrigJetCapacityCheckedConditionConfig> m_conditionMakers{
    this, "conditionMakers", {}, "hypo tree Condition builder AlgTools"};

  ToolHandleArray<ITrigJetCapacityCheckedConditionConfig>
  m_antiConditionMakers{this, "antiConditionMakers", {},
    "hypo tree AntiCondition builder AlgTools"};


  ToolHandleArray<ITrigJetCapacityCheckedConditionConfig> m_filtConditionMakers{
    this, "filtConditionsMakers", {},
    "hypo tree Condition builder AlgTools for Condition filters"};

  
  Gaudi::Property<std::vector<std::size_t>> m_treeVec{
    this, "treeVector", {}, "integer sequence representation of jet hypo tree"};

  Gaudi::Property<std::vector<int>> m_leafNodes{
    this, "leafVector", {}, "node ids for leaf nodes"};

  std::optional<ConditionPtrs> getCapacityCheckedConditions() const;


};
#endif
