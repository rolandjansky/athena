/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "L1CaloFEXSim/jFEXOutputCollection.h" 
//#include "L1CaloFEXSim/jFEXtauAlgo.h" //for the future
//#include "L1CaloFEXSim/jFEXtauTOB.h" //for the future
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

void jFEXFPGA::reset(){

  m_id = -1;
  m_jfexid = -1;

}

StatusCode jFEXFPGA::execute(){

  SG::ReadHandle<jTowerContainer> jk_jFEXFPGA_jTowerContainer(m_jFEXFPGA_jTowerContainerKey/*,ctx*/);
  if(!jk_jFEXFPGA_jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_jFEXFPGA_jTowerContainer " << m_jFEXFPGA_jTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }


//-----------jFEXSmallRJetAlgo----------------- 
  ATH_MSG_DEBUG("==== jFEXSmallRJetAlgo ========");
  jFEXOutputCollection* jFEXOutputs;
  StatusCode sc_tobs = evtStore()->retrieve(jFEXOutputs, "jFEXOutputCollection");

  if(sc_tobs == StatusCode::FAILURE){ATH_MSG_DEBUG("\n==== jFEXSmallRJetAlgo ======== Failed to find jFEXOutputCollection in jFEXFPGA");}
 
  for(int ieta = 1; ieta < 30; ieta++){
    for(int iphi = 1; iphi < 15; iphi++){

      //To note: Thin mapping needs to be fixed in order to see correct outputs.
      int tobtable[5][5]={
      {m_jTowersIDs_Thin[ieta +2][iphi -2], m_jTowersIDs_Thin[ieta +2][iphi -1], m_jTowersIDs_Thin[ieta][iphi], m_jTowersIDs_Thin[ieta +2][iphi +1], m_jTowersIDs_Thin[ieta +2][iphi +2]},
      {m_jTowersIDs_Thin[ieta +1][iphi -2], m_jTowersIDs_Thin[ieta +1][iphi -1], m_jTowersIDs_Thin[ieta][iphi], m_jTowersIDs_Thin[ieta +1][iphi +1], m_jTowersIDs_Thin[ieta +1][iphi +2]},
      {m_jTowersIDs_Thin[ieta][iphi -2], m_jTowersIDs_Thin[ieta][iphi -1], m_jTowersIDs_Thin[ieta][iphi], m_jTowersIDs_Thin[ieta][iphi +1], m_jTowersIDs_Thin[ieta][iphi +2]},
      {m_jTowersIDs_Thin[ieta -1][iphi -2], m_jTowersIDs_Thin[ieta -1][iphi -1], m_jTowersIDs_Thin[ieta][iphi], m_jTowersIDs_Thin[ieta -1][iphi +1], m_jTowersIDs_Thin[ieta -1][iphi +2]},
      {m_jTowersIDs_Thin[ieta -2][iphi -2], m_jTowersIDs_Thin[ieta -2][iphi -1], m_jTowersIDs_Thin[ieta][iphi], m_jTowersIDs_Thin[ieta -2][iphi +1], m_jTowersIDs_Thin[ieta -2][iphi +2]}
	};

      int smallRCluster_IDs[4][5];
      for(int i =-2; i< 2; i++){
        smallRCluster_IDs[i +2][0] = m_jTowersIDs_Thin[ieta +i][iphi +3];
        smallRCluster_IDs[i +2][1] = m_jTowersIDs_Thin[ieta +i][iphi -3];  
        smallRCluster_IDs[i +2][2] = m_jTowersIDs_Thin[ieta +3][iphi +i];
        smallRCluster_IDs[i +2][2] = m_jTowersIDs_Thin[ieta -3][iphi +i];
      }
      
      
      ATH_CHECK( m_jFEXSmallRJetAlgoTool.retrieve()); 
      ATH_CHECK( m_jFEXSmallRJetAlgoTool->safetyTest());
      m_jFEXSmallRJetAlgoTool->setup(tobtable); 
      m_jFEXSmallRJetAlgoTool->setupCluster(smallRCluster_IDs);

      //These are plots of the central TT for each 5x5 search window.
      jFEXOutputs->addValue_smallRJet("smallRJet_ET", m_jFEXSmallRJetAlgoTool->getTTowerET());
      jFEXOutputs->addValue_smallRJet("smallRJet_phi",m_jFEXSmallRJetAlgoTool->getRealPhi()/10.) ;
      jFEXOutputs->addValue_smallRJet("smallRJet_eta",m_jFEXSmallRJetAlgoTool->getRealEta()/10.) ;     

      m_jFEXSmallRJetAlgoTool->buildSeeds();
      m_jFEXSmallRJetAlgoTool->isSeedLocalMaxima();
      jFEXOutputs->addValue_smallRJet("smallRJet_isCentralTowerSeed",  m_jFEXSmallRJetAlgoTool->isSeedLocalMaxima()); 

      std::unique_ptr<jFEXSmallRJetTOB> tmp_tob = m_jFEXSmallRJetAlgoTool->getSmallRJetTOBs();
       
      // for plotting variables in TOBS- internal check:
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_eta",tmp_tob->setEta(ieta));
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_phi",tmp_tob->setPhi(iphi));
      jFEXOutputs->addValue_smallRJet("smallRJetTOB_ET",m_jFEXSmallRJetAlgoTool->getTTowerET());    
      //saturation and res to be completed in future.. 

      uint32_t tobword = formSmallRJetTOB(ieta, iphi);
      if ( tobword != 0 ) m_tobwords.push_back(tobword);

          
      jFEXOutputs->fill_smallRJet();  

      if(!m_jFEXSmallRJetAlgoTool->isSeedLocalMaxima()){continue;}  
    
    }   
  }

    return StatusCode::SUCCESS;
} //end of the execute function 

void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][17]){
    
  const int rows = 16*2;
  const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
  
  std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Wide[0][0]);
  
  //this prints out the jTower IDs that each FPGA is responsible for
  ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") [on jFEX " << m_jfexid << "] IS RESPONSIBLE FOR jTOWERS :");

  //comment below due to mapping bug 12.01.21 

/* for (int thisRow=rows-1; thisRow>=0; thisRow--){
    for (int thisCol=0; thisCol<cols; thisCol++){
      if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_jTowersIDs_Wide[thisRow][thisCol] << "  "); }
      else { ATH_MSG_DEBUG("|  " << m_jTowersIDs_Wide[thisRow][thisCol] << "  |"); }
    }
  } */
  
}

void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][24]){

    const int rows = 16*2;
    const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
    
    std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Thin[0][0]);

    //this prints out the jTower IDs that each FPGA is responsible for
    ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") [on jFEX " << m_jfexid << "] IS RESPONSIBLE FOR jTOWERS :");
    
    //comment below due to mapping bug 12.01.21 

 /* for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  "); }
	else { ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  |"); }
      }
    }*/

  }

std::vector <uint32_t> jFEXFPGA::getjTOBs()
{
  auto tobsSort = m_tobwords;
  
  ATH_MSG_DEBUG("number of tobs: " << tobsSort.size() << " in FPGA: " << m_id<< " before truncation");
    // sort tobs by their et ( 11 bits of the 32 bit tob word)
     std::sort (tobsSort.begin(), tobsSort.end(), etSort);
     tobsSort.resize(7);
     return tobsSort;  

}


uint32_t jFEXFPGA::formSmallRJetTOB(int & ieta, int & iphi)  
{
  uint32_t tobWord = 0;
  const unsigned int jFEXETResolution = 200; //LSB is 200MeV

  unsigned int et = m_jFEXSmallRJetAlgoTool->getClusterET();   
  unsigned int jFEXSmallRJetTOBEt = et/jFEXETResolution;

  if (jFEXSmallRJetTOBEt > 0x7ff) jFEXSmallRJetTOBEt = 0x7ff;  //0x7ff is 11 bits 

  int eta = ieta;
  int phi = iphi;
  int Res = 0; //11 bits reserved
  int Sat = 0; //1 bit for saturation flag, not coded yet

  //create basic tobword with 32 bits 
  tobWord = tobWord + jFEXSmallRJetTOBEt + (phi << 11) + (eta << 15) + (Res << 20) + (Sat << 31); 
  
  //arbitary et threshold to not overflow the TOBs 
  unsigned int minEtThreshold = 30;
  if (et < minEtThreshold) return 0;
  else return tobWord;
}
  
} // end of namespace bracket

