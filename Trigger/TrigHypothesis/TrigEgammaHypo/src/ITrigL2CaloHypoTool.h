/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITRIGL2CALOHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITRIGL2CALOHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"





/**
 * @class Base for tools dooing L2 Calo Hypo selection
 * @brief 
 **/

class ITrigL2CaloHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigL2CaloHypoTool, 1, 0);
  virtual ~ITrigL2CaloHypoTool(){}

  struct ClusterInfo {
  ClusterInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::TrigEMCluster* c,
	       const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      roi( r ), 
      cluster(c), 
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::TrigEMCluster* cluster;
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


#endif //> !TRIGEGAMMAHYPO_ITRIGL2CALOHYPOTOOL_H
