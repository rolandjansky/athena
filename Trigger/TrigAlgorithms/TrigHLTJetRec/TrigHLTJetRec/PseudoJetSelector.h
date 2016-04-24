/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_PSEUDOJETSELECTOR_H
#define TRIGHLTJETREC_PSEUDOJETSELECTOR_H

#include "GaudiKernel/IAlgTool.h"
#include "JetEDM/PseudoJetVector.h"


class StatusCode;

class PseudoJetSelector: virtual public IAlgTool {

public:

  static const InterfaceID& interfaceID() {
    static const InterfaceID
      IID_PseudoJetSelector("PseudoJetSelector", 1, 0);
    return IID_PseudoJetSelector;
  }

  virtual ~PseudoJetSelector();

  virtual StatusCode select(PseudoJetVector&) const = 0;
  
};

#endif
