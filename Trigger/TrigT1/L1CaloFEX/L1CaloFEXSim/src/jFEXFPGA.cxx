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
#include "L1CaloFEXSim/jFEXtauAlgo.h" //the future is now
#include "L1CaloFEXSim/jFEXtauTOB.h"  //the future is now
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include <vector>
#include "TH1F.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "SGTools/TestStore.h"

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
  //ATH_CHECK(m_jFEXFPGA_jFEXOutputCollectionKey.initialize());
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

}

StatusCode jFEXFPGA::execute() {

  SG::ReadHandle<jTowerContainer> jk_jFEXFPGA_jTowerContainer(m_jFEXFPGA_jTowerContainerKey/*,ctx*/);
  if(!jk_jFEXFPGA_jTowerContainer.isValid()) {
    ATH_MSG_FATAL("Could not retrieve jk_jFEXFPGA_jTowerContainer " << m_jFEXFPGA_jTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }


  //-----------jFEXSmallRJetAlgo-----------------
  ATH_MSG_DEBUG("==== jFEXSmallRJetAlgo ========");
  jFEXOutputCollection* jFEXOutputs;
  StatusCode sc_tobs = evtStore()->retrieve(jFEXOutputs, "jFEXOutputCollection");

  if(sc_tobs == StatusCode::FAILURE) {
    ATH_MSG_DEBUG("\n==== jFEXSmallRJetAlgo ======== Failed to find jFEXOutputCollection in jFEXFPGA");
  }

  int Jets_eta_limit = -99;
  bool barrel_region = false;
 
  if(m_jfexid > 0 && m_jfexid < 5){
    Jets_eta_limit = FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width -8;

    barrel_region = true;
  //  return StatusCode::SUCCESS;
  }

  if(m_jfexid == 0 || m_jfexid == 5){

    Jets_eta_limit = FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width -8;
    //return StatusCode::SUCCESS;
  }

  
  for(int mphi = 8; mphi < 24; mphi++) {   
    for(int meta = 8; meta < Jets_eta_limit; meta++) {


    //create search window including towerIDs required for seeding.
    int SRJet_SearchWindow[7][7] = {{0}};
    for(int i = -3 ; i <4; i++){
      for(int j = -3; j <4; j++){
        if(barrel_region){SRJet_SearchWindow[3 + i][3 + j] = m_jTowersIDs_Thin[mphi + i][meta +j];}
        else{SRJet_SearchWindow[3 + i][3 + j] = m_jTowersIDs_Wide[mphi + i][meta +j];}
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


      m_jFEXSmallRJetAlgoTool->setup(SRJet_SearchWindow, barrel_region);
      m_jFEXLargeRJetAlgoTool->setupCluster(largeRCluster_IDs);


      //These are plots of the central TT for each 5x5 search window.
      jFEXOutputs->addValue_smallRJet("smallRJet_ET", m_jFEXSmallRJetAlgoTool->getTTowerET());
      jFEXOutputs->addValue_smallRJet("smallRJet_phi",m_jFEXSmallRJetAlgoTool->getRealPhi()) ;
      jFEXOutputs->addValue_smallRJet("smallRJet_eta",m_jFEXSmallRJetAlgoTool->getRealEta()) ;

      m_jFEXSmallRJetAlgoTool->buildSeeds();
      bool SRJet_LM = m_jFEXSmallRJetAlgoTool->isSeedLocalMaxima();
      jFEXOutputs->addValue_smallRJet("smallRJet_isCentralTowerSeed",  SRJet_LM);

      std::unique_ptr<jFEXSmallRJetTOB> tmp_tob = m_jFEXSmallRJetAlgoTool->getSmallRJetTOBs();

      uint32_t tobword = formSmallRJetTOB(mphi, meta);
      if ( tobword != 0 ) m_SRJet_tobwords.push_back(tobword);

      //bool TOB_saturated = false;
      int smallClusterET = m_jFEXSmallRJetAlgoTool->getSmallClusterET();
      //if (smallClusterET/200. > 0x7ff){ATH_MSG_DEBUG("SRJet TOB is saturated"); TOB_saturated = true;}

      // for plotting variables in TOBS- internal check:
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_eta",tmp_tob->setEta(meta));
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_phi",tmp_tob->setPhi(mphi));
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_ET",smallClusterET);
      jFEXOutputs->fill_smallRJet();

      if(!SRJet_LM) {
        continue;
      }

      if(SRJet_LM){
        //LargeRJetAlgo is here as SmallRJetlocalMaxima is a requirement
        unsigned int largeClusterET = m_jFEXLargeRJetAlgoTool->getLargeClusterET(m_jFEXSmallRJetAlgoTool->getSmallClusterET(),m_jFEXLargeRJetAlgoTool->getRingET());
        jFEXOutputs->addValue_largeRJet("largeRJet_ET", largeClusterET);

        std::unique_ptr<jFEXLargeRJetTOB> tmp_tob = m_jFEXLargeRJetAlgoTool->getLargeRJetTOBs();

        jFEXOutputs->addValue_largeRJet("largeRJetTOB_ET", largeClusterET);
        jFEXOutputs->addValue_largeRJet("largeRJetTOB_phi", tmp_tob->setPhi(meta));
        jFEXOutputs->addValue_largeRJet("largeRJetTOB_eta", tmp_tob->setEta(mphi));

        uint32_t tobword = formLargeRJetTOB(mphi, meta);
        if ( tobword != 0 ) m_LRJet_tobwords.push_back(tobword);


        jFEXOutputs->fill_largeRJet();
      }

    }
  }


//******************************** TAU **********************************************


  memset(m_jTowersIDs, 0, sizeof(m_jTowersIDs[0][0]) * FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width); // Reseting m_jTowersIDs array with 0

  int max_meta=16;
  if(m_jfexid ==0){
    for(int i=0;i<FEXAlgoSpaceDefs::jFEX_algoSpace_height;i++){
      for(int j=0;j<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;j++){
        if(j==17){
          break;
        }
        m_jTowersIDs[i][j]=m_jTowersIDs_Wide[i][j];
      }
    }
    
    return StatusCode::SUCCESS;
    
  } 
  else if(m_jfexid ==5 ){ 

    // Filling m_jTowersIDs with the m_jTowersIDs_Wide ID values up to 2.5 eta 
    for(int i=0;i<FEXAlgoSpaceDefs::jFEX_algoSpace_height;i++){
      for(int j=0;j<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;j++){
        if(j==17){
          break;
        }
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

      

      ATH_CHECK( m_jFEXSmallRJetAlgoTool.retrieve());
      ATH_CHECK( m_jFEXSmallRJetAlgoTool->safetyTest());

      ATH_CHECK( m_jFEXtauAlgoTool.retrieve());
      ATH_CHECK( m_jFEXtauAlgoTool->safetyTest());
      m_jFEXtauAlgoTool->setup(TT_searchWindow_ID,TT_seed_ID);
      m_jFEXtauAlgoTool->buildSeeds();
      //bool is_tau_LocalMax = m_jFEXtauAlgoTool->isSeedLocalMaxima();
      m_jFEXtauAlgoTool->isSeedLocalMaxima();
      m_jFEXtauAlgoTool->setFirstEtRing(TT_First_ETring);


      jFEXOutputs->addValue_tau("tau_ET", m_jFEXtauAlgoTool->getTTowerET());
      jFEXOutputs->addValue_tau("tau_clusterET", m_jFEXtauAlgoTool->getClusterEt());
      jFEXOutputs->addValue_tau("tau_eta",abs(m_jFEXtauAlgoTool->getRealEta())) ;
      jFEXOutputs->addValue_tau("tau_phi",m_jFEXtauAlgoTool->getRealPhi()) ;
      jFEXOutputs->addValue_tau("tau_realeta",m_jFEXtauAlgoTool->getRealEta()) ;
      jFEXOutputs->addValue_tau("tau_ISO",m_jFEXtauAlgoTool->getFirstEtRing()) ;
      jFEXOutputs->addValue_tau("tau_TT_ID",TT_seed_ID[1][1]) ;
      jFEXOutputs->addValue_tau("tau_isLocalMax",m_jFEXtauAlgoTool->getIsLocalMaxima()) ;
      jFEXOutputs->addValue_tau("tau_jFEXid",m_jfexid) ;
      jFEXOutputs->addValue_tau("tau_FPGAid",m_id) ;
    
      uint32_t tobword = formTauTOB(mphi, meta);
      if ( tobword != 0 ){
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
  
  //comment below due to mapping bug 12.01.21 
  
  
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
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);
  tobsSort.resize(7);
  return tobsSort;  

}

std::vector <uint32_t> jFEXFPGA::getLargeRJetTOBs()
{
  auto tobsSort = m_LRJet_tobwords;

  ATH_MSG_DEBUG("number of largeRJet tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
  // sort tobs by their et ( 13 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);
  tobsSort.resize(1);
  return tobsSort;
  
}



uint32_t jFEXFPGA::formSmallRJetTOB(int & iphi, int &ieta )  
{
  uint32_t tobWord = 0;
  const unsigned int jFEXETResolution = 200; //LSB is 200MeV

  unsigned int et = m_jFEXSmallRJetAlgoTool->getSmallClusterET();   
  unsigned int jFEXSmallRJetTOBEt = et/jFEXETResolution;
  
  int eta = ieta;
  int phi = iphi;
  int Res = 0; //11 bits reserved
  int Sat = 0; //1 bit for saturation. Set to 1 if saturated.
  

  if (jFEXSmallRJetTOBEt > 0x7ff){ 
    jFEXSmallRJetTOBEt = 0x7ff;//0x7ff is 11 bits 
    Sat = 1;
  }

  //create basic tobword with 32 bits 
  tobWord = tobWord + jFEXSmallRJetTOBEt + (phi << 11) + (eta << 15) + (Res << 20) + (Sat << 31); 
  ATH_MSG_DEBUG("tobword smallRJet with et, phi, eta, res and sat : " << std::bitset<32>(tobWord) );
  
  //arbitary et threshold to not overflow the TOBs 
  unsigned int minEtThreshold = 30;
  if (et < minEtThreshold) return 0;
  else return tobWord;
}

uint32_t jFEXFPGA::formLargeRJetTOB(int & iphi, int &ieta )
{
  uint32_t tobWord = 0;
  const unsigned int jFEXETResolution = 200; //LSB is 200MeV

  unsigned int et = m_jFEXLargeRJetAlgoTool->getLargeClusterET(m_jFEXSmallRJetAlgoTool->getSmallClusterET(),m_jFEXLargeRJetAlgoTool->getRingET());
  unsigned int jFEXLargeRJetTOBEt = et/jFEXETResolution;



  int eta = ieta;
  int phi = iphi;
  int Sub = 0; //9 bits reserved
  int Sat = 1; //1 bit for saturation flag, not coded yet

  if (jFEXLargeRJetTOBEt > 0x1fff){
    jFEXLargeRJetTOBEt = 0x1fff;  //0x1fff is 13 bits
    Sat = 1;
  } 
  
  //create basic tobword with 32 bits
  tobWord = tobWord + jFEXLargeRJetTOBEt + (phi << 13) + (eta << 17) + (Sub << 22) + (Sat << 31);

  ATH_MSG_DEBUG("tobword largeRJet with et, phi, eta, sub and sat : " << std::bitset<32>(tobWord) );

  //arbitary et threshold to not overflow the TOBs
  unsigned int minEtThreshold = 30;
  if (et < minEtThreshold) return 0;
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

  unsigned int minEtThreshold = 30;
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








  
} // end of namespace bracket

