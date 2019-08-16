/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLCONFIG_FLOWNETWORK_H
#define TRIGJETHYPOTOOLCONFIG_FLOWNETWORK_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_flownetwork.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolConfig.h"
#include "./ConditionsDefsMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "./ITrigJetConditionConfig.h"

class TrigJetHypoToolConfig_flownetwork:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_flownetwork(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_flownetwork();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;
  virtual std::optional<ConditionsMT> getConditions() const override;
  virtual std::size_t requiresNJets() const override;
  virtual StatusCode checkVals() const override;


 private:
  ToolHandleArray<ITrigJetConditionConfig> m_conditionMakers;
  Gaudi::Property<std::vector<std::size_t>> m_treeVec; 
 
};
#endif
