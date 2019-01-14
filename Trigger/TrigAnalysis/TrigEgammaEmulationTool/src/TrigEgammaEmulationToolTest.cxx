/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  if(sc.isFailure()){
    ATH_MSG_ERROR("FAILURE to initialize the emulator tool!");
    return sc;
  }

  m_accept.addCut("L1Calo","");
  m_accept.addCut("L2Calo","");
  m_accept.addCut("L2","");
  m_accept.addCut("EFCalo","");
  m_accept.addCut("EFTrack","");
  m_accept.addCut("HLT","");


  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaEmulationToolTest::finalize() {
  
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
  
  for (const auto &trigger : m_triggerList) {
    
    for (const auto &eg : *m_offElectrons) {
      const HLT::TriggerElement *finalFC = nullptr;
      m_matchTool->match(eg,trigger,finalFC);

      if (!finalFC) {
        ATH_MSG_DEBUG("TE is nullptr");
        continue;
      }
      setAccept(finalFC);
      
      count("Method1__total__"+trigger);      
      if(m_accept.getCutResult("L1Calo"))   count("Method1__TDT__L1Calo__" +trigger);
      if(m_accept.getCutResult("L2Calo"))   count("Method1__TDT__L2Calo__" +trigger);
      if(m_accept.getCutResult("L2"))       count("Method1__TDT__L2__"     +trigger);
      if(m_accept.getCutResult("EFTrack"))  count("Method1__TDT__EFTrack__"+trigger);
      if(m_accept.getCutResult("EFCalo"))   count("Method1__TDT__EFCalo__" +trigger);
      if(m_accept.getCutResult("HLT"))      count("Method1__TDT__HLT__"    +trigger);

      Root::TAccept accept = m_emulationTool->executeTool(finalFC, trigger);
      if(accept.getCutResult("L1Calo"))   count("Method1__EMU__L1Calo__" +trigger);
      if(accept.getCutResult("L2Calo"))   count("Method1__EMU__L2Calo__" +trigger);
      if(accept.getCutResult("L2"))       count("Method1__EMU__L2__"     +trigger);
      if(accept.getCutResult("EFTrack"))  count("Method1__EMU__EFTrack__"+trigger);
      if(accept.getCutResult("EFCalo"))   count("Method1__EMU__EFCalo__" +trigger);
      if(accept.getCutResult("HLT"))      count("Method1__EMU__HLT__"    +trigger);
      
    }// loop over electrons offline
  }// loop over triggers
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationToolTest::Method2() {
  
  for (const auto &trigger : m_triggerList) {
    
    m_emulationTool->EventWiseContainer();
    for (const auto &eg : *m_offElectrons) {
      
      const HLT::TriggerElement *finalFC = nullptr;
      m_matchTool->match(eg,trigger,finalFC);
      
      if (!finalFC) {
        ATH_MSG_DEBUG("TE is nullptr");
        continue;
      }

      setAccept(finalFC);
      count("Method2__total__"+trigger);      
      if(m_accept.getCutResult("L1Calo"))   count("Method2__TDT__L1Calo__" +trigger);
      if(m_accept.getCutResult("L2Calo"))   count("Method2__TDT__L2Calo__" +trigger);
      if(m_accept.getCutResult("L2"))       count("Method2__TDT__L2__"     +trigger);
      if(m_accept.getCutResult("EFTrack"))  count("Method2__TDT__EFTrack__"+trigger);
      if(m_accept.getCutResult("EFCalo"))   count("Method2__TDT__EFCalo__" +trigger);
      if(m_accept.getCutResult("HLT"))      count("Method2__TDT__HLT__"    +trigger);

      Root::TAccept accept = m_emulationTool->executeTool(trigger);
      if(accept.getCutResult("L1Calo"))   count("Method2__EMU__L1Calo__" +trigger);
      if(accept.getCutResult("L2Calo"))   count("Method2__EMU__L2Calo__" +trigger);
      if(accept.getCutResult("L2"))       count("Method2__EMU__L2__"     +trigger);
      if(accept.getCutResult("EFTrack"))  count("Method2__EMU__EFTrack__"+trigger);
      if(accept.getCutResult("EFCalo"))   count("Method2__EMU__EFCalo__" +trigger);
      if(accept.getCutResult("HLT"))      count("Method2__EMU__HLT__"    +trigger);
    
    }
  }
  return StatusCode::SUCCESS;
}


void TrigEgammaEmulationToolTest::writeEmulationSummary(){

  std::ofstream countsFile("TriggerCountsEmulator.txt");
  std::vector<std::string> keys = {"L1Calo","L2Calo","L2","EFCalo","EFTrack","HLT"};

  if (countsFile.is_open()) {
    countsFile << "************************** Emulation summary *************************************\n";
    countsFile << "              Method1 Navigation \t Method2 Event-Wise Containers \n";
    for (auto trigger : m_triggerList) {
      countsFile << "*******************************************************************************" << "\n";
      countsFile << "Trigger: " << trigger << "\n";

      // TDT passed: 300/450   | emulated (method 1): 298/450 | emulated (method 2): 302/450

      for (auto key : keys){
        float  total=m_countMap["Method1__total__"+trigger];
        float  p0=m_countMap["Method1__TDT__"+key+"__"+trigger];
        float  p1=m_countMap["Method1__EMU__"+key+"__"+trigger];
        float  p2=m_countMap["Method2__EMU__"+key+"__"+trigger];

        countsFile << std::setw(10)<<key <<" | TDT passed: " << std::setw(10) << ratio(p0,total) << "(" << p0 << "/" << total << ")"
                                         <<" | emulated (method 1): " << std::setw(10) << ratio(p1,total) << "(" << p1 << "/" << total << ")"
                                         <<" | emulated (method 2): " << std::setw(10) << ratio(p2,total) << "(" << p2 << "/" << total << ")\n";
      }// loop over trigger levels
      
      countsFile << "*******************************************************************************" << "\n";
    }

    countsFile << "*******************************************************************************\n";
  }
  countsFile.close();
 
}

bool TrigEgammaEmulationToolTest::setAccept( const HLT::TriggerElement *finalFC){
  m_accept.clear();
  
  if(!finalFC)
    return false;
    
  bool passedL1         = false;
  bool passedL2Calo     = false;
  bool passedL2         = false;
  bool passedEFCalo     = false;
  bool passedEFTrack    = false;
  bool passedHLT        = false;
  bool hasRnn           = false;

  if( m_trigdec->ancestor<xAOD::TrigRNNOutput>(finalFC).te() )
    hasRnn=true;

  if( m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te() != nullptr )
    passedL1=m_trigdec->ancestor<xAOD::EmTauRoI>(finalFC).te()->getActiveState();
  
  if(hasRnn){
    if( m_trigdec->ancestor<xAOD::TrigRNNOutput>(finalFC).te() != nullptr )
      passedL2Calo=m_trigdec->ancestor<xAOD::TrigRNNOutput>(finalFC).te()->getActiveState();
  }else{
    if( m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te() != nullptr )
      passedL2Calo=m_trigdec->ancestor<xAOD::TrigEMCluster>(finalFC).te()->getActiveState();
  }
  
  
  if( m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te() != nullptr )
    passedL2=m_trigdec->ancestor<xAOD::TrigElectronContainer>(finalFC).te()->getActiveState();
  
  if( m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC,"TrigEFCaloCalibFex").te() != nullptr )
    passedEFCalo=m_trigdec->ancestor<xAOD::CaloClusterContainer>(finalFC,"TrigEFCaloCalibFex").te()->getActiveState(); 
  
  if( m_trigdec->ancestor<xAOD::TrackParticleContainer>(finalFC,"InDetTrigTrackingxAODCnv_Electron_IDTrig").te() != nullptr )
    passedEFTrack=m_trigdec->ancestor<xAOD::TrackParticleContainer>(finalFC,"InDetTrigTrackingxAODCnv_Electron_IDTrig").te()->getActiveState();
  
  if( m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te() != nullptr )
    passedHLT=m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC).te()->getActiveState();

  m_accept.setCutResult("L1Calo", passedL1);
  m_accept.setCutResult("L2Calo", passedL2Calo);
  m_accept.setCutResult("L2", passedL2);
  m_accept.setCutResult("EFTrack", passedEFTrack);
  m_accept.setCutResult("EFCalo", passedEFCalo);
  m_accept.setCutResult("HLT", passedHLT);

  return true;
}

}///namespace
//**********************************************************************
