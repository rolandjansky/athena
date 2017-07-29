/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGHLTJETHYPOTOOL_H
#define TRIGHLTJETHYPO_ITRIGHLTJETHYPOTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/JetContainer.h"

class ITrigHLTJetHypoTool : virtual public IAlgTool {
public:
  virtual StatusCode decide(const xAOD::JetContainer*, bool&) const = 0;
   static const InterfaceID& interfaceID() {
     static const InterfaceID _ITrigHLTJetHypoToolID("ITrigHLTJetHypoTool", 
                                                     1, 
                                                     0 );
     return _ITrigHLTJetHypoToolID;
   }
};
#endif
