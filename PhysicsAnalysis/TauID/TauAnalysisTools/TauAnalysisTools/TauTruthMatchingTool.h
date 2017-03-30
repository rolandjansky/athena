// Dear emacs, this is -*- c++ -*-
#ifndef  TAUANALYSISTOOLS_TAUTRUTHMATCHINGTOOL_H
#define  TAUANALYSISTOOLS_TAUTRUTHMATCHINGTOOL_H

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
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"

// Core include(s):
#include "TruthUtils/PIDHelpers.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

// EDM include(s):
#include "xAODTau/TauxAODHelpers.h"
#include "xAODJet/JetContainer.h"

// Local include(s):
#include "TauAnalysisTools/ITauTruthMatchingTool.h"

namespace TauAnalysisTools
{

enum eSampleType
{
  OTHER            = 0,
  PYTHIA           = 1,
  SHERPA           = 2,
};


class TauTruthMatchingTool
  : public virtual TauAnalysisTools::ITauTruthMatchingTool
  , public asg::AsgMetadataTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauTruthMatchingTool,
                  TauAnalysisTools::ITauTruthMatchingTool )

public:

  TauTruthMatchingTool( const std::string& name );

  virtual ~TauTruthMatchingTool();

  // initialize the tool
  virtual StatusCode initialize() override final;

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer() override final;

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer() override final;

  // apply match to a single tau
  virtual const xAOD::TruthParticle* applyTruthMatch(const xAOD::TauJet& xTau) override final;

  // apply match to all taus in a vector
  virtual std::vector<const xAOD::TruthParticle*> applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus) override final;

  // set pointer to truth particle container
  virtual StatusCode setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer)  override final __attribute__ ((deprecated("This function is deprecated. Please remove it from your code and use the configurable property \"TruthParticlesContainerName\" instead.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauTruthMatchingTool.rst")));

  // set pointer to event
  virtual StatusCode initializeEvent() override final __attribute__ ((deprecated("This function is deprecated. Please remove it from your code.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauTruthMatchingTool.rst")));

  // get pointer to truth tau, if no truth tau was found a null pointer is returned
  virtual const xAOD::TruthParticle* getTruth(const xAOD::TauJet& xTau) override final;

  StatusCode buildTruthTausFromTruthParticles() override final;
  StatusCode retrieveTruthTaus() override final;

  // wrapper function to get truth tau visible TLorentzvector
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TauJet& xTau) override final;
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) override final;

  // wrapper function to get truth tau prompt TLorentzvector
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TauJet& xTau) override final;
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) override final;

  // wrapper function to get truth tau invisible TLorentzvector
  virtual TLorentzVector getTruthTauP4Invis(const xAOD::TauJet& xTau) override final;
  virtual TLorentzVector getTruthTauP4Invis(const xAOD::TruthParticle& xTruthTau) override final;

  // get type of truth match particle (hadronic tau, leptonic tau, electron, muon, jet)
  virtual TauAnalysisTools::TruthMatchedParticleType getTruthParticleType(const xAOD::TauJet& xTau) override final;

  // wrapper function to count number of decay particles of given pdg id
  virtual int getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues = false) override final;
  virtual int getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues = false) override final;

  // wrapper function to obtain truth verion of xAOD::TauJetParameters::DecayMode
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TauJet& xTau) override final;
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TruthParticle& xTruthTau) override final;

private:

  // Execute at each event
  virtual StatusCode beginEvent() override final;

  int getNumPdgId(const xAOD::TauJet& xTau, std::vector<int> vPdgIdMatch) const;
  int getNumPdgId(const xAOD::TruthParticle& xTruthTau, std::vector<int> vPdgIdMatch) const;

  StatusCode findTruthTau(const xAOD::TauJet& xTau) const;

  StatusCode examineTruthTau(const xAOD::TruthParticle& xTruthParticle);
  StatusCode examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle);
  void printDecay(const xAOD::TruthParticle& xTruthParticle, int depth = 0) const;

  StatusCode checkTruthMatch (const xAOD::TauJet& xTau, const xAOD::TruthParticleContainer& xTauContainer) const;

private:
  bool m_bIsData;
  bool m_bIsConfigured;

  const xAOD::TruthParticleContainer* m_xTruthParticleContainer;
  const xAOD::TruthParticleContainer* m_xTruthTauContainerConst;
  const xAOD::TruthParticleContainer* m_xTruthMuonContainerConst;
  const xAOD::TruthParticleContainer* m_xTruthElectronContainerConst;
  const xAOD::JetContainer* m_xTruthJetContainerConst;
  xAOD::TruthParticleContainer* m_xTruthTauContainer;
  xAOD::TruthParticleAuxContainer* m_xTruthTauAuxContainer;

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

  bool m_bWriteInvisibleFourMomentum;
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

private:
  mutable bool m_bIsTruthMatchedAvailable;
  mutable bool m_bIsTruthMatchedAvailableChecked;
  mutable bool m_bNewEvent;

  SG::AuxElement::ConstAccessor<double> m_accPtVis;
  SG::AuxElement::ConstAccessor<double> m_accEtaVis;
  SG::AuxElement::ConstAccessor<double> m_accPhiVis;
  SG::AuxElement::ConstAccessor<double> m_accMVis;

  asg::AnaToolHandle<IMCTruthClassifier> m_tMCTruthClassifier;

}; // class TauTruthMatchingTool

}
#endif // TAUANALYSISTOOLS_TAUTRUTHMATCHINGTOOL_H
