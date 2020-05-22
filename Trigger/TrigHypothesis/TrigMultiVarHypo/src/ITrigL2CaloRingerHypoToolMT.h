/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMULTIVARHYPO_ITRIGL2CALORINGERHYPOTOOLMT_H
#define TRIGMULTIVARHYPO_ITRIGL2CALORINGERHYPOTOOLMT_H 1

#include "GaudiKernel/IAlgTool.h"



#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


/**
 * @class Base for tools dooing L2 Calo Hypo selection
 * @brief 
 **/

class ITrigL2CaloRingerHypoToolMT
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigL2CaloRingerHypoToolMT, 1, 0);
  virtual ~ITrigL2CaloRingerHypoToolMT(){}
 
  
  struct RingerInfo {
    RingerInfo( TrigCompositeUtils::Decision* d,
                const TrigRoiDescriptor* k,
                const xAOD::TrigRingerRings* r,
                const TrigCompositeUtils::Decision* previousDecision )
      : decision( d ), 
      roi( k ),
      ringerShape( r ), 
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
      TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}
      TrigCompositeUtils::Decision* decision;
      const TrigRoiDescriptor* roi;
      const xAOD::TrigRingerRings* ringerShape;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      };

  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<RingerInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const RingerInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGMULTIVARHYPO_ITRIGL2CALORINGERHYPOTOOLMT_H
