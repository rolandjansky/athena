/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#undef NDEBUG

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerBuilder.h"
#include "L1CaloFEXSim/jFEXDriver.h"

#include "L1CaloFEXSim/jSuperCellTowerMapper.h"

#include "L1CaloFEXSim/jFEXSim.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

#include "L1CaloFEXSim/jTowerContainer.h"

#include <cassert>
#include "SGTools/TestStore.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <ctime>

#define DEBUG_VHB 1


namespace LVL1 {

  jFEXDriver::jFEXDriver(const std::string& name, ISvcLocator* pSvcLocator)
    :  AthAlgorithm(name, pSvcLocator)//AthReentrantAlgorithm(name, pSvcLocator)
  { 
  
  }


 jFEXDriver::~jFEXDriver()
{
  ATH_MSG_DEBUG("Destroying " << name() << "...");
}


StatusCode jFEXDriver::initialize()
{

  m_numberOfEvents = 1;

  ServiceHandle<ITHistSvc> histSvc("THistSvc","");
  StatusCode scHist = histSvc.retrieve();
  if (scHist ==  StatusCode::FAILURE) {ATH_MSG_ERROR("Failed to retrieve THistSvc"); }

  //Reta
  TH1F* hReta = new TH1F("Reta", "Reta",20,0,1);
  hReta->GetXaxis()->SetTitle("TObs Reta");
  hReta->GetYaxis()->SetTitle("Events");
  
  StatusCode scReg = histSvc->regHist("/ISO/Reta", hReta); 
  if (scReg ==  StatusCode::FAILURE) {ATH_MSG_ERROR("Failed to define stream"); }


  ATH_CHECK( m_jTowerBuilderTool.retrieve() );

  ATH_CHECK( m_jSuperCellTowerMapperTool.retrieve() );

  ATH_CHECK( m_jFEXSysSimTool.retrieve() );

  ATH_CHECK( m_jTowerContainerSGKey.initialize() );

  //ATH_CHECK( m_jFEXOutputCollectionSGKey.initialize() );

  return StatusCode::SUCCESS;

}


StatusCode jFEXDriver::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


  StatusCode jFEXDriver::execute(/*const EventContext& ctx*/) //const
{

  ATH_MSG_DEBUG("Executing " << name() << ", processing event number " << m_numberOfEvents );

  // OLD DIMA STUFF---------------------- Maybe useful in the future again
  //if (fabsf((*cell)->eta()) > 2.55) continue;
  //if (!((*cell)->provenance() & 0x40)) continue; // BCID cut
  //// if (!((*cell)->provenance() & 0x200)) continue;
  //// 8192 & 0x40 = 0
  //// 8256 & 0x40 = 64
  //// 8704 & 0x40 = 0
  //// 8768 & 0x40 = 64

  // STEP 0 - Make a fresh local jTowerContainer
  std::unique_ptr<jTowerContainer> local_jTowerContainerRaw = std::make_unique<jTowerContainer>();

  // STEP 1 TO BE REPLACED IN THE NEAR FUTURE - KEPT HERE FOR REFERENCE
  // STEP 1 - Do some monitoring (code to exported in the future to another algorithm accessing only StoreGate and not appearing in this algorithm)
  
  jFEXOutputCollection* my_jFEXOutputCollection = new jFEXOutputCollection();
  //std::shared_ptr<jFEXOutputCollection> my_jFEXOutputCollection = std::make_shared<jFEXOutputCollection>();
  bool savetob = true;
  if(savetob)
  {
    StatusCode sctob = evtStore()->record(my_jFEXOutputCollection,"jFEXOutputCollection");
    if(sctob == StatusCode::SUCCESS){}
    else if (sctob == StatusCode::FAILURE){ATH_MSG_ERROR("Event " << m_numberOfEvents << " , Failed to put jFEXOutputCollection into Storegate.");}
    
    
    //SG::WriteHandle<jFEXOutputCollection> jFEXOutputCollectionSG(m_jFEXOutputCollectionSGKey,ctx);
    //ATH_CHECK(jFEXOutputCollectionSG.record(std::make_unique<jFEXOutputCollection>()));
    
  }
  

  // STEP 2 - Make some jTowers and fill the local container
  ATH_CHECK( m_jTowerBuilderTool.retrieve() );
  m_jTowerBuilderTool->init(local_jTowerContainerRaw);
  local_jTowerContainerRaw->clearContainerMap();
  local_jTowerContainerRaw->fillContainerMap();

  // STEP 3 - Do the supercell-tower mapping - put this information into the jTowerContainer
  ATH_CHECK( m_jSuperCellTowerMapperTool.retrieve() );
  ATH_CHECK(m_jSuperCellTowerMapperTool->AssignSuperCellsToTowers(local_jTowerContainerRaw));
  ATH_CHECK(m_jSuperCellTowerMapperTool->AssignTriggerTowerMapper(local_jTowerContainerRaw));

  // STEP 4 - Write the completed jTowerContainer into StoreGate (move the local copy in memory)
  SG::WriteHandle<LVL1::jTowerContainer> jTowerContainerSG(m_jTowerContainerSGKey/*, ctx*/);
  ATH_CHECK(jTowerContainerSG.record(std::move(/*my_jTowerContainerRaw*/local_jTowerContainerRaw)));

  // STEP 5 - Set up the jFEXSysSim
  ATH_CHECK( m_jFEXSysSimTool.retrieve() );
  m_jFEXSysSimTool->init();

  // STEP 6 - Run THE jFEXSysSim
  ATH_CHECK(m_jFEXSysSimTool->execute());

  // STEP 7 - Close and clean the event  
  m_jFEXSysSimTool->cleanup();
  m_jSuperCellTowerMapperTool->reset();
  m_jTowerBuilderTool->reset();

  ATH_MSG_DEBUG("Executed " << name() << ", closing event number " << m_numberOfEvents );

  m_numberOfEvents++;

  return StatusCode::SUCCESS;
}
  
  
} // end of LVL1 namespace
