/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2BMuMuXFex.h
 **
 **   Description: L2 hypothesis algorithms for B+/- -> K+/- mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: Pavel.Strachota@cern.ch
 **           Semen.Turchikhin@cern.ch
 **
 **   Created:   10.01.2010 based on C. Adorisio's code TrigL2DiMuFex.h & D. Scheirich's code TrigL2LbMuMuLFex.h
 **
 **   Modified:  26.09.2012 : "B_c -> D_s(*) (->Phi(->K+K-)) mu+mu-" added (Leonid Gladilin <gladilin@mail.cern.ch>)
 **   Modified:  
 **
 **************************************************************************/ 

#ifndef TrigL2BMuMuXFex_h 
#define TrigL2BMuMuXFex_h

#include "TrigInterfaces/ComboAlgo.h"
#include "BtrigUtils.h"


#include "TrigInDetToolInterfaces/ITrigVertexingTool.h"

#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"                         

#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

class TrigBphysHelperUtilsTool;

/// addon, not needed 
class TriggerElement;
class ITrigVertexFitter;
class ITrigL2VertexFitter;
class ITrigVertexingTool;
//

class TrigL2BMuMuXFex: public HLT::ComboAlgo                                     
{
  public:
    TrigL2BMuMuXFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2BMuMuXFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTEs, bool& pass );           
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTEs, HLT::TriggerElement* outputTE );   
            
  private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

    
    void checkBMuMuK(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                     const xAOD::TrackParticleContainer* trkCollection, int iTrk3);
    void checkBdMuMuKstar(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2,
                          const xAOD::TrackParticle* trk3, const xAOD::TrackParticle* trk4, const xAOD::TrackParticleContainer* trkCollection, int iTrk3, int iTrk4);
    void checkBsMuMuPhi(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                        const xAOD::TrackParticle* trk4, const xAOD::TrackParticleContainer* trkCollection, int iTrk3, int iTrk4);
    void checkLbMuMuLambda(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                           const xAOD::TrackParticle* trk4, const xAOD::TrackParticleContainer* trkCollection, int iTrk3, int iTrk4);
    void checkBcMuMuDs(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                       const xAOD::TrackParticle* trk4, const xAOD::TrackParticle* trk5, const xAOD::TrackParticleContainer* trkCollection,
                       int iTrk3, int iTrk4, double xPhiMass, int iTrk5);

    
    double KMuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2, const xAOD::TrackParticle* kaon);
    double XMass    (const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, int decay);
    double XMuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2,
                     const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, int decay);
    double X3Mass    (const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3);
    double X3MuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2,
                      const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3);

    
    // cuts - configurable by python config file
    BooleanProperty m_oppositeCharge; //require opposite charge for muons
//    float m_cutMuon;
    float m_lowerMuMuMassCut;         //mass of Mu Mu pair
    float m_upperMuMuMassCut;
    float m_lowerMuVtxMassCut;         //mass of Mu Mu pair after vertexing
    float m_upperMuVtxMassCut;
    float m_muVtxChi2Cut;

    // Maximum number of track combinations to try -- protection against TimeOuts
    int   m_maxNcombinations;
    
    // B+->K+MuMu cuts
    float m_lowerKMuMuMassCut;                                                  // cut on mass of combination K Mu Mu
    float m_upperKMuMuMassCut;
    float m_lowerBVtxMassCut;                                                   //mass of B after vertexing
    float m_upperBVtxMassCut;
    float m_bVtxChi2Cut;     
    // vertexing off
    float m_lowerB_KMuMuMassCutVtxOff;
    float m_upperB_KMuMuMassCutVtxOff;
                                                   

    // Bd->K*MuMu cuts
    float m_lowerKstarVtxMassCut;
    float m_upperKstarVtxMassCut;
    float m_lowerKstar_KaonMassCut;
    float m_upperKstar_KaonMassCut;
    float m_lowerBd_KstarMuMuMassCut;
    float m_upperBd_KstarMuMuMassCut; 
    float m_kStarVtxChi2Cut;    
    float m_lowerBdVtxMassCut;
    float m_upperBdVtxMassCut;
    float m_bDVtxChi2Cut;
    // vertexing off
    float m_lowerBd_KstarMuMuMassCutVtxOff;
    float m_upperBd_KstarMuMuMassCutVtxOff;

    // Bs->PhiMuMu cuts
    float m_lowerPhi1020VtxMassCut;
    float m_upperPhi1020VtxMassCut;
    float m_lowerPhi1020_KaonMassCut;
    float m_upperPhi1020_KaonMassCut;
    float m_phi1020VtxChi2Cut;
    float m_lowerBs_Phi1020MuMuMassCut;                                                        
    float m_upperBs_Phi1020MuMuMassCut;   
    float m_lowerBsVtxMassCut;                                                        
    float m_upperBsVtxMassCut;   
    float m_bSVtxChi2Cut;                    
    // vertexing off
    float m_lowerBs_Phi1020MuMuMassCutVtxOff;
    float m_upperBs_Phi1020MuMuMassCutVtxOff;

    // Lb->LMuMu cuts
    float m_lowerLambda_PrPiMassCut;
    float m_upperLambda_PrPiMassCut;
    float m_lowerLb_LambdaMuMuMassCut;                                                        
    float m_upperLb_LambdaMuMuMassCut;  
    float m_lowerLambdaVtxMassCut;
    float m_upperLambdaVtxMassCut;
    float m_lowerLbVtxMassCut;                                                        
    float m_upperLbVtxMassCut;   
    float m_lambdaVtxChi2Cut;
    float m_lBVtxChi2Cut;                                                   
//     float m_lBVtxDistanceCut;
//     float m_prImpactCut;
//     float m_piImpactCut;
    // vertexing off
    float m_lowerLb_LambdaMuMuMassCutVtxOff;
    float m_upperLb_LambdaMuMuMassCutVtxOff;

    // Bc->DsMuMu cuts
    float m_lowerDsVtxMassCut;
    float m_upperDsVtxMassCut;
    float m_lowerPhiDs_MassCut;
    float m_upperPhiDs_MassCut;
    float m_lowerDs_MassCut;
    float m_upperDs_MassCut;
    float m_DsVtxChi2Cut;
    float m_lowerBc_DsMuMuMassCut;                                                        
    float m_upperBc_DsMuMuMassCut;   
    float m_lowerBcVtxMassCut;                                                        
    float m_upperBcVtxMassCut;   
    float m_bCVtxChi2Cut;                    
    // vertexing off
    float m_lowerBc_DsMuMuMassCutVtxOff;
    float m_upperBc_DsMuMuMassCutVtxOff;
   
    // to set Accept-All mode: should be done with force-accept when possible
    BooleanProperty m_acceptAll;   
    
    BooleanProperty m_doVertexFit;                                           
    BooleanProperty m_doMuMuVertexFit;                                           

    // B+->K+MuMu
    BooleanProperty m_doB_KMuMuDecay;
    BooleanProperty m_doB_KMuMuVertexFit;                                      

    // Bd->K*MuMu
    BooleanProperty m_doBd_KstarMuMuDecay;                                            
    BooleanProperty m_doKstar_KaonPionVertexFit;                                 
    BooleanProperty m_doBd_KstarMuMuVertexFit;                                   

    // Bs->PhiMuMu
    BooleanProperty m_doBs_Phi1020MuMuDecay;                                            
    BooleanProperty m_doPhi1020_KaonKaonVertexFit;                               
    BooleanProperty m_doBs_Phi1020MuMuVertexFit;                                 
        
    // Lb->LMuMu cuts
    BooleanProperty m_doLb_LambdaMuMuDecay;                                            
    BooleanProperty m_doLambda_ProtonPionVertexFit;                               
    BooleanProperty m_doLb_LambdaMuMuVertexFit;                                 

    // Bc->DsMuMu
    BooleanProperty m_doBc_DsMuMuDecay;                                            
    BooleanProperty m_doDs_PhiPionVertexFit;                               
    BooleanProperty m_doBc_DsMuMuVertexFit;                                 

    TrigTimer* m_TotTimer;
    TrigTimer* m_VtxFitTimer;  
    
   // Configurable properties - vertexing tools
    ITrigL2VertexFitter* m_L2vertFitter;                                         // L2 vertex
    ITrigVertexingTool*  m_vertexingTool;                                        
    
    //output collections                                                        
    TrigL2BphysContainer* m_trigBphysColl_b;
    xAOD::TrigBphysContainer* m_trigBphysColl_bxAOD; // revert back to non-aod naming once complete
    
    xAOD::TrigBphysContainer* m_trigBphysColl_kStarxAOD;
    xAOD::TrigBphysContainer* m_trigBphysColl_phixAOD;
    xAOD::TrigBphysContainer* m_trigBphysColl_lambdaxAOD;
    xAOD::TrigBphysContainer* m_trigBphysColl_dsxAOD;

    
    // Counters
/*    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedRoIMatch;
    unsigned int m_countPassedMass;
*/  

    // Monitored variables
    // general
    std::vector<int>   m_mon_Errors;
    std::vector<int>   m_mon_Acceptance;
    // lvl1
    std::vector<float> m_mon_ROIsEta;
    std::vector<float> m_mon_ROIsPhi;

    // muons
    std::vector<float> m_mon_muon1Eta;
    std::vector<float> m_mon_muon1Phi;
    std::vector<float> m_mon_muon1Pt;
    std::vector<float> m_mon_muon1Z0;
    std::vector<float> m_mon_muon1A0;
    std::vector<float> m_mon_muon2Eta;
    std::vector<float> m_mon_muon2Phi;
    std::vector<float> m_mon_muon2Pt;
    std::vector<float> m_mon_muon2Z0;
    std::vector<float> m_mon_muon2A0;
    
    // tracks
    std::vector<int>   m_mon_nTracks;
    std::vector<float> m_mon_etaTracks;
    std::vector<float> m_mon_pTTracks;
    std::vector<float> m_mon_phiTracks;

    // B+/-
    int m_mon_BMuMuK_n;
    std::vector<float> m_mon_BMuMuK_InvMass_B;
    std::vector<float> m_mon_BMuMuK_PtB_K;  
    std::vector<float> m_mon_BMuMuK_EtaB_K;
    std::vector<float> m_mon_BMuMuK_PhiB_K;
    std::vector<float> m_mon_BMuMuK_VtxMass_B;
    std::vector<float> m_mon_BMuMuK_Chi2toNDoF_B;
    std::vector<float> m_mon_BMuMuK_VtxPtB_K;
    std::vector<float> m_mon_BMuMuK_VtxEtaB_K;
    std::vector<float> m_mon_BMuMuK_VtxPhiB_K;

    // Bs+/- before vertexing
    int m_mon_BsMuMuPhi_n;
    std::vector<float> m_mon_BsMuMuPhi_InvMassBs_Phi;
    std::vector<float> m_mon_BsMuMuPhi_InvMass_Bs;
    std::vector<float> m_mon_BsMuMuPhi_PtPhi_Kp;
    std::vector<float> m_mon_BsMuMuPhi_EtaPhi_Kp;
    std::vector<float> m_mon_BsMuMuPhi_PhiPhi_Kp;
    std::vector<float> m_mon_BsMuMuPhi_PtPhi_Km;
    std::vector<float> m_mon_BsMuMuPhi_EtaPhi_Km;
    std::vector<float> m_mon_BsMuMuPhi_PhiPhi_Km;
    // after Phi vertexing
    std::vector<float> m_mon_BsMuMuPhi_VtxMassBs_Phi;
    std::vector<float> m_mon_BsMuMuPhi_Chi2toNDoFBs_Phi;
    std::vector<float> m_mon_BsMuMuPhi_VtxPtPhi_Kp;
    std::vector<float> m_mon_BsMuMuPhi_VtxEtaPhi_Kp;
    std::vector<float> m_mon_BsMuMuPhi_VtxPhiPhi_Kp;
    std::vector<float> m_mon_BsMuMuPhi_VtxPtPhi_Km;
    std::vector<float> m_mon_BsMuMuPhi_VtxEtaPhi_Km;
    std::vector<float> m_mon_BsMuMuPhi_VtxPhiPhi_Km;
    // after Bs vertexing
    std::vector<float> m_mon_BsMuMuPhi_VtxMass_Bs;
    std::vector<float> m_mon_BsMuMuPhi_Chi2toNDoF_Bs;
    std::vector<float> m_mon_BsMuMuPhi_VtxPtBs_Kp;
    std::vector<float> m_mon_BsMuMuPhi_VtxEtaBs_Kp;
    std::vector<float> m_mon_BsMuMuPhi_VtxPhiBs_Kp;
    std::vector<float> m_mon_BsMuMuPhi_VtxPtBs_Km;
    std::vector<float> m_mon_BsMuMuPhi_VtxEtaBs_Km;
    std::vector<float> m_mon_BsMuMuPhi_VtxPhiBs_Km;
        
    // Bd
    int m_mon_BdMuMuKs_n;
    std::vector<float> m_mon_BdMuMuKs_InvMassBd_Ks;
    std::vector<float> m_mon_BdMuMuKs_InvMass_Bd;
    std::vector<float> m_mon_BdMuMuKs_PtKs_Ka;
    std::vector<float> m_mon_BdMuMuKs_EtaKs_Ka;
    std::vector<float> m_mon_BdMuMuKs_PhiKs_Ka;
    std::vector<float> m_mon_BdMuMuKs_PtKs_Pi;
    std::vector<float> m_mon_BdMuMuKs_EtaKs_Pi;
    std::vector<float> m_mon_BdMuMuKs_PhiKs_Pi;
    // after K* vertexing
    std::vector<float> m_mon_BdMuMuKs_VtxMassBd_Ks;
    std::vector<float> m_mon_BdMuMuKs_Chi2toNDoFBd_Ks;
    std::vector<float> m_mon_BdMuMuKs_VtxPtKs_Ka;
    std::vector<float> m_mon_BdMuMuKs_VtxEtaKs_Ka;
    std::vector<float> m_mon_BdMuMuKs_VtxPhiKs_Ka;
    std::vector<float> m_mon_BdMuMuKs_VtxPtKs_Pi;
    std::vector<float> m_mon_BdMuMuKs_VtxEtaKs_Pi;
    std::vector<float> m_mon_BdMuMuKs_VtxPhiKs_Pi;
    // after Bd vertexing
    std::vector<float> m_mon_BdMuMuKs_VtxMass_Bd;
    std::vector<float> m_mon_BdMuMuKs_Chi2toNDoF_Bd;
    std::vector<float> m_mon_BdMuMuKs_VtxPtBd_Ka;
    std::vector<float> m_mon_BdMuMuKs_VtxEtaBd_Ka;
    std::vector<float> m_mon_BdMuMuKs_VtxPhiBd_Ka;
    std::vector<float> m_mon_BdMuMuKs_VtxPtBd_Pi;
    std::vector<float> m_mon_BdMuMuKs_VtxEtaBd_Pi;
    std::vector<float> m_mon_BdMuMuKs_VtxPhiBd_Pi;
        
    // Lb+/- before vertexing
    // Lb+/-
    int m_mon_LbMuMuL_n;
    std::vector<float> m_mon_LbMuMuL_InvMassLb_L;
    std::vector<float> m_mon_LbMuMuL_InvMass_Lb;
    std::vector<float> m_mon_LbMuMuL_PtL_Pr;
    std::vector<float> m_mon_LbMuMuL_EtaL_Pr;
    std::vector<float> m_mon_LbMuMuL_PhiL_Pr;
    std::vector<float> m_mon_LbMuMuL_PtL_Pi;
    std::vector<float> m_mon_LbMuMuL_EtaL_Pi;
    std::vector<float> m_mon_LbMuMuL_PhiL_Pi;
    // after L vertexing
    std::vector<float> m_mon_LbMuMuL_VtxMassLb_L;
    std::vector<float> m_mon_LbMuMuL_Chi2toNDoFLb_L;
    std::vector<float> m_mon_LbMuMuL_VtxPtL_Pr;
    std::vector<float> m_mon_LbMuMuL_VtxEtaL_Pr;
    std::vector<float> m_mon_LbMuMuL_VtxPhiL_Pr;
    std::vector<float> m_mon_LbMuMuL_VtxPtL_Pi;
    std::vector<float> m_mon_LbMuMuL_VtxEtaL_Pi;
    std::vector<float> m_mon_LbMuMuL_VtxPhiL_Pi;
    // after Lb vertexing
    std::vector<float> m_mon_LbMuMuL_VtxMass_Lb;
    std::vector<float> m_mon_LbMuMuL_Chi2toNDoF_Lb;
    std::vector<float> m_mon_LbMuMuL_VtxPtLb_Pr;
    std::vector<float> m_mon_LbMuMuL_VtxEtaLb_Pr;
    std::vector<float> m_mon_LbMuMuL_VtxPhiLb_Pr;
    std::vector<float> m_mon_LbMuMuL_VtxPtLb_Pi;
    std::vector<float> m_mon_LbMuMuL_VtxEtaLb_Pi;
    std::vector<float> m_mon_LbMuMuL_VtxPhiLb_Pi;

    // Bc+/- before vertexing
    int m_mon_BcMuMuDs_n;
    std::vector<float> m_mon_BcMuMuDs_InvMassBc_Phi;
    std::vector<float> m_mon_BcMuMuDs_InvMassBc_Ds;
    std::vector<float> m_mon_BcMuMuDs_InvMass_Bc;
    std::vector<float> m_mon_BcMuMuDs_PtPhi_Kp;
    std::vector<float> m_mon_BcMuMuDs_EtaPhi_Kp;
    std::vector<float> m_mon_BcMuMuDs_PhiPhi_Kp;
    std::vector<float> m_mon_BcMuMuDs_PtPhi_Km;
    std::vector<float> m_mon_BcMuMuDs_EtaPhi_Km;
    std::vector<float> m_mon_BcMuMuDs_PhiPhi_Km;
    std::vector<float> m_mon_BcMuMuDs_PtDs_pi;
    std::vector<float> m_mon_BcMuMuDs_EtaDs_pi;
    std::vector<float> m_mon_BcMuMuDs_PhiDs_pi;
    // after Ds vertexing
    std::vector<float> m_mon_BcMuMuDs_VtxMassBc_Ds;
    std::vector<float> m_mon_BcMuMuDs_Chi2toNDoFBc_Ds;
    std::vector<float> m_mon_BcMuMuDs_VtxPtDs_Kp;
    std::vector<float> m_mon_BcMuMuDs_VtxEtaDs_Kp;
    std::vector<float> m_mon_BcMuMuDs_VtxPhiDs_Kp;
    std::vector<float> m_mon_BcMuMuDs_VtxPtDs_Km;
    std::vector<float> m_mon_BcMuMuDs_VtxEtaDs_Km;
    std::vector<float> m_mon_BcMuMuDs_VtxPhiDs_Km;
    std::vector<float> m_mon_BcMuMuDs_VtxPtDs_pi;
    std::vector<float> m_mon_BcMuMuDs_VtxEtaDs_pi;
    std::vector<float> m_mon_BcMuMuDs_VtxPhiDs_pi;
    // after Bc vertexing
    std::vector<float> m_mon_BcMuMuDs_VtxMass_Bc;
    std::vector<float> m_mon_BcMuMuDs_Chi2toNDoF_Bc;
    std::vector<float> m_mon_BcMuMuDs_VtxPtBc_Kp;
    std::vector<float> m_mon_BcMuMuDs_VtxEtaBc_Kp;
    std::vector<float> m_mon_BcMuMuDs_VtxPhiBc_Kp;
    std::vector<float> m_mon_BcMuMuDs_VtxPtBc_Km;
    std::vector<float> m_mon_BcMuMuDs_VtxEtaBc_Km;
    std::vector<float> m_mon_BcMuMuDs_VtxPhiBc_Km;
    std::vector<float> m_mon_BcMuMuDs_VtxPtBc_pi;
    std::vector<float> m_mon_BcMuMuDs_VtxEtaBc_pi;
    std::vector<float> m_mon_BcMuMuDs_VtxPhiBc_pi;
    
    // summary
    int                m_mon_nBphys;  
    int                m_mon_nTriedCombinations;  
    float              m_mon_TotalRunTime;
    float              m_mon_VertexingTime;
    
 
};

#endif
