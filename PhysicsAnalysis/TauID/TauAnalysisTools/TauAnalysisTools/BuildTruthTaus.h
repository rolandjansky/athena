/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUANALYSISTOOLS_BUILDTRUTHTAUS_H
#define  TAUANALYSISTOOLS_BUILDTRUTHTAUS_H

/*
  author: Dirk Duschinger
  documentation: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauTruthMatchingTool.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"

// Core include(s):
#include "MCTruthClassifier/IMCTruthClassifier.h"

// EDM include(s):
#include "xAODJet/JetContainer.h"

// Local include(s):
#include "TauAnalysisTools/IBuildTruthTaus.h"

#include "CxxUtils/CachedValue.h"
#include <atomic>

namespace TauAnalysisTools
{

class BuildTruthTaus
  : public virtual TauAnalysisTools::IBuildTruthTaus
  , public asg::AsgMetadataTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( BuildTruthTaus,
                  TauAnalysisTools::IBuildTruthTaus )

public:

  struct TruthTausEvent
    : public ITruthTausEvent
  {
    bool m_valid = false;
    const xAOD::TruthParticleContainer* m_xTruthTauContainerConst = nullptr;
    const xAOD::TruthParticleContainer* m_xTruthMuonContainerConst = nullptr;
    const xAOD::TruthParticleContainer* m_xTruthElectronContainerConst = nullptr;
    const xAOD::JetContainer* m_xTruthJetContainerConst = nullptr;
    xAOD::TruthParticleContainer* m_xTruthTauContainer = nullptr;
    xAOD::TruthParticleAuxContainer* m_xTruthTauAuxContainer = nullptr;
    const xAOD::TruthParticleContainer* m_xTruthParticleContainer = nullptr;
  };

  BuildTruthTaus( const std::string& name );

  virtual ~BuildTruthTaus();

  // initialize the tool
  virtual StatusCode initialize() override;

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer() override;

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer() override;

  virtual StatusCode retrieveTruthTaus() override;
  virtual StatusCode retrieveTruthTaus(ITruthTausEvent& truthTausEvent) const override;


protected:
  StatusCode retrieveTruthTaus(TruthTausEvent& truthTausEvent) const;


private:

  struct TauTruthInfo
  {
    size_t m_iNChargedPions = 0;
    size_t m_iNNeutralPions = 0;
    size_t m_iNChargedOthers = 0;
    size_t m_iNNeutralOthers = 0;
    size_t m_iNChargedDaughters = 0;
    std::vector<int> m_vDecayMode;
    // default false, if there is a hadron in decay products, it is switched to true
    bool m_bIsHadronicTau = false;

    // truth visible kinematic variables
    TLorentzVector m_vTruthVisTLV;
    TLorentzVector m_vTruthVisTLVCharged;
    TLorentzVector m_vTruthVisTLVNeutral;
  };

  // Execute at each event
  virtual StatusCode beginEvent() override;

  StatusCode buildTruthTausFromTruthParticles(TruthTausEvent& truthTausEvent) const;
  StatusCode examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const;
  StatusCode examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle,
                                  TauTruthInfo& truthInfo) const;
  void printDecay(const xAOD::TruthParticle& xTruthParticle, int depth = 0) const;

protected:

  // steering variables
  bool m_bWriteTruthTaus;
  mutable std::atomic<bool> m_bTruthTauAvailable;

  TruthTausEvent m_truthTausEvent;

  CxxUtils::CachedValue<bool> m_bIsTruthMatchedAvailable;

private:

  // steering variables
  std::string m_sNewTruthTauContainerName;
  std::string m_sNewTruthTauContainerNameAux;
  std::string m_sTruthTauContainerName;
  std::string m_sTruthMuonContainerName;
  std::string m_sTruthElectronContainerName;
  std::string m_sTruthJetContainerName;
  std::string m_sTruthParticlesContainerName;

  mutable std::atomic<bool> m_bTruthMuonAvailable;
  mutable std::atomic<bool> m_bTruthElectronAvailable;
  mutable std::atomic<bool> m_bTruthJetAvailable;

  bool m_bWriteInvisibleFourMomentum;
  bool m_bWriteVisibleChargedFourMomentum;
  bool m_bWriteVisibleNeutralFourMomentum;
  bool m_bWriteDecayModeVector;


private:

  asg::AnaToolHandle<IMCTruthClassifier> m_tMCTruthClassifier;

}; // class BuildTruthTaus

}
#endif // TAUANALYSISTOOLS_BUILDTRUTHTAUS_H
