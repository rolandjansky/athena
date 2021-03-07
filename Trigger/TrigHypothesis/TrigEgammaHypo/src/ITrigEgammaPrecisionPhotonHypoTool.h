/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITRIGPRECISIONPHOTONHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITRIGPRECISIONPHOTONHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODEgamma/Photon.h"

/**
 * @class Base for tools dooing precision Photon Hypo selection
 * @brief 
 **/

class ITrigEgammaPrecisionPhotonHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigEgammaPrecisionPhotonHypoTool, 1, 0);
  virtual ~ITrigEgammaPrecisionPhotonHypoTool(){}

  struct PhotonInfo {
  PhotonInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::Photon_v1* c,
	       const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      roi( r ), 
      photon(c), 
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::Photon_v1* photon;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all photons
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<PhotonInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const PhotonInfo& i ) const = 0;



}; 


#endif //> !TRIGEGAMMAHYPO_ITRIGPRECISIONPHOTONHYPOTOOL_H
