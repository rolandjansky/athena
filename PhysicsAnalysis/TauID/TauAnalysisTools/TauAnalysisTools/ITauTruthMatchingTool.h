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
  report any issues on JIRA: https://its.cern.ch/jira/browse/TAUAT/?selectedTab=com.atlassian.jira.jira-projects-plugin:issues-panel
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#ifndef __CINT__
// EDM include(s):
#include "xAODTau/TauJet.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
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
  virtual StatusCode createTruthTauContainer() __attribute__ ((deprecated("This function is deprecated. The TruthTau Container will be created automatically passing truth particle container via setTruthParticleContainer or passing Event via initializeEvent\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauTruthMatchingTool.rst"))) = 0;

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer() = 0;

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer() = 0;

  // apply match to a single tau
  virtual const xAOD::TruthParticle* applyTruthMatch(const xAOD::TauJet& xTau) = 0;

  // apply match to all taus in a vector
  virtual std::vector<const xAOD::TruthParticle*> applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus) = 0;

  // set pointer to truth particle container
  virtual StatusCode setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer) = 0;

  // set pointer to event
  virtual StatusCode initializeEvent() = 0;

  // get pointer to truth tau, if no truth tau was found a null pointer is returned
  virtual const xAOD::TruthParticle* getTruth(const xAOD::TauJet& xTau) const = 0;

  // wrapper function to get truth tau visible TLorentzvector
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TauJet& xTau) const = 0;
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) const = 0;

  // wrapper function to get truth tau prompt TLorentzvector
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TauJet& xTau) const = 0;
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) const = 0;

  // wrapper function to count number of decay particles of given pdg id
  virtual int getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues = false) const = 0;
  virtual int getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues = false) const = 0;

#ifdef XAODTAU_VERSIONS_TAUJET_V2_H
  // wrapper function to obtain truth verion of xAOD::TauJetParameters::DecayMode
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TauJet& xTau) const = 0;
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TruthParticle& xTruthTau) const = 0;
#endif // XAODTAU_VERSIONS_TAUJET_V2_H

#endif // not __MAKECINT__

}; // class ITauTruthMatchingTool

} // namespace TauAnalysisTools

#endif // ITAUTRUTHMATCHINGTOOL_H
