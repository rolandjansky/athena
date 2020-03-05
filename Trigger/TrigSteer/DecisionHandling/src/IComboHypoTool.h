/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_ICOMBOHYPOTOOL_H
#define DECISIONHANDLING_ICOMBOHYPOTOOL_H 1

#include "DecisionHandling/TrigCompositeUtils.h" 
#include "GaudiKernel/IAlgTool.h"


/**
 * @class $(klass) 
 * @brief This class describe the base functionalities of a HypoTool used by the ComboAlg
 **/

 /**
  * @brief describes the association between each decId and the decisions associated to that 
  **/

typedef std::map<TrigCompositeUtils::DecisionID, ElementLinkVector<TrigCompositeUtils::DecisionContainer>> LegDecisionsMap;

class IComboHypoTool: virtual public ::IAlgTool { 
  
 public: 
  DeclareInterfaceID(IComboHypoTool, 1, 0);

  virtual StatusCode decide( const LegDecisionsMap & IDCombMap, LegDecisionsMap & passingCombinations, const EventContext& ctx ) const = 0 ;
  
 
}; 



#endif //> !DECISIONHANDLING_ICOMBOHYPOTOOL_H
