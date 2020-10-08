/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITPRECISIONETCUTHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITPRECISIONETCUTHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


/**
 * @class Base for tools doing precision Etcut Hypo selection
 * @brief 
 **/

class ITrigEgammaPrecisionEtcutHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigEgammaPrecisionEtcutHypoTool, 1, 0);
  virtual ~ITrigEgammaPrecisionEtcutHypoTool(){}

  struct ClusterInfo {
  ClusterInfo( TrigCompositeUtils::Decision* d, const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<ClusterInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const ClusterInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGEGAMMAHYPO_ITPRECISIONETCUTHYPOTOOL_H
