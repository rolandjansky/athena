/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_IPSEUDOJETSELECTOR_H
#define TRIGHLTJETREC_IPSEUDOJETSELECTOR_H

#include "GaudiKernel/IAlgTool.h"
#include "JetEDM/PseudoJetVector.h"

class StatusCode;

class IPseudoJetSelector: virtual public IAlgTool {

public:

  static const InterfaceID& interfaceID() {
    static const InterfaceID
      IID_IPseudoJetSelector("IPseudoJetSelector", 1, 0);
    return IID_IPseudoJetSelector;
  }

  virtual ~IPseudoJetSelector();

  // Pseudojet vector setter
  virtual StatusCode select(const jet::PseudoJetVector&, jet::PseudoJetVector&) const = 0;
  
};

#endif
