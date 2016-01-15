/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMANAVTPNTUPLE_H
#define TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMANAVTPNTUPLE_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODTracking/TrackingPrimitives.h"

class TrigEgammaNavTPNtuple : public TrigEgammaNavTPBaseTool,
                          virtual public ITrigEgammaAnalysisBaseTool
{
  ASG_TOOL_CLASS(TrigEgammaNavTPNtuple, ITrigEgammaAnalysisBaseTool)

  public:
  
    TrigEgammaNavTPNtuple( const std::string& myname );
    ~TrigEgammaNavTPNtuple() {}; 
    StatusCode childInitialize();
    StatusCode childBook();
    StatusCode childExecute();
    StatusCode childFinalize();
 
  private:
    
    /* Dump trigger and offline probe object */
    bool executeProbesDump();

    /* helper function */ 
    template <class T> void InitBranch(TTree* fChain, std::string branch_name, T* param, bool message = true);

    /* Create branches */
    void bookEventBranches( TTree *t );
    void bookElectronBranches( TTree *t );
    //void bookPhotonBranches( TTree *t );
    void bookTriggerBranches( TTree *t );
    void bookMonteCarloBranches( TTree *t );

    /* uses InitBranch to connect the local variable and ttree pointer */
    void linkEventBranches( TTree *t);
    void linkElectronBranches( TTree *t);
    void linkPhotonBranches( TTree *t);
    void linkTriggerBranches( TTree *t);
    void linkMonteCarloBranches( TTree *t);    

    /* parse between objects and local variables connected to the 
     * ttree pointer */
    bool fillEvent        ( );
    bool fillMonteCarlo   ( const xAOD::Egamma        *eg );
    bool fillElectron     ( const xAOD::Electron      *el );
    //bool fillPhoton       ( const xAOD::Photon        *ph );

    bool fillCaloRings    ( const xAOD::Electron      *el );
    bool fillTrigCaloRings( const xAOD::TrigEMCluster *emCluster );
    bool fillEmTauRoI     ( const xAOD::EmTauRoI *emTauRoI        );
    bool fillTrigEMCluster( const xAOD::TrigEMCluster *emCluster  );
    bool fillTrigElectron ( const xAOD::TrigElectron *trigEl      );


    /* Space memory manager */
    void alloc_space();
    void release_space();
    void clear();

  private:

     /* Branch variables */
    uint32_t            m_runNumber;
    unsigned long long  m_eventNumber;
    unsigned int        m_eventCounter;    
    float               m_avgmu;    
     /* Egamma */
    float               m_el_et;
    float               m_el_pt;
    float               m_el_eta;
    float               m_el_phi;
    float               m_el_ethad1;
    float               m_el_ehad1;
    float               m_el_f1;
    float               m_el_f3;
    float               m_el_d0;
    float               m_el_f1core;
    float               m_el_f3core;
    float               m_el_weta2;
    float               m_el_wtots1;
    float               m_el_fracs1;
    float               m_el_Reta;
    float               m_el_Rphi;
    float               m_el_Eratio;
    float               m_el_Rhad;
    float               m_el_Rhad1;
    float               m_el_deta1;
    float               m_el_deta2;
    float               m_el_dphi2;
    float               m_el_dphiresc;
    float               m_el_dphiresc2;
    float               m_el_eprobht;
    float               m_el_charge;
    uint8_t             m_el_nblayerhits;
    uint8_t             m_el_nblayerolhits;
    uint8_t             m_el_npixhits;
    uint8_t             m_el_npixolhits;
    uint8_t             m_el_nscthits;
    uint8_t             m_el_nsctolhits;
    uint8_t             m_el_ntrthightreshits;
    uint8_t             m_el_ntrthits;
    uint8_t             m_el_ntrthighthresolhits;
    uint8_t             m_el_ntrtolhits;
    uint8_t             m_el_ntrtxenonhits;
    bool                m_el_expectblayerhit;
    uint8_t             m_el_nsihits         ;
    uint8_t             m_el_nsideadsensors  ;
    uint8_t             m_el_npixdeadsensors ;
    uint8_t             m_el_nsctdeadsensors ;
    bool                m_el_loose;
    bool                m_el_medium;
    bool                m_el_tight;
    bool                m_el_lhLoose;
    bool                m_el_lhMedium;
    bool                m_el_lhTight;     
    bool                m_el_rgLoose;
    bool                m_el_rgMedium;
    bool                m_el_rgTight;
    bool                m_el_multiLepton;
    std::vector<float> *m_el_ringsE;
    int                 m_el_nGoodVtx;
    int                 m_el_nPileupPrimaryVtx;
    ///Egamma Calo
    float               m_calo_et;
    float               m_calo_eta;
    float               m_calo_phi;
    
    // Level 1     
    float               m_trig_L1_eta;
    float               m_trig_L1_phi;
    float               m_trig_L1_emClus;
    float               m_trig_L1_tauClus;
    float               m_trig_L1_emIsol;
    float               m_trig_L1_hadIsol;
    std::vector<std::string> *m_trig_L1_thrNames;          
    bool                m_trig_L1_accept;
    // Level 2 Calo
    float               m_trig_L2_calo_et;         
    float               m_trig_L2_calo_eta;        
    float               m_trig_L2_calo_phi;        
    float               m_trig_L2_calo_e237;       
    float               m_trig_L2_calo_e277;      
    float               m_trig_L2_calo_fracs1;     
    float               m_trig_L2_calo_weta2;      
    float               m_trig_L2_calo_ehad1;      
    float               m_trig_L2_calo_emaxs1;     
    float               m_trig_L2_calo_e2tsts1;    
    float               m_trig_L2_calo_wstot;      
    float               m_trig_L2_calo_rnnOutput;      
    std::vector<float> *m_trig_L2_calo_energySample;
    std::vector<float> *m_trig_L2_calo_rings;
    bool                m_trig_L2_calo_accept;     
    // level 2 id
    std::vector<int>   *m_trig_L2_el_trackAlgID;          
    std::vector<float> *m_trig_L2_el_pt;          
    std::vector<float> *m_trig_L2_el_caloEta;         
    std::vector<float> *m_trig_L2_el_eta;         
    std::vector<float> *m_trig_L2_el_phi;         
    std::vector<float> *m_trig_L2_el_charge;      
    std::vector<float> *m_trig_L2_el_nTRTHits;        
    std::vector<float> *m_trig_L2_el_nTRTHiThresholdHits;        
    std::vector<float> *m_trig_L2_el_etOverPt;          
    std::vector<float> *m_trig_L2_el_trkClusDeta; 
    std::vector<float> *m_trig_L2_el_trkClusDphi;


    bool                m_trig_L2_el_accept;
    // Level EF
    bool                m_trig_EF_calo_accept;
    bool                m_trig_EF_el_accept;
    // Monte Carlo
    bool                m_mc_hasMC     ;
    float               m_mc_pt        ;
    float               m_mc_eta       ;
    float               m_mc_phi       ;
    bool                m_mc_isTop     ;
    bool                m_mc_isQuark   ;
    bool                m_mc_isParton  ;
    bool                m_mc_isMeson   ;
    bool                m_mc_isTau     ;
    bool                m_mc_isMuon    ;
    bool                m_mc_isPhoton  ;
    bool                m_mc_isElectron;
    bool                m_mc_hasZMother;
    bool                m_mc_hasWMother;

  
};
#endif
