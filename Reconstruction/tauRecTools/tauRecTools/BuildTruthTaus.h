// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAURECTOOLS_BUILDTRUTHTAUS_H
#define  TAURECTOOLS_BUILDTRUTHTAUS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"

// Core include(s):
#include "MCTruthClassifier/IMCTruthClassifier.h"

// EDM include(s):
#include "xAODJet/JetContainer.h"

// Local include(s):
#include "tauRecTools/IBuildTruthTaus.h"

namespace tauRecTools
{

class BuildTruthTaus
  : public virtual tauRecTools::IBuildTruthTaus
  , public asg::AsgMetadataTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( BuildTruthTaus,
                  tauRecTools::IBuildTruthTaus )

public:

  BuildTruthTaus( const std::string& name );

  virtual ~BuildTruthTaus();

  // initialize the tool
  virtual StatusCode initialize();

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer();

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer();

  StatusCode retrieveTruthTaus();

private:
  // Execute at each event
  virtual StatusCode beginEvent();

  StatusCode buildTruthTausFromTruthParticles();
  StatusCode examineTruthTau(const xAOD::TruthParticle& xTruthParticle);
  StatusCode examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle);
  void printDecay(const xAOD::TruthParticle& xTruthParticle, int depth = 0) const;

protected:
  bool m_bIsData;
  bool m_bIsConfigured;

  // steering variables
  bool m_bWriteTruthTaus;
  bool m_bTruthTauAvailable;

  const xAOD::TruthParticleContainer* m_xTruthTauContainerConst;
  const xAOD::TruthParticleContainer* m_xTruthMuonContainerConst;
  const xAOD::TruthParticleContainer* m_xTruthElectronContainerConst;
  const xAOD::JetContainer* m_xTruthJetContainerConst;
  xAOD::TruthParticleContainer* m_xTruthTauContainer;

  bool m_bIsTruthMatchedAvailable;
  bool m_bIsTruthMatchedAvailableChecked;
  bool m_bNewEvent;

  bool m_bTruthMuonAvailable;
  bool m_bTruthElectronAvailable;
  bool m_bTruthJetAvailable;
  
private:

  const xAOD::TruthParticleContainer* m_xTruthParticleContainer;
  xAOD::TruthParticleAuxContainer* m_xTruthTauAuxContainer;

  // steering variables
  std::string m_sNewTruthTauContainerName;
  std::string m_sNewTruthTauContainerNameAux;
  std::string m_sTruthTauContainerName;
  std::string m_sTruthMuonContainerName;
  std::string m_sTruthElectronContainerName;
  std::string m_sTruthJetContainerName;
  std::string m_sTruthParticlesContainerName;


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
  TLorentzVector m_vTruthVisTLVNeutralPions;
  TLorentzVector m_vTruthVisTLVNeutralOthers;  

  bool m_bIsHadronicTau;

private:

  asg::AnaToolHandle<IMCTruthClassifier> m_tMCTruthClassifier;

}; // class BuildTruthTaus

}
#endif // TAURECTOOLS_BUILDTRUTHTAUS_H
