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
#include "L1CaloFEXSim/eTower.h"
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

//================ Initialisation =================================================
  
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
  else if(sc_tobs == StatusCode::FAILURE) {ATH_MSG_DEBUG("\n==== eFEXegAlgo ========= Failed to find eFEXOutputCollection in eFEXFPGA"); }
  

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
      
      std::vector<unsigned int> RetaND = m_eFEXegAlgoTool->getReta();
      std::vector<unsigned int> RhadND; m_eFEXegAlgoTool->getRhad(RhadND);
      
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

  // =============== TAU =============
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

void eFEXFPGA::SetTowersAndCells_SG(int tmp_eTowersIDs_subset[][6]){
    
  int rows = 10;
  int cols = sizeof tmp_eTowersIDs_subset[0] / sizeof tmp_eTowersIDs_subset[0][0];
  
  std::copy(&tmp_eTowersIDs_subset[0][0], &tmp_eTowersIDs_subset[0][0]+(10*6),&m_eTowersIDs[0][0]);
  
  if(false){ //this prints out the eTower IDs that each FPGA is responsible for
    ATH_MSG_DEBUG("\n==== eFEXFPGA ========= FPGA (" << m_id << ") IS RESPONSIBLE FOR eTOWERS :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_eTowersIDs[thisRow][thisCol] << "  "); }
	else { ATH_MSG_DEBUG("|  " << m_eTowersIDs[thisRow][thisCol] << "  |"); }
      }
    }
  }
  
}
  
} // end of namespace bracket

