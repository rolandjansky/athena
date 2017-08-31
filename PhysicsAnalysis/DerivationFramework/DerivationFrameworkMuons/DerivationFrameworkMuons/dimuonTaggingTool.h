/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// dimuonTaggingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DIMUONTAGGINGTOOL_H
#define DERIVATIONFRAMEWORK_DIMUONTAGGINGTOOL_H

#include<string>
#include<vector>
#include<map>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// xAOD header files
#include "xAODMuon/MuonContainer.h"

class IMuonTPExtrapolationTool;
class IIDTrackCaloDepositsDecoratorTool;

namespace DerivationFramework {
  /** @class dimuonTaggingTool
      @author Dongliang.Zhang@cern.ch
     */
  class dimuonTaggingTool : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    dimuonTaggingTool( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~dimuonTaggingTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
    
  private:
    bool passMuonCuts(const xAOD::Muon *mu, const float ptMin, const float absEtaMax, const std::vector< int >& types, const std::vector< std::string >& trigs, const std::map< int, double > muIsoCuts) const;
    bool passMuonCuts(const xAOD::Muon *mu, const float ptMin, const float absEtaMax, const std::vector< int >& types, const std::vector< std::string >& trigs) const;
    bool checkTrigMatch(const xAOD::Muon *mu, const std::vector< std::string >& Trigs) const;
    bool muonPairCheck(const xAOD::Muon *mu1, float charge2, const TLorentzVector& mu2) const;
    bool passKinematicCuts(const xAOD::IParticle *mu, float ptMin, float absEtaMax) const; 
    StatusCode fillInfo(int* keep, std::vector<int>& trackMask) const;
    void maskNearbyIDtracks(const xAOD::IParticle *mu, std::vector< int >& trackMask, const xAOD::TrackParticleContainer* tracks) const;

    ToolHandle< IMuonTPExtrapolationTool > m_tpExpTool;
    ToolHandle< IIDTrackCaloDepositsDecoratorTool > m_caloDepoTool;
    ToolHandle< Trig::ITrigMuonMatching > m_matchTool;
    float m_triggerMatchDeltaR;
    ToolHandle< Trig::TrigDecisionTool > m_trigDecisionTool;
    std::vector< std::string > m_orTrigs;
    std::vector< std::string > m_andTrigs;

    std::string m_muonSGKey;
    float m_mu1PtMin;
    float m_mu1AbsEtaMax;
    std::vector< int > m_mu1Types;
    std::vector< std::string > m_mu1Trigs;
    std::map< int, double > m_mu1IsoCuts;
    float m_mu2PtMin;
    float m_mu2AbsEtaMax;
    std::vector< int > m_mu2Types;
    std::vector< std::string > m_mu2Trigs;
    std::map< int, double > m_mu2IsoCuts;

    std::string m_trackSGKey;
    bool m_useTrackProbe;
    std::string m_br_prefix;
//     SG::AuxElement::Decorator< int >* deco_trkFlag;

    bool m_requireOS;
    float m_dPhiMin;
    float m_invariantMassLow;
    float m_invariantMassHigh;
    float m_invariantMassLow2;
    float m_invariantMassHigh2;

    float m_thinningConeSize;
    float m_thinningConeSize2;
  }; 
}
#endif // DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H
