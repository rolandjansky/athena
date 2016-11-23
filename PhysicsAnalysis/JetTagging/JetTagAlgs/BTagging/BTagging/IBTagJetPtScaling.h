/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGJETPTSCALING_H
#define BTAGGING_IBTAGJETPTSCALING_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/JetContainer.h"

namespace Analysis {

  static const InterfaceID IID_IBTagJetPtScaling("IBTagJetPtScaling", 1, 0);

  class IBTagJetPtScaling : virtual public IAlgTool {
  public:

    /** Virtual destructor */
    virtual ~IBTagJetPtScaling(){};

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IBTagJetPtScaling; };

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    virtual StatusCode BTagJetPtScaling_exec(xAOD::JetContainer& jets) const = 0;
  };

} // End namespace

#endif
