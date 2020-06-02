/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_ICOMBOHYPOTOOL_H
#define DECISIONHANDLING_ICOMBOHYPOTOOL_H 1

#include "TrigCompositeUtils/TrigCompositeUtils.h" 
#include "GaudiKernel/IAlgTool.h"


/**
 * @class $(klass) 
 * @brief This class describe the base functionalities of a HypoTool used by the ComboAlg
 **/

 /**
  * @brief LegDecisionsMap describes the association between each decId and the decisions (via EL) associated to that 
  **/

typedef std::map<TrigCompositeUtils::DecisionID, ElementLinkVector<TrigCompositeUtils::DecisionContainer>> LegDecisionsMap;

 /**
  * @brief LegDecision is pair needed to use single elements, wihtout loosing the leg name
  **/
typedef std::pair <TrigCompositeUtils::DecisionID, ElementLink<TrigCompositeUtils::DecisionContainer> > LegDecision;


class IComboHypoTool: virtual public ::IAlgTool { 
  
 public: 
  DeclareInterfaceID(IComboHypoTool, 1, 0);

  virtual StatusCode decide( LegDecisionsMap & passingLegs, const EventContext& /* ctx */ ) const = 0 ;
  
 
}; 



#endif //> !DECISIONHANDLING_ICOMBOHYPOTOOL_H
