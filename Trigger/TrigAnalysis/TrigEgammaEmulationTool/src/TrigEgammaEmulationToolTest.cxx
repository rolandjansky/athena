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
  //init counters
  for(auto &trigger : m_triggerList){
    ///TDT counters
    m_counter["l1calo_"+trigger]          = 0;
    m_counter["l2calo_"+trigger]          = 0;
    m_counter["l2_"+trigger]              = 0;
    m_counter["efcalo_"+trigger]          = 0;
    m_counter["ef_"+trigger]              = 0;
    m_counter[trigger]                    = 0;
    //emulation counters
    m_counter_emu["l1calo_"+trigger]      = 0;
    m_counter_emu["l2calo_"+trigger]      = 0;
    m_counter_emu["l2_"+trigger]          = 0;
    m_counter_emu["efcalo_"+trigger]      = 0;
    m_counter_emu["ef_"+trigger]          = 0;
    m_counter_emu[trigger]                = 0;
    // For Event-Wise emulation
    m_counter2["l1calo_"+trigger]          = 0;
    m_counter2["l2calo_"+trigger]          = 0;
    m_counter2["l2_"+trigger]              = 0;
    m_counter2["efcalo_"+trigger]          = 0;
    m_counter2["ef_"+trigger]              = 0;
    m_counter2[trigger]                    = 0;
    //emulation2 counters
    m_counter_emu2["l1calo_"+trigger]      = 0;
    m_counter_emu2["l2calo_"+trigger]      = 0;
    m_counter_emu2["l2_"+trigger]          = 0;
    m_counter_emu2["efcalo_"+trigger]      = 0;
    m_counter_emu2["ef_"+trigger]          = 0;
    m_counter_emu2[trigger]                = 0;
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
    ATH_MSG_INFO("L1Calo => TDT passed: " << m_counter["l1calo_"+trigger] << "\t emulated: " << m_counter_emu["l1calo_"+trigger] );
    ATH_MSG_INFO("L2Calo => TDT passed: " << m_counter["l2calo_"+trigger] << "\t emulated: " << m_counter_emu["l2calo_"+trigger] );
    ATH_MSG_INFO("L2     => TDT passed: " << m_counter["l2_"    +trigger] << "\t emulated: " << m_counter_emu["l2_"    +trigger] );
    ATH_MSG_INFO("EFCalo => TDT passed: " << m_counter["efcalo_"+trigger] << "\t emulated: " << m_counter_emu["efcalo_"+trigger] );
    ATH_MSG_INFO("EF     => TDT passed: " << m_counter["ef_"    +trigger] << "\t emulated: " << m_counter_emu["ef_"    +trigger] );
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
  ATH_MSG_DEBUG("TrigEgammaEmulationTool::execute()");
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
  ATH_MSG_DEBUG("TrigEgammaEmulationToolTest::Method1()");
  for (const auto &trigger : m_triggerList) {
    ATH_MSG_DEBUG("Emulation trigger: "<< trigger);
    const HLT::TriggerElement *finalFC = nullptr;
    // TDT Counts
    auto fc = m_trigdec->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs);
    auto vec = fc.get<xAOD::ElectronContainer>();
    for (const auto &feat : vec) {
      ATH_MSG_DEBUG("new TE...");
      finalFC = feat.te();
      if (finalFC != nullptr) { 
        ATH_MSG_DEBUG("good TE");
        bool passedL1         = false;
        bool passedL2Calo     = false;
        bool passedL2Electron = false;
        bool passedEFCalo     = false;
        bool passedEFElectron = false;
        //Decision from TDT
        if( m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te() != nullptr )
          passedL1=m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te()->getActiveState();
        if( m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te() != nullptr )
          passedL2Calo=m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te()->getActiveState();
        if( m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te() != nullptr )
          passedL2Electron=m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te()->getActiveState();
        if( m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC).te() != nullptr )
          passedEFCalo=m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC).te()->getActiveState();
        if( m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te() != nullptr )
          passedEFElectron=m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te()->getActiveState();
        if (passedL1)         m_counter["l1calo_"+trigger]       += 1;
        if (passedL2Calo)     m_counter["l2calo_"+trigger]       += 1;
        if (passedL2Electron) m_counter["l2_"+trigger]           += 1;
        if (passedEFCalo)     m_counter["efcalo_"+trigger]       += 1;
        if (passedEFElectron) m_counter["ef_"+trigger]           += 1;
        if (passedEFCalo && passedEFElectron) m_counter[trigger] += 1;
        // Deceision from Emulator  
        Root::TAccept accept = m_emulationTool->executeTool(finalFC, "HLT_"+trigger);
        bool emuPassedL1         = accept.getCutResult("EmTauRoI");
        bool emuPassedL2Calo     = accept.getCutResult("TrigEMCluster");
        bool emuPassedL2Electron = accept.getCutResult("TrigElectronContainer");
        bool emuPassedEFCalo     = accept.getCutResult("CaloClusterContainer");
        bool emuPassedEFElectron = accept.getCutResult("ElectronContainer");

        if (emuPassedL1)         m_counter_emu["l1calo_"+trigger] += 1;
        if (emuPassedL2Calo)     m_counter_emu["l2calo_"+trigger] += 1;
        if (emuPassedL2Electron) m_counter_emu["l2_"+trigger]     += 1;
        if (emuPassedEFCalo)     m_counter_emu["efcalo_"+trigger] += 1;
        if (emuPassedEFElectron) m_counter_emu["ef_"+trigger]     += 1;
        if (emuPassedEFCalo && emuPassedEFElectron) m_counter_emu[trigger] +=1; 

        ATH_MSG_DEBUG("Trigger is: " << trigger );
        ATH_MSG_DEBUG("L1Calo     passed: " << int(passedL1)         << "\t emulated: " << int(emuPassedL1));
        ATH_MSG_DEBUG("L2Calo     passed: " << int(passedL2Calo)     << "\t emulated: " << int(emuPassedL2Calo));
        ATH_MSG_DEBUG("L2Electron passed: " << int(passedL2Electron) << "\t emulated: " << int(emuPassedL2Electron));
        ATH_MSG_DEBUG("EFCalo     passed: " << int(passedEFCalo)     << "\t emulated: " << int(emuPassedEFCalo));
        ATH_MSG_DEBUG("EFElectron passed: " << int(passedEFElectron) << "\t emulated: " << int(emuPassedEFElectron));
      }
      else {
        ATH_MSG_DEBUG("Feature Container is nullptr");
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationToolTest::Method2() {
  ATH_MSG_DEBUG("TrigEgammaEmulationToolTest::Method2()");
  const xAOD::ElectronContainer *offElectrons = 0;
  if ( (m_storeGate->retrieve(offElectrons,"Electrons")).isFailure() ){
    ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
  }
  for (const auto &trigger : m_triggerList) {
    m_emulationTool->EventWiseContainer();
    for (const auto &eg : *offElectrons) {
      const HLT::TriggerElement *finalFC = nullptr;
      m_matchTool->match(eg,"HLT_"+trigger,finalFC);
      ATH_MSG_DEBUG("new TE...");
      if (!finalFC) {
        ATH_MSG_DEBUG("TE is nullptr");
        continue;
      }
      ATH_MSG_DEBUG("good TE");
      bool passedL1         = false;
      bool passedL2Calo     = false;
      bool passedL2Electron = false;
      bool passedEFCalo     = false;
      bool passedEFElectron = false;
      //Decision from TDT
      if( m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te() != nullptr )
        passedL1=m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te() != nullptr )
        passedL2Calo=m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te() != nullptr )
        passedL2Electron=m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC).te() != nullptr )
        passedEFCalo=m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC).te()->getActiveState();
      if( m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te() != nullptr )
        passedEFElectron=m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te()->getActiveState();

      if (passedL1)         m_counter2["l1calo_"+trigger]       += 1;
      if (passedL2Calo)     m_counter2["l2calo_"+trigger]       += 1;
      if (passedL2Electron) m_counter2["l2_"+trigger]           += 1;
      if (passedEFCalo)     m_counter2["efcalo_"+trigger]       += 1;
      if (passedEFElectron) m_counter2["ef_"+trigger]           += 1;
      if (passedEFCalo && passedEFElectron) m_counter2[trigger] += 1;
      // Deceision from Emulator  
      Root::TAccept accept = m_emulationTool->executeTool(finalFC, "HLT_"+trigger);
      bool emuPassedL1         = accept.getCutResult("EmTauRoI");
      bool emuPassedL2Calo     = accept.getCutResult("TrigEMCluster");
      bool emuPassedL2Electron = accept.getCutResult("TrigElectronContainer");
      bool emuPassedEFCalo     = accept.getCutResult("CaloClusterContainer");
      bool emuPassedEFElectron = accept.getCutResult("ElectronContainer");

      if (emuPassedL1)         m_counter_emu2["l1calo_"+trigger] += 1;
      if (emuPassedL2Calo)     m_counter_emu2["l2calo_"+trigger] += 1;
      if (emuPassedL2Electron) m_counter_emu2["l2_"+trigger]     += 1;
      if (emuPassedEFCalo)     m_counter_emu2["efcalo_"+trigger] += 1;
      if (emuPassedEFElectron) m_counter_emu2["ef_"+trigger]     += 1;
      if (emuPassedEFCalo && emuPassedEFElectron) m_counter_emu2[trigger] +=1;   

      ATH_MSG_DEBUG("Trigger is: " << trigger );
      ATH_MSG_DEBUG("L1Calo     passed: " << int(passedL1)         << "\t emulated: " << int(emuPassedL1));
      ATH_MSG_DEBUG("L2Calo     passed: " << int(passedL2Calo)     << "\t emulated: " << int(emuPassedL2Calo));
      ATH_MSG_DEBUG("L2Electron passed: " << int(passedL2Electron) << "\t emulated: " << int(emuPassedL2Electron));
      ATH_MSG_DEBUG("EFCalo     passed: " << int(passedEFCalo)     << "\t emulated: " << int(emuPassedEFCalo));
      ATH_MSG_DEBUG("EFElectron passed: " << int(passedEFElectron) << "\t emulated: " << int(emuPassedEFElectron));
    }
  }
  return StatusCode::SUCCESS;
}


void TrigEgammaEmulationToolTest::writeEmulationSummary(){

  std::ofstream countsFile("TriggerCountsEmulator.txt");

  if (countsFile.is_open()) {
    countsFile << "************************** Emulation summary *************************************\n";
    for (auto trigger : m_triggerList) {
      countsFile << "*******************************************************************************" << "\n";
      countsFile << "Trigger: " << trigger << "\n";
      countsFile << "              Method1 Navigation \t Method2 Event-Wise Containers \n";
      countsFile << "L1Calo     => TDT passed: " << m_counter["l1calo_"+trigger] << "\t emulated: " << m_counter_emu["l1calo_"+trigger] 
                 << "\t ratio " << ratio(m_counter_emu["l1calo_"+trigger],m_counter["l1calo_"+trigger]) << "\t TDT passed: " 
                 << m_counter2["l1calo_"+trigger] << "\t emulated: " << m_counter_emu2["l1calo_"+trigger] << "\t ratio " 
                 << ratio(m_counter_emu2["l1calo_"+trigger],m_counter2["l1calo_"+trigger]) << "\n";

      countsFile << "L2Calo     => TDT passed: " << m_counter["l2calo_"+trigger] << "\t emulated: " << m_counter_emu["l2calo_"+trigger] 
                 << "\t ratio " << ratio(m_counter_emu["l2calo_"+trigger],m_counter["l2calo_"+trigger]) << "\t TDT passed: " 
                 << m_counter2["l2calo_"+trigger] << "\t emulated: " << m_counter_emu2["l2calo_"+trigger] << "\t ratio " 
                 << ratio(m_counter_emu2["l2calo_"+trigger],m_counter2["l2calo_"+trigger]) << "\n";

      countsFile << "L2         => TDT passed: " << m_counter["l2_"    +trigger] << "\t emulated: " << m_counter_emu["l2_"    +trigger] 
                 << "\t ratio " << ratio(m_counter_emu["l2_"    +trigger],m_counter["l2_"    +trigger]) << "\t TDT passed: " 
                 << m_counter2["l2_"    +trigger] << "\t emulated: " << m_counter_emu2["l2_"    +trigger] << "\t ratio " 
                 << ratio(m_counter_emu2["l2_"    +trigger],m_counter2["l2_"    +trigger]) << "\n";

      countsFile << "EFCalo     => TDT passed: " << m_counter["efcalo_"+trigger] << "\t emulated: " << m_counter_emu["efcalo_"+trigger] 
                 << "\t ratio " << ratio(m_counter_emu["efcalo_"+trigger],m_counter["efcalo_"+trigger]) << "\t TDT passed: " 
                 << m_counter2["efcalo_"+trigger] << "\t emulated: " << m_counter_emu2["efcalo_"+trigger] << "\t ratio " 
                 << ratio(m_counter_emu2["efcalo_"+trigger],m_counter2["efcalo_"+trigger]) << "\n";

      countsFile << "EF         => TDT passed: " << m_counter["ef_"    +trigger] << "\t emulated: " << m_counter_emu["ef_"    +trigger] 
                 << "\t ratio " << ratio(m_counter_emu["ef_"    +trigger],m_counter["ef_"    +trigger]) << "\t TDT passed: " 
                 << m_counter2["ef_"    +trigger] << "\t emulated: " << m_counter_emu2["ef_"    +trigger] << "\t ratio " 
                 << ratio(m_counter_emu2["ef_"    +trigger],m_counter2["ef_"    +trigger]) << "\n";

      countsFile << "EFCalo&&EF => TDT passed: " << m_counter[trigger]           << "\t emulated: " << m_counter_emu[trigger]           
                 << "\t ratio " << ratio(m_counter_emu[trigger],m_counter[trigger])                     << "\t TDT passed: " 
                 << m_counter2[trigger]           << "\t emulated: " << m_counter_emu2[trigger]           << "\t ratio " 
                 << ratio(m_counter_emu2[trigger],m_counter2[trigger])                     << "\n";
      countsFile << "*******************************************************************************" << "\n";
      }

      countsFile << "*******************************************************************************\n";
  }
  countsFile.close();
 
}



}///namespace
//**********************************************************************
