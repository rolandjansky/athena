/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_TRIGGERSUMMARYALG_H
#define DECISIONHANDLING_TRIGGERSUMMARYALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigTimeAlgs/TrigTimeStamp.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigOutputHandling/IHLTOutputTool.h"
#include "TrigCostMonitorMT/ITrigCostMTSvc.h"

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
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_inputDecisionKey{ this, "InputDecision", "", 
      "Partial decisions from the last stage of chains processing, they nay be missing from event to event"};

  
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionKeys{ this, "FinalDecisions", {}, 
      "Object ontaining all the HLT input and outpu decision sets"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey { this, "HLTSummary", "HLTSummary", "Output summary" };
  //  SG::WriteHandleKey m_hltResultKey{ this, "HLTResult", "", "Output HLT Result" };

  SG::ReadHandleKey<TrigTimeStamp> m_startStampKey{ this, "StartStampKey", "L1DecoderStart", 
      "Object with the time stamp when decoding started" };

  typedef short ChainCounter_t;
  std::map<HLT::Identifier, ChainCounter_t> m_chainIdToChainCounter;
  
  ToolHandleArray<IHLTOutputTool> m_outputTools{ this, "OutputTools", {}, "Set of tools to prepare make HLT output ready for writing out" };

  ServiceHandle<ITrigCostMTSvc> m_trigCostSvcHandle { this, "TrigCostMTSvc", "TrigCostMTSvc", 
    "The trigger cost service" };

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_costWriteHandleKey { this, "CostWriteHandleKey", "TrigCostContainer",
    "Trig composite collections summarising the HLT execution" };

  Gaudi::Property<bool> m_enableCostMonitoring{this, "EnableCostMonitoring", false, 
    "Enables end-of-event cost monitoring behavior. Only to be enabled on the very final summary alg."};

}; 


#endif //> !DECISIONHANDLING_TRIGGERSUMMARY_H
