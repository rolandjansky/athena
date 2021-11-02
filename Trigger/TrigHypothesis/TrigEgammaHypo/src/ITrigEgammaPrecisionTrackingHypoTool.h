/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITPRECISIONTrackingHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITPRECISIONTrackingHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODCaloEvent/CaloCluster.h"

/**
 * @class Base for tools doing precision Tracking Hypo selection
 * @brief 
 **/

class ITrigEgammaPrecisionTrackingHypoTool
  : virtual public ::IAlgTool
{ 

  public: 
    DeclareInterfaceID(ITrigEgammaPrecisionTrackingHypoTool, 1, 0);
    virtual ~ITrigEgammaPrecisionTrackingHypoTool(){}

    struct ClusterInfo {
      ClusterInfo( TrigCompositeUtils::Decision* d, const xAOD::CaloCluster_v1* c, const TrigCompositeUtils::Decision* previousDecision )
        : decision( d ),
          cluster( c ),
          previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
                             TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}
    
      TrigCompositeUtils::Decision* decision;
      const xAOD::CaloCluster_v1* cluster;
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
    virtual bool decide() const = 0;

  protected:


}; 


#endif //> !TRIGEGAMMAHYPO_ITPRECISIONTrackingHYPOTOOL_H
