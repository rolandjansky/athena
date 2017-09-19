// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_IBUILDTRUTHTAUS_H
#define TAUANALYSISTOOLS_IBUILDTRUTHTAUS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

namespace TauAnalysisTools
{

class IBuildTruthTaus :
  public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::IBuildTruthTaus )

public:
  // initialize the tool
  virtual StatusCode initialize() = 0;

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer() = 0;

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer() = 0;

  virtual StatusCode retrieveTruthTaus() = 0;

}; // class IBuildTruthTaus

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_IBUILDTRUTHTAUS_H
