/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ITRIGTRTHTHHYPOTOOL_H 
#define ITRIGTRTHTHHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigRinger/TrigRNNOutput.h"

/**
 * @class Base for tools doing TRTHTH Hypo selection
 * @brief 
 **/

class ITrigTRTHTHhypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigTRTHTHhypoTool, 1, 0);
  virtual ~ITrigTRTHTHhypoTool(){}

  struct RNNOutputInfo {
  RNNOutputInfo( TrigCompositeUtils::Decision* d,  
	       const xAOD::TrigRNNOutput* rnn,
               const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
    rnnOutput(rnn),
    previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigRNNOutput* rnnOutput;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon rnnOutput
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<RNNOutputInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const RNNOutputInfo& i ) const = 0;

 protected:


}; 


#endif //> !ITRIGTRTHTHHYPOTOOL_H

