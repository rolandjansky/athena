/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_TRIGGERSUMMARYALG_H
#define DECISIONHANDLING_TRIGGERSUMMARYALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigTimeAlgs/TrigTimeStamp.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/IHLTOutputTool.h"
#include "TrigCostMonitorMT/ITrigCostMTSvc.h"

/**
 * @class TriggerSummaryAlg
 * @brief In addition, a merged decisions collection is prepared. But not currently written.
 *        An algorithm reading partial decisions and print them. Use to debug at the end of a sequence.
 * @see   DecisionSummaryMakerAlg for the class which assembles the final summary after HLT execution
 **/

class TriggerSummaryAlg : public ::AthReentrantAlgorithm { 
 public: 
  TriggerSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TriggerSummaryAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  
 private: 

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_inputDecisionKey{ this, "InputDecision", "", 
      "Partial decisions from the last stage of chains processing, they nay be missing from event to event"};
  
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionKeys{ this, "FinalDecisions", {}, 
      "Object containing all the HLT input and output decision sets"};

  SG::ReadHandleKey<TrigTimeStamp> m_startStampKey{ this, "StartStampKey", "L1DecoderStart", 
      "Object with the time stamp when decoding started" };

  typedef short ChainCounter_t;
  std::map<HLT::Identifier, ChainCounter_t> m_chainIdToChainCounter;
  
  // TODO (tamartin / tbold) move this to DecisionSummaryMakerAlg ?
  ToolHandleArray<IHLTOutputTool> m_outputTools{ this, "OutputTools", {}, "Set of tools to prepare make HLT output ready for writing out" };

}; 

#endif //> !DECISIONHANDLING_TRIGGERSUMMARY_H
