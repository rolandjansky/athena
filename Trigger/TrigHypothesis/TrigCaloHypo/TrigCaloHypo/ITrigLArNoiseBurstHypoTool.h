
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
#include "Identifier/HWIdentifier.h"


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

  struct CaloCellNoiseInfo {
  CaloCellNoiseInfo( TrigCompositeUtils::Decision* d, 
                     const TrigRoiDescriptor* r,
                     const CaloCellContainer* c,
                     const std::set<unsigned int>* bf,
                     const std::vector<HWIdentifier>* MNBfeb,
                     const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
    roi( r ), 
    cells(c), 
    knownBadFEBs(bf), knownMNBFEBs(MNBfeb),
    previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
               TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const CaloCellContainer* cells;
    const std::set<unsigned int>* knownBadFEBs;
    const std::vector<HWIdentifier>* knownMNBFEBs;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  
  
  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  virtual StatusCode decide( std::vector<CaloCellNoiseInfo>& input )  const = 0;

  /**
   * @brief Makes a decision for a single object
   * The decision needs to be returned
   **/ 
  virtual bool decide( const CaloCellNoiseInfo& i ) const = 0;

 protected:


}; 


#endif //> !TRIGCALOHYPO_ITRIGLARNOISEBURSTHYPOTOOL_H
