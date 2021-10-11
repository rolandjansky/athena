
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_ITRIGL2CALOLAYERSHYPOTOOL_H
#define TRIGCALOHYPO_ITRIGL2CALOLAYERSHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"


/**
 * @class Base for tools dooing CaloLayers Hypo selection
 * @brief 
 **/

class ITrigL2CaloLayersHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigL2CaloLayersHypoTool, 1, 0);
  virtual ~ITrigL2CaloLayersHypoTool(){}

  struct CaloLayersInfo {
  CaloLayersInfo( TrigCompositeUtils::Decision* d, 
                     const xAOD::TrigEMClusterContainer* c,
                     const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
    clusters( c ),
    previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
               TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigEMClusterContainer* clusters;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<CaloLayersInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const CaloLayersInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGCALOHYPO_ITRIGL2CALOLAYERSHYPOTOOL_H
