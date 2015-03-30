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
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"


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


StatusCode HLTTauMonTool::Emulation(const std::string & trigItem, const std::string & level){

  ATH_MSG_DEBUG("HLTTauMonTool::Emulation, " << trigItem << " at " << level );
  
  std::string l1_chain(LowerChain("HLT_"+trigItem));

  if(level=="L1"){
    ATH_MSG_DEBUG("Emulating " << l1_chain << " at " << level );

  } else if(level=="HLT"){
    ATH_MSG_DEBUG("Emulating " << trigItem << " at " << level );

  } else {
    ATH_MSG_WARNING("Level of emulation not valid, exiting!"); return StatusCode::FAILURE;
  }  

  return StatusCode::SUCCESS;

}


