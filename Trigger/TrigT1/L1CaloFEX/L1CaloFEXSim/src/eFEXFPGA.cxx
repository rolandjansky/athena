/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "TrigConfData/L1Menu.h"


#include <iostream>
#include <fstream>

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
  ATH_CHECK(m_l1MenuKey.initialize());

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

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
  ATH_CHECK(l1Menu.isValid());

  auto & thr_eEM = l1Menu->thrExtraInfo().eEM();

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

      // ignore any tobs without a seed, move on to the next window
      if (m_eFEXegAlgoTool->hasSeed() == false) continue;
      unsigned int seed = 0;
      seed = m_eFEXegAlgoTool->getSeed();

      // the minimum energy to send to topo (not eta dependent yet, but keep inside loop as it will be eventually?)
      unsigned int ptMinToTopoCounts = 0;
      ptMinToTopoCounts = thr_eEM.ptMinToTopoCounts(); 

      //returns a unsigned integer et value corresponding to the... eFEX EM cluster? in MeV?
      unsigned int eEMTobEt = 0;
      eEMTobEt = m_eFEXegAlgoTool->getET();
      const unsigned int eFexTobstep = 100;
      unsigned int eEMTobEtCounts = 0;
      eEMTobEtCounts = eEMTobEt/eFexTobstep;//steps of 100 MeV for the TOB

      // thresholds from Trigger menu
      auto iso_loose  = thr_eEM.isolation(TrigConf::Selection::WP::LOOSE, ieta);
      auto iso_medium = thr_eEM.isolation(TrigConf::Selection::WP::MEDIUM, ieta);
      auto iso_tight  = thr_eEM.isolation(TrigConf::Selection::WP::TIGHT, ieta);  

      std::vector<unsigned int> threshReta;
      threshReta.push_back(iso_loose.reta());
      threshReta.push_back(iso_medium.reta());
      threshReta.push_back(iso_tight.reta());

      std::vector<unsigned int> threshRhad;
      threshRhad.push_back(iso_loose.rhad());
      threshRhad.push_back(iso_medium.rhad());
      threshRhad.push_back(iso_tight.rhad());

      std::vector<unsigned int> threshWstot;
      threshWstot.push_back(iso_loose.wstot());
      threshWstot.push_back(iso_medium.wstot());
      threshWstot.push_back(iso_tight.wstot());

      ATH_MSG_DEBUG("ieta=" << ieta << "  loose => reta=" << threshReta[0] << ", had=" << threshRhad[0] << ", wstot=" << threshWstot[0]);
      ATH_MSG_DEBUG("ieta=" << ieta << "  medium => reta=" << threshReta[1] << ", had=" << threshRhad[1] << ", wstot=" << threshWstot[1]);
      ATH_MSG_DEBUG("ieta=" << ieta << "  tight => reta=" << threshReta[2] << ", had=" << threshRhad[2] << ", wstot=" << threshWstot[2]);

      // Get Reta and Rhad outputs
      std::vector<unsigned int> RetaCoreEnv; 
      m_eFEXegAlgoTool->getReta(RetaCoreEnv);
      std::vector<unsigned int> RhadCoreEnv; 
      m_eFEXegAlgoTool->getRhad(RhadCoreEnv);
      std::vector<unsigned int> WstotCoreEnv;
      m_eFEXegAlgoTool->getWstot(WstotCoreEnv);

      // Set Reta, Rhad and Wstot WP
      unsigned int RetaWP = 0;
      unsigned int RhadWP = 0;
      unsigned int WstotWP = 0;
      SetIsoWP(RetaCoreEnv,threshReta,RetaWP);
      SetIsoWP(RhadCoreEnv,threshRhad,RhadWP);
      SetIsoWP(WstotCoreEnv,threshWstot,WstotWP);
      int eta_ind = ieta; // No need to offset eta index with new 0-5 convention
      int phi_ind = iphi - 1;

      //form the egamma tob word
      uint32_t tobword = formEmTOB(m_id,eta_ind,phi_ind,RhadWP,WstotWP,RetaWP,seed,eEMTobEtCounts,ptMinToTopoCounts);
      if ( (tobword != 0) && (eEMTobEtCounts != 0) ) m_tobwords.push_back(tobword);

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
      eFEXOutputs->addValue_eg("haveSeed", m_eFEXegAlgoTool->hasSeed());
      eFEXOutputs->addValue_eg("ET", m_eFEXegAlgoTool->getET());
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
  if (tobsSort.size() > 6) tobsSort.resize(6);
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


  //-----------------------------------------------------------
  // Set up a the second CSV file if necessary (should only need to be done if the mapping changes, which should never happen unless major changes to the simulation are required)
  if(false){ // CSV CODE TO BE RE-INTRODUCED VERY SOON
    SG::ReadHandle<eTowerContainer> jk_eFEXFPGA_eTowerContainer(m_eFEXFPGA_eTowerContainerKey);
    if(!jk_eFEXFPGA_eTowerContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve jk_eFEXFPGA_eTowerContainer " << m_eFEXFPGA_eTowerContainerKey.key() );
    }
    
    std::ofstream tower_fpga_efex_map;
    tower_fpga_efex_map.open ("./tower_fpga_efex_map.csv", std::ios_base::app);
    
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	
	const LVL1::eTower * tmpTower = jk_eFEXFPGA_eTowerContainer->findTower(m_eTowersIDs[thisRow][thisCol]);
	
	tower_fpga_efex_map << m_efexid << "," << m_id << "," << m_eTowersIDs[thisRow][thisCol] << "," << tmpTower->eta() << "," << tmpTower->phi() << "\n";
	
      }
    }
  }
  //------------------------------------------------------------

  
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
      if ( (CoreEnv[0] > (thresholds[0]*CoreEnv[1])) && ThrEnvOverflowL == false ) {
	workingPoint = 1;
      } 
      else if ( (CoreEnv[0] > (thresholds[1]*CoreEnv[1])) && ThrEnvOverflowM == false ) {
	workingPoint = 2;
      } 
      else if ( (CoreEnv[0] > (thresholds[2]*CoreEnv[1])) && ThrEnvOverflowT == false ) {
	workingPoint = 3;
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




uint32_t eFEXFPGA::formEmTOB(int & fpga, int & eta, int & phi, unsigned int & rhad, unsigned int & wstot, unsigned int & reta, unsigned int & seed, unsigned int & et, unsigned int & ptMinTopo)
{
  uint32_t tobword = 0;

  if (et > 0xfff) et = 0xfff; //truncate at 12 bits, set to max value of 4095, 0xfff, or 111111111111

  //Create bare minimum tob word with et, eta, phi, and fpga index, bitshifted to the appropriate locations
  tobword = tobword + (fpga << 30) + (eta << 27) + (phi << 24) + (rhad << 22) + (wstot << 20) + (reta << 18) + (seed << 16) + et;

  ATH_MSG_DEBUG("tobword: " << std::bitset<32>(tobword) );

  //some arbitrary cut so that we're not flooded with tobs. taken from the Trigger menu!
  unsigned int minEtThreshold = ptMinTopo;
  if (et < minEtThreshold) return 0;  
  else return tobword;

}
  
} // end of namespace bracket

