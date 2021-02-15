/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_ITrigTauGenericHypoTool_H
#define TRIGTAUHYPO_ITrigTauGenericHypoTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODTau/TauJetContainer.h"

/**
 * @brief Base for tools doing tau hypo selection
 **/
class ITrigTauGenericHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigTauGenericHypoTool, 1, 0);
  virtual ~ITrigTauGenericHypoTool(){}

  struct ClusterInfo {
  ClusterInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::TauJetContainer *c,
	       const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      roi( r ), 
      taucontainer(c), 
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision{nullptr};
    const TrigRoiDescriptor* roi{nullptr};
    const xAOD::TauJetContainer* taucontainer{nullptr};
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<ClusterInfo>& input ) const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const ClusterInfo& i ) const = 0;

};


#endif //> !TRIGTAUHYPO_ITrigTauGenericHypoTool_H
