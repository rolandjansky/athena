/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "MuonSelectorTools/IMuonSelectionTool.h"


namespace DerivationFramework {

  class VHLowTrackJetFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    VHLowTrackJetFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    virtual ~VHLowTrackJetFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize() override;
    StatusCode  finalize() override;
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const override;
    
  private:
    mutable unsigned int m_nEventsTotal;
    mutable unsigned int m_nEventsPass;
    mutable unsigned int m_nEventsPassJet;
    mutable unsigned int m_nEventsPassElectron;
    mutable unsigned int m_nEventsPassMuon;
    mutable unsigned int m_nJetsPassAlphaMax;
    mutable unsigned int m_nJetsPassCHF;
    bool m_debug;
    std::string m_jetSGKey;
    float m_jetPtCut;
    float m_jetEtaCut;
    float m_TrackMinPt;
    float m_TrackZ0Max;
    float m_TrackD0Max;
    float m_AlphaMaxCut;
    float m_CHFCut;
    float m_nJetsReq;
    std::string m_electronSGKey;
    std::string m_electronIDKey;
    float m_electronPtCut;
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    std::string m_muonSGKey;
    std::string m_muonIDKey;
    float m_muonPtCut;
  }; 
  
}


#endif
