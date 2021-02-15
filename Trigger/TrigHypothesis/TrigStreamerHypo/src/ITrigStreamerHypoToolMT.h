/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTREAMER_ITRIGSTREAMERHYPOTOOLMT_H
#define TRIGSTREAMER_ITRIGSTREAMERHYPOTOOLMT_H

#include "AsgTools/IAsgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include <vector>


class ITrigStreamerHypoToolMT : virtual public::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigStreamerHypoToolMT, 1, 0);

  struct HypoInfo {
    HypoInfo( TrigCompositeUtils::Decision* newD, const TrigCompositeUtils::Decision* previousD )
    : m_newDecision( newD ), 
      m_previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousD ).begin(), 
                             TrigCompositeUtils::decisionIDs( previousD ).end() )
    {}
    TrigCompositeUtils::Decision* m_newDecision;
    const TrigCompositeUtils::DecisionIDContainer m_previousDecisionIDs;
  };

  virtual ~ITrigStreamerHypoToolMT(){};
  virtual StatusCode decide(std::vector<HypoInfo>&) const = 0;
  virtual const HLT::Identifier& getId() const = 0; 


};
#endif
