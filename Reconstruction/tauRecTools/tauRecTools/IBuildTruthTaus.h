// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_IBUILDTRUTHTAUS_H
#define TAURECTOOLS_IBUILDTRUTHTAUS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

namespace tauRecTools
{

class IBuildTruthTaus :
  public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( tauRecTools::IBuildTruthTaus )

public:
  // initialize the tool
  virtual StatusCode initialize() = 0;

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer() = 0;

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer() = 0;

  virtual StatusCode retrieveTruthTaus() = 0;

}; // class IBuildTruthTaus

} // namespace tauRecTools

#endif // TAURECTOOLS_IBUILDTRUTHTAUS_H
