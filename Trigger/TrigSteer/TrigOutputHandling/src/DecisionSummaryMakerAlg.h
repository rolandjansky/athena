/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_DECISIONTSUMMARYMAKERALG_H
#define TRIGOUTPUTHANDLING_DECISIONTSUMMARYMAKERALG_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/TrigCompositeUtils.h"


/**
 * @class DecisionsSummaryMakerAlg
 * @brief makes one decision object containing only one decision ID per accepted chain
 **/
class DecisionSummaryMakerAlg : public AthReentrantAlgorithm {
public:
  DecisionSummaryMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~DecisionSummaryMakerAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute_r(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey{ this, "DecisionsSummaryKey", "HLTFinalDecisions", 
      "location of final decision" };
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionKeys{ this, "FinalDecisionKeys", {}, 
      "Final stage of all decisions" };
  Gaudi::Property< std::map< std::string, std::string > > m_lastStepForChain{ this, "FinalStepDecisions", {}, 
      "The map of chain name to name of the collection in which the final decision is found" };
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_collectionFilter;

};

#endif // TRIGOUTPUTHANDLING_DECISIONSSUMMARYMAKERALG_H
