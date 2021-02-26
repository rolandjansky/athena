/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VHLowTrackJetFilterTool.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_VHLOWTRACKJETFILTERTOOL_H
#define DERIVATIONFRAMEWORK_VHLOWTRACKJETFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "StoreGate/ReadHandleKey.h"


namespace DerivationFramework {

  class VHLowTrackJetFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    VHLowTrackJetFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    virtual ~VHLowTrackJetFilterTool() = default;
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize() override;
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const override;
    
  private:
    mutable std::atomic<unsigned int> m_nEventsTotal;
    mutable std::atomic<unsigned int> m_nEventsPass;
    mutable std::atomic<unsigned int> m_nEventsPassJet;
    mutable std::atomic<unsigned int> m_nEventsPassElectron;
    mutable std::atomic<unsigned int> m_nEventsPassMuon;
    mutable std::atomic<unsigned int> m_nJetsPassAlphaMax;
    mutable std::atomic<unsigned int> m_nJetsPassCHF;
    bool m_debug;
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey { this, "JetContainerKey", "AntiKt4EMTopoJets", ""};
    float m_jetPtCut;
    float m_jetEtaCut;
    float m_TrackMinPt;
    float m_TrackZ0Max;
    float m_TrackD0Max;
    float m_AlphaMaxCut;
    float m_CHFCut;
    float m_nJetsReq;
    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronSGKey { this, "ElectronContainerKey", "Electrons", ""};
    std::string m_electronIDKey;
    float m_electronPtCut;
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    SG::ReadHandleKey<xAOD::MuonContainer> m_muonSGKey { this, "MuonContainerKey", "Muons", ""};
    std::string m_muonIDKey;
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey {this, "EventInfoKey", "EventInfo", ""}; 
    SG::ReadHandleKey<xAOD::VertexContainer> m_primaryVerticesKey {this, "PrimaryVerticesKey", "PrimaryVertices", ""}; 
    float m_muonPtCut;

  }; 
  
}


#endif
