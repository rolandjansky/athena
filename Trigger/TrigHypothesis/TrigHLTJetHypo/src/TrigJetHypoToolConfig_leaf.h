/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLConfig_LEAF_H
#define TRIGJETHYPOTOOLConfig_LEAF_H

/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_leaf.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolConfig.h"
#include "./ConditionsDefsMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "./ConditionsDefsMT.h"
#include "./ITrigJetConditionConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetHypoToolConfig_leaf:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_leaf(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_leaf();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;
  virtual std::optional<ConditionsMT> getConditions() const override;
  virtual std::size_t requiresNJets() const override;

 private:

  ToolHandleArray<ITrigJetConditionConfig> m_conditionMakers{
    this, "conditionMakers", {}, "conditions makers for a leaf node."};



  virtual StatusCode checkVals()  const override;
 
};
#endif
