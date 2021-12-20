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
#include <unordered_map>

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
  ATH_CHECK( m_eFEXegAlgoTool.retrieve() );
  ATH_CHECK( m_eFEXtauAlgoTool.retrieve() );
  
  
  ATH_CHECK(m_l1MenuKey.initialize());

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

StatusCode eFEXFPGA::execute(eFEXOutputCollection* inputOutputCollection){

  m_emTobwords.clear();
  m_tauTobwords.clear();

  SG::ReadHandle<eTowerContainer> jk_eFEXFPGA_eTowerContainer(m_eFEXFPGA_eTowerContainerKey/*,ctx*/);
  if(!jk_eFEXFPGA_eTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_eFEXFPGA_eTowerContainer " << m_eFEXFPGA_eTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
  ATH_CHECK(l1Menu.isValid());

  auto & thr_eEM = l1Menu->thrExtraInfo().eEM();
  auto & thr_eTAU = l1Menu->thrExtraInfo().eTAU();

  for(int ieta = 1; ieta < 5; ieta++) {
    for(int iphi = 1; iphi < 9; iphi++) {
      int tobtable[3][3]={
        {m_eTowersIDs[iphi-1][ieta-1], m_eTowersIDs[iphi-1][ieta], m_eTowersIDs[iphi-1][ieta+1]},
        {m_eTowersIDs[iphi][ieta-1], m_eTowersIDs[iphi][ieta], m_eTowersIDs[iphi][ieta+1]},
        {m_eTowersIDs[iphi+1][ieta-1], m_eTowersIDs[iphi+1][ieta], m_eTowersIDs[iphi+1][ieta+1]},
      };

      ATH_CHECK( m_eFEXegAlgoTool->safetyTest() );
      m_eFEXegAlgoTool->setup(tobtable);

      // ignore any tobs without a seed, move on to the next window
      if (m_eFEXegAlgoTool->hasSeed() == false) continue;
      unsigned int seed = 0;
      seed = m_eFEXegAlgoTool->getSeed();

      // the minimum energy to send to topo (not eta dependent yet, but keep inside loop as it will be eventually?)
      unsigned int ptMinToTopoCounts = 0;
      ptMinToTopoCounts = thr_eEM.ptMinToTopoCounts(); 

      //returns a unsigned integer et value corresponding to the... eFEX EM cluster in 25 MeV internal calculation scale
      unsigned int eEMTobEt = 0;
      eEMTobEt = m_eFEXegAlgoTool->getET();
            
      // thresholds from Trigger menu
      auto iso_loose  = thr_eEM.isolation(TrigConf::Selection::WP::LOOSE, ieta);
      auto iso_medium = thr_eEM.isolation(TrigConf::Selection::WP::MEDIUM, ieta);
      auto iso_tight  = thr_eEM.isolation(TrigConf::Selection::WP::TIGHT, ieta);  

      std::vector<unsigned int> threshReta;
      threshReta.push_back(iso_loose.reta_fw());
      threshReta.push_back(iso_medium.reta_fw());
      threshReta.push_back(iso_tight.reta_fw());

      std::vector<unsigned int> threshRhad;
      threshRhad.push_back(iso_loose.rhad_fw());
      threshRhad.push_back(iso_medium.rhad_fw());
      threshRhad.push_back(iso_tight.rhad_fw());

      std::vector<unsigned int> threshWstot;
      threshWstot.push_back(iso_loose.wstot_fw());
      threshWstot.push_back(iso_medium.wstot_fw());
      threshWstot.push_back(iso_tight.wstot_fw());

      ATH_MSG_DEBUG("ieta=" << ieta << "  loose => reta_fw=" << threshReta[0] << ", rhad_fw=" << threshRhad[0] << ", wstot_fw=" << threshWstot[0]);
      ATH_MSG_DEBUG("ieta=" << ieta << "  medium => reta_fw=" << threshReta[1] << ", rhad_fw=" << threshRhad[1] << ", wstot_fw=" << threshWstot[1]);
      ATH_MSG_DEBUG("ieta=" << ieta << "  tight => reta_fw=" << threshReta[2] << ", rhad_fw=" << threshRhad[2] << ", wstot_fw=" << threshWstot[2]);

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
      
      // bitshifts for the different iso vars
      unsigned int RetaBitS = 3;
      unsigned int RhadBitS = 3;
      unsigned int WstotBitS = 5;

      unsigned int maxEtCounts = thr_eEM.maxEtCounts(m_eFexStep);
      if (eEMTobEt >= maxEtCounts){
	RetaWP = 3;
	RhadWP = 3;
	WstotWP = 3;
      }
      else{
	SetIsoWP(RetaCoreEnv,threshReta,RetaWP,RetaBitS);
	SetIsoWP(RhadCoreEnv,threshRhad,RhadWP,RhadBitS);
	SetIsoWP(WstotCoreEnv,threshWstot,WstotWP,WstotBitS);
      }
      int eta_ind = ieta; // No need to offset eta index with new 0-5 convention
      int phi_ind = iphi - 1;

      //form the egamma tob word
      uint32_t tobword = m_eFEXFormTOBsTool->formEmTOBWord(m_id,eta_ind,phi_ind,RhadWP,WstotWP,RetaWP,seed,eEMTobEt,ptMinToTopoCounts);
      if ( (tobword != 0) && (eEMTobEt != 0) ) m_emTobwords.push_back(tobword);

      std::unique_ptr<eFEXegTOB> tmp_tob = m_eFEXegAlgoTool->geteFEXegTOB();
      
      tmp_tob->setFPGAID(m_id);
      // TODO tmp_tob->.setEFEXID(xxx);
      tmp_tob->setEta(ieta);
      tmp_tob->setPhi(iphi);

      // for plotting
      if (inputOutputCollection->getdooutput()) {
        inputOutputCollection->addeFexNumber(m_efexid);
        inputOutputCollection->addEMtob(tobword);
        inputOutputCollection->addValue_eg("WstotNum", tmp_tob->getWstotNum());
        inputOutputCollection->addValue_eg("WstotDen", tmp_tob->getWstotDen());
        inputOutputCollection->addValue_eg("RetaNum", tmp_tob->getRetaNum());
        inputOutputCollection->addValue_eg("RetaDen", tmp_tob->getRetaDen());
        inputOutputCollection->addValue_eg("RhadNum", tmp_tob->getRhadNum());
        inputOutputCollection->addValue_eg("RhadDen", tmp_tob->getRhadDen());
        inputOutputCollection->addValue_eg("haveSeed", m_eFEXegAlgoTool->hasSeed());
        inputOutputCollection->addValue_eg("ET", m_eFEXegAlgoTool->getET());
        float eta = 9999;
        m_eFEXegAlgoTool->getRealEta(eta);
        inputOutputCollection->addValue_eg("eta", eta);
        float phi = 9999;
        m_eFEXegAlgoTool->getRealPhi(phi);
        inputOutputCollection->addValue_eg("phi", phi);
        unsigned int em_et = 9999; 
        m_eFEXegAlgoTool->getCoreEMTowerET(em_et);
        inputOutputCollection->addValue_eg("em", em_et);
        unsigned int had_et = 9999;
        m_eFEXegAlgoTool->getCoreHADTowerET(had_et);
        inputOutputCollection->addValue_eg("had", had_et);
        inputOutputCollection->fill_eg();
      }

    }
  }

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
      
      ATH_CHECK( m_eFEXtauAlgoTool->safetyTest() );
      m_eFEXtauAlgoTool->setup(tobtable);

      if (!m_eFEXtauAlgoTool->isCentralTowerSeed()){ continue; }

      // the minimum energy to send to topo (not eta dependent yet, but keep inside loop as it will be eventually?)
      unsigned int ptTauMinToTopoCounts = 0;
      ptTauMinToTopoCounts = thr_eTAU.ptMinToTopoCounts();

      // Get Et of eFEX tau object in internal units (25 MeV)
      unsigned int eTauTobEt = 0;
      eTauTobEt = m_eFEXtauAlgoTool->getEt();

      // thresholds from Trigger menu
      auto iso_loose  = thr_eTAU.isolation(TrigConf::Selection::WP::LOOSE, ieta);
      auto iso_medium = thr_eTAU.isolation(TrigConf::Selection::WP::MEDIUM, ieta);
      auto iso_tight  = thr_eTAU.isolation(TrigConf::Selection::WP::TIGHT, ieta);  

      std::vector<unsigned int> threshRCore;
      threshRCore.push_back(iso_loose.rCore_fw());
      threshRCore.push_back(iso_medium.rCore_fw());
      threshRCore.push_back(iso_tight.rCore_fw());

      // Get isolation values
      std::vector<unsigned int> rCoreVec; 
      m_eFEXtauAlgoTool->getRCore(rCoreVec);

      // Set isolation WP
      unsigned int rCoreWP = 0;

      // Isolation bitshift value
      unsigned int RcoreBitS = 3;

      SetIsoWP(rCoreVec,threshRCore,rCoreWP,RcoreBitS);

      // Currently only one WP defined for tau iso, decided to set as Medium WP for freedom to add looser and tighter WPs in the future 
      if (rCoreWP > 2) {
        rCoreWP = 1;
      }

      unsigned int seed = 0;
      seed = m_eFEXtauAlgoTool->getSeed();
      // Seed as returned is supercell value within 3x3 area, here want it within central cell
      seed = seed - 4;      

      unsigned int und = 0;
      und = m_eFEXtauAlgoTool->getUnD();

      int eta_ind = ieta; // No need to offset eta index with new 0-5 convention
      int phi_ind = iphi - 1;
      
      uint32_t tobword = m_eFEXFormTOBsTool->formTauTOBWord(m_id, eta_ind, phi_ind, eTauTobEt, rCoreWP, seed, und, ptTauMinToTopoCounts);
      if ( tobword != 0 ) m_tauTobwords.push_back(tobword);

      // for plotting
      if (inputOutputCollection->getdooutput()) {
        inputOutputCollection->addValue_tau("isCentralTowerSeed", m_eFEXtauAlgoTool->isCentralTowerSeed());
        inputOutputCollection->addValue_tau("Et", m_eFEXtauAlgoTool->getEt());
        inputOutputCollection->addValue_tau("Eta", ieta);
        inputOutputCollection->addValue_tau("Phi", iphi);
        const LVL1::eTower * centerTower = jk_eFEXFPGA_eTowerContainer->findTower(m_eTowersIDs[iphi][ieta]);
        inputOutputCollection->addValue_tau("FloatEta", centerTower->eta() * centerTower->getPosNeg());
        inputOutputCollection->addValue_tau("FloatPhi", centerTower->phi());
        inputOutputCollection->addValue_tau("IsoCore", rCoreVec[0]);
        inputOutputCollection->addValue_tau("IsoEnv", rCoreVec[1]);
        inputOutputCollection->addValue_tau("RealIso", m_eFEXtauAlgoTool->getRealIso());
        inputOutputCollection->addValue_tau("IsoWP", rCoreWP);
        inputOutputCollection->addValue_tau("Seed", seed);
        inputOutputCollection->addValue_tau("UnD", und);
        
        inputOutputCollection->fill_tau();
      }
    }
  }

  return StatusCode::SUCCESS;

}



std::vector<uint32_t> eFEXFPGA::getEmTOBs()
{
  auto tobsSort = m_emTobwords;

  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in FPGA: " << m_id << " before truncation");

  // sort tobs by their et (last 12 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);

  // return the top 6 highest ET TOBs from the FPGA
  if (tobsSort.size() > 6) tobsSort.resize(6);
  return tobsSort;

}

std::vector<uint32_t> eFEXFPGA::getTauTOBs()
{
  auto tobsSort = m_tauTobwords;

  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in FPGA: " << m_id << " before truncation");

  // sort tobs by their et (last 12 bits of the 32 bit tob word)
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);

  // return the top 6 highest ET TOBs from the FPGA
  if (tobsSort.size() > 6) tobsSort.resize(6);
  return tobsSort;

}

void eFEXFPGA::SetTowersAndCells_SG(int tmp_eTowersIDs_subset[][6]){
    
  int rows = 10;
  int cols = sizeof tmp_eTowersIDs_subset[0] / sizeof tmp_eTowersIDs_subset[0][0];
  
  std::copy(&tmp_eTowersIDs_subset[0][0], &tmp_eTowersIDs_subset[0][0]+(10*6),&m_eTowersIDs[0][0]);
  
  if(false){ //this prints out the eTower IDs that each FPGA is responsible for
    ATH_MSG_DEBUG("\n---- eFEXFPGA --------- eFEX (" << m_efexid << " ----- FPGA (" << m_id << ") IS RESPONSIBLE FOR eTOWERS :");
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


void eFEXFPGA::SetIsoWP(std::vector<unsigned int> & CoreEnv, std::vector<unsigned int> & thresholds, unsigned int & workingPoint, unsigned int & bitshift) {
  // Working point evaluted by Core * 2^bitshift > Threshold * Environment conditions
  bool CoreOverflow = false;
  bool EnvOverflow = false;
  bool ThrEnvOverflowL = false;
  bool ThrEnvOverflowM = false;
  bool ThrEnvOverflowT = false;

  std::unordered_map<unsigned int, unsigned int> bsmap { {3, 8}, {5, 32}};

  if (CoreEnv[0]*bsmap[bitshift] > 0xffff) CoreOverflow = true;
  if (CoreEnv[1] > 0xffff) EnvOverflow = true;
  if (CoreEnv[1]*thresholds[0] > 0xffff) ThrEnvOverflowL = true;
  if (CoreEnv[1]*thresholds[1] > 0xffff) ThrEnvOverflowM = true;
  if (CoreEnv[1]*thresholds[2] > 0xffff) ThrEnvOverflowT = true;

  if (CoreOverflow ==  false) {
    if (EnvOverflow == false) {
      if ( (CoreEnv[0]*bsmap[bitshift] < (thresholds[0]*CoreEnv[1])) || ThrEnvOverflowL == true ) {
	workingPoint = 0;
      }
      else if ( (CoreEnv[0]*bsmap[bitshift] < (thresholds[1]*CoreEnv[1])) || ThrEnvOverflowM == true ) {
	workingPoint = 1;
      }
      else if ( (CoreEnv[0]*bsmap[bitshift] < (thresholds[2]*CoreEnv[1])) || ThrEnvOverflowT == true ) {
        workingPoint = 2;
      }
      else {
        workingPoint = 3;
      }
    } else {
      //env overflow
      workingPoint = 0;
    }
  } 
  else {
    // core overflow
    workingPoint = 3;
  }

}

} // end of namespace bracket

