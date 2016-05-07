/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*    @file HLTTauMonTool_emulation.cxx
 *
 *    
 *    L1 and HLT tau trigger emulation 
 *    created by dzanzi and mpickeri
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "AnalysisUtils/AnalysisMisc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>
#include "xAODEventInfo/EventInfo.h"

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/ChainGroup.h"
//#include "TrigSteeringEvent/TrigOperationalInfo.h"
//#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

//#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "VxVertex/VxContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TEfficiency.h"

#include <vector>
#include <iostream>
#include <fstream>
//#define _USE_MATH_DEFINES
#include <math.h>


#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigConfHLTData/HLTChain.h"
#include "HLTTauMonTool.h"

using namespace std;

std::string HLTTauMonTool::LowerChain(std::string hlt_item){

  ATH_MSG_DEBUG("HLTTauMonTool::LowerChain()");
  
  std::string l1_chain("");

  TrigConf::HLTChain* hlt_chain;
  if( (hlt_chain = m_configsvc->chainList()->chain(hlt_item)) ){ 
    ATH_MSG_DEBUG("HLT Chain " << hlt_item << " found in TrigConfSvc!");
    std::string lower_chain( hlt_chain->lower_chain_name() );
    ATH_MSG_DEBUG("L1 Chain " << lower_chain << " found in TrigConfSvc!");
    l1_chain = lower_chain;
  }

  return l1_chain;
}


StatusCode HLTTauMonTool::Emulation(){

  ATH_MSG_DEBUG("HLTTauMonTool::Emulation starting");
 
  const xAOD::EmTauRoIContainer* l1taus = 0;
  if ( m_storeGate->retrieve( l1taus, "LVL1EmTauRoIs").isFailure() ){ 
    ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
  }   

  const xAOD::JetRoIContainer *l1jets = 0;
  if ( m_storeGate->retrieve( l1jets, "LVL1JetRoIs").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve LVL1JetRoIs container. Exiting.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_DEBUG("found LVL1JetRoIs in SG");
  }

  const xAOD::MuonRoIContainer *l1muons = 0;
  if ( m_storeGate->retrieve( l1muons, "LVL1MuonRoIs").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve LVL1MuonRoIs container. Exiting.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_DEBUG("found LVL1MuonRoIs in SG");
  }

  const xAOD::EnergySumRoI *l1xe = 0;
  if ( m_storeGate->retrieve( l1xe, "LVL1EnergySumRoI").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve LVL1EnergySumRoI container. Exiting.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_DEBUG("found LVL1EnergySumRoI in SG");
  }

  // retrieve HLT containers
  const xAOD::TauJetContainer * tauPreselCont = 0;
  if( evtStore()->retrieve(tauPreselCont, "HLT_xAOD__TauJetContainer_TrigTauRecPreselection").isFailure() ){
	ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TauJetContainer_TrigTauRecPreselection container. Exiting!");
	return StatusCode::FAILURE;
  }

  const xAOD::TauJetContainer * tauHLTCont = 0;
  if( evtStore()->retrieve(tauHLTCont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting!");
        return StatusCode::FAILURE;
  }


 ATH_CHECK(m_l1emulationTool->calculate(l1taus, l1jets, l1muons, l1xe));
//  //m_l1emulationTool->PrintCounters();
//
 // Print the decision for all the tested chains and the TDT decision

 for (auto it: m_emulation_l1_tau) {  
  //if(m_bootstrap && !getTDT()->isPassed("L1_TAU12")) continue;
  bool emulation_decision = m_l1emulationTool->decision(it);
  ATH_MSG_DEBUG(it << " emulation : " << emulation_decision);
  setCurrentMonGroup("HLT/TauMon/Expert/Emulation");
  if(emulation_decision) hist("hL1EmulationPassEmul")->Fill(it.c_str(),1.);
  auto chain_group = getTDT()->getChainGroup(it);  
  bool cg_passes_event = chain_group->isPassed();
  //if(getTDT()->isPassedBits(it) & TrigDefs::L1_isPassedBeforePrescale) cg_passes_event = true; 
  ATH_MSG_DEBUG(it << " TDT : " <<  cg_passes_event);
  setCurrentMonGroup("HLT/TauMon/Expert/Emulation");
  if(cg_passes_event) hist("hL1EmulationPassTDT")->Fill(it.c_str(),1.);
  //int L1_PSCut = (int) getTDT()->getPrescale(it);
  //float L1_PS = TrigConf::PrescaleSet::getPrescaleFromCut(L1_PSCut);
  if (emulation_decision != cg_passes_event){
    ATH_MSG_DEBUG("TDT and emulation decision different, TDT gives : " 
		 << cg_passes_event 
		 << " emulation gives : " 
		 << emulation_decision);
    setCurrentMonGroup("HLT/TauMon/Expert/Emulation");
    hist("hL1Emulation")->Fill(it.c_str(),1.);
  }
 }
 // std::string l1_chain(LowerChain("HLT_"+trigItem));

 // if(level=="L1"){
 //   ATH_MSG_DEBUG("Emulating " << l1_chain << " at " << level );

 // 


 // } else if(level=="HLT"){
 //   ATH_MSG_DEBUG("Emulating " << trigItem << " at " << level );

 // } else {
 //   ATH_MSG_WARNING("Level of emulation not valid, exiting!"); return StatusCode::FAILURE;
 // }  

 // clear the decoration at the end of the run
 l1taus->clearDecorations();
 l1jets->clearDecorations();
 l1muons->clearDecorations();
 l1xe->clearDecorations();
 tauHLTCont->clearDecorations();
 return StatusCode::SUCCESS;

}



