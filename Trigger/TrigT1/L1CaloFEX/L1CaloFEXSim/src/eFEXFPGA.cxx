/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXFPGA  -  description
//                              -------------------
//     begin                : 15 10 2019
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/
#include "L1CaloFEXSim/eFEXFPGA.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXSim/eFEXegAlgo.h"
#include "L1CaloFEXSim/eFEXegTOB.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"
#include "L1CaloFEXSim/eFEXtauAlgo.h"
#include "L1CaloFEXSim/eFEXtauTOB.h"
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

eFEXFPGA::eFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent)
{
  declareInterface<IeFEXFPGA>(this);
}
 
    
  /** Destructor */
  eFEXFPGA::~eFEXFPGA()
  {
  }

//---------------- Initialisation -------------------------------------------------
  
StatusCode eFEXFPGA::initialize()
{

  ATH_CHECK(m_eFEXFPGA_eTowerContainerKey.initialize());

  //ATH_CHECK(m_eFEXFPGA_eFEXOutputCollectionKey.initialize());
  return StatusCode::SUCCESS;
}
  

StatusCode eFEXFPGA::init(int id, int efexid)
{
  m_id = id;
  m_efexid = efexid;

  return StatusCode::SUCCESS;

}

void eFEXFPGA::reset(){

  m_id = -1;
  m_efexid = -1;

}

StatusCode eFEXFPGA::execute(){

  m_tobwords.clear();

  SG::ReadHandle<eTowerContainer> jk_eFEXFPGA_eTowerContainer(m_eFEXFPGA_eTowerContainerKey/*,ctx*/);
  if(!jk_eFEXFPGA_eTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_eFEXFPGA_eTowerContainer " << m_eFEXFPGA_eTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }
    
  eFEXOutputCollection* eFEXOutputs;

  // To be replaced soon but left here commented for information
  /*
  SG::ReadHandle<eFEXOutputCollection> jk_eFEXFPGA_eFEXOutputCollection(m_eFEXFPGA_eFEXOutputCollectionKey,ctx);
  if(!jk_eFEXFPGA_eFEXOutputCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_eFEXFPGA_eFEXOutputCollection " << m_eFEXFPGA_eFEXOutputCollectionKey.key() );
    return StatusCode::FAILURE;
  }
  */
  
  StatusCode sc_tobs = evtStore()->retrieve(eFEXOutputs, "eFEXOutputCollection");
  if(sc_tobs == StatusCode::SUCCESS){ }
  else if(sc_tobs == StatusCode::FAILURE) {ATH_MSG_DEBUG("\n---- eFEXegAlgo --------- Failed to find eFEXOutputCollection in eFEXFPGA"); }
  

  for(int ieta = 1; ieta < 5; ieta++) {
    for(int iphi = 1; iphi < 9; iphi++) {
      int tobtable[3][3]={
        {m_eTowersIDs[iphi-1][ieta-1], m_eTowersIDs[iphi-1][ieta], m_eTowersIDs[iphi-1][ieta+1]},
        {m_eTowersIDs[iphi][ieta-1], m_eTowersIDs[iphi][ieta], m_eTowersIDs[iphi][ieta+1]},
        {m_eTowersIDs[iphi+1][ieta-1], m_eTowersIDs[iphi+1][ieta], m_eTowersIDs[iphi+1][ieta+1]},
      };

      ATH_CHECK( m_eFEXegAlgoTool.retrieve() );
  
      ATH_CHECK( m_eFEXegAlgoTool->safetyTest() );
      m_eFEXegAlgoTool->setup(tobtable);

      // temporarily(?) removed for debugging
      //if (eFEXFPGA_egAlgo->haveSeed() == false) continue;
      
      // Get Reta and Rhad outputs
      std::vector<unsigned int> RetaCoreEnv; 
      m_eFEXegAlgoTool->getReta(RetaCoreEnv);
      std::vector<unsigned int> RhadCoreEnv; 
      m_eFEXegAlgoTool->getRhad(RhadCoreEnv);
      std::vector<unsigned int> WstotCoreEnv;
      m_eFEXegAlgoTool->getWstot(WstotCoreEnv);

      // temp thresholds that will come from Trigger menu
      std::vector<unsigned int> tempThrs;
      tempThrs.push_back(40);
      tempThrs.push_back(30);
      tempThrs.push_back(20);
      
      // Set Reta, Rhad and Wstot WP
      unsigned int RetaWP = 0;
      unsigned int RhadWP = 0;
      unsigned int WstotWP = 0;
      SetIsoWP(RetaCoreEnv,tempThrs,RetaWP);
      SetIsoWP(RhadCoreEnv,tempThrs,RhadWP);
      SetIsoWP(WstotCoreEnv,tempThrs,WstotWP);
      int eta_ind = ieta - 1;
      int phi_ind = iphi - 1;    


      uint32_t tobword = formEmTOB(eta_ind,phi_ind);
      if ( tobword != 0 ) m_tobwords.push_back(tobword);

      std::unique_ptr<eFEXegTOB> tmp_tob = m_eFEXegAlgoTool->geteFEXegTOB();
      
      tmp_tob->setFPGAID(m_id);
      // TODO tmp_tob->.setEFEXID(xxx);
      tmp_tob->setEta(ieta);
      tmp_tob->setPhi(iphi);

      // for plotting
      eFEXOutputs->addValue_eg("WstotNum", tmp_tob->getWstotNum());
      eFEXOutputs->addValue_eg("WstotDen", tmp_tob->getWstotDen());
      eFEXOutputs->addValue_eg("RetaNum", tmp_tob->getRetaNum());
      eFEXOutputs->addValue_eg("RetaDen", tmp_tob->getRetaDen());
      eFEXOutputs->addValue_eg("RhadNum", tmp_tob->getRhadNum());
      eFEXOutputs->addValue_eg("RhadDen", tmp_tob->getRhadDen());
      eFEXOutputs->addValue_eg("haveSeed", m_eFEXegAlgoTool->haveSeed());
      eFEXOutputs->addValue_eg("Et", m_eFEXegAlgoTool->getET());
      float eta = 9999;
      m_eFEXegAlgoTool->getRealEta(eta);
      eFEXOutputs->addValue_eg("eta", eta);
      float phi = 9999;
      m_eFEXegAlgoTool->getRealPhi(phi);
      eFEXOutputs->addValue_eg("phi", phi);
      unsigned int em_et = 9999; 
      m_eFEXegAlgoTool->getCoreEMTowerET(em_et);
      eFEXOutputs->addValue_eg("em", em_et);
      unsigned int had_et = 9999;
      m_eFEXegAlgoTool->getCoreHADTowerET(had_et);
      eFEXOutputs->addValue_eg("had", had_et);
      eFEXOutputs->fill_eg();

    }
  }


  //ATH_CHECK(store())

  // --------------- TAU -------------
  for(int ieta = 1; ieta < 5; ieta++)
  {
    for(int iphi = 1; iphi < 9; iphi++)
    {
      int tobtable[3][3]={
        {m_eTowersIDs[iphi-1][ieta-1], m_eTowersIDs[iphi-1][ieta], m_eTowersIDs[iphi-1][ieta+1]},
        {m_eTowersIDs[iphi][ieta-1], m_eTowersIDs[iphi][ieta], m_eTowersIDs[iphi][ieta+1]},
        {m_eTowersIDs[iphi+1][ieta-1], m_eTowersIDs[iphi+1][ieta], m_eTowersIDs[iphi+1][ieta+1]},
      };
      
      ATH_CHECK( m_eFEXtauAlgoTool.retrieve() );
  
      ATH_CHECK( m_eFEXtauAlgoTool->safetyTest() );
      m_eFEXtauAlgoTool->setup(tobtable);

      // for plotting
      eFEXOutputs->addValue_tau("isCentralTowerSeed", m_eFEXtauAlgoTool->isCentralTowerSeed());
      eFEXOutputs->addValue_tau("Et", m_eFEXtauAlgoTool->getEt());
      eFEXOutputs->addValue_tau("Iso", m_eFEXtauAlgoTool->getIso());
      
      eFEXOutputs->fill_tau();

      if (!m_eFEXtauAlgoTool->isCentralTowerSeed()){ continue; }

    }
  }


  return StatusCode::SUCCESS;

}



std::vector<uint32_t> eFEXFPGA::getEmTOBs()
{
  auto tobsSort = m_tobwords;

  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in FPGA: " << m_id << " before truncation");

  // sort tobs by their et (last 12 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);

  /*
  for(auto &j : tobsSort){
    std::cout << "values: post sort " << std::bitset<32>(j) << std::endl;
  }
  */

  // return the top 6 highest ET TOBs from the FPGA
  tobsSort.resize(6);
  return tobsSort;

}


void eFEXFPGA::SetTowersAndCells_SG(int tmp_eTowersIDs_subset[][6]){
    
  int rows = 10;
  int cols = sizeof tmp_eTowersIDs_subset[0] / sizeof tmp_eTowersIDs_subset[0][0];
  
  std::copy(&tmp_eTowersIDs_subset[0][0], &tmp_eTowersIDs_subset[0][0]+(10*6),&m_eTowersIDs[0][0]);
  
  if(false){ //this prints out the eTower IDs that each FPGA is responsible for
    ATH_MSG_DEBUG("\n---- eFEXFPGA --------- FPGA (" << m_id << ") IS RESPONSIBLE FOR eTOWERS :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_eTowersIDs[thisRow][thisCol] << "  "); }
	else { ATH_MSG_DEBUG("|  " << m_eTowersIDs[thisRow][thisCol] << "  |"); }
      }
    }
  }
  
}


void eFEXFPGA::SetIsoWP(std::vector<unsigned int> & CoreEnv, std::vector<unsigned int> & thresholds, unsigned int & workingPoint) {

  bool CoreOverflow = false;
  bool EnvOverflow = false;
  bool ThrEnvOverflowL = false;
  bool ThrEnvOverflowM = false;
  bool ThrEnvOverflowT = false;

  if (CoreEnv[0] > 0xffff) CoreOverflow = true;
  if (CoreEnv[1] > 0xffff) EnvOverflow = true;
  if (CoreEnv[1]*thresholds[0] > 0xffff) ThrEnvOverflowL = true;
  if (CoreEnv[1]*thresholds[1] > 0xffff) ThrEnvOverflowM = true;
  if (CoreEnv[1]*thresholds[2] > 0xffff) ThrEnvOverflowT = true;

  if (CoreOverflow == false) {
    if (EnvOverflow == false) {
      if ( (CoreEnv[0] > (thresholds[2]*CoreEnv[1])) && ThrEnvOverflowT == false ) {
	workingPoint = 3;
      } 
      else if ( (CoreEnv[0] > (thresholds[1]*CoreEnv[1])) && ThrEnvOverflowM == false ) {
	workingPoint = 2;
      } 
      else if ( (CoreEnv[0] > (thresholds[0]*CoreEnv[1])) && ThrEnvOverflowL == false ) {
	workingPoint = 1;
      }
      else { 
	workingPoint = 0;
      }
    } else {
      workingPoint = 0; //env overflow
    }
  } 
  else {
    workingPoint = 3; // core overflow 
  }

}




uint32_t eFEXFPGA::formEmTOB(int & ieta, int & iphi)
{
  uint32_t tobWord = 0;
  const unsigned int eFexTobstep = 100;


  //returns a unsigned integer et value corresponding to the... eFEX EM cluster? in MeV
  unsigned int et = m_eFEXegAlgoTool->getET();
 
  unsigned int eFexTobEt = et/eFexTobstep;//steps of 100 MeV for the TOB

  if (eFexTobEt > 0xfff) eFexTobEt = 0xfff; //truncate at 12 bits, set to max value of 4095, 0xfff, or 111111111111
  int eta = ieta;
  int phi = iphi;

  //Create bare minimum tob word with et, eta, phi, and fpga index, bitshifted to the appropriate locations
  tobWord = tobWord + eFexTobEt + (phi << 24) + (eta << 27) + (m_id << 30);

  ATH_MSG_DEBUG("tobword with et, eta, phi, fpga: " << std::bitset<32>(tobWord) );

  //some arbitrary cut so that we're not flooded with tobs. to be taken from the Trigger menu in the future!
  unsigned int minEtThreshold = 30;
  if (et < minEtThreshold) return 0;  
  else return tobWord;

}
  
} // end of namespace bracket

