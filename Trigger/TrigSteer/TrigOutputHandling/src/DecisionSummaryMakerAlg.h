/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_DECISIONTSUMMARYMAKERALG_H
#define TRIGOUTPUTHANDLING_DECISIONTSUMMARYMAKERALG_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigCostMonitorMT/ITrigCostMTSvc.h"


/**
 * @class DecisionsSummaryMakerAlg
 * @brief Executes after both the first-pass and secon-pass (rerun) chains have finished.
 *        Makes decision objects containing decision IDs per passing chain, prescaled chain and rerun chain.
 *        Sends end-of-HLT processing trigger to cost monitoring. Writes HLTSummary container.
 **/
class DecisionSummaryMakerAlg : public AthReentrantAlgorithm {
public:
  DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~DecisionSummaryMakerAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey{ this, "DecisionsSummaryKey", "HLTSummary", 
      "location of final decision" };

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionKeys{ this, "FinalDecisionKeys", {}, 
      "Final stage of all decisions" };

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_costWriteHandleKey { this, "CostWriteHandleKey", "TrigCostContainer",
    "Trig composite collections summarising the HLT execution" };

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_l1SummaryKey { this, "L1DecoderSummaryKey", "L1DecoderSummary",
    "Chains status after L1 and prescaling" };

  ServiceHandle<ITrigCostMTSvc> m_trigCostSvcHandle { this, "TrigCostMTSvc", "TrigCostMTSvc", 
    "The trigger cost service" };

  Gaudi::Property< std::map< std::string, std::string > > m_lastStepForChain{ this, "FinalStepDecisions", {}, 
    "The map of chain name to name of the collection in which the final decision is found" };

  Gaudi::Property<bool> m_enableCostMonitoring{this, "EnableCostMonitoring", false, 
    "Enables end-of-event cost monitoring behavior."};

  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_collectionFilter;

};

#endif // TRIGOUTPUTHANDLING_DECISIONSSUMMARYMAKERALG_H
