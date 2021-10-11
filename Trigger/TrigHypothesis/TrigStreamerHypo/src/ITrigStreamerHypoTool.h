/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTREAMER_ITRIGSTREAMERHYPOTOOL_H
#define TRIGSTREAMER_ITRIGSTREAMERHYPOTOOL_H

#include "AsgTools/IAsgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include <vector>


class ITrigStreamerHypoTool : virtual public::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigStreamerHypoTool, 1, 0);

  struct HypoInfo {
    HypoInfo( TrigCompositeUtils::Decision* newD, const TrigCompositeUtils::Decision* previousD )
    : m_newDecision( newD ), 
      m_previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousD ).begin(), 
                             TrigCompositeUtils::decisionIDs( previousD ).end() )
    {}
    TrigCompositeUtils::Decision* m_newDecision;
    const TrigCompositeUtils::DecisionIDContainer m_previousDecisionIDs;
  };

  virtual ~ITrigStreamerHypoTool(){};
  virtual StatusCode decide(std::vector<HypoInfo>&) const = 0;
  virtual const HLT::Identifier& getId() const = 0; 


};
#endif
