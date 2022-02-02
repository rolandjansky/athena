/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_ICOMBOHYPOTOOL_H
#define DECISIONHANDLING_ICOMBOHYPOTOOL_H 1

#include "TrigCompositeUtils/TrigCompositeUtils.h" 
#include "GaudiKernel/IAlgTool.h"


namespace Combo {

  /**
   * @brief Map from the chain name to the multiplicities required at each input. 
   * E.g. map["HLT_5j20_e10_2mu30"] = {5,1,2}
   **/
  typedef std::map<std::string, std::vector<int>> MultiplicityReqMap;

  /**
   * @brief Map from the chain name to legs required at each input. 
   * E.g. map["HLT_5j20_e10_2mu30"] = 
   **/
  typedef std::map<std::string, std::vector<int>> LegMap;

  /**
   * @brief LegDecisionsMap For a given chain leg key, this map holds all Decision Objects which are active on the leg.  
   **/
  typedef std::map<TrigCompositeUtils::DecisionID, std::vector<ElementLink<TrigCompositeUtils::DecisionContainer>>> LegDecisionsMap;

  /**
   * @brief LegDecision keeps a given Decision Object associated with a specific leg when being used inside a single combination.
   **/
  typedef std::pair<TrigCompositeUtils::DecisionID, ElementLink<TrigCompositeUtils::DecisionContainer> > LegDecision;
}


/**
 * @class $(klass) 
 * @brief This class describe the base functionalities of a HypoTool used by the ComboAlg
 **/
class IComboHypoTool: virtual public ::IAlgTool { 
  
 public: 
  DeclareInterfaceID(IComboHypoTool, 1, 0);

  virtual StatusCode decide(Combo::LegDecisionsMap& passingLegs, const EventContext& /*ctx*/) const = 0 ;
 
}; 


#endif //> !DECISIONHANDLING_ICOMBOHYPOTOOL_H
