/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_TRIGGERSUMMARYALG_H
#define DECISIONHANDLING_TRIGGERSUMMARYALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

/**
 * @class An algorithm reading partial decisions and storing them in an HLTResult
 * @brief In addition a merged decisions collection is prepared
 **/

class TriggerSummaryAlg : public ::AthReentrantAlgorithm { 
 public: 
  TriggerSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TriggerSummaryAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  
 
 private: 
  //Gaudi::Propert std::vector<m_chainCounters> { this,  "ChainCounters", {},
  //      "Chain names maping to counters. In the form of: name counter" };
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_l1decisionKey{ this, "L1Decision", "", 
      "Partial decisions from the last stage of chains processing, they nay be missing from event to event"};

  
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionKeys{ this, "FinalDecisions", {}, 
      "Object ontaining all the HLT input and outpu decision sets"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey { this, "HLTSummary", "HLTSummary", "Output summary" };
  //  SG::WriteHandleKey m_hltResultKey{ this, "HLTResult", "", "Output HLT Result" };


  typedef short ChainCounter_t;
  std::map<HLT::Identifier, ChainCounter_t> m_chainIdToChainCounter;
  
  
}; 


#endif //> !DECISIONHANDLING_TRIGGERSUMMARY_H
