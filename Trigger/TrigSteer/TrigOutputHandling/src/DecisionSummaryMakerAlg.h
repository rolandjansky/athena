/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_DECISIONSUMMARYMAKERALG_H
#define TRIGOUTPUTHANDLING_DECISIONSUMMARYMAKERALG_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCostMonitor/ITrigCostSvc.h"


/**
 * @class DecisionsSummaryMakerAlg
 * @brief Executes after both the first-pass and second-pass (rerun) chains have finished.
 *        Makes decision objects containing decision IDs per passing chain, prescaled chain and rerun chain.
 *        Sends end-of-HLT processing trigger to cost monitoring. Writes HLTNav_Summary container.
 **/
class DecisionSummaryMakerAlg : public AthReentrantAlgorithm {
public:
  DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~DecisionSummaryMakerAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
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

  Gaudi::Property< std::map< std::string, std::vector<std::string> > > m_lastStepForChain{ this, "FinalStepDecisions", {},
    "The map of chain name to names of the collections in which the final decision is found" };

  Gaudi::Property<bool> m_doCostMonitoring{this, "DoCostMonitoring", false,
    "Enables end-of-event cost monitoring behavior."};

  Gaudi::Property<bool> m_setFilterStatus{this, "SetFilterStatus", false,
    "Enables chain-passed filter. This will cause the downstream EDMCreator to not run if no chains pass, saving CPU in rejected events. "
    "Cannot be used in jobs producing RDO output."};

  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_collectionFilter;

};

#endif // TRIGOUTPUTHANDLING_DECISIONSUMMARYMAKERALG_H