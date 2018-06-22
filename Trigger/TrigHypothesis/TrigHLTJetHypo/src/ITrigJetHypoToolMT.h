/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOTOOLMT_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOTOOLMT_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/JetContainer.h"

#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

class ITrigJetHypoToolMT : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetHypoToolMT, 1, 0);
  virtual ~ITrigJetHypoToolMT(){};
  virtual StatusCode decide(const xAOD::JetContainer*,
                            std::unique_ptr<DecisionContainer>&,
                            const DecisionContainer*) const = 0;
};
#endif
