/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOTOOLMT_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOTOOLMT_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/JetContainer.h"

#include "DecisionHandling/TrigCompositeUtils.h"

namespace HLT{
  class Identifier;
}

class ITrigJetHypoToolMT : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetHypoToolMT, 1, 0);
  virtual ~ITrigJetHypoToolMT(){};
  // make a decision (yes/no) based on the jets in the jet container,
  // set the new decision object according to this decision, and previous
  // devicsion objects. Jets typically have only a decision object
  // generated at L1.
  virtual StatusCode decide(const xAOD::JetContainer*, bool& pass) const = 0;
  virtual const HLT::Identifier& getId() const = 0; 

};
#endif
