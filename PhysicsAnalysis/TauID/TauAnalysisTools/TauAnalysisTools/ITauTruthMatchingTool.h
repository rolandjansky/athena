// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUTRUTHMATCHINGTOOL_H
#define ITAUTRUTHMATCHINGTOOL_H

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

#ifndef __CINT__
// EDM include(s):
#include "xAODTau/TauJet.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertex.h"
#endif // not __MAKECINT__

namespace TauAnalysisTools
{

class ITauTruthMatchingTool :
  public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauTruthMatchingTool )

public:
  // initialize the tool
  virtual StatusCode initialize() = 0;

#ifndef __CINT__
  // create TruthTauContainer
  virtual StatusCode createTruthTauContainer() = 0;

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer() = 0;

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer() = 0;

  // apply match to a single tau
  virtual const xAOD::TruthParticle* applyTruthMatch(const xAOD::TauJet& xTau) = 0;

  // apply match to all taus in a vector
  virtual std::vector<const xAOD::TruthParticle*> applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus) = 0;
#endif // not __MAKECINT__

  // set pointer to truth particle container
  virtual StatusCode setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer) = 0;

}; // class ITauTruthMatchingTool

} // namespace TauAnalysisTools

#endif // ITAUTRUTHMATCHINGTOOL_H
