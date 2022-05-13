/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_DECISIONSUMMARYMAKERALG_H
#define TRIGOUTPUTHANDLING_DECISIONSUMMARYMAKERALG_H

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCostMonitor/ITrigCostSvc.h"
#include "HLTSeeding/IPrescalingTool.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <string>


/**
 * @class DecisionsSummaryMakerAlg
 * @brief Executes after all chains have finished. Makes decision objects containing decision IDs per passing chain,
 *        prescaled chain, and express-prescaled chain. Writes HLTNav_Summary container.
 **/
class DecisionSummaryMakerAlg : public AthReentrantAlgorithm {
public:
  DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~DecisionSummaryMakerAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  /// Monitor RoI updates between initial and final RoI
  void monitorRoIs(const TrigCompositeUtils::Decision* terminusNode) const;

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey{ this, "DecisionsSummaryKey", "HLTNav_Summary",
      "Location of final decision" };

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionKeys{ this, "FinalDecisionKeys", {},
      "Final stage of all decisions" };

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_costWriteHandleKey { this, "CostWriteHandleKey", "HLT_TrigCostContainer",
    "TrigComposite collections summarising the HLT execution" };

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_rosWriteHandleKey { this, "ROSWriteHandleKey", "HLT_TrigCostROSContainer",
    "TrigComposite collections summarising the ROS requests" };

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_hltSeedingSummaryKey { this, "HLTSeedingSummaryKey", "HLTSeedingSummary",
    "Chains status after L1 and prescaling" };

  ServiceHandle<ITrigCostSvc> m_trigCostSvcHandle { this, "TrigCostSvc", "TrigCostSvc",
    "The trigger cost service" };

  ToolHandle<IPrescalingTool> m_prescaler{this, "Prescaler", "PrescalingTool/PrescalingTool",
    "Prescaling tool used to determine express stream prescale decisions"};

  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "",
    "Monitoring tool" };

  Gaudi::Property< std::map< std::string, std::vector<std::string> > > m_lastStepForChain{ this, "FinalStepDecisions", {},
    "The map of chain name to names of the collections in which the final decision is found" };

  Gaudi::Property<bool> m_doCostMonitoring{this, "DoCostMonitoring", false,
    "Enables end-of-event cost monitoring behavior."};

  Gaudi::Property<bool> m_warnOnLargeRoIUpdates{this, "WarnOnLargeRoIUpdates", true,
    "Print warnings from RoI update monitoring if the difference between initial and final RoI is large"};

  Gaudi::Property<bool> m_setFilterStatus{this, "SetFilterStatus", false,
    "Enables chain-passed filter. This will cause the downstream EDMCreator to not run if no chains pass, saving CPU in rejected events. "
    "Cannot be used in jobs producing RDO output."};

  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_collectionFilter;

};

#endif // TRIGOUTPUTHANDLING_DECISIONSUMMARYMAKERALG_H
