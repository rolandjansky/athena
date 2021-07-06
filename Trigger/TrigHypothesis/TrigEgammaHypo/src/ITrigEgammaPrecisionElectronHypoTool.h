/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITRIGPRECISIONELECTRONHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITRIGPRECISIONELECTRONHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODEgamma/Electron.h"

/**
 * @class Base for tools doing precision Electron Hypo selection
 * @brief 
 **/

class ITrigEgammaPrecisionElectronHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigEgammaPrecisionElectronHypoTool, 1, 0);
  virtual ~ITrigEgammaPrecisionElectronHypoTool(){}

  struct ElectronInfo {
  ElectronInfo( TrigCompositeUtils::Decision* d, 
                const TrigRoiDescriptor* r, 
                const xAOD::Electron* c,
                const TrigCompositeUtils::Decision* previousDecision ): 
      decision( d ), 
      roi( r ), 
      electron(c), 
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
         TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::Electron* electron;
    std::map<std::string, bool> pidDecorator;
    std::map<std::string, float> valueDecorator;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all electrons
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<ElectronInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const ElectronInfo& i ) const = 0;



}; 


#endif //> !TRIGEGAMMAHYPO_ITRIGPRECISIONELECTRONHYPOTOOL_H
