/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGHLTJETHYPOTOOL_H
#define TRIGHLTJETHYPO_ITRIGHLTJETHYPOTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/JetContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"

class ITrigHLTJetHypoTool : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID() {
     static const InterfaceID ITrigHLTJetHypoToolID("ITrigHLTJetHypoTool", 
                                                    1, 
                                                    0 );
     return ITrigHLTJetHypoToolID;
   }

  virtual StatusCode decide(const xAOD::JetContainer*, bool&) const = 0;
  virtual TrigCompositeUtils::DecisionID decisionId() const = 0; 
};
#endif
