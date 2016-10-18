/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.cxx 

#include "TrigEgammaEmulationToolTest.h"
#include "GaudiKernel/Property.h"
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "PATCore/TAccept.h"
using std::string;

//**********************************************************************
namespace Trig{
TrigEgammaEmulationToolTest::
TrigEgammaEmulationToolTest(const std::string& name, 
                    ISvcLocator* pSvcLocator )
: ::AthAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this),
    m_emulationTool("Trig::TrigEgammaEmulationTool/TrigEgammaEmulationTool",this)
{
    declareProperty("TrigEgammaMatchingTool", m_matchTool);
    declareProperty("TrigEgammaEmulationTool",m_emulationTool);
    declareProperty("TriggerList",m_triggerList);
}

//**********************************************************************

TrigEgammaEmulationToolTest::~TrigEgammaEmulationToolTest() { }

//**********************************************************************

StatusCode TrigEgammaEmulationToolTest::initialize() {

  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
  }
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
      return sc;
  }

  /*
  sc = m_emulationTool->initialize();
  if(sc.isFailure()) {
      ATH_MSG_ERROR( "Emulation tool failure." );
      return sc;
  }*/

  sc = m_emulationTool->initialize();
  if(sc.isFailure()){
    ATH_MSG_ERROR("FAILURE to initialize the emulator tool!");
    return sc;
  }

  //init counters
  for(auto &trigger : m_triggerList){
    ///TDT counters
    m_counter["L1Calo_"  +trigger]      = 0;
    m_counter["L2Calo_"  +trigger]      = 0;
    m_counter["L2_"      +trigger]      = 0;
    m_counter["EFCalo_"  +trigger]      = 0;
    m_counter["EFTrack_" +trigger]      = 0;
    m_counter["HLT_"     +trigger]      = 0;
    m_counter[trigger]                  = 0;
    //emulation counters
    m_counter_emu["L1Calo_" +trigger]   = 0;
    m_counter_emu["L2Calo_" +trigger]   = 0;
    m_counter_emu["L2_"     +trigger]   = 0;
    m_counter_emu["EFCalo_" +trigger]   = 0;
    m_counter_emu["EFTrack_"+trigger]   = 0;
    m_counter_emu["HLT_"    +trigger]   = 0;
    m_counter_emu[trigger]              = 0;
    // For Event-Wise emulation
    m_counter2["L1Calo_"  +trigger]     = 0;
    m_counter2["L2Calo_"  +trigger]     = 0;
    m_counter2["L2_"      +trigger]     = 0;
    m_counter2["EFCalo_"  +trigger]     = 0;
    m_counter2["EFTrack_" +trigger]     = 0;
    m_counter2["HLT_"     +trigger]     = 0;
    m_counter2[trigger]                 = 0;
    //emulation2 counters
    m_counter_emu2["L1Calo_"  +trigger] = 0;
    m_counter_emu2["L2Calo_"  +trigger] = 0;
    m_counter_emu2["L2_"      +trigger] = 0;
    m_counter_emu2["EFCalo_"  +trigger] = 0;
    m_counter_emu2["EFTrack_" +trigger] = 0;
    m_counter_emu2["HLT_"     +trigger] = 0;
    m_counter_emu2[trigger]             = 0;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaEmulationToolTest::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  ATH_MSG_INFO("************************** Emulation summary *************************************");
  for (auto trigger : m_triggerList) {
    ATH_MSG_INFO("*******************************************************************************");
    ATH_MSG_INFO("Trigger: " << trigger);
    ATH_MSG_INFO("L1Calo  => TDT passed: " << m_counter["L1Calo_"+trigger]  << "\t emulated: " << m_counter_emu["L1Calo_"+trigger] );
    ATH_MSG_INFO("L2Calo  => TDT passed: " << m_counter["L2Calo_"+trigger]  << "\t emulated: " << m_counter_emu["L2Calo_"+trigger] );
    ATH_MSG_INFO("L2      => TDT passed: " << m_counter["l2_"    +trigger]  << "\t emulated: " << m_counter_emu["L2_"    +trigger] );
    ATH_MSG_INFO("EFCalo  => TDT passed: " << m_counter["EFCalo_"+trigger]  << "\t emulated: " << m_counter_emu["EFCalo_"+trigger] );
    ATH_MSG_INFO("EFTrack => TDT passed: " << m_counter["EFTrack_"+trigger] << "\t emulated: " << m_counter_emu["EFTrack_"+trigger] );
    ATH_MSG_INFO("HLT     => TDT passed: " << m_counter["HLT_"    +trigger] << "\t emulated: " << m_counter_emu["HLT_"    +trigger] );
    ATH_MSG_INFO("*******************************************************************************");
  }

  writeEmulationSummary();
  return StatusCode::SUCCESS;
}
//**********************************************************************
float TrigEgammaEmulationToolTest::ratio(float a, float b) {
  if (b > 0) {
    return a/b;
  }
  else {
    return -1;
  }
}

//**********************************************************************
StatusCode TrigEgammaEmulationToolTest::execute() {  
  ATH_MSG_DEBUG("================================ TrigEgammaEmulationTool::execute() ==============================");
  
  if ( (m_storeGate->retrieve(m_offElectrons,"Electrons")).isFailure() ){
    ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    return StatusCode::FAILURE;
  }
  
  StatusCode sc1 = Method1();
  if (sc1 != StatusCode::SUCCESS) {
    ATH_MSG_ERROR("Something went wrong");
    return StatusCode::FAILURE;
  }
  StatusCode sc2 = Method2();
  if (sc2 != StatusCode::SUCCESS) {
    ATH_MSG_ERROR("Something went wrong");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//**********************************************************************

StatusCode TrigEgammaEmulationToolTest::Method1() {
  
  ATH_MSG_DEBUG("================== TrigEgammaEmulationToolTest::Method1() ===================");
  for (const auto &trigger : m_triggerList) {
    ATH_MSG_DEBUG("Emulation trigger: "<< trigger);
    // TDT Counts
    //auto fc = m_trigdec->features(trigger,TrigDefs::alsoDeactivateTEs);
    //auto vec = fc.get<xAOD::ElectronContainer>();
    
    for (const auto &eg : *m_offElectrons) {
      const HLT::TriggerElement *finalFC = nullptr;
      m_matchTool->match(eg,trigger,finalFC);

      if (!finalFC) {
        ATH_MSG_DEBUG("TE is nullptr");
        continue;
      }
 
      ATH_MSG_DEBUG("good TE");
      bool passedL1         = false;
      bool passedL2Calo     = false;
      bool passedL2         = false;
      bool passedEFCalo     = false;
      bool passedEFTrack    = false;
      bool passedHLT        = false;
      
      //Decision from TDT
      ATH_MSG_DEBUG("L1Calo...");
      if( m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te() != nullptr )
        passedL1=m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te()->getActiveState();
      ATH_MSG_DEBUG("L2Calo...");
      if( m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te() != nullptr )
        passedL2Calo=m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te()->getActiveState();
      ATH_MSG_DEBUG("L2...");
      if( m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te() != nullptr )
        passedL2=m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te()->getActiveState();
      ATH_MSG_DEBUG("EFCalo...");
      if( m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC,"TrigEFCaloCalibFex").te() != nullptr )
        passedEFCalo=m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC,"TrigEFCaloCalibFex").te()->getActiveState(); 
      ATH_MSG_DEBUG("EFTrack...");
      if( m_trigdec->ancestor<xAOD::TrackParticleContainer>(finalFC,"InDetTrigTrackingxAODCnv_Electron_IDTrig").te() != nullptr )
        passedEFTrack=m_trigdec->ancestor<xAOD::TrackParticleContainer>(finalFC,"InDetTrigTrackingxAODCnv_Electron_IDTrig").te()->getActiveState();
      ATH_MSG_DEBUG("HLT...");
      if( m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te() != nullptr )
        passedHLT=m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te()->getActiveState();

      if (passedL1)         m_counter["L1Calo_"  +trigger] += 1;
      if (passedL2Calo)     m_counter["L2Calo_"  +trigger] += 1;
      if (passedL2)         m_counter["L2_"      +trigger] += 1;
      if (passedEFCalo)     m_counter["EFCalo_"  +trigger] += 1;
      if (passedEFTrack)    m_counter["EFTrack_" +trigger] += 1;
      if (passedHLT)        m_counter["HLT_"     +trigger] += 1;
      if (passedHLT)        m_counter[trigger] += 1;
      
      // Decision from Emulator  
      Root::TAccept accept = m_emulationTool->executeTool(finalFC, trigger);
      bool emuPassedL1         = accept.getCutResult("L1Calo");
      bool emuPassedL2Calo     = accept.getCutResult("L2Calo");
      bool emuPassedL2         = accept.getCutResult("L2");
      bool emuPassedEFCalo     = accept.getCutResult("EFCalo");
      bool emuPassedEFTrack    = accept.getCutResult("EFTrack");
      bool emuPassedHLT        = accept.getCutResult("HLT");

      if (emuPassedL1)         m_counter_emu["L1Calo_"  +trigger] += 1;
      if (emuPassedL2Calo)     m_counter_emu["L2Calo_"  +trigger] += 1;
      if (emuPassedL2)         m_counter_emu["L2_"      +trigger] += 1;
      if (emuPassedEFCalo)     m_counter_emu["EFCalo_"  +trigger] += 1;
      if (emuPassedEFTrack)    m_counter_emu["EFTrack_" +trigger] += 1;
      if (emuPassedHLT)        m_counter_emu["HLT_"     +trigger] += 1;
      //if (emuPassedEFCalo && emuPassedEFElectron) m_counter_emu[trigger] +=1; 
      if (emuPassedHLT)        m_counter_emu[trigger] +=1;   

      ATH_MSG_DEBUG("Trigger is: " << trigger );
      ATH_MSG_DEBUG("L1Calo     passed: " << int(passedL1)         << "\t emulated: " << int(emuPassedL1));
      ATH_MSG_DEBUG("L2Calo     passed: " << int(passedL2Calo)     << "\t emulated: " << int(emuPassedL2Calo));
      ATH_MSG_DEBUG("L2         passed: " << int(passedL2)         << "\t emulated: " << int(emuPassedL2));
      ATH_MSG_DEBUG("EFCalo     passed: " << int(passedEFCalo)     << "\t emulated: " << int(emuPassedEFCalo));
      ATH_MSG_DEBUG("EFTrack    passed: " << int(passedEFTrack)    << "\t emulated: " << int(emuPassedEFTrack));
      ATH_MSG_DEBUG("HLT        passed: " << int(passedHLT)        << "\t emulated: " << int(emuPassedHLT));
      
      
    }// loop over electrons offline
  }// loop over triggers
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationToolTest::Method2() {
  ATH_MSG_DEBUG("================== TrigEgammaEmulationToolTest::Method2() ===================");
  for (const auto &trigger : m_triggerList) {
    m_emulationTool->EventWiseContainer();
    for (const auto &eg : *m_offElectrons) {
      const HLT::TriggerElement *finalFC = nullptr;
      m_matchTool->match(eg,trigger,finalFC);
      ATH_MSG_DEBUG("new TE...");
      if (!finalFC) {
        ATH_MSG_DEBUG("TE is nullptr");
        continue;
      }
      ATH_MSG_DEBUG("good TE");
      bool passedL1         = false;
      bool passedL2Calo     = false;
      bool passedL2         = false;
      bool passedEFCalo     = false;
      bool passedEFTrack    = false;
      bool passedHLT        = false;

      //Decision from TDT
      if( m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te() != nullptr )
        passedL1=m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te() != nullptr )
        passedL2Calo=m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te() != nullptr )
        passedL2=m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC,"TrigEFCaloCalibFex").te() != nullptr )
        passedEFCalo=m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC,"TrigEFCaloCalibFex").te()->getActiveState(); 
      if( m_trigdec->ancestor<xAOD::TrackParticleContainer>(finalFC,"InDetTrigTrackingxAODCnv_Electron_IDTrig").te() != nullptr )
        passedEFTrack=m_trigdec->ancestor<xAOD::TrackParticleContainer>(finalFC,"InDetTrigTrackingxAODCnv_Electron_IDTrig").te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te() != nullptr )
        passedHLT=m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te()->getActiveState();

      if (passedL1)         m_counter2["L1Calo_"  +trigger] += 1;
      if (passedL2Calo)     m_counter2["L2Calo_"  +trigger] += 1;
      if (passedL2)         m_counter2["L2_"      +trigger] += 1;
      if (passedEFCalo)     m_counter2["EFCalo_"  +trigger] += 1;
      if (passedEFTrack)    m_counter2["EFTrack_" +trigger] += 1;
      if (passedHLT)        m_counter2["HLT_"     +trigger] += 1;
      if (passedHLT)        m_counter2[trigger] += 1;
      
      // Decision from Emulator  
      Root::TAccept accept = m_emulationTool->executeTool(trigger);
      bool emuPassedL1         = accept.getCutResult("L1Calo");
      bool emuPassedL2Calo     = accept.getCutResult("L2Calo");
      bool emuPassedL2         = accept.getCutResult("L2");
      bool emuPassedEFCalo     = accept.getCutResult("EFCalo");
      bool emuPassedEFTrack    = accept.getCutResult("EFTrack");
      bool emuPassedHLT        = accept.getCutResult("HLT");

      if (emuPassedL1)         m_counter_emu2["L1Calo_"  +trigger] += 1;
      if (emuPassedL2Calo)     m_counter_emu2["L2Calo_"  +trigger] += 1;
      if (emuPassedL2)         m_counter_emu2["L2_"      +trigger] += 1;
      if (emuPassedEFCalo)     m_counter_emu2["EFCalo_"  +trigger] += 1;
      if (emuPassedEFTrack)    m_counter_emu2["EFTrack_" +trigger] += 1;
      if (emuPassedHLT)        m_counter_emu2["HLT_"     +trigger] += 1;
      //if (emuPassedEFCalo && emuPassedEFElectron) m_counter_emu[trigger] +=1; 
      if (emuPassedHLT)        m_counter_emu2[trigger] +=1;   

      ATH_MSG_DEBUG("Trigger is: " << trigger );
      ATH_MSG_DEBUG("L1Calo     passed: " << int(passedL1)         << "\t emulated: " << int(emuPassedL1));
      ATH_MSG_DEBUG("L2Calo     passed: " << int(passedL2Calo)     << "\t emulated: " << int(emuPassedL2Calo));
      ATH_MSG_DEBUG("L2         passed: " << int(passedL2)         << "\t emulated: " << int(emuPassedL2));
      ATH_MSG_DEBUG("EFCalo     passed: " << int(passedEFCalo)     << "\t emulated: " << int(emuPassedEFCalo));
      ATH_MSG_DEBUG("EFTrack    passed: " << int(passedEFTrack)    << "\t emulated: " << int(emuPassedEFTrack));
      ATH_MSG_DEBUG("HLT        passed: " << int(passedHLT)        << "\t emulated: " << int(emuPassedHLT));
   
    
    
    }
  }
  return StatusCode::SUCCESS;
}


void TrigEgammaEmulationToolTest::writeEmulationSummary(){

  std::ofstream countsFile("TriggerCountsEmulator.txt");
  std::vector<std::string> keys = {"L1Calo_","L2Calo_","L2_","EFCalo_","EFTrack_","HLT_"};

  if (countsFile.is_open()) {
    countsFile << "************************** Emulation summary *************************************\n";
    countsFile << "              Method1 Navigation \t Method2 Event-Wise Containers \n";
    for (auto trigger : m_triggerList) {
      countsFile << "*******************************************************************************" << "\n";
      countsFile << "Trigger: " << trigger << "\n";


      for (auto key : keys){
        countsFile << std::setw(10)<<key <<" => TDT passed: " << m_counter[key+trigger] << "\t emulated: " << m_counter_emu[key+trigger]           
                 << "\t ratio " << ratio(m_counter_emu[key+trigger], m_counter[key+trigger]) << "\t TDT passed: " 
                 << m_counter2[key+trigger] << "\t emulated: " << m_counter_emu2[key+trigger] << "\t ratio " 
                 << ratio(m_counter_emu2[key+trigger],m_counter2[key+trigger]) << "\n";
      }// loop over trigger levels
      
      countsFile << "*******************************************************************************" << "\n";
    }

    countsFile << "*******************************************************************************\n";
  }
  countsFile.close();
 
}



}///namespace
//**********************************************************************
