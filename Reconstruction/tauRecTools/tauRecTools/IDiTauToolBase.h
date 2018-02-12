/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_IDITAUTOOLBASE_H
#define TAURECTOOLS_IDITAUTOOLBASE_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"

namespace tauRecTools
{

  class IDiTauToolBase :
    public virtual asg::IAsgTool
  {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( tauRecTools::IDiTauToolBase )

    public:
    // calculate ID variables
    virtual StatusCode execute(const xAOD::DiTauJet& xDiTau) = 0;

  }; // class IDiTauToolBase

} // namespace tauRecTools

#endif // TAURECTOOLS_IDITAUTOOLBASE_H


