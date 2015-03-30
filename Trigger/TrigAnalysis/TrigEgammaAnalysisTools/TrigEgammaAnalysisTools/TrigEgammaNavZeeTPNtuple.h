/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMANAVZEETPNTUPLE_H
#define TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMANAVZEETPNTUPLE_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTracking/Vertex.h"


class TrigEgammaNavZeeTPNtuple : public TrigEgammaNavZeeTPBaseTool,
                          virtual public ITrigEgammaAnalysisBaseTool
{
  ASG_TOOL_CLASS(TrigEgammaNavZeeTPNtuple, ITrigEgammaAnalysisBaseTool)

  public:
  
    TrigEgammaNavZeeTPNtuple( const std::string& myname );
    ~TrigEgammaNavZeeTPNtuple() {};
  
    StatusCode childInitialize();
    StatusCode childExecute();
    StatusCode childFinalize();
 
  private:

    /* helper function */ 
    template <class T> void InitBranch(TTree* fChain, std::string branch_name, T* param, bool message = true);

    void conect_branchs(TTree *t);
    void alloc_space();
    void release_space();
    void clear();

    bool attach_monteCarlo( const xAOD::Electron      *eg );
    bool attach_likelihood( const xAOD::Electron      *eg );
    bool attach_ringer    ( const HLT::TriggerElement *te );


  private:

    bool m_doRinger; // ringer stuff
    const xAOD::EventInfo *m_eventInfo;
    const xAOD::TruthParticleContainer *m_truthContainer;

     /* Branch variables */
    uint32_t            m_runNumber;
    unsigned long long  m_eventNumber;
    unsigned int        m_eventCounter;    
    float               m_el_pt;
    float               m_el_eta;
    float               m_el_phi;
    // Likelihood
    uint8_t             m_el_nSi             ;
    uint8_t             m_el_nSiDeadSensors  ;
    uint8_t             m_el_nPix            ;
    uint8_t             m_el_nSCT            ;
    uint8_t             m_el_nPixDeadSensors ;
    uint8_t             m_el_nSCTDeadSensors ;
    uint8_t             m_el_expectBlayer    ;
    uint8_t             m_el_nBlayerHits     ;
    uint8_t             m_el_nBlayerOutliers ;
    float               m_el_d0              ;
    float               m_el_deltaEta        ;
    float               m_el_deltaPhiRescaled2;
    int                 m_el_convBit ; // this no longer works
    // offline
    bool                m_el_loose;
    bool                m_el_medium;
    bool                m_el_tight;
    bool                m_el_lhLoose;
    bool                m_el_lhMedium;
    bool                m_el_lhTight; 
    int                 m_trk_nGoodVtx;
    int                 m_trk_nPileupPrimaryVtx;
    float               m_calo_et;
    float               m_calo_eta;
    float               m_calo_phi;
    // Level 2 Calo
    bool                m_trig_L1_accept;
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
    std::vector<float> *m_trig_L2_calo_rings;
    bool                m_trig_L2_calo_accept;     
    // level 2 id+Calo
    std::vector<float> *m_trig_L2_el_pt;          
    std::vector<float> *m_trig_L2_el_eta;         
    std::vector<float> *m_trig_L2_el_phi;         
    std::vector<float> *m_trig_L2_el_charge;      
    std::vector<float> *m_trig_L2_el_nTRTHits;        
    std::vector<float> *m_trig_L2_el_rcore;       
    std::vector<float> *m_trig_L2_el_eratio;      
    std::vector<float> *m_trig_L2_el_ethad;       
    std::vector<float> *m_trig_L2_el_f0;          
    std::vector<float> *m_trig_L2_el_f1;          
    std::vector<float> *m_trig_L2_el_f2;          
    std::vector<float> *m_trig_L2_el_f3;            
    std::vector<float> *m_trig_L2_el_etOverPt;          
    std::vector<float> *m_trig_L2_el_trkClusDeta; 
    std::vector<float> *m_trig_L2_el_trkClusDphi; 
    bool                m_trig_L2_el_accept;
    // Level EF
    bool                m_trig_EF_calo_accept;
    bool                m_trig_EF_el_accept;
    // Monte Carlo
    float               m_mc_pt;
    float               m_mc_eta;
    float               m_mc_phi;
    bool                m_mc_isEl;
    bool                m_mc_hasZMother;
   
 };
#endif
