/* 
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MissingETMonitoring/METMonitorAlgorithm.h" 
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
      //    { "MET_PFlow_RefFinal", { "MET_Reference_AntiKt4EMPFlow", "FinalClus" } }, // add track 
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


METMonitoringAlg::METMonitoringAlg( const std::string& name, ISvcLocator* pSvcLocator ) :AthMonitorAlgorithm(name,pSvcLocator) ,m_metTrackKey("MET_Track") { 

  declareProperty("MET_EMTopoContainerName",m_metAntiKtEMTopoKey);
  declareProperty("MET_TrackContainerName",m_metTrackKey);
  declareProperty("metFinKey", m_metFinKey); 
  declareProperty("metCalKey", m_metCalKey);  
  declareProperty("metRegKey", m_metRegKey);  
  declareProperty("jetColKey", m_jetColKey);
  declareProperty("eleColKey", m_eleColKey);
  declareProperty("muoColKey", m_muoColKey);
  

} 


METMonitoringAlg::~METMonitoringAlg() {} 

StatusCode METMonitoringAlg::initialize() {
  ATH_CHECK(m_metTrackKey.initialize());
  ATH_CHECK(m_metAntiKtEMTopoKey.initialize());
  ATH_CHECK(m_metCaloContainerKey.initialize());
  //  ATH_CHECK( m_jetContainerKey.initialize() );
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
  SG::ReadHandle<xAOD::MissingETContainer> metTrackContainer(m_metTrackKey,ctx);
  if (!metTrackContainer.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain metTrack Collection with name "<< m_metTrackKey);
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::MissingETContainer> metAntiKtEMTopoContainer(m_metAntiKtEMTopoKey,ctx);
   if (!metAntiKtEMTopoContainer.isValid() ) {
     ATH_MSG_ERROR("evtStore() does not contain metAntiKtEMTopoContainer Collection with name "<< m_metAntiKtEMTopoKey);
    return StatusCode::FAILURE;
  }
   SG::ReadHandle<xAOD::MissingETContainer> metCaloContainer(m_metCaloContainerKey,ctx);
   if (!metCaloContainer.isValid() ) {
     ATH_MSG_ERROR("evtStore() does not contain metAntiKtEMTopoContainer Collection with name "<< m_metCaloContainerKey);
    return StatusCode::FAILURE;
  }


  for (unsigned int i = 0; i < m_metKeys.size(); i++)       
    { 
      //      std::cout<<"met keys "<<m_metKeys[i]<<std::endl;
      auto foundIterator = key2SubSkeyMap.find( m_metKeys[i] );  
	if ( foundIterator == key2SubSkeyMap.end() ) continue;      
	std::string xaod_key = foundIterator->second.first;     
	std::string xaod_subkey = foundIterator->second.second; 
	//	const xAOD::MissingETContainer* xMissEt = 0;  

	SG::ReadHandle<xAOD::MissingETContainer> metContainer(xaod_key,ctx);
	//	std::cout<<"XAOD "<<xaod_key<<" "<<xaod_subkey<<std::endl;
	if (!metContainer.isValid() ) {
	  ATH_MSG_ERROR("evtStore() does not contain metTrack Collection with name "<< xaod_key);
	  return StatusCode::FAILURE;
	}
	const xAOD::MissingET* xMissEt = (*metContainer.cptr())[xaod_subkey];
	if (xMissEt) {          

	  std::string name = m_metKeys[i] + "_et";
	  //	  std::cout<<"name "<<name<<std::endl;
	  auto et = Monitored::Scalar<double>(name,-1.0);
	  name = m_metKeys[i] + "_ex";
	  auto ex = Monitored::Scalar<double>(name,-1.0);
	  name = m_metKeys[i] + "_ey";
	  auto ey = Monitored::Scalar<double>(name,-1.0);
	  name = m_metKeys[i] + "_phi";
	  auto phi = Monitored::Scalar<double>(name,-1.0);
	  name = m_metKeys[i] + "_sumet";
	  auto sumet = Monitored::Scalar<double>(name,-1.0);
	  ex = xMissEt->mpx() / 1000.;        
	  ey =  xMissEt->mpy() / 1000.;            
	  et =  xMissEt->met() / 1000.;        
	  phi = xMissEt->phi();           
	  sumet = xMissEt->sumet() /1000.; 

	  if (m_alltrigger &&  m_doBadJets == false) 
	    fill("METMonitor",ex,ey,et,phi,sumet); 
	  if (m_doBadJets) { 
	    if (!isGoodEvent()) {
	      //	      std::cout<<"met keys "<<m_metKeys[i]<<std::endl;
	      fill("METMonitor",ex,ey,et,phi,sumet); 		
	    }
	  }
	
	  
	  if (m_dometcut) {
	    if (et > m_metcut) fill("METMonitor",ex,ey,et,phi,sumet); 	  	  
	  }
	  if (m_doJetCleaning && m_doBadJets == false) { 
	    if (isGoodEvent()) {
	   //   std::cout<<"met keys "<<m_metKeys[i]<<" "<<m_jetContainerName <<std::endl;
	      fill("METMonitor",ex,ey,et,phi,sumet); 

	    }
	  }
	  
	}
	
	
	
    }

  for (unsigned int i = 0; i < m_calStrings.size(); i++) {      
     const xAOD::MissingET* xmetCal = (*metCaloContainer.cptr())[m_calStrings[i]];
     if (xmetCal){
       std::string name = "MET_"+m_calStrings[i] + "_et";
       auto et = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" + m_calStrings[i] + "_ex";
       auto ex = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" +m_calStrings[i] + "_ey";
       auto ey = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" +m_calStrings[i] + "_phi";
       auto phi = Monitored::Scalar<double>(name,-1.0);
       name = "MET_" +m_calStrings[i] + "_sumet";
       auto sumet = Monitored::Scalar<double>(name,-1.0);
       ex = xmetCal->mpx() / 1000.;
       ey = xmetCal->mpy() / 1000.;
       et = xmetCal->met() / 1000.;
       phi = xmetCal->phi();
       sumet = xmetCal->sumet() / 1000.;
       if (m_alltrigger)fill("METMonitor",ex,ey,et,phi,sumet); 
     }
   }
  return StatusCode::SUCCESS;
}


bool METMonitoringAlg::isGoodEvent() const {
  if (! m_doJetCleaning) return true;

  SG::ReadHandle<xAOD::JetContainer> jetContainer(m_jetContainerName);
  if (! jetContainer.isValid() ) {
    //    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey);
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerName);
    return false;;///StatusCode::FAILURE;
  }
  
  for (const xAOD::Jet* jet : *jetContainer) {
    if (m_jetCleaningTool->keep(*jet) == false) return false;    
  }
  return true;
}
