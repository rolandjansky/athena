
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_ITRIGLARNOISEBURSTHYPOTOOL_H
#define TRIGCALOHYPO_ITRIGLARNOISEBURSTHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


/**
 * @class Base for tools dooing LArNoiseBurst Hypo selection
 * @brief 
 **/

class ITrigLArNoiseBurstHypoTool
  : virtual public ::IAlgTool
{ 

 public: 
  DeclareInterfaceID(ITrigLArNoiseBurstHypoTool, 1, 0);
  virtual ~ITrigLArNoiseBurstHypoTool(){}

  struct FlagNoiseInfo {
  FlagNoiseInfo( TrigCompositeUtils::Decision* d, 
		     unsigned int& f,
                     const TrigRoiDescriptor* r,
                     const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
    flag(f),
    roi( r ), 
    previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
               TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    unsigned int& flag;
    const TrigRoiDescriptor* roi;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<FlagNoiseInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const FlagNoiseInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGCALOHYPO_ITRIGLARNOISEBURSTHYPOTOOL_H
