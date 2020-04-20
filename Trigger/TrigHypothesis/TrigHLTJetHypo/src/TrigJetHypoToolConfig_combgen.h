/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLConfig_COMBGEN_H
#define TRIGJETHYPOTOOLConfig_COMBGEN_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_combgen.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolConfig.h"
#include "./ConditionsDefsMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"


#include "./ConditionsDefsMT.h"
#include "./ITrigJetConditionConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class TrigJetHypoToolConfig_combgen:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_combgen(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_combgen();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;

  virtual std::optional<ConditionsMT> getConditions() const override;

  virtual std::size_t requiresNJets() const override;
  
 private:

  ToolHandleArray<ITrigJetConditionConfig> m_conditionMakers{
    this, "conditionMakers", {}, "conditions makers for a leaf node."};
  
  std::size_t m_size{0};  // size of jet groups to pass to children

  // m_children: used to make the matcher. The matcher takes care of
  // passign jet groups to the child helpers.
  ToolHandleArray<ITrigJetHypoToolHelperMT> m_children {
    this, "children", {}, "list of child jet hypo helpers"};

  virtual StatusCode checkVals()  const override;
 
};
#endif
