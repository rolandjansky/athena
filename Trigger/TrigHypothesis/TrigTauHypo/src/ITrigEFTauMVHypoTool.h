/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_ITRIGEFTAUMVHYPOTOOL_H
#define TRIGTAUHYPO_ITRIGEFTAUMVHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"





class ITrigEFTauMVHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigEFTauMVHypoTool, 1, 0);
  virtual ~ITrigEFTauMVHypoTool(){}

  struct TauJetInfo {
  TauJetInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::TauJetContainer *c,
	       const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ),
      roi( r ),
      taujetcontainer(c),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::TauJetContainer* taujetcontainer;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<TauJetInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const TauJetInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGTAUHYPO_ITRIGEFTAUMVHYPOTOOL_H
