
#ifndef TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMAPHYSVALBASE_H
#define TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMAPHYSVALBASE_H

#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "PATCore/TAccept.h"
#include "TTree.h"

class TrigEgammaPhysValBase
{
  public:
    TrigEgammaPhysValBase();
    ~TrigEgammaPhysValBase() {};
 
  protected: 
    /* helper function */ 
    template <class T> void InitBranch(TTree* fChain, std::string branch_name, T* param, bool message = true);
   
    /* Create branches */
    void bookEventBranches( TTree *t );
    void bookElectronBranches( TTree *t );
    void bookTriggerBranches( TTree *t );
    void bookMonteCarloBranches( TTree *t );
    void bookTDTBranches( TTree *t );
    
    /* uses InitBranch to connect the local variable and ttree pointer */
    void linkEventBranches( TTree *t);
    void linkElectronBranches( TTree *t);
    void linkTriggerBranches( TTree *t);
    void linkSelectorBranches( TTree *t);
    void linkMonteCarloBranches( TTree *t);    
    void linkTDTBranches( TTree *t);    
    
    /* Space memory manager */
    void alloc_space();
    void release_space();
    void clear();
    void createTDTMetadata( TTree *t, std::vector<std::string> trigList );
    
    bool                m_doMCDetailed;
    bool                m_doL2Detailed;
    bool                m_doL2CaloDetailed;
    bool                m_doRecoDetailed;

    /* Branch variables */
    uint32_t            m_runNumber{};
    unsigned long long  m_eventNumber{};
    unsigned int        m_eventCounter{};   
    float               m_avgmu{};    
    float               m_lumiBlock{};    
    
    /* Egamma */
     
    bool                m_el_hasCalo   ;
    bool                m_el_hasTrack  ;
    float               m_el_e;
    float               m_el_et;
    float               m_el_eta;
    float               m_el_phi;
    float               m_el_ethad1;
    float               m_el_ehad1;
    float               m_el_f1;
    float               m_el_f3;
    float               m_el_f1core;
    float               m_el_f3core;
    float               m_el_weta1;
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
    float               m_el_deltaPhiRescaled2;
    float               m_el_deltaEta1;
    float               m_el_deltaE;
    float               m_el_e277;
    std::vector<float> *m_el_etCone;
    std::vector<float> *m_el_ptCone;
   
    

    float               m_el_trk_pt;
    float               m_el_trk_eta; 
    float               m_el_trk_charge; 
    float               m_el_trk_sigd0;
    float               m_el_trk_d0;
    float               m_el_trk_eProbabilityHT;
    float               m_el_trk_transformed_eProbabilityHT;
    float               m_el_trk_d0significance;
    float               m_el_trk_deltaPOverP;
    float               m_el_trk_qOverP;
    std::vector<uint8_t> *m_el_trk_summaryValues;

    bool                m_el_loose{};
    bool                m_el_medium{};
    bool                m_el_tight{};
    bool                m_el_lhVLoose{};
    bool                m_el_lhLoose{};
    bool                m_el_lhMedium{};
    bool                m_el_lhTight{}; 
    bool                m_el_rgLoose{};
    bool                m_el_rgMedium{};
    bool                m_el_rgTight{};
    bool                m_el_multiLepton{};
    std::vector<float> *m_el_ringsE;
    int                 m_el_nGoodVtx{};
    int                 m_el_nPileupPrimaryVtx{};
    ///Egamma Calo
    float               m_el_calo_et{};
    float               m_el_calo_eta{};
    float               m_el_calo_phi{};
    float               m_el_calo_etaBE2{};
    float               m_el_calo_e{};
    // Level 1     
    float               m_trig_L1_eta{};
    float               m_trig_L1_phi{};
    float               m_trig_L1_emClus{};
    float               m_trig_L1_tauClus{};
    float               m_trig_L1_emIsol{};
    float               m_trig_L1_hadIsol{};
    float               m_trig_L1_hadCore{};
    std::vector<std::string> *m_trig_L1_thrNames;          

    // Level 2 Calo
    float               m_trig_L2_calo_et{};         
    float               m_trig_L2_calo_eta{};        
    float               m_trig_L2_calo_phi{};        
    float               m_trig_L2_calo_e237{};       
    float               m_trig_L2_calo_e277{};      
    float               m_trig_L2_calo_fracs1{};     
    float               m_trig_L2_calo_weta2{};      
    float               m_trig_L2_calo_ehad1{};      
    float               m_trig_L2_calo_emaxs1{};     
    float               m_trig_L2_calo_e2tsts1{};    
    float               m_trig_L2_calo_wstot{};      
    float               m_trig_L2_calo_nnOutput{};      
    std::vector<float> *m_trig_L2_calo_energySample;
    std::vector<float> *m_trig_L2_calo_rings;
    std::vector<float> *m_trig_L2_calo_rnnOutput;
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
   
    // EFCalo and HLT steps
    
    std::vector<float>               *m_trig_EF_calo_e;
    std::vector<float>               *m_trig_EF_calo_et;
    std::vector<float>               *m_trig_EF_calo_eta;
    std::vector<float>               *m_trig_EF_calo_phi;
    std::vector<float>               *m_trig_EF_calo_etaBE2;
    
    std::vector<float>               *m_trig_EF_el_calo_e;
    std::vector<float>               *m_trig_EF_el_calo_et;
    std::vector<float>               *m_trig_EF_el_calo_eta;
    std::vector<float>               *m_trig_EF_el_calo_phi;
    std::vector<float>               *m_trig_EF_el_calo_etaBE2;
    
    std::vector<float>               *m_trig_EF_el_e;
    std::vector<float>               *m_trig_EF_el_et;
    std::vector<float>               *m_trig_EF_el_eta;
    std::vector<float>               *m_trig_EF_el_phi;
    std::vector<float>               *m_trig_EF_el_ethad1;
    std::vector<float>               *m_trig_EF_el_ehad1;
    std::vector<float>               *m_trig_EF_el_f1;
    std::vector<float>               *m_trig_EF_el_f3;
    std::vector<float>               *m_trig_EF_el_f1core;
    std::vector<float>               *m_trig_EF_el_f3core;
    std::vector<float>               *m_trig_EF_el_weta1;
    std::vector<float>               *m_trig_EF_el_weta2;
    std::vector<float>               *m_trig_EF_el_wtots1;
    std::vector<float>               *m_trig_EF_el_fracs1;
    std::vector<float>               *m_trig_EF_el_Reta;
    std::vector<float>               *m_trig_EF_el_Rphi;
    std::vector<float>               *m_trig_EF_el_Eratio;
    std::vector<float>               *m_trig_EF_el_Rhad;
    std::vector<float>               *m_trig_EF_el_Rhad1;
    std::vector<float>               *m_trig_EF_el_deta2;
    std::vector<float>               *m_trig_EF_el_dphi2;
    std::vector<float>               *m_trig_EF_el_dphiresc;
    std::vector<float>               *m_trig_EF_el_e277;
    std::vector<float>               *m_trig_EF_el_deltaPhiRescaled2;
    std::vector<float>               *m_trig_EF_el_deltaEta1;
    std::vector<float>               *m_trig_EF_el_deltaE;
    std::vector<float>               *m_trig_EF_el_etCone;
    std::vector<float>               *m_trig_EF_el_ptCone;
    
   
    std::vector<float>               *m_trig_EF_el_trk_pt;
    std::vector<float>               *m_trig_EF_el_trk_eta; 
    std::vector<float>               *m_trig_EF_el_trk_charge; 
    std::vector<float>               *m_trig_EF_el_trk_sigd0;
    std::vector<float>               *m_trig_EF_el_trk_d0;
    std::vector<float>               *m_trig_EF_el_trk_eProbabilityHT;
    std::vector<float>               *m_trig_EF_el_trk_transformed_eProbabilityHT;
    std::vector<float>               *m_trig_EF_el_trk_d0significance;
    std::vector<float>               *m_trig_EF_el_trk_deltaPOverP;
    std::vector<float>               *m_trig_EF_el_trk_qOverP;
    std::vector<uint8_t>              *m_trig_EF_el_trk_summaryValues;

   
    std::vector<bool>                *m_trig_EF_el_hasCalo   ;
    std::vector<bool>                *m_trig_EF_el_hasTrack  ;
 

    
    std::vector<bool>                *m_trig_EF_el_loose;
    std::vector<bool>                *m_trig_EF_el_medium;
    std::vector<bool>                *m_trig_EF_el_tight;
    std::vector<bool>                *m_trig_EF_el_lhvloose;
    std::vector<bool>                *m_trig_EF_el_lhloose;
    std::vector<bool>                *m_trig_EF_el_lhmedium;
    std::vector<bool>                *m_trig_EF_el_lhtight; 
    std::vector<bool>                *m_trig_EF_calo_loose;
    std::vector<bool>                *m_trig_EF_calo_medium;
    std::vector<bool>                *m_trig_EF_calo_tight;
    std::vector<bool>                *m_trig_EF_calo_lhvloose;
    std::vector<bool>                *m_trig_EF_calo_lhloose;
    std::vector<bool>                *m_trig_EF_calo_lhmedium;
    std::vector<bool>                *m_trig_EF_calo_lhtight; 
 
    std::vector<int>                  *m_trig_tdt_L1_calo_accept;
    std::vector<int>                  *m_trig_tdt_L2_calo_accept;
    std::vector<int>                  *m_trig_tdt_L2_el_accept  ;
    std::vector<int>                  *m_trig_tdt_EF_calo_accept;
    std::vector<int>                  *m_trig_tdt_EF_el_accept  ;
    std::vector<int>                  *m_trig_tdt_emu_L1_calo_accept;
    std::vector<int>                  *m_trig_tdt_emu_L2_calo_accept;
    std::vector<int>                  *m_trig_tdt_emu_L2_el_accept  ;
    std::vector<int>                  *m_trig_tdt_emu_EF_calo_accept;
    std::vector<int>                  *m_trig_tdt_emu_EF_el_accept  ;



    // Monte Carlo
    bool                m_mc_hasMC{}     ;
    float               m_mc_pt{}        ;
    float               m_mc_eta{}       ;
    float               m_mc_phi{}       ;
    bool                m_mc_isTop{}     ;
    bool                m_mc_isParton{}  ;
    bool                m_mc_isMeson{}   ;
    bool                m_mc_isQuark{}   ;
    bool                m_mc_isTau{}     ;
    bool                m_mc_isMuon{}    ;
    bool                m_mc_isPhoton{}  ;
    bool                m_mc_isElectron{};
    bool                m_mc_hasZMother{};
    bool                m_mc_hasWMother{};

    /* 
     
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
    uint8_t             m_el_expectblayerhit;
    uint8_t             m_el_nsihits         ;
    uint8_t             m_el_nsideadsensors  ;
    uint8_t             m_el_npixdeadsensors ;
    uint8_t             m_el_nsctdeadsensors ;
    uint8_t             m_el_expectNextToInnerMostLayer;
    uint8_t             m_el_nNextToInnerMostLayerHits;
    uint8_t             m_el_nNextToInnerMostLayerOutliers;
    
    std::vector<float>               *m_trig_EF_el_charge;
    std::vector<uint8_t>             *m_trig_EF_el_nblayerhits;
    std::vector<uint8_t>             *m_trig_EF_el_nblayerolhits;
    std::vector<uint8_t>             *m_trig_EF_el_npixhits;
    std::vector<uint8_t>             *m_trig_EF_el_npixolhits;
    std::vector<uint8_t>             *m_trig_EF_el_nscthits;
    std::vector<uint8_t>             *m_trig_EF_el_nsctolhits;
    std::vector<uint8_t>             *m_trig_EF_el_ntrthightreshits;
    std::vector<uint8_t>             *m_trig_EF_el_ntrthits;
    std::vector<uint8_t>             *m_trig_EF_el_ntrthighthresolhits;
    std::vector<uint8_t>             *m_trig_EF_el_ntrtolhits;
    std::vector<uint8_t>             *m_trig_EF_el_ntrtxenonhits;
    std::vector<uint8_t>             *m_trig_EF_el_expectblayerhit;
    std::vector<uint8_t>             *m_trig_EF_el_transformed_expectblayerhit;
    std::vector<uint8_t>             *m_trig_EF_el_nsihits         ;
    std::vector<uint8_t>             *m_trig_EF_el_nsideadsensors  ;
    std::vector<uint8_t>             *m_trig_EF_el_npixdeadsensors ;
    std::vector<uint8_t>             *m_trig_EF_el_nsctdeadsensors ;
    std::vector<uint8_t>             *m_trig_EF_el_expectNextToInnerMostLayer;
    std::vector<uint8_t>             *m_trig_EF_el_nNextToInnerMostLayerHits;
    std::vector<uint8_t>             *m_trig_EF_el_nNextToInnerMostLayerOutliers;
    */
};


#endif
