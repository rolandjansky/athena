/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "L1CaloFEXSim/jFEXPileupAndNoise.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>
#include "TH1F.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "SGTools/TestStore.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfData/L1Menu.h"

namespace LVL1 {

  // default constructor for persistency

jFEXFPGA::jFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent)
{
  declareInterface<IjFEXFPGA>(this);
}

/** Destructor */
jFEXFPGA::~jFEXFPGA()
{
}

//================ Initialisation =================================================
  
StatusCode jFEXFPGA::initialize()
{

  ATH_CHECK(m_jFEXFPGA_jTowerContainerKey.initialize());
  ATH_CHECK(m_jFEXFPGA_jFEXOutputCollectionKey.initialize());
  ATH_CHECK(m_l1MenuKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode jFEXFPGA::init(int id, int jfexid)
{
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

}

StatusCode jFEXFPGA::execute() {

  SG::ReadHandle<jTowerContainer> jk_jFEXFPGA_jTowerContainer(m_jFEXFPGA_jTowerContainerKey/*,ctx*/);
  if(!jk_jFEXFPGA_jTowerContainer.isValid()) {
    ATH_MSG_FATAL("Could not retrieve jk_jFEXFPGA_jTowerContainer " << m_jFEXFPGA_jTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }
  jFEXOutputCollection* jFEXOutputs = nullptr;
  ATH_CHECK( evtStore()->retrieve(jFEXOutputs, "jFEXOutputCollection") );

  
    //ATH_CHECK( m_jFEXPileupAndNoiseTool.retrieve());
    ATH_CHECK( m_jFEXPileupAndNoiseTool->safetyTest());
    ATH_CHECK( m_jFEXPileupAndNoiseTool->reset());        
    if(m_jfexid == 0 || m_jfexid == 5){
        m_jFEXPileupAndNoiseTool->setup(m_jTowersIDs_Wide);
    }
    else{
        m_jFEXPileupAndNoiseTool->setup(m_jTowersIDs_Thin);
        
                
    }
    //Calculating and sustracting pileup
    std::vector<int> pileup_rho;
    pileup_rho = m_jFEXPileupAndNoiseTool->CalculatePileup();
    jFEXOutputs->addValue_pileup("pileup_FPGAid"  , m_id);
    jFEXOutputs->addValue_pileup("pileup_jFEXid"  , m_jfexid);
    jFEXOutputs->addValue_pileup("pileup_rho_EM"  , pileup_rho[0]);
    jFEXOutputs->addValue_pileup("pileup_rho_HAD1", pileup_rho[1]);
    jFEXOutputs->addValue_pileup("pileup_rho_HAD2", pileup_rho[2]);
    jFEXOutputs->addValue_pileup("pileup_rho_HAD3", pileup_rho[3]);
    jFEXOutputs->addValue_pileup("pileup_rho_FCAL", pileup_rho[4]);
    jFEXOutputs->fill_pileup();
    
    
    //Applying pileup sustraction in jet or met independently - this sets the flags to true in m_jFEXPileupAndNoiseTool
    // now masked -> no pileup applied.
    //m_jFEXPileupAndNoiseTool->ApplyPileupJets();     
    //m_jFEXPileupAndNoiseTool->ApplyPileupMet();
    
    //Getting the values 
    m_map_HAD_Etvalues_FPGA = m_jFEXPileupAndNoiseTool->Get_HAD_Et_values();
    m_map_EM_Etvalues_FPGA = m_jFEXPileupAndNoiseTool->Get_EM_Et_values();
    m_map_Etvalues_FPGA = m_jFEXPileupAndNoiseTool->GetEt_values();
    


    if(m_id==0 || m_id==2){
        //ATH_CHECK( m_jFEXsumETAlgoTool.retrieve());
        ATH_CHECK( m_jFEXsumETAlgoTool->safetyTest());
        ATH_CHECK( m_jFEXsumETAlgoTool->reset());  
        //ATH_CHECK( m_jFEXmetAlgoTool.retrieve());
        ATH_CHECK( m_jFEXmetAlgoTool->safetyTest());
        ATH_CHECK( m_jFEXmetAlgoTool->reset());  
        m_jFEXsumETAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        m_jFEXmetAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
        
        if(m_jfexid > 0 && m_jfexid < 5){
        
            //-----------------jFEXsumETAlgo-----------------
            m_jFEXsumETAlgoTool->setup(m_jTowersIDs_Thin);
            m_jFEXsumETAlgoTool->buildBarrelSumET();
            //The number 3 below is arbitrary, NEEDS TO BE CHANGED with the numbers from the Trigger Menu (future MR)
            m_sumET_tobwords.push_back(formSumETTOB(m_jFEXsumETAlgoTool->getETlowerEta(3),m_jFEXsumETAlgoTool->getETupperEta(3)));
            
            //-----------------jFEXmetAlgo-----------------            
            m_jFEXmetAlgoTool->setup(m_jTowersIDs_Thin);     
            m_jFEXmetAlgoTool->buildBarrelmet();
            m_Met_tobwords.push_back(formMetTOB(m_jFEXmetAlgoTool->GetMetXComponent(),m_jFEXmetAlgoTool->GetMetYComponent()));
        }
        else if(m_jfexid == 0 ){
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
            //The number 3 below is arbitrary, NEEDS TO BE CHANGED with the numbers from the Trigger Menu (future MR)
            m_sumET_tobwords.push_back(formSumETTOB(m_jFEXsumETAlgoTool->getETlowerEta(3),m_jFEXsumETAlgoTool->getETupperEta(3)));
            
            //-----------------jFEXmetAlgo-----------------    
            m_jFEXmetAlgoTool->setup(flipped_jTowersIDs);
            m_jFEXmetAlgoTool->buildFWDmet();
            m_Met_tobwords.push_back(formMetTOB(m_jFEXmetAlgoTool->GetMetXComponent(),m_jFEXmetAlgoTool->GetMetYComponent()));
        }
        else if(m_jfexid == 5){
            //-----------------jFEXsumETAlgo-----------------
            m_jFEXsumETAlgoTool->setup(m_jTowersIDs_Wide);
            m_jFEXsumETAlgoTool->buildFWDSumET();
            //The number 3 below is arbitrary, NEEDS TO BE CHANGED with the numbers from the Trigger Menu (future MR)
            m_sumET_tobwords.push_back(formSumETTOB(m_jFEXsumETAlgoTool->getETlowerEta(3),m_jFEXsumETAlgoTool->getETupperEta(3)));
              
            //-----------------jFEXmetAlgo-----------------    
            m_jFEXmetAlgoTool->setup(m_jTowersIDs_Wide);
            m_jFEXmetAlgoTool->buildFWDmet();
            m_Met_tobwords.push_back(formMetTOB(m_jFEXmetAlgoTool->GetMetXComponent(),m_jFEXmetAlgoTool->GetMetYComponent()));
        }
    }
  

  //-----------jFEXSmallRJet & Large R Jet Algo-----------------
  ATH_MSG_DEBUG("==== jFEXSmallRJetAlgo ========");



 //Central region algorithms
 if(m_jfexid > 0 && m_jfexid < 5){ 
  for(int mphi = 8; mphi < FEXAlgoSpaceDefs::jFEX_algoSpace_height -8; mphi++) {   
    for(int meta = 8; meta < FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width -8; meta++) {

  
    //create search window including towerIDs required for seeding.
    int SRJet_SearchWindow[7][7] = {{0}};
    for(int i = -3 ; i <4; i++){
      for(int j = -3; j <4; j++){
        SRJet_SearchWindow[3 + i][3 + j] = m_jTowersIDs_Thin[mphi + i][meta +j];
      }
    }
      int largeRCluster_IDs[15][15]= {{0}};
      for(int i = -7; i< 8; i++ ) {
        for(int j = -7; j< 8; j++) {
          largeRCluster_IDs[7 +i][7 +j] = m_jTowersIDs_Thin[mphi + i][meta +j];
        }
      }

      //remove corners from large ET ring
      for(int i =4; i <8; i++) {
        if( i != 7) {
          largeRCluster_IDs[7 +i][14] = 0;
          largeRCluster_IDs[7 +i][0] = 0;
          largeRCluster_IDs[7 -i][14] = 0;
          largeRCluster_IDs[7 -i][0] = 0;
        }

        largeRCluster_IDs[14][7 +i] = 0;
        largeRCluster_IDs[14][7 -i] = 0;
        largeRCluster_IDs[0][7 +i] = 0;
        largeRCluster_IDs[0][7 -i] = 0;
        largeRCluster_IDs[13][13] = 0;
        largeRCluster_IDs[1][1] = 0;
        largeRCluster_IDs[13][1] = 0;
        largeRCluster_IDs[1][13] = 0;
      }

      //this prevents adding ET from small RT ring
      for(int i = -3; i< 4; i++) {
        for(int j = -3; j<4 ; j++) {
          if(!(i == 3 && j == -3) || !(i == -3 && j == 3) || !(i == 3 && j == 3) || !(i == -3 && j == -3)) {
            largeRCluster_IDs[7 +i][7 +j] = 0;
          }
        }
      }
      m_jFEXSmallRJetAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
      m_jFEXLargeRJetAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
      m_jFEXSmallRJetAlgoTool->setup(SRJet_SearchWindow);
      m_jFEXLargeRJetAlgoTool->setupCluster(largeRCluster_IDs);
      m_jFEXSmallRJetAlgoTool->buildSeeds();
      bool SRJet_LM = m_jFEXSmallRJetAlgoTool->isSeedLocalMaxima(); 

      jFEXOutputs->addValue_smallRJet("smallRJet_isCentralTowerSeed", SRJet_LM);

      int smallClusterET = m_jFEXSmallRJetAlgoTool->getSmallClusterET();
      //These are plots of the central TT for each 5x5 search window.
      jFEXOutputs->addValue_smallRJet("smallRJet_ET", m_jFEXSmallRJetAlgoTool->getTTowerET(m_jTowersIDs_Thin[mphi][meta]));
      jFEXOutputs->addValue_smallRJet("smallRJet_phi",m_jFEXSmallRJetAlgoTool->getRealPhi(m_jTowersIDs_Thin[mphi][meta]));
      jFEXOutputs->addValue_smallRJet("smallRJet_eta",m_jFEXSmallRJetAlgoTool->getRealEta(m_jTowersIDs_Thin[mphi][meta]));

      jFEXOutputs->addValue_smallRJet("smallRJet_clusterET", smallClusterET);
  
      if(!SRJet_LM){continue;}
      std::unique_ptr<jFEXSmallRJetTOB> tmp_SRJet_tob = m_jFEXSmallRJetAlgoTool->getSmallRJetTOBs();

      bool check = m_jFEXSmallRJetAlgoTool->checkDisplacedLM();
      //to check if Local Maxima is displaced.        
      int meta_LM = meta;
      int mphi_LM = mphi;
      if(check  && SRJet_LM){
          meta_LM = meta -1;
          mphi_LM = mphi -1;
      
      } 

      bool SR_TOB_saturated = false;
      if (smallClusterET/200. > 0x7ff) SR_TOB_saturated = true;

      // for plotting variables in TOBS- internal check:
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_eta", tmp_SRJet_tob->setEta(meta_LM)-8);
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_phi", tmp_SRJet_tob->setPhi(mphi_LM)-8);
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_ET", tmp_SRJet_tob->setET(smallClusterET/200));    
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_sat", tmp_SRJet_tob->setSat(SR_TOB_saturated));  
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_jfexID", m_jfexid);
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_fpgaID", m_id);

      uint32_t SRJet_tobword = formSmallRJetTOB(mphi_LM, meta_LM);
      m_SRJet_tobwords.push_back(SRJet_tobword);
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_word",SRJet_tobword); 
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_jfexID", m_jfexid);
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_fpgaID", m_id);	
     
      jFEXOutputs->fill_smallRJet();  

      ATH_MSG_DEBUG("==== jFEXLargeRJetAlgo ========"); 
      //LargeRJetAlgo is here as SmallRJetlocalMaxima is a requirement
      unsigned int largeClusterET = m_jFEXLargeRJetAlgoTool->getLargeClusterET(m_jFEXSmallRJetAlgoTool->getSmallClusterET(),m_jFEXLargeRJetAlgoTool->getRingET());
      std::unique_ptr<jFEXLargeRJetTOB> tmp_LRJet_tob = m_jFEXLargeRJetAlgoTool->getLargeRJetTOBs();
      unsigned int LR_TOB_saturated = 0;
      if (largeClusterET/200. >  0x1fff) LR_TOB_saturated = 1;
 
      jFEXOutputs->addValue_largeRJet("largeRJet_ET", largeClusterET);
      jFEXOutputs->addValue_largeRJet("largeRJet_phi", m_jFEXSmallRJetAlgoTool->getRealPhi(m_jTowersIDs_Thin[mphi][meta]));
      jFEXOutputs->addValue_largeRJet("largeRJet_eta", m_jFEXSmallRJetAlgoTool->getRealEta(m_jTowersIDs_Thin[mphi][meta]));

      jFEXOutputs->addValue_largeRJet("largeRJetTOB_ET",tmp_LRJet_tob->setET(largeClusterET/200)); 
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_eta",tmp_SRJet_tob->setEta(meta));
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_phi",tmp_SRJet_tob->setPhi(mphi));
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_sat",tmp_LRJet_tob->setSat(LR_TOB_saturated));
      jFEXOutputs->fill_largeRJet(); 
      uint32_t LRJet_tobword = formLargeRJetTOB(mphi, meta);
      if ( LRJet_tobword != 0 ) m_LRJet_tobwords.push_back(LRJet_tobword);
      m_SRJet_tobwords.push_back(SRJet_tobword);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_word", LRJet_tobword);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_jfexID", m_jfexid);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_fpgaID", m_id);
      }
  }

} //end of if statement for checking if in central jfex modules

jFEXOutputs->fill_smallRJet();
jFEXOutputs->fill_largeRJet();

//**********Forward Jets***********************

//FCAL region algorithm 
if((m_jfexid ==0 || m_jfexid ==5) and 0 ){ //This is currently masked due to on going debugging in this FCAL Jets Algorithm


  m_jFEXForwardJetsAlgoTool->setup(m_jTowersIDs_Wide);
  m_FCALJets =  m_jFEXForwardJetsAlgoTool->calculateJetETs();

  for(std::map<int, jFEXForwardJetsInfo>::iterator it = m_FCALJets.begin(); it!=(m_FCALJets.end()); ++it){
   
    jFEXForwardJetsInfo FCALJets = it->second;
    int iphi = FCALJets.getCentreLocalTTPhi();
    int ieta = FCALJets.getCentreLocalTTEta();
     
    uint32_t SRFCAL_Jet_tobword = formSmallRJetTOB(iphi, ieta);
    if ( SRFCAL_Jet_tobword != 0 ) m_SRJet_tobwords.push_back(SRFCAL_Jet_tobword);

    uint32_t LRFCAL_Jet_tobword = formLargeRJetTOB(iphi, ieta);
    if ( LRFCAL_Jet_tobword != 0 ) m_LRJet_tobwords.push_back(LRFCAL_Jet_tobword);
  
    float centre_eta = std::round(FCALJets.getCentreTTEta()*10);
    float centre_phi = std::round(FCALJets.getCentreTTPhi()*10);
    int output_centre_eta = static_cast<int> (centre_eta);
    int output_centre_phi = static_cast<int> (centre_phi);
    int SRJetET = FCALJets.getSeedET() + FCALJets.getFirstEnergyRingET();
    int LRJetET = SRJetET + FCALJets.getSecondEnergyRingET();

    int SRFCAL_TOB_saturated = 0;
    if (SRJetET/200. > 0x7ff) SRFCAL_TOB_saturated = 1;

    unsigned int LRFCAL_TOB_saturated = 0;
    if (LRJetET/200. >  0x1fff) LRFCAL_TOB_saturated = 1;
   
    //Output Collection for Small R Jet 
    jFEXOutputs->addValue_smallRJet("smallRJet_phi", output_centre_phi);
    jFEXOutputs->addValue_smallRJet("smallRJet_eta", output_centre_eta);
    jFEXOutputs->addValue_smallRJet("smallRJet_clusterET", SRJetET);
    jFEXOutputs->addValue_smallRJet("smallRJet_sat", SRFCAL_TOB_saturated);
    jFEXOutputs->addValue_smallRJet("smallRJet_isCentralTowerSeed",true);

    if(m_jfexid == 0){
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_eta", 37-ieta);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_eta", 37-ieta);
    }

    if(m_jfexid == 5){     
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_eta", ieta-8);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_eta", ieta-8);
    }
 
    if(iphi >=FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_start_phi && iphi< FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_end_phi){
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_phi", iphi-8);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_phi", iphi-8);
    }

    if(iphi >=FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_start_phi && iphi< FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_end_phi){
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_phi", iphi-4);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_phi", iphi-4);
    }

    if(iphi >=FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_start_phi && iphi< FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_end_phi){
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_phi", iphi-2);
      jFEXOutputs->addValue_largeRJet("largeRJetTOB_phi", iphi-2);
    }

    jFEXOutputs->addValue_smallRJet("smallRJetTOB_ET", SRJetET/200);
    jFEXOutputs->addValue_smallRJet("smallRJetTOB_sat", SRFCAL_TOB_saturated);
 
    jFEXOutputs->addValue_smallRJet("smallRJetTOB_word", SRFCAL_Jet_tobword);
    jFEXOutputs->addValue_smallRJet("smallRJetTOB_jfexID", m_jfexid);
    jFEXOutputs->addValue_smallRJet("smallRJetTOB_fpgaID", m_id);

    jFEXOutputs->fill_smallRJet();

    //Output Collection for Large R Jet
    jFEXOutputs->addValue_largeRJet("largeRJet_ET", LRJetET);
    jFEXOutputs->addValue_largeRJet("largeRJet_phi", output_centre_phi);
    jFEXOutputs->addValue_largeRJet("largeRJet_eta", output_centre_eta);
    jFEXOutputs->addValue_largeRJet("largeRJet_sat", LRFCAL_TOB_saturated);

    jFEXOutputs->addValue_largeRJet("largeRJetTOB_ET", LRJetET/200);
    jFEXOutputs->addValue_largeRJet("largeRJetTOB_sat",LRFCAL_TOB_saturated);

    jFEXOutputs->addValue_largeRJet("largeRJetTOB_word", LRFCAL_Jet_tobword);
    jFEXOutputs->addValue_largeRJet("largeRJetTOB_jfexID", m_jfexid);
    jFEXOutputs->addValue_largeRJet("largeRJetTOB_fpgaID", m_id);

    jFEXOutputs->fill_largeRJet();

  }
}


//******************************** TAU **********************************************


  memset(m_jTowersIDs, 0, sizeof(m_jTowersIDs[0][0]) * FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width); // Reseting m_jTowersIDs array with 0

  int max_meta=16;
  if(m_jfexid ==0){
    
    for(int i=0;i<FEXAlgoSpaceDefs::jFEX_algoSpace_height;i++){
      for(int j=28;j<(FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width-6);j++){ //lower values of j (j<28) are the Fcals not entering in the jFEX tau range
        m_jTowersIDs[i][j-28+8]=m_jTowersIDs_Wide[i][j]; // second argument in m_jTowersIDs is to center the FPGA core area in te same region as the central FPGAs 
      }
    }
    max_meta++; // increase max of eta because te core module has one more TT to be considered
  } 
  else if(m_jfexid ==5 ){ 

    // Filling m_jTowersIDs with the m_jTowersIDs_Wide ID values up to 2.5 eta 
    for(int i=0;i<FEXAlgoSpaceDefs::jFEX_algoSpace_height;i++){
      for(int j=4;j<17;j++){//higher values of j (j>16) are the Fcals not entering in the jFEX tau range
        m_jTowersIDs[i][j]=m_jTowersIDs_Wide[i][j];
      }
    }
    max_meta++; // increase max of eta because te core module has one more TT to be considered
  }
  else{
    //For Module 1,2,3,4 (central modules) the m_jTowersIDs array is m_jTowersIDs_Thin
    std::copy(&m_jTowersIDs_Thin[0][0], &m_jTowersIDs_Thin[0][0] + FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width, &m_jTowersIDs[0][0]);
  }
  ATH_MSG_DEBUG("============================ jFEXtauAlgo ============================");
  for(int mphi = 8; mphi < 24; mphi++) {   
    for(int meta = 8; meta < max_meta; meta++) {
    
      int TT_seed_ID[3][3]={{0}};
      int TT_searchWindow_ID[5][5]={{0}};
      int TT_First_ETring[36]={0};
      int First_ETring_it = 0;
      
      for(int i = -3; i< 4; i++ ) {
          for(int j = -3; j< 4; j++) {
              if(sqrt(pow(i,2)+pow(j,2))<3){
                TT_searchWindow_ID[i+2][j+2] = m_jTowersIDs[mphi +i][meta +j]; // Search window for the tau algo used for the LocalMaxima studies
              }
              
              if(sqrt(pow(i,2)+pow(j,2))<2){
                  TT_seed_ID[i+1][j+1] = m_jTowersIDs[mphi +i][meta +j]; // Seed 0.3x0.3 in phi-eta plane
              }
              else if(sqrt(pow(i,2)+pow(j,2))<4){
                TT_First_ETring[First_ETring_it]= m_jTowersIDs[mphi +i][meta +j]; // First energy ring, will be used as tau ISO
                ++First_ETring_it;
                
              } 
          }
      }
      
      ATH_CHECK( m_jFEXtauAlgoTool.retrieve());
      ATH_CHECK( m_jFEXtauAlgoTool->safetyTest());
      m_jFEXtauAlgoTool->setFPGAEnergy(m_map_Etvalues_FPGA);
      m_jFEXtauAlgoTool->setup(TT_searchWindow_ID,TT_seed_ID);
      m_jFEXtauAlgoTool->buildSeeds();
      bool is_tau_LocalMax = m_jFEXtauAlgoTool->isSeedLocalMaxima();
      m_jFEXtauAlgoTool->setFirstEtRing(TT_First_ETring);

      jFEXOutputs->addValue_tau("tau_ET", m_jFEXtauAlgoTool->getTTowerET(m_jTowersIDs[mphi][meta]));
      jFEXOutputs->addValue_tau("tau_clusterET", m_jFEXtauAlgoTool->getClusterEt());
      jFEXOutputs->addValue_tau("tau_eta",std::abs(m_jFEXtauAlgoTool->getRealEta(m_jTowersIDs[mphi][meta]))) ;
      jFEXOutputs->addValue_tau("tau_phi",m_jFEXtauAlgoTool->getRealPhi(m_jTowersIDs[mphi][meta])) ;
      jFEXOutputs->addValue_tau("tau_realeta",m_jFEXtauAlgoTool->getRealEta(m_jTowersIDs[mphi][meta])) ;
      jFEXOutputs->addValue_tau("tau_ISO",m_jFEXtauAlgoTool->getFirstEtRing()) ;
      jFEXOutputs->addValue_tau("tau_TT_ID",TT_seed_ID[1][1]) ;
      jFEXOutputs->addValue_tau("tau_isLocalMax",is_tau_LocalMax) ;
      jFEXOutputs->addValue_tau("tau_jFEXid",m_jfexid) ;
      jFEXOutputs->addValue_tau("tau_FPGAid",m_id) ;
    
      uint32_t tobword = formTauTOB(mphi, meta);
      if ( is_tau_LocalMax ){
          m_tau_tobwords.push_back(tobword);
      }
    
      std::unique_ptr<jFEXtauTOB> tmp_tob = m_jFEXtauAlgoTool->getTauTOBs(mphi, meta);
      // for plotting variables in TOBS- internal check:
      jFEXOutputs->addValue_tau("tau_TOB_word" ,tobword);
      jFEXOutputs->addValue_tau("tau_TOB_ET" ,tmp_tob->GetET());
      jFEXOutputs->addValue_tau("tau_TOB_eta",tmp_tob->GetEta());
      jFEXOutputs->addValue_tau("tau_TOB_phi",tmp_tob->GetPhi());
      jFEXOutputs->addValue_tau("tau_TOB_ISO" ,tmp_tob->GetIso());
      jFEXOutputs->addValue_tau("tau_TOB_Sat" ,tmp_tob->GetSat());        
        
      jFEXOutputs->fill_tau();
         
    }
  }


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

void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]){

  const int rows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
  const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
    
  std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Thin[0][0]);
  
  //this prints out the jTower IDs that each FPGA is responsible for
  ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") [on jFEX " << m_jfexid << "] IS RESPONSIBLE FOR jTOWERS :");
   
  for (int thisRow=rows-1; thisRow>=0; thisRow--){
    for (int thisCol=0; thisCol<cols; thisCol++){
      if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  "); }
      else { ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  |"); }
    }
  }

}

std::vector <uint32_t> jFEXFPGA::getSmallRJetTOBs()
{
  auto tobsSort = m_SRJet_tobwords;
  
  ATH_MSG_DEBUG("number of smallRJet tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
  // sort tobs by their et ( 11 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etSRJetSort);
  tobsSort.resize(7);

  return tobsSort;  

}

std::vector <uint32_t> jFEXFPGA::getLargeRJetTOBs()
{
  auto tobsSort = m_LRJet_tobwords;

  // ATH_MSG_DEBUG("number of largeRJet tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
  // sort tobs by their et ( 13 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etLRJetSort);
  tobsSort.resize(1);

  return tobsSort;
  
}

uint32_t jFEXFPGA::formSmallRJetTOB(int &iphi, int &ieta)  {
    uint32_t tobWord = 0;
    const unsigned int jFEXETResolution = 200; //LSB is 200MeV
    int eta = 0;
    int phi = 0;
    unsigned int et = 0;
    unsigned int jFEXSmallRJetTOBEt = 0;
    int Res = 0; // 11 bits reserved
    int Sat = 0; //  1 bit for saturation. Set to 1 when jet energy is saturated

    if(m_jfexid > 0 && m_jfexid < 5) {
        et = m_jFEXSmallRJetAlgoTool->getSmallClusterET();
        jFEXSmallRJetTOBEt = et/jFEXETResolution;
        eta = ieta -8;
        phi = iphi -8;
    }

    if(m_jfexid ==0 || m_jfexid ==5) {
        for(std::map<int, jFEXForwardJetsInfo>::iterator it = m_FCALJets.begin(); it!=(m_FCALJets.end()); ++it) {
            jFEXForwardJetsInfo FCALJetInfo = it->second;
            eta = ieta -8;
            phi = iphi -8;
            et = FCALJetInfo.getSeedET() + FCALJetInfo.getFirstEnergyRingET();
            jFEXSmallRJetTOBEt = et/jFEXETResolution;
        }
    }

    if (jFEXSmallRJetTOBEt > 0x7ff) {
        jFEXSmallRJetTOBEt = 0x7ff;
        Sat = 1;
    }

    //create basic tobword with 32 bits
    tobWord = tobWord + (eta<<27) + (phi << 23) + (jFEXSmallRJetTOBEt << 12) + (Res<<1) + (Sat);
    ATH_MSG_DEBUG("tobword smallRJet with et, phi, eta, res and sat : " << std::bitset<32>(tobWord) );
    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    auto & thr_jJ = l1Menu->thrExtraInfo().jJ();
    unsigned int minEtThreshold = thr_jJ.ptMinToTopoSmallMeV()/jFEXETResolution;
    
    if (jFEXSmallRJetTOBEt < minEtThreshold) return 0;
    else return tobWord;
}

uint32_t jFEXFPGA::formLargeRJetTOB(int & iphi, int &ieta) {
    
    uint32_t tobWord = 0;
    const unsigned int jFEXETResolution = 200; //LSB is 200MeV
    int eta = 0;
    int phi = 0;
    unsigned int et = 0;
    unsigned int jFEXLargeRJetTOBEt = 0;
    int Res = 0; // 9 bits reserved
    int Sat = 0; //  1 bit for saturation. Set to 1 when jet energy is saturated

    if(m_jfexid > 0 && m_jfexid < 5) {
        et = m_jFEXLargeRJetAlgoTool->getLargeClusterET(m_jFEXSmallRJetAlgoTool->getSmallClusterET(),m_jFEXLargeRJetAlgoTool->getRingET());
        jFEXLargeRJetTOBEt = et/jFEXETResolution;
        eta = ieta -8;
        phi = iphi -8;
    }

    if(m_jfexid ==0 || m_jfexid ==5) {
        for(std::map<int, jFEXForwardJetsInfo>::iterator it = m_FCALJets.begin(); it!=(m_FCALJets.end()); ++it) {
            jFEXForwardJetsInfo FCALJetInfo = it->second;
            eta = ieta -8;
            phi = iphi -8;
            et = FCALJetInfo.getSeedET() + FCALJetInfo.getFirstEnergyRingET() + FCALJetInfo.getSecondEnergyRingET();
            jFEXLargeRJetTOBEt = et/jFEXETResolution;
        }
    }

    if (jFEXLargeRJetTOBEt > 0x1fff) {
        jFEXLargeRJetTOBEt = 0x1fff;  //0x1fff is 13 bits
        Sat = 1;
    }
    //create basic tobword with 32 bits
    tobWord = tobWord + (eta<<27) + (phi << 23) + (jFEXLargeRJetTOBEt << 10) + (Res<<1) + (Sat);
    ATH_MSG_DEBUG("tobword largeRJet with et, phi, eta, sub and sat : " << std::bitset<32>(tobWord) );

    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    auto & thr_jJ = l1Menu->thrExtraInfo().jJ();
    unsigned int minEtThreshold = thr_jJ.ptMinToTopoLargeMeV()/jFEXETResolution;
    if (jFEXLargeRJetTOBEt < minEtThreshold) return 0;
    else return tobWord;
}


uint32_t jFEXFPGA::formTauTOB(int & iphi, int &ieta )
{
    uint32_t tobWord = 0;
    const unsigned int jFEXETResolution = 200; //LSB is 200MeV

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
    tobWord = tobWord + (eta << 27) + (phi << 23) + (et << 12) + (iso << 1) + sat ;

    ATH_MSG_DEBUG("tobword tau with eta, phi, et, iso and sat : " << std::bitset<32>(tobWord) );

    //arbitary et threshold to not overflow the TOBs


    /*****************  NEEDS TO BE IMPLEMENTED IN THE FUTURE (JIRA TICKED OPENED) *****************/ 
    // Retrieve the L1 menu configuration
    //SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
    //auto & thr_jTAU = l1Menu->thrExtraInfo().jTAU();
    //unsigned int minEtThreshold = thr_jTAU.ptMinToTopo()/jFEXETResolution;
    
    //arbitary et threshold to not overflow the TOBs
    unsigned int minEtThreshold = 100;
    
    if (et < minEtThreshold) return 0;
    else return tobWord;

}


std::vector <uint32_t> jFEXFPGA::getTauTOBs()
{
  auto tobsSort = m_tau_tobwords;

  ATH_MSG_DEBUG("number of tau tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
  // sort tobs by their et ( 13 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etTauSort);

  tobsSort.resize(6);
  return tobsSort;

}

std::vector <uint32_t> jFEXFPGA::getTauxTOBs()
{

  return m_tau_tobwords;

}

uint32_t jFEXFPGA::formSumETTOB(int ETlow, int EThigh )
{
  uint32_t tobWord = 0;
  const unsigned int jFEXETResolution = 200; //LSB is 200MeV

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
  tobWord = tobWord + (etlow << 17) + (satlow << 16) + (ethigh << 1) + sathigh ;
  ATH_MSG_DEBUG("tobword SumET with ETlow, Satlow, EThigh and Sathigh : " << std::bitset<32>(tobWord) );

  return tobWord;

}

std::vector <uint32_t> jFEXFPGA::getSumEtTOBs()
{
  auto tobsSort = m_sumET_tobwords;

  ATH_MSG_DEBUG("number of SumEt tobs: " << tobsSort.size() << " in FPGA: " << m_id);
  return tobsSort;

}

uint32_t jFEXFPGA::formMetTOB(int METX, int METY )
{
  uint32_t tobWord = 0;
  const float_t jFEXETResolution = 200.0; //LSB is 200MeV

  int sat = 0;
  int res = 0; 

  int metX = METX/jFEXETResolution;
  
  //0x7fff is 15 bits (decimal value 32767), however as MET is a signed value (can be negative) only 14 bits are allowed (16383) the MSB is the sign
  if (std::abs(metX) > 0x3fff) { 
    ATH_MSG_DEBUG("sumEtlow saturated: " << metX );
    metX = 0x7fff;
    sat=1;
  }

  int metY = METY/jFEXETResolution;
  if (std::abs(metY) > 0x3fff) { //0x7fff is 15 bits (decimal value 32767), however as MET is a signed value (can be negative) only 14 bits are allowed (16383)
    ATH_MSG_DEBUG("sumEthigh saturated: " << metY );
    metY = 0x7fff;
    sat=1;
  }
  
  //create basic tobword with 32 bits
  tobWord = tobWord + ((metX & 0x7fff) << 17) + (sat << 16) + ((metY & 0x7fff) << 1) + res ;
  ATH_MSG_DEBUG("tobword MET with MET_X, Sat, MET_Y and Res : " << std::bitset<32>(tobWord) );

  return tobWord;

}

std::vector <uint32_t> jFEXFPGA::getMetTOBs()
{
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

    SG::ReadHandle<jTowerContainer> jFEXFPGA_jTowerContainer(m_jFEXFPGA_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jFEXFPGA_jTowerContainer->findTower(TTID);
    return tmpTower->getTotalET();
}





} // end of namespace bracket

