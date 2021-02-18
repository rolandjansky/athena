/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_KINKTRKSINGLEJETMETFILTERTOOL_H
#define DERIVATIONFRAMEWORK_KINKTRKSINGLEJETMETFILTERTOOL_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {
  class KinkTrkSingleJetMetFilterTool : public AthAlgTool, public ISkimmingTool {

  public: 
    /** Constructor with parameters */
    KinkTrkSingleJetMetFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~KinkTrkSingleJetMetFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    bool m_passAll;
    bool m_LeptonVeto;
    bool m_isolatedTrack;
    float m_metHtCut;
    std::string m_metTerm;
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    std::string m_muonIDKey;
    std::string m_electronIDKey;
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey { this, "JetContainerKey", "AntiKt4LCTopoJets", ""};
    SG::ReadHandleKey<xAOD::MissingETContainer> m_metSGKey { this, "MetContainerKey", "MET_RefFinal", ""};
    SG::ReadHandleKey<xAOD::MuonContainer> m_muonSGKey { this, "MuonContainerKey", "Muons", ""};
    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronSGKey { this, "ElectronContainerKey", "ElectronCollection", ""};
    float m_metCut;
    std::vector<float> m_jetPtCuts;
    float m_jetEtaMax;
    int   m_jetNumCut;
    float m_jetMetDphiMin;
    float m_jetMetPtMin;
    float m_leptonPtCut;
    float m_leptonEtaMax;

  }; 
  
}


#endif
