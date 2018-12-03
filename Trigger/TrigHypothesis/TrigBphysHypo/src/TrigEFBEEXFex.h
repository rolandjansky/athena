/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: tabstop=2:shiftwidth=2:expandtab
/**************************************************************************
**
**   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFBEEXFex.h
**
**   Description: EF hypothesis algorithms for 
**                B^0_{d,s},\Lambda_b \to X \mu^+ \mu^-
**               
**
**   Authors: O. Igonkina and H.Russell 
**
**   Created:   25.09.2017
**
***************************************************************************/
            
#ifndef TRIG_TrigEFBEEXFex_H 
#define TRIG_TrigEFBEEXFex_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/ComboAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

//Ntuples
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

//#include "TrigTimeAlgs/TrigTimerSvc.h"
//#include "TrigTimeAlgs/TrigTimer.h"

#include "TrigBphysHypo/Constants.h"
#include "BtrigUtils.h"
#include "TrigBphysHelperUtilsTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

class StoreGateSvc;
class TriggerElement;
class Electron;

class TrigBphysHelperUtilsTool;

class TrigEFBEEXFex: public HLT::ComboAlgo  {
  
  public:
    TrigEFBEEXFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFBEEXFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
  private:
    
    ToolHandle < Trk::IVertexFitter  >       m_fitterSvc;
    
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;
    
    // container inputs names
    std::string m_bphysCollectionKey;
    std::string m_input_trackCollectionKey;
    
    // Cuts and properties
    bool m_oppositeCharge;
    float m_lowerEEMassCut;
    float m_upperEEMassCut;
    float m_VtxChi2Cut;
    
    // Maximum number of track combinations to try -- protection against TimeOuts
    int   m_maxNcombinations;
    
    bool m_doB_KEEDecay;
    float m_lowerKEEMassCut;
    float m_upperKEEMassCut;
    bool m_doB_KEEVertexing;
    float m_bVtxChi2Cut;
    int m_maxBpToStore;
    
    bool m_doBd_KstarEEDecay;
    float m_lowerKstar_KaonMassCut;
    float m_upperKstar_KaonMassCut;
    float m_lowerBd_KstarEEMassCut;
    float m_upperBd_KstarEEMassCut;
    bool m_doKstar_KPiVertexing;
    bool m_doBd_KstarEEVertexing;
    float m_kStarVtxChi2Cut;
    float m_bDVtxChi2Cut;
    int m_maxBdToStore;
    
    bool m_doBs_Phi1020EEDecay;
    float m_lowerPhi1020_KaonMassCut;
    float m_upperPhi1020_KaonMassCut;
    float m_lowerBs_Phi1020EEMassCut;
    float m_upperBs_Phi1020EEMassCut;
    bool m_doPhi1020_KKVertexing;
    bool m_doBs_Phi1020EEVertexing;
    float m_phi1020VtxChi2Cut;
    float m_bSVtxChi2Cut;
    int m_maxBsToStore;
    

    
    // FTK Flag
    bool m_FTK;

    // Monitoring variables and containers
    //   General
    std::vector<int>   m_mon_Errors;
    std::vector<int>   m_mon_Acceptance;
    int                m_mon_nTriedCombinations;
    //   Timing
    float              m_mon_TotalRunTime;
    float              m_mon_VertexingTime;
    //   RoIs
    std::vector<float> m_mon_RoI_RoI1Eta;
    std::vector<float> m_mon_RoI_RoI2Eta;
    std::vector<float> m_mon_RoI_RoI1Phi;
    std::vector<float> m_mon_RoI_RoI2Phi;
    std::vector<float> m_mon_RoI_dEtaRoI;
    std::vector<float> m_mon_RoI_dPhiRoI;
    //   DiEon
    int                m_mon_DiE_n;
    std::vector<float> m_mon_DiE_Pt_Lep1;
    std::vector<float> m_mon_DiE_Pt_Lep2;
    std::vector<float> m_mon_DiE_Eta_Lep1;
    std::vector<float> m_mon_DiE_Eta_Lep2;
    std::vector<float> m_mon_DiE_Phi_Lep1;
    std::vector<float> m_mon_DiE_Phi_Lep2;
    std::vector<float> m_mon_DiE_dEtaEE;
    std::vector<float> m_mon_DiE_dPhiEE;
    std::vector<float> m_mon_DiE_pTsumEE;
    std::vector<float> m_mon_DiE_InvMassEE;
    std::vector<float> m_mon_DiE_VtxMassEE;
    std::vector<float> m_mon_DiE_Chi2EE;
    //   Tracks
    int                m_mon_Tracks_n;
    std::vector<float> m_mon_Tracks_Eta;
    std::vector<float> m_mon_Tracks_Pt;
    std::vector<float> m_mon_Tracks_Phi;
    //   B+
    int                m_mon_BEEK_n;
    std::vector<float> m_mon_BEEK_Pt_K;  
    std::vector<float> m_mon_BEEK_Eta_K;
    std::vector<float> m_mon_BEEK_Phi_K;
    std::vector<float> m_mon_BEEK_InvMass_B;
    std::vector<float> m_mon_BEEK_VtxMass_B;
    std::vector<float> m_mon_BEEK_Chi2_B;
    //   Bd
    int                m_mon_BdEEKs_n;
    std::vector<float> m_mon_BdEEKs_Pt_K;
    std::vector<float> m_mon_BdEEKs_Eta_K;
    std::vector<float> m_mon_BdEEKs_Phi_K;
    std::vector<float> m_mon_BdEEKs_Pt_Pi;
    std::vector<float> m_mon_BdEEKs_Eta_Pi;
    std::vector<float> m_mon_BdEEKs_Phi_Pi;
    std::vector<float> m_mon_BdEEKs_InvMass_Kstar;
    std::vector<float> m_mon_BdEEKs_VtxMass_Kstar;
    std::vector<float> m_mon_BdEEKs_Chi2_Kstar;
    std::vector<float> m_mon_BdEEKs_InvMass_Bd;
    std::vector<float> m_mon_BdEEKs_VtxMass_Bd;
    std::vector<float> m_mon_BdEEKs_Chi2_Bd;
    //   Bs
    int                m_mon_BsEEPhi_n;
    std::vector<float> m_mon_BsEEPhi_Pt_K1;
    std::vector<float> m_mon_BsEEPhi_Eta_K1;
    std::vector<float> m_mon_BsEEPhi_Phi_K1;
    std::vector<float> m_mon_BsEEPhi_Pt_K2;
    std::vector<float> m_mon_BsEEPhi_Eta_K2;
    std::vector<float> m_mon_BsEEPhi_Phi_K2;
    std::vector<float> m_mon_BsEEPhi_InvMass_Phi1020;
    std::vector<float> m_mon_BsEEPhi_VtxMass_Phi1020;
    std::vector<float> m_mon_BsEEPhi_Chi2_Phi1020;
    std::vector<float> m_mon_BsEEPhi_InvMass_Bs;
    std::vector<float> m_mon_BsEEPhi_VtxMass_Bs;
    std::vector<float> m_mon_BsEEPhi_Chi2_Bs;


    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;


    // TrigEFBphysContainer* m_trigBphysColl_b;
    // TrigEFBphysContainer* m_trigBphysColl_X;

    xAOD::TrigBphysContainer * m_TrigBphysColl_b;
    xAOD::TrigBphysContainer * m_TrigBphysColl_X;
    
    // Timers
    TrigTimer* m_TotTimer;
    TrigTimer* m_VtxFitTimer;
    
    Trk::TrkVKalVrtFitter* m_VKVFitter;       
    
    //Counters
    int m_lastEvent;
    int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    
    int m_lastEventPassedBplus;
    int m_lastEventPassedBd;
    int m_lastEventPassedBs;
    int m_lastEventPassedLb;
    int m_lastEventPassedBc;
    int m_lastEventPassedBcDplus;
    int m_lastEventPassedBcDstar;
    int m_lastEventPassedBcD0;
    unsigned int m_countPassedEventsBplus;
    unsigned int m_countPassedEventsBs;
    unsigned int m_countPassedEventsBd;
    unsigned int m_countPassedEventsLb;
    unsigned int m_countPassedEventsBc;
    unsigned int m_countPassedEventsBcDplus;
    unsigned int m_countPassedEventsBcDstar;
    unsigned int m_countPassedEventsBcD0;
    
    unsigned int m_countPassedEEID;
    unsigned int m_countPassedEEOS;
    unsigned int m_countPassedEEMass;
    unsigned int m_countPassedEEVtx;
    unsigned int m_countPassedEEVtxChi2;
    
    unsigned int m_countPassedBplusMass;
    unsigned int m_countPassedBplusVtx;
    unsigned int m_countPassedBplusVtxChi2;
    int m_countBpToStore;
    
    unsigned int m_countPassedKstarMass;
    unsigned int m_countPassedBdMass;
    unsigned int m_countPassedKstarVtx;
    unsigned int m_countPassedKstarVtxChi2;
    unsigned int m_countPassedBdVtx;
    unsigned int m_countPassedBdVtxChi2;
    int m_countBdToStore;
    
    unsigned int m_countPassedPhi1020Mass;
    unsigned int m_countPassedBsMass;
    unsigned int m_countPassedPhi1020Vtx;
    unsigned int m_countPassedPhi1020VtxChi2;
    unsigned int m_countPassedBsVtx;
    unsigned int m_countPassedBsVtxChi2;
    int m_countBsToStore;
    

    
    void addUnique(std::vector<const Trk::Track*>&, const Trk::Track*);
    void addUnique(std::vector<const xAOD::TrackParticle*>&, const xAOD::TrackParticle*);
    void addUnique(std::vector<ElementLink<xAOD::TrackParticleContainer> >&, const ElementLink<xAOD::TrackParticleContainer>&);
    bool isUnique(const  xAOD::TrackParticle* id1, const  xAOD::TrackParticle* id2) const;

    
    double XMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, int decay); /// checking the mass
    double X3Mass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 );
    double XKPiPiMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 );
    
    double KEEMass( const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* kaon);
    double XEEMass( const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* particle1,
                     const xAOD::TrackParticle* particle2, int decay);
    double X3EEMass(const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* particle1,
                      const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 );
    double XKPiPiEEMass(const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* particle1,
                      const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 );
    
    xAOD::TrigBphys* checkBplusEEKplus(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                         const ElementLink<xAOD::TrackParticleContainer> & ellep1,
                                         const ElementLink<xAOD::TrackParticleContainer> & ellep2);
    
    xAOD::TrigBphys* checkBEE2X(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                  const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                  const ElementLink<xAOD::TrackParticleContainer> & ellep1,
                                  const ElementLink<xAOD::TrackParticleContainer> & ellep2,
                                  int decay, xAOD::TrigBphys* & trigPartX);
    

    
};

// Define the bins for error-monitoring histogram
#define ERROR_No_EventInfo           0
#define ERROR_Not_2_InputTEs         1
#define ERROR_No_RoIs                2
#define ERROR_No_ElectronContainer 3
#define ERROR_No_ElectronCandidate       4

#define ERROR_SameLep               5
#define ERROR_DiEVtxFit_Fails       6
#define ERROR_DiEVtxMass_Fails      7

#define ERROR_No_TrackColl           8
#define ERROR_No_LepTrackMatch      17

#define ERROR_BplusVtxFit_Fails      9
#define ERROR_BplusVtxMass_Fails     10
#define ERROR_WrongDecayID           11
#define ERROR_XVtxFit_Fails          12
#define ERROR_XVtxMass_Fails         13
#define ERROR_XEEVtxFit_Fails      14
#define ERROR_XEEVtxMass_Fails     15

#define ERROR_BphysCollStore_Fails   16

#define ERROR_TooManyComb_Acc        18
#define ERROR_TooManyComb_Rej        19

#define ERROR_MaxNumBpReached        20
#define ERROR_MaxNumBdReached        21
#define ERROR_MaxNumBsReached        22
#define ERROR_MaxNumLbReached        23
#define ERROR_MaxNumBcReached        24
#define ERROR_MaxNumBcDplusReached   24
#define ERROR_MaxNumBcDstarReached   24
#define ERROR_MaxNumBcD0Reached   24

// // Define the bins for acceptance-monitoring histogram
// #define ACCEPT_Input                 0
// #define ACCEPT_AcceptAll             1
// #define ACCEPT_Got_RoIs              2
// #define ACCEPT_Got_Muons             3
// #define ACCEPT_Got_TrackColl         4
// #define ACCEPT_First_TrackColl       5
// #define ACCEPT_Both_TrackColls       6
// #define ACCEPT_Full_IDTracks         7
// #define ACCEPT_Pass_OppChargeC       8
// #define ACCEPT_MuonTracks_Added      8
// #define ACCEPT_Muon_Vertexing        9
// #define ACCEPT_CalcInvMass          10 
// #define ACCEPT_MuonVtx_Part         11
// #define ACCEPT_EE_Mass            12
// #define ACCEPT_MotherVtxCreated     13
// #define ACCEPT_BphysCollParticle    14

#endif
