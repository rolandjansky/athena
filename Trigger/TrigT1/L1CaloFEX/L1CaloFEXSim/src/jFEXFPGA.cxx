/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXFPGA  -  description
//                              -------------------
//     begin                : 19 10 2020
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/
#include "L1CaloFEXSim/jFEXFPGA.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXSmallRJetAlgo.h" 
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h" 
#include "L1CaloFEXSim/jFEXLargeRJetTOB.h"
#include "L1CaloFEXSim/jFEXLargeRJetAlgo.h" 
#include "L1CaloFEXSim/jFEXOutputCollection.h" 
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "L1CaloFEXSim/jFEXtauAlgo.h" 
#include "L1CaloFEXSim/jFEXtauTOB.h"  
#include "L1CaloFEXSim/jFEXsumETAlgo.h" 
#include "L1CaloFEXSim/jFEXsumETTOB.h"  
#include "L1CaloFEXSim/jFEXmetAlgo.h" 
#include "L1CaloFEXSim/jFEXmetTOB.h"  
#include "L1CaloFEXSim/jFEXForwardJetsAlgo.h"
#include "L1CaloFEXSim/jFEXForwardJetsInfo.h"
#include "L1CaloFEXSim/jFEXForwardElecAlgo.h"
#include "L1CaloFEXSim/jFEXPileupAndNoise.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>
#include "TrigConfData/L1Menu.h"
#include "TH1F.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "SGTools/TestStore.h"
#include "StoreGate/StoreGateSvc.h"


namespace LVL1 {

  // default constructor for persistency

jFEXFPGA::jFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent): AthAlgTool(type,name,parent) {
    declareInterface<IjFEXFPGA>(this);
}

/** Destructor */
jFEXFPGA::~jFEXFPGA()
{
}

//================ Initialisation =================================================
  
StatusCode jFEXFPGA::initialize() {

    ATH_CHECK(m_jTowerContainerKey.initialize());
    ATH_CHECK(m_l1MenuKey.initialize());
    ATH_CHECK(m_jFEXtauAlgoTool.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode jFEXFPGA::init(int id, int jfexid) {
    m_id = id;
    m_jfexid = jfexid;

    return StatusCode::SUCCESS;

}

void jFEXFPGA::reset() {

    m_id = -1;
    m_jfexid = -1;
    m_tau_tobwords.clear();
    m_SRJet_tobwords.clear();
    m_LRJet_tobwords.clear();
    m_sumET_tobwords.clear();
    m_Met_tobwords.clear();
    m_map_Etvalues_FPGA.clear();
    m_map_EM_Etvalues_FPGA.clear();
    m_map_HAD_Etvalues_FPGA.clear();

}

StatusCode jFEXFPGA::execute(jFEXOutputCollection* inputOutputCollection) {
    


    SG::ReadHandle<jTowerContainer> jTowerContainer(m_jTowerContainerKey/*,ctx*/);
    if(!jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve container " << m_jTowerContainerKey.key() );
        return StatusCode::FAILURE;
    }
    
    ATH_CHECK( m_jFEXPileupAndNoiseTool->safetyTest());
    ATH_CHECK( m_jFEXPileupAndNoiseTool->reset());
    if(m_jfexid == 0 || m_jfexid == 5) {
        m_jFEXPileupAndNoiseTool->setup(m_jTowersIDs_Wide);
    }
    else {
        m_jFEXPileupAndNoiseTool->setup(m_jTowersIDs_Thin);
    }

    //Calculating and sustracting pileup
    std::vector<float> pileup_rho;
    pileup_rho = m_jFEXPileupAndNoiseTool->CalculatePileup();

    //Applying pileup sustraction in jet or met independently - this sets the flags to true in m_jFEXPileupAndNoiseTool
    m_jFEXPileupAndNoiseTool->ApplyPileup2Jets(true);
    m_jFEXPileupAndNoiseTool->ApplyPileup2Met(true);
    
    //Noise should be always applied
    m_jFEXPileupAndNoiseTool->ApplyNoise2Jets(true);
    m_jFEXPileupAndNoiseTool->ApplyNoise2Met(true);
    //Getting the values
    m_map_HAD_Etvalues_FPGA = m_jFEXPileupAndNoiseTool->Get_HAD_Et_values();
    m_map_EM_Etvalues_FPGA  = m_jFEXPileupAndNoiseTool->Get_EM_Et_values();
    m_map_Etvalues_FPGA     = m_jFEXPileupAndNoiseTool->GetEt_values();
    std::vector<int> pileup_ID;
    std::vector<int> pileup_HAD_jet;
    std::vector<int> pileup_EM_jet;
    std::vector<int> pileup_Total_jet;
    std::vector<int> pileup_HAD_met;
    std::vector<int> pileup_EM_met;
    std::vector<int> pileup_Total_met;
    for (auto const& [key, val] : m_map_HAD_Etvalues_FPGA)
    {
        pileup_ID.push_back(key);
        pileup_HAD_jet.push_back(val[0]);
        pileup_EM_jet.push_back(m_map_EM_Etvalues_FPGA[key][0]);
        pileup_Total_jet.push_back(m_map_Etvalues_FPGA[key][0]);
        pileup_HAD_met.push_back(val[1]);
        pileup_EM_met.push_back(m_map_EM_Etvalues_FPGA[key][1]);
        pileup_Total_met.push_back(m_map_Etvalues_FPGA[key][1]);
    }    
    
    //saving pileup information
    inputOutputCollection->addValue_pileup("pileup_FPGAid", m_id);
    inputOutputCollection->addValue_pileup("pileup_jFEXid", m_jfexid);
    inputOutputCollection->addValue_pileup("pileup_rho_EM", pileup_rho[0]);
    inputOutputCollection->addValue_pileup("pileup_rho_HAD1", pileup_rho[1]);
    inputOutputCollection->addValue_pileup("pileup_rho_HAD2", pileup_rho[2]);
    inputOutputCollection->addValue_pileup("pileup_rho_HAD3", pileup_rho[3]);
    inputOutputCollection->addValue_pileup("pileup_rho_FCAL", pileup_rho[4]);
    inputOutputCollection->addValue_pileup("pileup_map_ID"  , pileup_ID);
    inputOutputCollection->addValue_pileup("pileup_map_Et_values_HAD_jet"  , pileup_HAD_jet);
    inputOutputCollection->addValue_pileup("pileup_map_Et_values_EM_jet"   , pileup_EM_jet);
    inputOutputCollection->addValue_pileup("pileup_map_Et_values_Total_jet", pileup_Total_jet);
    inputOutputCollection->addValue_pileup("pileup_map_Et_values_HAD_met"  , pileup_HAD_met);
    inputOutputCollection->addValue_pileup("pileup_map_Et_values_EM_met"   , pileup_EM_met);
    inputOutputCollection->addValue_pileup("pileup_map_Et_values_Total_met", pileup_Total_met);
    inputOutputCollection->fill_pileup();    
    
    if(m_id==0 || m_id==2) {
        ATH_CHECK( m_jFEXsumETAlgoTool->safetyTest());
        ATH_CHECK( m_jFEXsumETAlgoTool->reset());
        ATH_CHECK( m_jFEXmetAlgoTool->safetyTest());
        ATH_CHECK( m_jFEXmetAlgoTool->reset());
        
        m_jFEXsumETAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        m_jFEXmetAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
           
           
        //Retrieve the L1 menu configuration
        SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
        const TrigConf::L1ThrExtraInfo_jTE & thr_jTE = l1Menu->thrExtraInfo().jTE();
        std::string str_jfexname = m_jfex_string[m_jfexid];
        unsigned int bin_pos = thr_jTE.etaBoundary_fw(str_jfexname);

        if(m_jfexid > 0 && m_jfexid < 5) {

            //-----------------jFEXsumETAlgo-----------------
            m_jFEXsumETAlgoTool->setup(m_jTowersIDs_Thin);
            m_jFEXsumETAlgoTool->buildBarrelSumET();
            m_sumET_tobwords.push_back(formSumETTOB(m_jFEXsumETAlgoTool->getETlowerEta(bin_pos),m_jFEXsumETAlgoTool->getETupperEta(bin_pos)));

            //-----------------jFEXmetAlgo-----------------
            m_jFEXmetAlgoTool->setup(m_jTowersIDs_Thin);
            m_jFEXmetAlgoTool->buildBarrelmet();
            m_Met_tobwords.push_back(formMetTOB(m_jFEXmetAlgoTool->GetMetXComponent(),m_jFEXmetAlgoTool->GetMetYComponent()));
        }
        else if(m_jfexid == 0 ) {
            int flipped_jTowersIDs      [FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width] = {{0}};
            int max_phi_it = FEXAlgoSpaceDefs::jFEX_algoSpace_height-1;
            int max_eta_it = FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width-1;
            for(int mphi = 0; mphi <= max_phi_it; mphi++) {
                for(int meta = 0; meta <= max_eta_it; meta++) {
                    flipped_jTowersIDs[mphi][meta]=m_jTowersIDs_Wide[mphi][max_eta_it-meta];
                }
            }
            //-----------------jFEXsumETAlgo-----------------
            m_jFEXsumETAlgoTool->setup(flipped_jTowersIDs);
            m_jFEXsumETAlgoTool->buildFWDSumET();
            m_sumET_tobwords.push_back(formSumETTOB(m_jFEXsumETAlgoTool->getETlowerEta(bin_pos),m_jFEXsumETAlgoTool->getETupperEta(bin_pos)));

            //-----------------jFEXmetAlgo-----------------
            m_jFEXmetAlgoTool->setup(flipped_jTowersIDs);
            m_jFEXmetAlgoTool->buildFWDmet();
            m_Met_tobwords.push_back(formMetTOB(m_jFEXmetAlgoTool->GetMetXComponent(),m_jFEXmetAlgoTool->GetMetYComponent()));
        }
        else if(m_jfexid == 5) {
            //-----------------jFEXsumETAlgo-----------------
            m_jFEXsumETAlgoTool->setup(m_jTowersIDs_Wide);
            m_jFEXsumETAlgoTool->buildFWDSumET();
            m_sumET_tobwords.push_back(formSumETTOB(m_jFEXsumETAlgoTool->getETlowerEta(bin_pos),m_jFEXsumETAlgoTool->getETupperEta(bin_pos)));

            //-----------------jFEXmetAlgo-----------------
            m_jFEXmetAlgoTool->setup(m_jTowersIDs_Wide);
            m_jFEXmetAlgoTool->buildFWDmet();
            m_Met_tobwords.push_back(formMetTOB(m_jFEXmetAlgoTool->GetMetXComponent(),m_jFEXmetAlgoTool->GetMetYComponent()));
        }
    }

    //-----------jFEXSmallRJet & Large R Jet Algo-----------------
    ATH_MSG_DEBUG("================ Central Algorithms ================");

    //Central region algorithms
    if(m_jfexid > 0 && m_jfexid < 5) {
        m_jFEXSmallRJetAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        m_jFEXLargeRJetAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        m_jFEXtauAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        
        for(int mphi = 8; mphi < FEXAlgoSpaceDefs::jFEX_algoSpace_height -8; mphi++) {
            for(int meta = 8; meta < FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width -8; meta++) {
                //definition of arrays
                int TT_seed_ID[3][3]= {{0}};
                int TT_searchWindow_ID[5][5]= {{0}};
                int TT_First_ETring[36]= {0};
                int First_ETring_it = 0; 
                               
                int SRJet_SearchWindow[7][7] = {{0}};
                int largeRCluster_IDs[15][15]= {{0}};

                //filling up array to send them to the algorithms
                for(int i = -7; i< 8; i++ ) {
                    for(int j = -7; j< 8; j++) {

                        if(std::abs(i)<4 && std::abs(j)<4) {
                            SRJet_SearchWindow[3 + i][3 + j] = m_jTowersIDs_Thin[mphi + i][meta +j];
                        }
                        
                        uint deltaR = std::sqrt(std::pow(i,2)+std::pow(j,2));
                        
                        if(deltaR<3) {
                            TT_searchWindow_ID[i+2][j+2] = m_jTowersIDs_Thin[mphi +i][meta +j]; // Search window for the tau algo used for the LocalMaxima studies
                        }

                        if(deltaR<2) {
                            TT_seed_ID[i+1][j+1] = m_jTowersIDs_Thin[mphi +i][meta +j]; // Seed 0.3x0.3 in phi-eta plane
                        }
                        else if(deltaR<4) {
                            TT_First_ETring[First_ETring_it]= m_jTowersIDs_Thin[mphi +i][meta +j]; // First energy ring, will be used as tau ISO
                            ++First_ETring_it;

                        }                       
                        else if(deltaR<8) {
                            largeRCluster_IDs[7 +i][7 +j] = m_jTowersIDs_Thin[mphi + i][meta +j];
                        }
                        

                    }
                }                
                
                // ********  jJ and jLJ algorithms  ********
                
                m_jFEXSmallRJetAlgoTool->setup(SRJet_SearchWindow);
                m_jFEXLargeRJetAlgoTool->setupCluster(largeRCluster_IDs);
                m_jFEXSmallRJetAlgoTool->buildSeeds();
                
                bool is_Jet_LM = m_jFEXSmallRJetAlgoTool->isSeedLocalMaxima();
                if(is_Jet_LM) {
                    
                    //check if Local Maxima is displaced.
                    bool check = m_jFEXSmallRJetAlgoTool->checkDisplacedLM();
                    
                    int meta_LM = meta;
                    int mphi_LM = mphi;
                    if(check && meta>8 && mphi>8) {
                        meta_LM = meta -1;
                        mphi_LM = mphi -1;

                    }

                    //Creating SR TOB
                    uint32_t SRJet_tobword = formSmallRJetTOB(mphi_LM, meta_LM);
                    std::vector<uint32_t> SRtob_aux{SRJet_tobword,(uint32_t) m_jTowersIDs_Thin[mphi_LM][meta_LM]};
                    if ( SRJet_tobword != 0 ) m_SRJet_tobwords.push_back( SRtob_aux);
                    
                    //Creating LR TOB
                    uint32_t LRJet_tobword = formLargeRJetTOB(mphi_LM, meta_LM);
                    std::vector<uint32_t> LRtob_aux{LRJet_tobword,(uint32_t) m_jTowersIDs_Thin[mphi_LM][meta_LM]};
                    if ( LRJet_tobword != 0 ) m_LRJet_tobwords.push_back(LRtob_aux);
                }
                // ********  jTau algorithm  ********
                ATH_CHECK( m_jFEXtauAlgoTool->safetyTest());
                m_jFEXtauAlgoTool->setup(TT_searchWindow_ID,TT_seed_ID);
                m_jFEXtauAlgoTool->buildSeeds();
                bool is_tau_LocalMax = m_jFEXtauAlgoTool->isSeedLocalMaxima();
        
                // Save TOB is tau is a local maxima
                if ( is_tau_LocalMax ) { 
                    
                    //calculates the 1st energy ring
                    m_jFEXtauAlgoTool->setFirstEtRing(TT_First_ETring);
                    
                    uint32_t jTau_tobword = formTauTOB(mphi, meta);
                    std::vector<uint32_t> TAUtob_aux{jTau_tobword,(uint32_t) m_jTowersIDs_Thin[mphi][meta]};
                    if ( jTau_tobword != 0 ) m_tau_tobwords.push_back(TAUtob_aux); 
                }                
            }
        }

        
    } //end of if statement for checking if in central jfex modules
    
    //FCAL region algorithm
    if(m_jfexid ==0 || m_jfexid ==5) {
        
        //**********Forward Jets***********************
        ATH_CHECK(m_jFEXForwardJetsAlgoTool->safetyTest());
        ATH_CHECK(m_jFEXForwardJetsAlgoTool->reset());
        m_jFEXForwardJetsAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        m_jFEXForwardJetsAlgoTool->setup(m_jTowersIDs_Wide,m_jfexid,m_id);

        m_FCALJets =  m_jFEXForwardJetsAlgoTool->calculateJetETs();
        for(std::unordered_map<int, jFEXForwardJetsInfo>::iterator it = m_FCALJets.begin(); it!=(m_FCALJets.end()); ++it) {

            uint32_t TTID = it->first;
            jFEXForwardJetsInfo FCALJets = it->second;
            

            int iphi = FCALJets.getCentreLocalTTPhi();
            int ieta = FCALJets.getCentreLocalTTEta();

            m_SRJetET = FCALJets.getSeedET() + FCALJets.getFirstEnergyRingET();
            m_LRJetET = m_SRJetET + FCALJets.getSecondEnergyRingET();
            
            
            
            uint32_t SRFCAL_Jet_tobword = formSmallRJetTOB(iphi, ieta);
            std::vector<uint32_t> SRtob_aux{SRFCAL_Jet_tobword,TTID};
            if ( SRFCAL_Jet_tobword != 0 ) m_SRJet_tobwords.push_back(SRtob_aux);
            
            uint32_t LRFCAL_Jet_tobword = 0;
            if(std::fabs(FCALJets.getCentreTTEta())<2.6){
                LRFCAL_Jet_tobword = formLargeRJetTOB(iphi, ieta);
                std::vector<uint32_t> LRtob_aux{LRFCAL_Jet_tobword,TTID};
                if ( LRFCAL_Jet_tobword != 0 ) m_LRJet_tobwords.push_back(LRtob_aux);                
            }
            
        }
        //********** Forward Electrons ***********************
        ATH_CHECK(m_jFEXForwardElecAlgoTool->safetyTest());
        ATH_CHECK(m_jFEXForwardElecAlgoTool->reset());
        m_jFEXForwardElecAlgoTool->setFPGAEnergy(m_map_EM_Etvalues_FPGA,m_map_HAD_Etvalues_FPGA);
        
        /* This is a work in progress, PLEASE DO NOT REMOVE IT YET
         * To enter in the algorithm, just uncomment the line below
         * it should also enter when is a FWD FPGA so 8 times per eventloop
        m_jFEXForwardElecAlgoTool->setup();        
        */
        

    
        //******************************** TAU **********************************************
        int jTowersIDs      [FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width] = {{0}};
        int max_meta=17;
        
        if(m_jfexid ==0) {

            for(int i=0; i<FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++) {
                for(int j=28; j<(FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width-6); j++) { //lower values of j (j<28) are the Fcals not entering in the jFEX tau range
                    jTowersIDs[i][j-28+8]=m_jTowersIDs_Wide[i][j]; // second argument in m_jTowersIDs is to center the FPGA core area in te same region as the central FPGAs
                }
            }
        }
        else if(m_jfexid ==5 ) {

            // Filling m_jTowersIDs with the m_jTowersIDs_Wide ID values up to 2.5 eta
            for(int i=0; i<FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++) {
                for(int j=4; j<17; j++) { //higher values of j (j>16) are the Fcals not entering in the jFEX tau range
                    jTowersIDs[i][j]=m_jTowersIDs_Wide[i][j];
                }
            }
        }
        ATH_MSG_DEBUG("============================ jFEXtauAlgo ============================");
        ATH_CHECK( m_jFEXtauAlgoTool->safetyTest());
        m_jFEXtauAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        for(int mphi = 8; mphi < 24; mphi++) {
            for(int meta = 8; meta < max_meta; meta++) {

                int TT_seed_ID[3][3]= {{0}};
                int TT_searchWindow_ID[5][5]= {{0}};
                int TT_First_ETring[36]= {0};
                int First_ETring_it = 0;

                for(int i = -3; i< 4; i++ ) {
                    for(int j = -3; j< 4; j++) {
                        int DeltaR = std::sqrt(std::pow(i,2)+std::pow(j,2));
                        if(DeltaR<3) {
                            TT_searchWindow_ID[i+2][j+2] = jTowersIDs[mphi +i][meta +j]; // Search window for the tau algo used for the LocalMaxima studies
                        }

                        if(DeltaR<2) {
                            TT_seed_ID[i+1][j+1] = jTowersIDs[mphi +i][meta +j]; // Seed 0.3x0.3 in phi-eta plane
                        }
                        else if(DeltaR<4) {
                            TT_First_ETring[First_ETring_it]= jTowersIDs[mphi +i][meta +j]; // First energy ring, will be used as tau ISO
                            ++First_ETring_it;

                        }
                    }
                }

                m_jFEXtauAlgoTool->setup(TT_searchWindow_ID,TT_seed_ID);
                m_jFEXtauAlgoTool->buildSeeds();
                bool is_tau_LocalMax = m_jFEXtauAlgoTool->isSeedLocalMaxima();
        
                // Save TOB is tau is a local maxima
                if ( is_tau_LocalMax ) { 
                    
                    //calculates the 1st energy ring
                    m_jFEXtauAlgoTool->setFirstEtRing(TT_First_ETring);
                    
                    uint32_t jTau_tobword = formTauTOB(mphi, meta);
                    std::vector<uint32_t> TAUtob_aux{jTau_tobword,(uint32_t) jTowersIDs[mphi][meta]};
                    if ( jTau_tobword != 0 ) m_tau_tobwords.push_back(TAUtob_aux); 
                }
            }
        }
    } //end of if statement for checking if in central jfex modules
    return StatusCode::SUCCESS;
} //end of the execute function

void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]){
    
  const int rows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
  const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
  
  std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Wide[0][0]);

  ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") [on jFEX " << m_jfexid << "] IS RESPONSIBLE FOR jTOWERS :");

  for (int thisRow=rows-1; thisRow>=0; thisRow--){
    for (int thisCol=0; thisCol<cols; thisCol++){
      if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_jTowersIDs_Wide[thisRow][thisCol] << "  "); }
      else { ATH_MSG_DEBUG("|  " << m_jTowersIDs_Wide[thisRow][thisCol] << "  |"); }
    }
  }
  
}

void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) {

    const int rows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
    const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

    std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Thin[0][0]);

    //this prints out the jTower IDs that each FPGA is responsible for
    ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") [on jFEX " << m_jfexid << "] IS RESPONSIBLE FOR jTOWERS :");

    for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  |");
            }
        }
    }

}

std::vector <std::vector <uint32_t>> jFEXFPGA::getSmallRJetTOBs()
{
    auto tobsSort = m_SRJet_tobwords;
    
    ATH_MSG_DEBUG("number of smallRJet tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
    // sort tobs by their et ( 11 bits of the 32 bit tob word)
    std::sort (tobsSort.begin(), tobsSort.end(), etSRJetSort);
    
    while(tobsSort.size()<7){
        std::vector <uint32_t> v{0,0};
        tobsSort.push_back(v);
    }
    tobsSort.resize(7);

    return tobsSort;

}

std::vector <std::vector <uint32_t>> jFEXFPGA::getLargeRJetTOBs()
{
    auto tobsSort = m_LRJet_tobwords;

    ATH_MSG_DEBUG("number of largeRJet tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
    //sort tobs by their et ( 13 bits of the 32 bit tob word)
    std::sort (tobsSort.begin(), tobsSort.end(), etLRJetSort);
    
    while(tobsSort.size()<1) {
        std::vector <uint32_t> v{0,0};
        tobsSort.push_back(v);
    }
    tobsSort.resize(1);



    return tobsSort;

}

uint32_t jFEXFPGA::formSmallRJetTOB(int &iphi, int &ieta) {
    uint32_t tobWord = 0;
    unsigned int eta = 0;
    unsigned int phi = 0;
    unsigned int et = 0;
    unsigned int jFEXSmallRJetTOBEt = 0;
    int Res = 0; // 11 bits reserved
    int Sat = 0; //  1 bit for saturation. Set to 1 when jet energy is saturated

    if(m_jfexid > 0 && m_jfexid < 5) {
        et = m_jFEXSmallRJetAlgoTool->getSmallClusterET();
        eta = ieta -8;
        phi = iphi -8;
    }
    else if(m_jfexid == 5) {
        et = m_SRJetET;
        eta = ieta -8;
        if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta) { // ieta lower than EMIE stats -> belong to EMB
            phi = iphi-8;
        }
        else if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta) { // ieta lower than FCAL stats -> belong to EMIE
            phi = iphi -4;
        }
        else { // rest ieta belongs to FCAL
            phi = iphi -2;
        }
    }
    else if(m_jfexid == 0) {
        et = m_SRJetET;   
        eta = 36 - ieta;
        
        if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_end_eta) { // ieta lower than FCal ends -> FCAL
            phi = iphi -2 ;
        }
        else if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_end_eta) {// ieta lower than EMIE ends -> EMIE
            phi = iphi -4 ;
        }
        else {// rest of ieta -> EMB
            phi = iphi -8 ;
        } 
    }
    
    //Initializing Trigger menu
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    const TrigConf::L1ThrExtraInfo_jJ & thr_jJ = l1Menu->thrExtraInfo().jJ();
    
    // retrieving the jJ ET resolution for TOBs
    const int jFEXETResolution = thr_jJ.resolutionMeV(); 
    
    jFEXSmallRJetTOBEt = et/jFEXETResolution;
    if(jFEXSmallRJetTOBEt > 0x7ff) {
        jFEXSmallRJetTOBEt = 0x7ff;
        Sat = 1;
    }
    //create basic tobword with 32 bits
    tobWord = tobWord + (Res << FEXAlgoSpaceDefs::jJ_resBit) + (jFEXSmallRJetTOBEt << FEXAlgoSpaceDefs::jJ_etBit) + (eta << FEXAlgoSpaceDefs::jJ_etaBit) + (phi << FEXAlgoSpaceDefs::jJ_phiBit)  + (Sat);
    ATH_MSG_DEBUG("tobword smallRJet with res, et, eta and phi: " << std::bitset<32>(tobWord) );
    
    // retrieving the threshold for the TOB Et
    std::string str_jfexname = m_jfex_string[m_jfexid];
    unsigned int minEtThreshold = thr_jJ.ptMinToTopoMeV(str_jfexname)/jFEXETResolution;
    
    if (jFEXSmallRJetTOBEt < minEtThreshold) return 0;
    else return tobWord;
}

uint32_t jFEXFPGA::formLargeRJetTOB(int &iphi, int &ieta) {
    
    uint32_t tobWord = 0;
    unsigned int eta = 0;
    unsigned int phi = 0;
    unsigned int et = 0;
    unsigned int jFEXLargeRJetTOBEt = 0;
    int Res = 0; // 9 bits reserved
    int Sat = 0; //  1 bit for saturation. Set to 1 when jet energy is saturated

    if(m_jfexid > 0 && m_jfexid < 5) {
        et = m_jFEXLargeRJetAlgoTool->getLargeClusterET(m_jFEXSmallRJetAlgoTool->getSmallClusterET(),m_jFEXLargeRJetAlgoTool->getRingET());
        eta = ieta -8;
        phi = iphi -8;
    }

    else if(m_jfexid == 5) {
        et = m_LRJetET;
        eta = ieta -8;

        if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta) { // ieta lower than EMIE stats -> belong to EMB
            phi = iphi-8;
        }
        else if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta) { // ieta lower than FCAL stats -> belong to EMIE
            phi = iphi -4;
        }
        else { // rest ieta belongs to FCAL
            phi = iphi -2;
        }
    }
    else if(m_jfexid == 0) {
        et = m_LRJetET;
        eta = 36 - ieta;

        if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_end_eta) { // ieta lower than FCal ends -> FCAL
            phi = iphi -2 ;
        }
        else if(ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_end_eta) {// ieta lower than EMIE ends -> EMIE
            phi = iphi -4 ;
        }
        else {// rest of ieta -> EMB
            phi = iphi -8 ;
        }
    }
    
    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    const TrigConf::L1ThrExtraInfo_jLJ & thr_jLJ = l1Menu->thrExtraInfo().jLJ();
    const int jFEXETResolution = thr_jLJ.resolutionMeV(); //LSB is 200MeV

    jFEXLargeRJetTOBEt = et/jFEXETResolution;
    if (jFEXLargeRJetTOBEt > 0x1fff) {
        jFEXLargeRJetTOBEt = 0x1fff;  //0x1fff is 13 bits
        Sat = 1;
    }
    //create basic tobword with 32 bits
    tobWord = tobWord + (Res << FEXAlgoSpaceDefs::jLJ_resBit) + (jFEXLargeRJetTOBEt << FEXAlgoSpaceDefs::jLJ_etBit) + (eta << FEXAlgoSpaceDefs::jLJ_etaBit) + (phi << FEXAlgoSpaceDefs::jLJ_phiBit) + (Sat);
    ATH_MSG_DEBUG("tobword largeRJet with res, et, eta, phi: " << std::bitset<32>(tobWord) );

    
    std::string str_jfexname = m_jfex_string[m_jfexid];
    unsigned int minEtThreshold = thr_jLJ.ptMinToTopoMeV(str_jfexname)/jFEXETResolution;

    if (jFEXLargeRJetTOBEt < minEtThreshold) return 0;
    else return tobWord;
}


uint32_t jFEXFPGA::formTauTOB(int & iphi, int &ieta )
{
    uint32_t tobWord = 0;
    
    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    const TrigConf::L1ThrExtraInfo_jTAU & thr_jTAU = l1Menu->thrExtraInfo().jTAU();    
    const int jFEXETResolution = thr_jTAU.resolutionMeV(); //LSB is 200MeV

    int eta = ieta-8; // needed to substract 8 to be in the FPGA core area
    int phi = iphi-8; // needed to substract 8 to be in the FPGA core area
    int sat = 0; //1 bit for saturation flag, not coded yet

    unsigned int et = m_jFEXtauAlgoTool->getClusterEt()/jFEXETResolution;
    if (et > 0x7ff) { //0x7ff is 11 bits
        ATH_MSG_DEBUG("Et saturated: " << et );
        et = 0x7ff;
        sat=1;
    }

    unsigned int iso = m_jFEXtauAlgoTool->getFirstEtRing()/jFEXETResolution;
    if (iso > 0x7ff) iso = 0x7ff;  //0x7ff is 11 bits

    //create basic tobword with 32 bits
    tobWord = tobWord + (iso << FEXAlgoSpaceDefs::jTau_isoBit) + (et << FEXAlgoSpaceDefs::jTau_etBit) + (eta << FEXAlgoSpaceDefs::jTau_etaBit) + (phi << FEXAlgoSpaceDefs::jTau_phiBit) + sat ;

    ATH_MSG_DEBUG("tobword tau with iso, et, eta and phi: " << std::bitset<32>(tobWord) );


    std::string str_jfexname = m_jfex_string[m_jfexid];
    unsigned int minEtThreshold = thr_jTAU.ptMinToTopoMeV(str_jfexname)/jFEXETResolution;

    if (et < minEtThreshold) return 0;
    else return tobWord;

}


std::vector <std::vector <uint32_t>> jFEXFPGA::getTauTOBs() {
    auto tobsSort = m_tau_tobwords;

    ATH_MSG_DEBUG("number of tau tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
    // sort tobs by their et ( 13 bits of the 32 bit tob word)
    std::sort (tobsSort.begin(), tobsSort.end(), etTauSort);

    
    while(tobsSort.size()<6) {
        std::vector <uint32_t> v{0,0};
        tobsSort.push_back(v);
    }
    tobsSort.resize(6);
    
    return tobsSort;

}

std::vector <std::vector <uint32_t>> jFEXFPGA::getTauxTOBs()
{

  return m_tau_tobwords;

}

uint32_t jFEXFPGA::formSumETTOB(int ETlow, int EThigh )
{
    uint32_t tobWord = 0;

    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    const TrigConf::L1ThrExtraInfo_jTE & thr_jTE = l1Menu->thrExtraInfo().jTE();
    const int jFEXETResolution = thr_jTE.resolutionMeV(); //LSB is 200MeV

    int satlow = 0;
    int sathigh = 0;

    unsigned int etlow = ETlow/jFEXETResolution;
    if (etlow > 0x7fff) { //0x7fff is 15 bits
        ATH_MSG_DEBUG("sumEtlow saturated: " << etlow );
        etlow = 0x7fff;
        satlow=1;
    }

    unsigned int ethigh = EThigh/jFEXETResolution;
    if (ethigh > 0x7fff) { //0x7fff is 15 bits
        ATH_MSG_DEBUG("sumEthigh saturated: " << ethigh );
        ethigh = 0x7fff;
        sathigh=1;
    }

    //create basic tobword with 32 bits
    tobWord = tobWord + (sathigh << FEXAlgoSpaceDefs::jTE_Sat_upperBit) + (ethigh << FEXAlgoSpaceDefs::jTE_Et_upperBit) + (etlow << FEXAlgoSpaceDefs::jTE_Et_lowerBit) + (satlow << FEXAlgoSpaceDefs::jTE_Sat_lowerBit) ;
    ATH_MSG_DEBUG("tobword SumET with Sathigh, EThigh, ETlow and Satlow  : " << std::bitset<32>(tobWord) );

    return tobWord;

}

std::vector <uint32_t> jFEXFPGA::getSumEtTOBs() {
    auto tobsSort = m_sumET_tobwords;

    ATH_MSG_DEBUG("number of SumEt tobs: " << tobsSort.size() << " in FPGA: " << m_id);
    return tobsSort;

}

uint32_t jFEXFPGA::formMetTOB(int METX, int METY ) {
    uint32_t tobWord = 0;

    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    const TrigConf::L1ThrExtraInfo_jXE & thr_jXE = l1Menu->thrExtraInfo().jXE();
    const int jFEXETResolution = thr_jXE.resolutionMeV(); //LSB is 200MeV

    int sat = 0;
    int res = 0;
    
    

    int metX = METX/jFEXETResolution;
    int metY = METY/jFEXETResolution;

    //0x7fff is 15 bits (decimal value 32767), however as MET is a signed value (can be negative) only 14 bits are allowed (16383) the MSB is the sign
    if (std::abs(metX) > 0x3fff) {
        ATH_MSG_DEBUG("sumEtlow saturated: " << metX );
        metX = 0x7fff;
        sat=1;
    }

    
    if (std::abs(metY) > 0x3fff) { //0x7fff is 15 bits (decimal value 32767), however as MET is a signed value (can be negative) only 14 bits are allowed (16383)
        ATH_MSG_DEBUG("sumEthigh saturated: " << metY );
        metY = 0x7fff;
        sat=1;
    }

    //create basic tobword with 32 bits
    tobWord = tobWord + (res << FEXAlgoSpaceDefs::jXE_ResBit) + ((metY & 0x7fff) << FEXAlgoSpaceDefs::jXE_Ey_Bit) + ((metX & 0x7fff) << FEXAlgoSpaceDefs::jXE_Ex_Bit) + (sat << FEXAlgoSpaceDefs::jXE_SatBit)  ;
    ATH_MSG_DEBUG("tobword MET with Res, MET_Y, MET_X, Sat: " << std::bitset<32>(tobWord) );

    return tobWord;

}

std::vector <uint32_t> jFEXFPGA::getMetTOBs() {
    auto tobsSort = m_Met_tobwords;

    ATH_MSG_DEBUG("number of Met tobs: " << tobsSort.size() << " in FPGA: " << m_id);
    return tobsSort;

}


//Returns the Electromagnetic energy for Jet Algos (NOT MET/SumET)
int jFEXFPGA::getTTowerET_EM(unsigned int TTID) {
    
    if(m_map_EM_Etvalues_FPGA.find(TTID) != m_map_EM_Etvalues_FPGA.end()){
        return m_map_EM_Etvalues_FPGA[TTID][0];
    }
    
    ATH_MSG_DEBUG("In jFEXFPGA::getTTowerET_EM, TTower ID not found in map: " << TTID );
    return -99999;
    
}


//Returns the Hadronic energy for Jet Algos (NOT MET/SumET)
int jFEXFPGA::getTTowerET_HAD(unsigned int TTID) {
    
    if(m_map_HAD_Etvalues_FPGA.find(TTID) != m_map_HAD_Etvalues_FPGA.end()){
        return m_map_HAD_Etvalues_FPGA[TTID][0];
    }
    
    ATH_MSG_DEBUG("In jFEXFPGA::getTTowerET_HAD, TTower ID not found in map: " << TTID );
    return -99999;
    
}


//Returns the Total TT energy for Jet Algos (NOT MET/SumET)
int jFEXFPGA::getTTowerET(unsigned int TTID) {

    return getTTowerET_EM(TTID)+getTTowerET_HAD(TTID);

}  


//Returns the Total TT energy for MET/SumÉT Algos
int jFEXFPGA::getTTowerET_forMET(unsigned int TTID) {

    int tmp_EM = 0;
    if(m_map_EM_Etvalues_FPGA.find(TTID) != m_map_EM_Etvalues_FPGA.end()){
        tmp_EM = m_map_EM_Etvalues_FPGA[TTID][1];
    }
    else{
        ATH_MSG_DEBUG("In jFEXFPGA::getTTowerET_forMET (EM energy), TTower ID not found in map: " << TTID );
        tmp_EM = -99999;
    }


    int tmp_HAD = 0;
    if(m_map_HAD_Etvalues_FPGA.find(TTID) != m_map_HAD_Etvalues_FPGA.end()){
        tmp_HAD = m_map_HAD_Etvalues_FPGA[TTID][1];
    }
    else{
        ATH_MSG_DEBUG("In jFEXFPGA::getTTowerET_forMET (HAD energy), TTower ID not found in map: " << TTID );
        tmp_HAD = -99999;
    }
    
    
    return tmp_EM + tmp_HAD;

}  


//Returns de ET of a given TT ID for Algorithm
int jFEXFPGA::getTTowerET_SG(unsigned int TTID) {

    SG::ReadHandle<jTowerContainer> jTowerContainer(m_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jTowerContainer->findTower(TTID);
    return tmpTower->getTotalET();
}





} // end of namespace bracket
