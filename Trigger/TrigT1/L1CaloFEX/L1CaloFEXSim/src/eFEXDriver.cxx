/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#undef NDEBUG

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerBuilder.h"
#include "L1CaloFEXSim/eFEXDriver.h"

#include "L1CaloFEXSim/eSuperCellTowerMapper.h"

#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"
#include "L1CaloFEXSim/eFEXegTOB.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

#include "L1CaloFEXSim/eTowerContainer.h"

#include <cassert>
#include "SGTools/TestStore.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <ctime>

#define DEBUG_VHB 1


namespace LVL1 {

  eFEXDriver::eFEXDriver(const std::string& name, ISvcLocator* pSvcLocator)
    :  AthAlgorithm(name, pSvcLocator)//AthReentrantAlgorithm(name, pSvcLocator)
  { 
  
  }


 eFEXDriver::~eFEXDriver()
{
  ATH_MSG_DEBUG("Destroying " << name() << "...");
}


StatusCode eFEXDriver::initialize()
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


  ATH_CHECK( m_eTowerBuilderTool.retrieve() );

  ATH_CHECK( m_eSuperCellTowerMapperTool.retrieve() );

  ATH_CHECK( m_eFEXSysSimTool.retrieve() );

  ATH_CHECK( m_eTowerContainerSGKey.initialize() );

  //ATH_CHECK( m_eFEXOutputCollectionSGKey.initialize() );

  return StatusCode::SUCCESS;

}


StatusCode eFEXDriver::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


  StatusCode eFEXDriver::execute(/*const EventContext& ctx*/) //const
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

  // STEP 0 - Make a fresh local eTowerContainer
  //eTowerContainer* local_eTowerContainerRaw = new eTowerContainer();
  std::unique_ptr<eTowerContainer> local_eTowerContainerRaw = std::make_unique<eTowerContainer>();

  // STEP 1 - Do some monitoring (code to exported in the future to another algorithm accessing only StoreGate and not appearing in this algorithm)
  eFEXOutputCollection* my_eFEXOutputCollection = new eFEXOutputCollection();
  bool savetob = true;
  if(savetob)
  {
    StatusCode sctob = evtStore()->record(my_eFEXOutputCollection,"eFEXOutputCollection");
    if(sctob == StatusCode::SUCCESS){}
    else if (sctob == StatusCode::FAILURE){ATH_MSG_ERROR("Event " << m_numberOfEvents << " , Failed to put eFEXOutputCollection into Storegate.");}
    
    /*
    SG::WriteHandle<eFEXOutputCollection> eFEXOutputCollectionSG(m_eFEXOutputCollectionSGKey,ctx);
    ATH_CHECK(eFEXOutputCollectionSG.record(std::make_unique<eFEXOutputCollection>()));
    */
  }

  // STEP 2 - Make some eTowers and fill the local container
  ATH_CHECK( m_eTowerBuilderTool.retrieve() );
  m_eTowerBuilderTool->init(local_eTowerContainerRaw);
  local_eTowerContainerRaw->clearContainerMap();
  local_eTowerContainerRaw->fillContainerMap();

  // STEP 3 - Do the supercell-tower mapping - put this information into the eTowerContainer
  ATH_CHECK( m_eSuperCellTowerMapperTool.retrieve() );
  ATH_CHECK(m_eSuperCellTowerMapperTool->AssignSuperCellsToTowers(local_eTowerContainerRaw));
  ATH_CHECK(m_eSuperCellTowerMapperTool->AssignTriggerTowerMapper(local_eTowerContainerRaw));

  // STEP 4 - Write the completed eTowerContainer into StoreGate (move the local copy in memory)
  SG::WriteHandle<LVL1::eTowerContainer> eTowerContainerSG(m_eTowerContainerSGKey/*, ctx*/);
  //std::unique_ptr<LVL1::eTowerContainer> my_eTowerContainerRaw(local_eTowerContainerRaw);
  ATH_CHECK(eTowerContainerSG.record(std::move(/*my_eTowerContainerRaw*/local_eTowerContainerRaw)));

  // STEP 5 - Set up the eFEXSysSim
  ATH_CHECK( m_eFEXSysSimTool.retrieve() );
  m_eFEXSysSimTool->init();

  // STEP 6 - Run THE eFEXSysSim
  ATH_CHECK(m_eFEXSysSimTool->execute());

  // STEP 7 - Close and clean the event  
  m_eFEXSysSimTool->cleanup();
  m_eSuperCellTowerMapperTool->reset();
  m_eTowerBuilderTool->reset();

  ATH_MSG_DEBUG("Executed " << name() << ", closing event number " << m_numberOfEvents );

  m_numberOfEvents++;

  return StatusCode::SUCCESS;
}
  
  
} // end of LVL1 namespace
