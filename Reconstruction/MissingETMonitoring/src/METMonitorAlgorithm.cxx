/* 
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MissingETMonitoring/METMonitorAlgorithm.h" 
#include "xAODMissingET/MissingET.h" 
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODJet/Jet.h" 


static const std::map<std::string, std::pair<std::string, std::string> > key2SubSkeyMap( {
    { "MET_RefFinal", { "MET_Reference_AntiKt4EMTopo", "FinalTrk"  } },// add track   	
    { "MET_RefEle", { "MET_Reference_AntiKt4EMTopo", "RefEle" } },   
    { "MET_RefGamma", { "MET_Reference_AntiKt4EMTopo", "RefGamma" } } ,   
    { "MET_RefTau", { "MET_Reference_AntiKt4EMTopo", "RefTau" } },     
    { "MET_SoftClus", { "MET_Reference_AntiKt4EMTopo", "SoftClus" } },    
    { "MET_SoftTrk", { "MET_Reference_AntiKt4EMTopo", "FinalTrk" } },   
    { "MET_PVSoftTrk", { "MET_Reference_AntiKt4EMTopo", "PVSoftTrk" } },   
    { "MET_RefJet_JVFCut", { "MET_Reference_AntiKt4EMTopo", "RefJet" } }, // RefJet_JVFCut   
    { "MET_RefJet", { "MET_Reference_AntiKt4EMTopo", "RefJet" } },    
    { "MET_Muon", { "MET_Reference_AntiKt4EMTopo", "Muons" } },    
    { "MET_PFlow", { "MET_Reference_AntiKt4EMPFlow", "FinalTrk" } }, // add track 
    { "MET_PFlow_RefEle", { "MET_Reference_AntiKt4EMPFlow", "RefEle" } },  
    { "MET_PFlow_RefGamma", { "MET_Reference_AntiKt4EMPFlow", "RefGamma" } },    
    { "MET_PFlow_RefTau", { "MET_Reference_AntiKt4EMPFlow", "RefTau" } },   
    { "MET_PFlow_SoftClus", { "MET_Reference_AntiKt4EMPFlow", "SoftClus" } },  
    { "MET_PFlow_SoftTrk", { "MET_Reference_AntiKt4EMPFlow", "FinalTrk" } },
    { "MET_PFlow_PVSoftTrk", { "MET_Reference_AntiKt4EMPFlow", "PVSoftTrk" } },   
    { "MET_PFlow_RefJet_JVFCut", { "MET_Reference_AntiKt4EMPFlow", "RefJet" } }, // RefJet_JVFCut
    { "MET_PFlow_RefJet", { "MET_Reference_AntiKt4EMPFlow", "RefJet" } },     
    { "MET_PFlow_Muon", { "MET_Reference_AntiKt4EMPFlow", "Muons" } },    
    { "MET_LocalHadTopo", { "MET_LocHadTopo", "LocHadTopo" } },   
    { "MET_LocHadTopo", { "MET_LocHadTopo", "LocHadTopo" } }, // same as above  
    { "MET_Topo", { "MET_EMTopo", "EMTopo" } }, 
    { "MET_Track", { "MET_Track", "Track" } },   
    { "MET_PFlow", { "MET_Reference_AntiKt4EMPFlow", "FinalClus" } }     } );


METMonitoringAlg::METMonitoringAlg( const std::string& name, ISvcLocator* pSvcLocator ) :AthMonitorAlgorithm(name,pSvcLocator) {   

} 


METMonitoringAlg::~METMonitoringAlg() {} 

StatusCode METMonitoringAlg::initialize() {
  for (const auto& key : m_metKeys) {
    auto foundIterator = key2SubSkeyMap.find( key );
    if ( foundIterator == key2SubSkeyMap.end() ) {
      ATH_MSG_FATAL("Unable to find MET key " << key << " in lookup table; quit");
      return StatusCode::FAILURE;
    }
    if (foundIterator->second.first != m_metContainerKey.key()) {
      ATH_MSG_FATAL("Mismatch of requested MET key " << key << ", which comes from "
		    << foundIterator->second.first
		    << ", with configured container key " << m_metContainerKey
		    << "; quit");
      return StatusCode::FAILURE;
    }
  }
  ATH_CHECK(m_metContainerKey.initialize());
  ATH_CHECK(m_metCaloContainerKey.initialize());
  ATH_CHECK(m_metAKt4EMTopoContainerKey.initialize());

  ATH_CHECK(m_jetContainerKey.initialize());
  if (m_doJetCleaning) {    
    ATH_MSG_DEBUG("::initializing JetCleaningTool"); 
    ATH_CHECK(m_jetCleaningTool.retrieve());  
    ATH_MSG_DEBUG("::initialized JetCleaningTool");  
  } else {     
    
    m_jetCleaningTool.disable();     
  } 
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode METMonitoringAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  using Gaudi::Units::GeV;
  
  SG::ReadHandle<xAOD::MissingETContainer> metContainer(m_metContainerKey,ctx);
  if (!metContainer.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain MET Collection with name "<< m_metContainerKey);
    return StatusCode::FAILURE;
  }
  SG::ReadHandle<xAOD::MissingETContainer> metCaloContainer(m_metCaloContainerKey,ctx);
  if (!metCaloContainer.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain METCalo Collection with name "<< m_metCaloContainerKey);
    return StatusCode::FAILURE;
  }
  
  SG::ReadHandle<xAOD::MissingETContainer> metAKt4EMTopoContainer(m_metAKt4EMTopoContainerKey,ctx);
  if (!metCaloContainer.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain METAKt4EMTopo Collection with name "<< m_metAKt4EMTopoContainerKey);
    return StatusCode::FAILURE;
  }
  const auto* trigDecTool = (getTrigDecisionTool().empty() ?
                             nullptr : getTrigDecisionTool().operator->());
  bool isMETtopocalo = false;
  for (const auto& key : m_metKeys) {
    if (key == "MET_Topo") isMETtopocalo = true;    
  }
  if (isMETtopocalo){
    if (m_dometcut && (*metAKt4EMTopoContainer)[m_metTotalKey]->met() < m_metcut*GeV) return StatusCode::SUCCESS;
    
  }  else {
    if (m_dometcut && (*metContainer)[m_metTotalKey]->met() < m_metcut*GeV) return StatusCode::SUCCESS;  
    
  }

  if (m_dotrig) {    
    if (trigDecTool !=0 && !(trigDecTool->isPassed("L1_XE30")))
      return StatusCode::SUCCESS;
  }

  for (const auto& metKey : m_metKeys)       
    { 
      const std::string& xaod_subkey = key2SubSkeyMap.at(metKey).second;
      const xAOD::MissingET* xMissEt = (*metContainer)[xaod_subkey];
      if (xMissEt) {          
	std::string name = metKey + "_et";
	auto et = Monitored::Scalar<double>(name,-1.0);
	name = metKey + "_ex";
	auto ex = Monitored::Scalar<double>(name,-1.0);
	name = metKey + "_ey";
	auto ey = Monitored::Scalar<double>(name,-1.0);
	name = metKey + "_phi";
	auto phi = Monitored::Scalar<double>(name,-1.0);
	name = metKey + "_sumet";
	auto sumet = Monitored::Scalar<double>(name,-1.0);
	ex = xMissEt->mpx() / GeV;        
	ey =  xMissEt->mpy() / GeV;            
	et =  xMissEt->met() / GeV;        
	phi = xMissEt->phi();           
	sumet = xMissEt->sumet() / GeV; 
	
	if (m_alltrigger &&  m_doBadJets == false) {
	  fill("METMonitor",ex,ey,et,phi,sumet);
	} else if (m_doBadJets) { 
	  if (!isGoodEvent(ctx)) {
	    fill("METMonitor",ex,ey,et,phi,sumet); 		
	  }
	} else if (m_doJetCleaning && m_doBadJets == false) { 
	  if (isGoodEvent(ctx)) {
	    fill("METMonitor",ex,ey,et,phi,sumet); 
	  }
	} else {
	  fill("METMonitor",ex,ey,et,phi,sumet);
	}
      }
    }

  for (const auto& calString : m_calStrings) {
     const xAOD::MissingET* xmetCal = (*metCaloContainer)[calString];
     if (xmetCal){
       std::string name = "MET_"+calString + "_et";
       auto et = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" + calString + "_ex";
       auto ex = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" +calString + "_ey";
       auto ey = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" +calString + "_phi";
       auto phi = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" +calString + "_sumet";
       auto sumet = Monitored::Scalar<double>(name,-1.0);
       ex = xmetCal->mpx() / GeV;
       ey = xmetCal->mpy() / GeV;
       et = xmetCal->met() / GeV;
       phi = xmetCal->phi();
       sumet = xmetCal->sumet() / GeV;

        if (m_alltrigger &&  m_doBadJets == false) {
	  fill("METMonitor",ex,ey,et,phi,sumet);
	} else if (m_doBadJets) { 
	  if (!isGoodEvent(ctx)) {
	    fill("METMonitor",ex,ey,et,phi,sumet); 		
	  }
	} else if (m_doJetCleaning && m_doBadJets == false) { 
	  if (isGoodEvent(ctx)) {
	    fill("METMonitor",ex,ey,et,phi,sumet); 

	  }
	} else {
	  fill("METMonitor",ex,ey,et,phi,sumet);
	}
       
     }
   }
  return StatusCode::SUCCESS;
}


bool METMonitoringAlg::isGoodEvent( const EventContext& ctx ) const {
  if (! m_doJetCleaning) return true;

  SG::ReadHandle<xAOD::JetContainer> jetContainer(m_jetContainerKey, ctx);
  if (! jetContainer.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey);
    return false;///StatusCode::FAILURE;
  }
  
  for (const xAOD::Jet* jet : *jetContainer) {
    if (m_jetCleaningTool->keep(*jet) == false) return false;    
  }
  return true;
}
