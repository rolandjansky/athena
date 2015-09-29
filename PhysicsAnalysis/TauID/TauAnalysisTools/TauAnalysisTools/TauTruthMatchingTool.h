// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUTRUTHMATCHINGTOOL_H
#define  TAUTRUTHMATCHINGTOOL_H

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
#include "AsgTools/AsgTool.h"

// Core include(s):
#include "TruthUtils/PIDHelpers.h"

#ifndef __CINT__
// EDM include(s):
#include "xAODTau/TauJet.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODJet/JetContainer.h"
#endif // not __MAKECINT__

// Local include(s):
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "TauAnalysisTools/Enums.h"

namespace TauAnalysisTools
{

enum eSampleType
{
  OTHER            = 0,
  PYTHIA           = 1,
  SHERPA           = 2,
};


class TauTruthMatchingTool
  : public TauAnalysisTools::ITauTruthMatchingTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauTruthMatchingTool,
                  TauAnalysisTools::ITauTruthMatchingTool )

public:

  TauTruthMatchingTool( const std::string& name );

  virtual ~TauTruthMatchingTool();

  // initialize the tool
  virtual StatusCode initialize();

#ifndef __MAKECINT__
  // create TruthTauContainer
  virtual StatusCode createTruthTauContainer() __attribute__ ((deprecated("This function is deprecated. The TruthTau Container will be created automatically passing truth particle container via setTruthParticleContainer or passing Event via initializeEvent.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauTruthMatchingTool.rst")));

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer();

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer();

  // apply match to a single tau
  virtual const xAOD::TruthParticle* applyTruthMatch(const xAOD::TauJet& xTau);

  // apply match to all taus in a vector
  virtual std::vector<const xAOD::TruthParticle*> applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus);

  // set pointer to truth particle container
  virtual StatusCode setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer);

  // set pointer to event
  virtual StatusCode initializeEvent();

  // get pointer to truth tau, if no truth tau was found a null pointer is returned
  virtual const xAOD::TruthParticle* getTruth(const xAOD::TauJet& xTau) const;

  // wrapper function to get truth tau visible TLorentzvector
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TauJet& xTau) const;
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) const;

  // wrapper function to get truth tau prompt TLorentzvector
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TauJet& xTau) const;
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) const;

  // wrapper function to count number of decay particles of given pdg id
  virtual int getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues = false) const;
  virtual int getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues = false) const;

#ifdef XAODTAU_VERSIONS_TAUJET_V2_H
  // wrapper function to obtain truth verion of xAOD::TauJetParameters::DecayMode
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TauJet& xTau) const;
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TruthParticle& xTruthTau) const;
#endif // XAODTAU_VERSIONS_TAUJET_V2_H

#endif // not __MAKECINT__

private:
  int getNumPdgId(const xAOD::TauJet& xTau, std::vector<int> vPdgIdMatch) const;
  int getNumPdgId(const xAOD::TruthParticle& xTruthTau, std::vector<int> vPdgIdMatch) const;

  StatusCode findTruthTau(const xAOD::TauJet& xTau) const;
  StatusCode retrieveTruthTaus();
  StatusCode buildTruthTausFromTruthParticles();

  StatusCode examineTruthTau(const xAOD::TruthParticle& xTruthParticle);
  StatusCode examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle);
  void printDecay(const xAOD::TruthParticle& xTruthParticle, int depth = 0) const;

  StatusCode checkTruthMatch (const xAOD::TauJet& xTau, const xAOD::TruthParticleContainer& xTauContainer) const;

#ifndef __MAKECINT__
  const xAOD::TruthParticleContainer* m_xTruthParticleContainer;
  const xAOD::TruthParticleContainer* m_xTruthTauContainerConst;
  const xAOD::TruthParticleContainer* m_xTruthMuonContainerConst;
  const xAOD::TruthParticleContainer* m_xTruthElectronContainerConst;
  const xAOD::JetContainer* m_xTruthJetContainerConst;
  xAOD::TruthParticleContainer* m_xTruthTauContainer;
  xAOD::TruthParticleAuxContainer* m_xTruthTauAuxContainer;
#endif // not __MAKECINT__

  // steering variables
  double m_dMaxDeltaR;
  bool m_bWriteTruthTaus;
  std::string m_sNewTruthTauContainerName;
  std::string m_sNewTruthTauContainerNameAux;
  std::string m_sTruthTauContainerName;
  std::string m_sTruthMuonContainerName;
  std::string m_sTruthElectronContainerName;
  std::string m_sTruthJetContainerName;
  std::string m_sTruthParticlesContainerName;

  bool m_bTruthTauAvailable;
  bool m_bTruthMuonAvailable;
  bool m_bTruthElectronAvailable;
  bool m_bTruthJetAvailable;

  bool m_bWriteVisibleChargedFourMomentum;
  bool m_bWriteVisibleNeutralFourMomentum;
  bool m_bWriteDecayModeVector;

  size_t m_iNChargedPions;
  size_t m_iNNeutralPions;
  size_t m_iNChargedOthers;
  size_t m_iNNeutralOthers;
  size_t m_iNChargedDaughters;
  std::vector<int> m_vDecayMode;

  TLorentzVector m_vTruthVisTLV;
  TLorentzVector m_vTruthVisTLVCharged;
  TLorentzVector m_vTruthVisTLVNeutral;

  bool m_bIsHadronicTau;

  // deprecated variables
  int m_iSampleType;
  bool m_bOptimizeForReco;
}; // class TauTruthMatchingTool

}
#endif // TAUTRUTHMATCHINGTOOL_H
