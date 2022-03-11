/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "L1CaloFEXSim/jFEXLargeRJetTOB.h"
#include "L1CaloFEXSim/jFEXtauTOB.h"


#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TPad.h"
#include "TFile.h"
#include "TCanvas.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

#include "L1CaloFEXSim/jTowerContainer.h"

#include "xAODTrigger/jFexSRJetRoI.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h" 

#include "xAODTrigger/jFexLRJetRoI.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"

#include "xAODTrigger/jFexTauRoI.h"
#include "xAODTrigger/jFexTauRoIContainer.h" 

#include <cassert>
#include "SGTools/TestStore.h"
#include "PathResolver/PathResolver.h"
#include <ctime>

#define DEBUG_VHB 1


namespace LVL1 {

jFEXDriver::jFEXDriver(const std::string& name, ISvcLocator* pSvcLocator):  AthAlgorithm(name, pSvcLocator){
    declareProperty("PileupWeigthFile", m_PileupWeigthFile = "Run3L1CaloSimulation/Noise/jTowerCorrection.20210308.r12406.root");
    declareProperty("PileupHelperFile", m_PileupHelperFile = "Run3L1CaloSimulation/Calibrations/jFEX_MatchedMapping.2022Mar10.r12406.root");
}


 jFEXDriver::~jFEXDriver()
{
  ATH_MSG_DEBUG("Destroying " << name() << "...");
}


StatusCode jFEXDriver::initialize()
{

  m_numberOfEvents = 1;

  ATH_CHECK( m_jTowerBuilderTool.retrieve() );
  ATH_CHECK( m_jSuperCellTowerMapperTool.retrieve() );
  ATH_CHECK( m_jFEXSysSimTool.retrieve() );
  ATH_CHECK( m_jTowerContainerSGKey.initialize() );
  ATH_CHECK( m_jFexSRJetEDMKey.initialize() );
  ATH_CHECK( m_jFexLRJetEDMKey.initialize() );
  ATH_CHECK( m_jFexTauEDMKey.initialize() );
  ATH_CHECK( m_jFEXOutputCollectionSGKey.initialize() );
  
    std::unique_ptr<TFile> jTowerFile(TFile::Open(PathResolver::find_calib_file(m_PileupWeigthFile).c_str()));
    std::unique_ptr<TFile> jTowerMapFile(TFile::Open(PathResolver::find_calib_file(m_PileupHelperFile).c_str()));
    if (!jTowerFile || jTowerFile->IsZombie()) {
        ATH_MSG_ERROR("Failed to open cell timing file " << m_PileupWeigthFile);
        return StatusCode::FAILURE;
    }
    if (!jTowerMapFile || jTowerMapFile->IsZombie()) {
        ATH_MSG_ERROR("Failed to open cell timing file " << m_PileupHelperFile);
        return StatusCode::FAILURE;
    }    
    
    m_jTowerArea_hist    = (TH1F*) jTowerFile->Get("jTowerArea_final_hist");
    m_Firmware2BitwiseID = (TH1I*) jTowerMapFile->Get("Firmware2BitwiseID");
    m_BinLayer           = (TH1I*) jTowerMapFile->Get("BinLayer");
    m_EtaCoords          = (TH1F*) jTowerMapFile->Get("EtaCoords");
    m_PhiCoords          = (TH1F*) jTowerMapFile->Get("PhiCoords");
    
    //detach the Histograms from the TFiles
    m_jTowerArea_hist->SetDirectory(0);
    m_Firmware2BitwiseID->SetDirectory(0);
    m_BinLayer->SetDirectory(0);
    m_EtaCoords->SetDirectory(0);
    m_PhiCoords->SetDirectory(0);
    
    jTowerFile->Close();
    jTowerMapFile->Close();  
  

  return StatusCode::SUCCESS;

}


StatusCode jFEXDriver::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


StatusCode jFEXDriver::execute() {
    ATH_MSG_DEBUG("Executing " << name() << ", processing event number " << m_numberOfEvents );

    // OLD DIMA STUFF---------------------- Maybe useful in the future again
    //if (std::fabsf((*cell)->eta()) > 2.55) continue;
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
    my_jFEXOutputCollection->setdooutput(true);

    // STEP 2 - Make some jTowers and fill the local container
    m_jTowerBuilderTool->init(local_jTowerContainerRaw);
    local_jTowerContainerRaw->clearContainerMap();
    local_jTowerContainerRaw->fillContainerMap();
    
    // STEP 3 - Do the supercell-tower mapping - put this information into the jTowerContainer
    ATH_CHECK(m_jSuperCellTowerMapperTool->AssignSuperCellsToTowers(local_jTowerContainerRaw));
    ATH_CHECK(m_jSuperCellTowerMapperTool->AssignTriggerTowerMapper(local_jTowerContainerRaw));
    ATH_CHECK(m_jSuperCellTowerMapperTool->AssignPileupAndNoiseValues(local_jTowerContainerRaw,m_jTowerArea_hist,m_Firmware2BitwiseID,m_BinLayer,m_EtaCoords,m_PhiCoords));

    // STEP 4 - Write the completed jTowerContainer into StoreGate (move the local copy in memory)
    SG::WriteHandle<LVL1::jTowerContainer> jTowerContainerSG(m_jTowerContainerSGKey/*, ctx*/);
    ATH_CHECK(jTowerContainerSG.record(std::move(/*my_jTowerContainerRaw*/local_jTowerContainerRaw)));

    // STEP 5 - Set up the jFEXSysSim
    m_jFEXSysSimTool->init();


    // STEP 6 - Run THE jFEXSysSim
    ATH_CHECK(m_jFEXSysSimTool->execute(my_jFEXOutputCollection));
    
    //STEP 6.5- test the EDMs
    ATH_CHECK(testSRJetEDM());
    ATH_CHECK(testLRJetEDM());
    ATH_CHECK(testTauEDM());

    // STEP 7 - Close and clean the event  
    m_jFEXSysSimTool->cleanup();
    m_jSuperCellTowerMapperTool->reset();
    m_jTowerBuilderTool->reset();

    // STEP 8 - Write the completed jFEXOutputCollection into StoreGate (move the local copy in memory)
    std::unique_ptr<jFEXOutputCollection> local_jFEXOutputCollection = std::unique_ptr<jFEXOutputCollection>(my_jFEXOutputCollection);
    SG::WriteHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionSG(m_jFEXOutputCollectionSGKey);
    ATH_CHECK(jFEXOutputCollectionSG.record(std::move(local_jFEXOutputCollection)));

    ATH_MSG_DEBUG("Executed " << name() << ", closing event number " << m_numberOfEvents );

    m_numberOfEvents++;

    return StatusCode::SUCCESS;
}

StatusCode jFEXDriver::testSRJetEDM(){
  const xAOD::jFexSRJetRoI* myRoI = 0;
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> myRoIContainer(m_jFexSRJetEDMKey);
    if(!myRoIContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve EDM Container " << m_jFexSRJetEDMKey.key());
      return StatusCode::FAILURE;
   }

    ATH_MSG_DEBUG("----got container: " << myRoIContainer.key());

    for(const auto& it : * myRoIContainer){
      myRoI = it;
      ATH_MSG_DEBUG("SR Jet EDM jFex Number: "
            << +myRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number
            << " et: "
            << myRoI->et() // returns the et value of the EM cluster in MeV
            << " eta: "
            << myRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
            << " phi: "
            << myRoI->phi() // returns a floating point global phi
            );
    }

    return StatusCode::SUCCESS;
}  

StatusCode jFEXDriver::testLRJetEDM(){

  const xAOD::jFexLRJetRoI* myRoI = 0;
  SG::ReadHandle<xAOD::jFexLRJetRoIContainer> myRoIContainer(m_jFexLRJetEDMKey);
    if(!myRoIContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve EDM Container " << m_jFexLRJetEDMKey.key());
      return StatusCode::FAILURE;
   }

    ATH_MSG_DEBUG("----got container: " << myRoIContainer.key());

    for(const auto& it : * myRoIContainer){
      myRoI = it;
      ATH_MSG_DEBUG("LR Jet EDM jFex Number: "
            << +myRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number
            << " et: "
            << myRoI->et() // returns the et value of the EM cluster in MeV
            << " eta: "
            << myRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
            << " phi: "
            << myRoI->phi() // returns a floating point global phi
            );
    }

    return StatusCode::SUCCESS;
}

StatusCode jFEXDriver::testTauEDM(){

  const xAOD::jFexTauRoI* myRoI = 0;
  SG::ReadHandle<xAOD::jFexTauRoIContainer> myRoIContainer(m_jFexTauEDMKey);
    if(!myRoIContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve EDM Container " << m_jFexTauEDMKey.key());
      return StatusCode::FAILURE;
   }

    ATH_MSG_DEBUG("----got container: " << myRoIContainer.key());

    for(const auto& it : * myRoIContainer){
      myRoI = it;
      ATH_MSG_DEBUG("EDM jFex Number: "
            << +myRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number
            << " et: "
            << myRoI->et() // returns the et value of the EM cluster in MeV
            << " eta: "
            << myRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
            << " phi: "
            << myRoI->phi() // returns a floating point global phi
            );
    }

    return StatusCode::SUCCESS;
}

} // end of LVL1 namespace
