/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITRIGPRECISIONELECTRONHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITRIGPRECISIONELECTRONHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


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
  ElectronInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::Electron_v1* c,
                bool pass_vl,
                bool pass_l,
                bool pass_m,
                bool pass_t,
                float lhval_vl,
                float lhval_l,
                float lhval_m,
                float lhval_t,
                float avg_mu,
	       const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      roi( r ), 
      electron(c), 
      accept_vl(pass_vl),
      accept_l(pass_l),
      accept_m(pass_m),
      accept_t(pass_t),
      LHValue_vl(lhval_vl),
      LHValue_l(lhval_l),
      LHValue_m(lhval_m),
      LHValue_t(lhval_t),
      avgMu (avg_mu),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::Electron_v1* electron;
    bool accept_vl;
    bool accept_l;
    bool accept_m;
    bool accept_t;
    float LHValue_vl;
    float LHValue_l;
    float LHValue_m;
    float LHValue_t;
    float avgMu;
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
