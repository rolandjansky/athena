/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITRIGEGAMMAFASTCALOHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITRIGEGAMMAFASTCALOHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"





/**
 * @class Base for tools dooing Egamma Fast Calo Hypo selection
 * @brief 
 **/

class ITrigEgammaFastCaloHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigEgammaFastCaloHypoTool, 1, 0);
  virtual ~ITrigEgammaFastCaloHypoTool(){}

  struct FastClusterInfo {
  FastClusterInfo( TrigCompositeUtils::Decision* d, 
               const TrigRoiDescriptor* r, 
               const xAOD::TrigEMCluster* c,
	       const xAOD::TrigRingerRings* ring,
               const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
    roi( r ), 
    cluster(c), 
    ringerShape(ring),
    previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::TrigEMCluster* cluster;
    const xAOD::TrigRingerRings* ringerShape;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<FastClusterInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const FastClusterInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGEGAMMAHYPO_ITRIGEGAMMAFASTCALOHYPOTOOL_H
