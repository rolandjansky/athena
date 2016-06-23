/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTTauMonTool.cxx
 *
 *    Artur Kalinowski (Artur.Kalinowski@cern.ch)
 *    Based on HLTMonTool example by Christiane Risler and Martin zur Nedden
 *    Maintained by ccuenca, jengbou
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
#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
//#include "TrigSteeringEvent/TrigOperationalInfo.h"
//#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

//#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "TrigConfL1Data/PrescaleSet.h"

#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"

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

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "VxVertex/VxContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TProfile.h"

#include <vector>
#include <iostream>
#include <fstream>
//#define _USE_MATH_DEFINES
#include <math.h>


#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "HLTTauMonTool.h"


using namespace std;
using namespace AnalysisUtils;

const float PI=2.0*acos(0.);
const float TWOPI=2.0*PI;


HLTTauMonTool::HLTTauMonTool(const std::string & type, const std::string & n, const IInterface* parent)
 : IHLTMonTool(type, n, parent),
   m_l1emulationTool(this),
   m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"), // offline mu
   m_luminosityToolOnline("LuminosityTool/OnlLuminosityTool")  //online mu?
  {
    
    ATH_MSG_DEBUG("HLTTauMonTool::HLTTauMonTool()");
    
    declareProperty("primary_tau", 		m_primary_tau);
    declareProperty("monitoring_tau", 		m_monitoring_tau);
    declareProperty("prescaled_tau", 		m_prescaled_tau);
    declareProperty("Highpt_tau",               m_highpt_tau);
    declareProperty("Ztt_tau",                  m_ztt_tau);
    declareProperty("EffOffTauPtCut", 		m_effOffTauPtCut=20000.);
    declareProperty("TurnOnCurves", 		m_turnOnCurves=true);
    declareProperty("TurnOnCurvesDenom",        m_turnOnCurvesDenom="RecoID");
    declareProperty("doTruth", 			m_truth=false);
    declareProperty("doRealZtautauEff",		m_RealZtautauEff=false);
    declareProperty("dodijetFakeTausEff",       m_dijetFakeTausEff=false);
    declareProperty("doBootstrap",              m_bootstrap=true);
    declareProperty("doEmulation",         	m_emulation=false, "boolean to switch on emulation");
    declareProperty("emulation_l1_tau",         m_emulation_l1_tau, "List of L1 chains to emulate");    
    declareProperty("emulation_hlt_tau",        m_emulation_hlt_tau, "List of HLT chains to emulate");
    declareProperty("L1EmulationTool",          m_l1emulationTool, "Handle to the L1 emulation tool");
    //declareProperty("HltEmulationTool", 	m_hltemulationTool,  "Handle to the HLT emulation tool");
    declareProperty("doTrackCurves",            m_doTrackCurves=false, "Efficiency plots of track distributions");
    //declareProperty("doTestTracking", 		m_doTestTracking=false);
    declareProperty("doTopoValidation",         m_doTopoValidation=false);
    declareProperty("topo_chains",              m_topo_chains);
    declareProperty("topo_support_chains",      m_topo_support_chains);
    declareProperty("LowestSingleTau", 		m_lowest_singletau="");
    declareProperty("L1TriggerCondition", 	m_L1StringCondition="Physics");
    declareProperty("HLTTriggerCondition",      m_HLTStringCondition="Physics");
    declareProperty("nTrkMax",			m_selection_nTrkMax=-1);
    declareProperty("nTrkMin",			m_selection_nTrkMin=-1);
    declareProperty("PtMax",			m_selection_ptMax=-1.);
    declareProperty("PtMin",			m_selection_ptMin=-1.);
    declareProperty("AbsEtaMax",		m_selection_absEtaMax=-1.);
    declareProperty("AbsEtaMin",		m_selection_absEtaMin=-1.);
    declareProperty("AbsPhiMax",		m_selection_absPhiMax=-1.);
    declareProperty("AbsPhiMin",		m_selection_absPhiMin=-1.);

    m_L1TriggerCondition = 0;
    m_HLTTriggerCondition = 0;
    mu_offline = 0.;
    mu_online = 0;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
HLTTauMonTool::~HLTTauMonTool() { 
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::init() {
   
  if (m_emulation) {
    ATH_MSG_INFO("Initializing " << m_l1emulationTool->name());
    ATH_CHECK(m_l1emulationTool.retrieve());
    //ATH_MSG_INFO("Initializing " << m_hltemulationTool->name());
    //ATH_CHECK(m_hltemulationTool.retrieve());
  }

  if (m_luminosityToolOnline.retrieve().isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve LuminosityToolOnline");
  } else {
    ATH_MSG_INFO("Successfully retrieved LuminosityToolOnline");
  }

  if (m_lumiBlockMuTool.retrieve().isFailure()) {                                     
	msg() << MSG::WARNING << "Unable to retrieve LumiBlockMuTool" << endreq;     
  } else {                                                                     
	msg() << MSG::DEBUG << "Successfully retrieved LumiBlockMuTool" << endreq;
  }  

  mu_offline = 0.;
  mu_online = 0;

    // put all trigger names into one arry
    for(std::vector<std::string>::iterator it = m_monitoring_tau.begin(); it != m_monitoring_tau.end(); ++it) {
        m_trigItems.push_back(*it);
    }
    for(std::vector<std::string>::iterator it = m_primary_tau.begin(); it != m_primary_tau.end(); ++it) {
        m_trigItems.push_back(*it);
    }
    for(std::vector<std::string>::iterator it = m_prescaled_tau.begin(); it != m_prescaled_tau.end(); ++it) {
        m_trigItems.push_back(*it);
    }
    for(std::vector<std::string>::iterator it = m_highpt_tau.begin(); it != m_highpt_tau.end(); ++it) {
      m_trigItemsHighPt.push_back(*it);
    }
    for(std::vector<std::string>::iterator it = m_ztt_tau.begin(); it != m_ztt_tau.end(); ++it) {
      m_trigItemsZtt.push_back(*it);
    }


//   ATH_MSG_WARNING("Retrieving HLT tau chains");
//   const Trig::ChainGroup* m_allHLTTauItems = getTDT()->getChainGroup(".*");
//   std::vector<std::string> TauItems = m_allHLTTauItems->getListOfTriggers();
//   ATH_MSG_WARNING("Retrieved "<< TauItems.size() << " chains");
//   for(int i=0;i<TauItems.size();i++){
//	ATH_MSG_WARNING("ChainGroup "<< TauItems.at(i).c_str());
//   }

    m_L1TriggerCondition=TrigDefs::Physics;
    m_HLTTriggerCondition=TrigDefs::Physics; 
    
    if(m_L1StringCondition=="allowResurrectedDecision") m_L1TriggerCondition=TrigDefs::Physics | TrigDefs::allowResurrectedDecision;
    if(m_HLTStringCondition=="allowResurrectedDecision") m_HLTTriggerCondition=TrigDefs::Physics | TrigDefs::allowResurrectedDecision;


    return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTTauMonTool::book()
#else
StatusCode HLTTauMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun)
#endif
{
    
    for(unsigned int i=0;i<m_trigItems.size(); ++i) addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+m_trigItems[i],run));
    
    ATH_MSG_DEBUG("initialize being called");
    
    if (newRun){ 
      for(unsigned int i=0;i<m_trigItems.size(); ++i) bookHistogramsForItem(m_trigItems[i]);
      bookHistogramsAllItem();
    } else if ( newEventsBlock || newLumiBlock ) {
        return StatusCode::SUCCESS;
    }
    return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
StatusCode HLTTauMonTool::fill() {
    
    ATH_MSG_DEBUG(" ====== Begin fillHists() ====== ");
    StatusCode sc;


    // skip HLTResult truncated events
    if(getTDT()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip event");
        return StatusCode::SUCCESS;
    }
 
//    if(!m_trigItemsAll.size()){ 
//      ATH_MSG_DEBUG("Retrieving HLT tau chains");
//      const Trig::ChainGroup* m_allHLTTauItems = getTDT()->getChainGroup("HLT_.*tau.*");
//      m_trigItemsAll = m_allHLTTauItems->getListOfTriggers();
//      ATH_MSG_DEBUG("Retrieved "<< m_trigItemsAll.size() << " chains");
//      for(unsigned int i=0;i<m_trigItemsAll.size();++i){
//        ATH_MSG_DEBUG("ChainGroup "<< m_trigItemsAll.at(i).c_str());
//       }
//      for(unsigned int j=0;j<m_trigItems.size();++j){
//        bool activeChain(false);
//	for(unsigned int i=0;i<m_trigItemsAll.size();++i) if("HLT_"+m_trigItems.at(j) == m_trigItemsAll.at(i)) activeChain = true;
//	if(!activeChain && m_trigItems.at(j)!="Dump") ATH_MSG_WARNING("trigger chain " << m_trigItems.at(j) << " not active! skipping");
//      }
//   }

     //Pileup
     //mu_offline = Pileup();
     if(m_lumiBlockMuTool){
        double avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
        mu_offline = avg_mu;
        ATH_MSG_DEBUG("offline mu "<<avg_mu);
     }
     if(m_luminosityToolOnline){
        double avg_mu = m_luminosityToolOnline->lbAverageInteractionsPerCrossing();
        mu_online = avg_mu;
        ATH_MSG_DEBUG("online mu "<<avg_mu);
     }
 
    for(unsigned int j=0;j<m_trigItems.size();++j)
    {

	// test Trigger Before Prescale (TBP) decision:
	std::string trig_item_EF = "HLT_"+m_trigItems.at(j);
        std::string trig_item_L1(LowerChain( trig_item_EF ) );
        const unsigned int TBP_bit = getTDT()->isPassedBits(trig_item_L1);
	if(TBP_bit & TrigDefs::L1_isPassedBeforePrescale) hist("hL1TBPCounts","HLT/TauMon/Expert")->Fill(m_trigItems.at(j).c_str(),1.);

//	bool activeChain(false);
//	for(unsigned int i=0;i<m_trigItemsAll.size();++i) if("HLT_"+m_trigItems.at(j) == m_trigItemsAll.at(i)) activeChain = true;
//	if(!activeChain && m_trigItems.at(j)!="Dump"){ATH_MSG_DEBUG("trigger chain " << m_trigItems.at(j) << " not active! skipping");}
//	else {
//		std::string trig_item_EF = "HLT_"+m_trigItems.at(j);
//		std::string trig_item_L1(LowerChain( trig_item_EF ) );

	if ( getTDT()->isPassed(trig_item_L1,m_L1TriggerCondition) ) hist("hL1Counts","HLT/TauMon/Expert")->Fill(m_trigItems.at(j).c_str(),1.);
	if ( getTDT()->isPassed(trig_item_EF,m_HLTTriggerCondition) ) hist("hHLTCounts","HLT/TauMon/Expert")->Fill(m_trigItems.at(j).c_str(),1.);  
		//testL1TopoNavigation(m_trigItems[j]);
		//testPrescaleRetrieval(m_trigItems[j]);
	sc = fillHistogramsForItem(m_trigItems[j]);
	if(sc.isFailure()){ ATH_MSG_WARNING("Failed at fillHistogramsForItem. Exiting!"); return StatusCode::FAILURE;} 
	if(m_doTrackCurves){
		sc = trackCurves (m_trigItems[j]);
		if(sc.isFailure()){ ATH_MSG_WARNING("Failed trackCurves(). Exiting!"); return StatusCode::FAILURE;}
	}
//      	}      		 

//	if(trig_item_L1=="L1_TAU20IM_2TAU12IM_J25_2J20_3J12"){
//
//		const xAOD::JetContainer * jet_cont = 0;
//		if(evtStore()->retrieve(jet_cont, "AntiKt4EMTopoJets").isFailure())
//	  	{
//	    		ATH_MSG_WARNING("Failed to retrieve AntiKt4EMTopoJets container. Exiting.");
//	    		return StatusCode::FAILURE;
//	  	}
//		float maxPt=0;
//		xAOD::JetContainer::const_iterator jetItr, jet_cont_end = jet_cont->end();
//		for(jetItr=jet_cont->begin(); jetItr!=jet_cont_end; ++jetItr){
//	    		if((*jetItr)->pt()>maxPt) maxPt=(*jetItr)->pt();
//		}
//		setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");
//	    	if(getTDT()->isPassed("L1_TAU20IM_2TAU12IM_J25_2J20_3J12",m_L1TriggerCondition)) profile("TProfRecoL1_J25PtEfficiency")->Fill(maxPt/1000.,1);
//		else if(getTDT()->isPassed("L1_TAU20IM_2TAU12IM",m_L1TriggerCondition)) profile("TProfRecoL1_J25PtEfficiency")->Fill(maxPt/1000.,0);
//    	}
    }

    if(m_doTopoValidation){
	if(m_topo_chains.size()!=m_topo_support_chains.size()) ATH_MSG_WARNING("List of topo and support chains are different. Skipping!");
        else for(unsigned int topo=0;topo<m_topo_chains.size();topo++){
		sc = fillTopoValidation(m_topo_chains.at(topo), m_topo_support_chains.at(topo));
        	if(sc.isFailure()){ ATH_MSG_WARNING("Failed TopoValidation. Exiting!"); return StatusCode::FAILURE;}
	}
    }

    if(m_emulation) {
        sc = Emulation();
        if(sc.isFailure()){ ATH_MSG_WARNING("Failed emulation. Exiting!"); return StatusCode::FAILURE;}
    }

    if(m_RealZtautauEff)
      {
        sc = RealZTauTauEfficiency();
        if(sc.isFailure()){ ATH_MSG_WARNING("Failed RealZTauTauEfficiency(). Exiting!"); return StatusCode::FAILURE;}
      }

    if(m_dijetFakeTausEff)
      {
        sc = dijetFakeTausEfficiency();
        if(sc.isFailure()){ ATH_MSG_WARNING("Failed dijetFakeTausEfficiency(). Exiting!"); return StatusCode::FAILURE;}
      }
  
    //if(m_doTestTracking){ sc = test2StepTracking();
    //  if(sc.isFailure()){ ATH_MSG_WARNING("Failed at test2Steptracking. Exiting!"); return StatusCode::FAILURE;}
    //}
    return sc;
    //ATH_MSG_DEBUG(" ====== End fillHists() ====== ");
    
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void HLTTauMonTool::cloneProfile(const std::string name, const std::string folder){

  std::string name_clone( profile(name,folder)->GetName() );
  name_clone += "_shifter";
  addProfile( (TProfile*)profile(name,folder)->Clone( name_clone.c_str() ) );

}

void HLTTauMonTool::cloneHistogram(const std::string name, const std::string folder){

  std::string name_clone( hist(name,folder)->GetName() );
  name_clone += "_shifter";
  addHistogram( (TH1F*)hist(name,folder)->Clone( name_clone.c_str() ) );

}


void HLTTauMonTool::cloneHistogram2(const std::string name, const std::string folder){

  std::string name_clone( hist2(name,folder)->GetName() );
  name_clone += "_shifter";
  addHistogram( (TH2F*)hist2(name,folder)->Clone( name_clone.c_str() ) );

}



StatusCode HLTTauMonTool::fillHistogramsForItem(const std::string & trigItem){
    
    ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem " << trigItem);
    
    StatusCode sc;
    
    std::string trig_item_EF = "HLT_"+trigItem;
    std::string trig_item_L1(LowerChain( trig_item_EF ) );
    if(trig_item_L1=="") {ATH_MSG_DEBUG("L1 chain for "<< trig_item_EF << " not found");}
    
    if(trigItem=="Dump"){

	const xAOD::EmTauRoIContainer* l1Tau_cont = 0;

        if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){ // retrieve arguments: container type, container key
          ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
          return StatusCode::FAILURE;
        } else{
          ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
        }

        xAOD::EmTauRoIContainer::const_iterator itEMTau = l1Tau_cont->begin();
        xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();

        for(; itEMTau!=itEMTau_e; ++itEMTau){
          if(!Selection(*itEMTau)) continue;   
          setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1RoI");
	  sc = fillL1Tau(*itEMTau);	
	  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1VsOffline");
          sc = fillL1TauVsOffline(*itEMTau);

	  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to retrieve L1 tau. Exiting!"); return StatusCode::FAILURE;}
	}
       
      
        const xAOD::TauJetContainer * tauJetCont = 0;
        
        if( evtStore()->retrieve(tauJetCont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
            ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting!");
            return StatusCode::FAILURE;
        }
        
        xAOD::TauJetContainer::const_iterator CI = tauJetCont->begin(),tauJetCont_end = tauJetCont->end();
         
        for(; CI!=tauJetCont_end; ++CI){
          if(!Selection(*CI)) continue;
	  sc = fillEFTau(*CI, trigItem, "basicVars");
	  sc = fillEFTau(*CI, trigItem, "1p_NonCorr");
	  sc = fillEFTau(*CI, trigItem, "mp_NonCorr");
	  sc = fillEFTau(*CI, trigItem, "1p_muCorr");
	  sc = fillEFTau(*CI, trigItem, "mp_muCorr");
	  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill BDT input histograms for fillEFTau(). Exiting!"); return StatusCode::FAILURE;}
	  sc = fillEFTauVsOffline(*CI, trigItem, "basicVars");
	  sc = fillEFTauVsOffline(*CI, trigItem, "1p_NonCorr");
	  sc = fillEFTauVsOffline(*CI, trigItem, "mp_NonCorr");
	  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill histograms for fillEFTauVsOffline(). Exiting!"); return StatusCode::FAILURE;}
	}
        
    } else {
        
      // L1 Histograms ...
     if (getTDT()->isPassed(trig_item_L1,m_L1TriggerCondition)) {
           
       ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem passed " << trig_item_L1);
       
       Trig::FeatureContainer f = ( getTDT()->features(trig_item_L1,m_L1TriggerCondition) );  
       Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
       if(comb==combEnd){
         ATH_MSG_DEBUG("No features for chain " << trig_item_L1);
         hist("hL1CountsDebug","HLT/TauMon/Expert")->Fill(trigItem.c_str(),1.);
       } else if (f.getCombinations().size()>100) {
		ATH_MSG_WARNING("Chain " << trig_item_L1 << " has " << f.getCombinations().size() << " combinations. Looping over first 100");
		combEnd = comb;
		std::advance(combEnd,100);	
	}
       else ATH_MSG_DEBUG("The chain " << trig_item_L1 << " has " << f.getCombinations().size() << " combinations");


       std::vector< uint32_t > tau_roIWord;
       std::vector< float > tau_roi_eta;
       std::vector< float > tau_roi_phi;
       std::vector< uint32_t > jet_roIWord;
       std::vector< float > jet_roi_eta;
       std::vector< float > jet_roi_phi;

       const xAOD::EmTauRoIContainer* l1Tau_cont = 0;
       if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){ // retrieve arguments: container type, container key
           ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
       } else {
           ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
       }
       xAOD::EmTauRoIContainer::const_iterator itEMTau;
       xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();

       const xAOD::JetRoIContainer *l1jets = 0;
       if ( m_storeGate->retrieve( l1jets, "LVL1JetRoIs").isFailure() ){
                ATH_MSG_WARNING("Failed to retrieve LVL1JetRoIs container. Exiting.");
                
       } else {
                ATH_MSG_DEBUG("found LVL1JetRoIs in SG");
       }

       xAOD::JetRoIContainer::const_iterator itJetRoI;
       xAOD::JetRoIContainer::const_iterator itJetRoI_e = l1jets->end();

       for(;comb!=combEnd;++comb){

         const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb->get<TrigRoiDescriptor>("initialRoI",m_L1TriggerCondition);
         std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();
         ATH_MSG_DEBUG("Combination with " << vec_roi.size() << "RoIs"); 
	 

         for(roi = vec_roi.begin(); roi != roi_e; ++roi)
           if(roi->cptr()){
             for(itEMTau = l1Tau_cont->begin(); itEMTau!=itEMTau_e; ++itEMTau){
	       if( (*itEMTau)->roiType()!=2 ) continue; // only Run2 TAU RoI!
     	       if(roi->cptr()->roiWord()==(*itEMTau)->roiWord()){
     	         if(!Selection(*itEMTau)) continue;
                 bool newRoI=true;
		 for(unsigned int l1tau=0;l1tau<tau_roIWord.size();l1tau++) if(roi->cptr()->roiWord()==tau_roIWord.at(l1tau)) newRoI=false;
                 if(!newRoI) break; 
                 tau_roIWord.push_back(roi->cptr()->roiWord()); 
                 tau_roi_eta.push_back((*itEMTau)->eta());
                 tau_roi_phi.push_back((*itEMTau)->phi());
          	 ATH_MSG_DEBUG("Found RoI in (" << (*itEMTau)->eta() << "," << (*itEMTau)->phi() <<")");
                 setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1RoI");
                 sc = fillL1Tau(*itEMTau);
                 if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill L1RoI histo. Exiting!"); return StatusCode::FAILURE;}
                 setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1VsOffline");
                 sc = fillL1TauVsOffline(*itEMTau);
                 if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill L1VsOffline histo. Exiting!"); return StatusCode::FAILURE;}
		 break;
               }
             }
           }
  
	  std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi1 = vec_roi.begin(), roi1_e = vec_roi.end();
	  
	  for(roi = vec_roi.begin(); roi != roi_e; ++roi) if(roi->cptr()){
		for(itJetRoI = l1jets->begin(); itJetRoI!=itJetRoI_e; ++itJetRoI){
			if(roi->cptr()->roiWord()==(*itJetRoI)->roiWord()){ // found jet RoI
			  bool isAlsoEMTauRoI = false;
                          for(unsigned int l1tau = 0; l1tau<tau_roIWord.size();l1tau++) if(roi->cptr()->roiWord()==tau_roIWord.at(l1tau)) {isAlsoEMTauRoI = true; break;}
			  if(isAlsoEMTauRoI) break; // skip, it's a tau RoI
                          bool newRoI=true;
                          for(unsigned int l1jet=0;l1jet<jet_roIWord.size();l1jet++) if(roi->cptr()->roiWord()==jet_roIWord.at(l1jet)) newRoI=false;
                          if(!newRoI) break;
                          jet_roIWord.push_back(roi->cptr()->roiWord());
                 	  jet_roi_eta.push_back((*itJetRoI)->eta());
                 	  jet_roi_phi.push_back((*itJetRoI)->phi());
			  ATH_MSG_DEBUG("Found L1 Jet RoI in chain " << trig_item_L1);
			  ATH_MSG_DEBUG("Found RoI in (" << (*itJetRoI)->eta() << "," << (*itJetRoI)->phi() <<")");
  			  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/L1RoI");
          		  sc = fillL1Jet(*itJetRoI);
			  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill L1RoI Jet histo. Exiting!"); return StatusCode::FAILURE;}		
			  break;
			}
		}
	  }
	
	  /*if(trig_item_L1=="L1_TAU20IM_2TAU12IM_J25_2J20_3J12"){ 
		ATH_MSG_WARNING("fired L1_TAU20IM_2TAU12IM_J25_2J20_3J12");
		for(roi = vec_roi.begin(); roi != roi_e; ++roi) if(roi->cptr()){
			ATH_MSG_WARNING("Initial RoI: " << roi->cptr()->roiWord() << ", " << roi->cptr()->phi() << ", " << roi->cptr()->eta() );
		}
		for(itEMTau = l1Tau_cont->begin(); itEMTau!=itEMTau_e; ++itEMTau){
			if( (*itEMTau)->roiType()!=2 ) continue; // only Run2 TAU RoI!
			ATH_MSG_WARNING("EMTau RoI: " << (*itEMTau)->roiWord() << ", " << (*itEMTau)->phi() << ", " << (*itEMTau)->eta() << ", " << (*itEMTau)->eT()/CLHEP::GeV);
		}
                for(itJetRoI = l1jets->begin(); itJetRoI!=itJetRoI_e; ++itJetRoI){
				ATH_MSG_WARNING("Jet RoI: " << (*itJetRoI)->roiWord() << ", " << (*itJetRoI)->phi() << ", " << (*itJetRoI)->eta() << ", " << (*itJetRoI)->etLarge()/CLHEP::GeV);
                }
	  }*/

	} //end loop over L1 combinations

        if(trig_item_L1=="L1_TAU20IM_2TAU12IM" && getTDT()->isPassed(trig_item_L1,m_L1TriggerCondition)){

                const xAOD::JetContainer * jet_cont = 0;
                if(evtStore()->retrieve(jet_cont, "AntiKt4LCTopoJets").isFailure())
                {
                        ATH_MSG_WARNING("Failed to retrieve AntiKt4EMTopoJets container. Exiting.");
                        return StatusCode::FAILURE;
                }
                float maxPt=0;
                xAOD::JetContainer::const_iterator jetItr, jet_cont_end = jet_cont->end();
                for(jetItr=jet_cont->begin(); jetItr!=jet_cont_end; ++jetItr){
			if(fabs((*jetItr)->eta())>3.2) continue;
			bool tauRoi = false;
  			for(unsigned int l1tau = 0; l1tau<tau_roIWord.size();l1tau++){
                        	if(deltaR((*jetItr)->eta(), tau_roi_eta.at(l1tau), (*jetItr)->phi(), tau_roi_phi.at(l1tau)) < 0.3) tauRoi = true;
			}
			if(tauRoi) continue;
                        if((*jetItr)->pt()>maxPt) maxPt=(*jetItr)->pt();
                }
                setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");
                if(getTDT()->isPassed("L1_TAU20IM_2TAU12IM_J25_2J20_3J12",m_L1TriggerCondition)) profile("TProfRecoL1_J25PtEfficiency")->Fill(maxPt/1000.,1);
                else  profile("TProfRecoL1_J25PtEfficiency")->Fill(maxPt/1000.,0);
        } 

     }// end L1 histos

//     // Testing tau+met chains...
//     if(getTDT()->isPassed("HLT_tau35_medium1_tracktwo_xe70_L1XE45")) {
//        ATH_MSG_WARNING ("Event passed HLT_tau35_medium1_tracktwo_xe70_L1XE45");
//        Trig::FeatureContainer f = ( getTDT()->features("HLT_tau35_medium1_tracktwo_xe70_L1XE45") );
//        Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
//        if(comb==combEnd) ATH_MSG_WARNING ("But NO combination of features associated");
//        if(comb!=combEnd) ATH_MSG_WARNING ("WITH combination of features associated");
//        const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_HLTtau = comb->get<xAOD::TauJetContainer>();
//        std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator CI = vec_HLTtau.begin(), CI_e = vec_HLTtau.end();
//        if(CI==CI_e) ATH_MSG_WARNING("TrigTauMerged TauJet container EMPTY in HLT_tau35_medium1_tracktwo_xe70_L1XE45");
//     }
     // HLT histsos ...
     if (getTDT()->isPassed(trig_item_EF,m_HLTTriggerCondition)) {

       ATH_MSG_DEBUG ("HLTTauMonTool::fillHistogramsForItem passed " << trig_item_EF);

       Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,m_HLTTriggerCondition) );
       Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
       if(comb==combEnd){
         ATH_MSG_DEBUG("No features for chain " << trig_item_EF);
         hist("hHLTCountsDebug","HLT/TauMon/Expert")->Fill(trigItem.c_str(),1.);
         //return StatusCode::FAILURE;
       } else if (f.getCombinations().size()>100) {
                ATH_MSG_WARNING("Chain " << trig_item_EF << " has " << f.getCombinations().size() << " combinations. Looping over first 100");
                combEnd = comb;
                std::advance(combEnd,100);
       }

       for(;comb!=combEnd;++comb){

         const std::vector< Trig::Feature<xAOD::TauJetContainer> >  vec_preseltau = comb->get<xAOD::TauJetContainer>("TrigTauRecPreselection",m_HLTTriggerCondition);
         std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator preselCI = vec_preseltau.begin(), preselCI_e = vec_preseltau.end();
         if(preselCI==preselCI_e) ATH_MSG_DEBUG("TrigTauPreselection TauJet container EMPTY in " << trig_item_EF);
         if(preselCI!=preselCI_e){
           ATH_MSG_DEBUG("Item "<< trigItem << ": " << vec_preseltau.size() << " " << preselCI->label() << " containers");  
           for(; preselCI != preselCI_e; ++preselCI)
             if(preselCI->cptr()){
             if(preselCI->cptr()->size()==0) ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << preselCI->cptr()->size() << " TauJets"); 
             ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << preselCI->cptr()->size() << " TauJets");
      	        xAOD::TauJetContainer::const_iterator tauItr = preselCI->cptr()->begin();
               xAOD::TauJetContainer::const_iterator tauEnd = preselCI->cptr()->end();
                    
               for(; tauItr != tauEnd; ++tauItr) {
     	          if(!Selection(*tauItr)) continue;
      	  	  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/PreselectionTau");
                  if(*tauItr) sc = fillPreselTau(*tauItr);
      	          if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill PreselTau histo. Exiting!"); return StatusCode::FAILURE;}	
      		  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/PreselectionVsOffline");
     	          if(*tauItr) sc = fillPreselTauVsOffline(*tauItr);
                  if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill PreselectionVsOffline histo. Exiting!"); return StatusCode::FAILURE;}
                 }
               }
         }              

         const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_HLTtau = comb->get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
         std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator CI = vec_HLTtau.begin(), CI_e = vec_HLTtau.end();
         if(CI==CI_e) ATH_MSG_DEBUG("TrigTauMerged TauJet container EMPTY in " << trig_item_EF); 
         ATH_MSG_DEBUG("Item "<< trigItem << ": " << vec_HLTtau.size() << " " << CI->label() << " containers");
         for(; CI != CI_e; ++CI){
           if(CI->cptr()){
             if(CI->cptr()->size()==0) ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << CI->cptr()->size() << " TauJets");
             ATH_MSG_DEBUG("item "<< trigItem << ": TauJetContainer with " << CI->cptr()->size() << " TauJets");
             xAOD::TauJetContainer::const_iterator tauItr = CI->cptr()->begin();
             xAOD::TauJetContainer::const_iterator tauEnd = CI->cptr()->end();

             for(; tauItr != tauEnd; ++tauItr) {
     	if(!Selection(*tauItr)) continue;
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "basicVars");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "1p_NonCorr");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "mp_NonCorr");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "1p_muCorr");
     	if(*tauItr) sc = fillEFTau(*tauItr, trigItem, "mp_muCorr");
     	if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill BDT input histograms for fillEFTau(). Exiting!"); return StatusCode::FAILURE;}
     	if(m_truth) if(*tauItr) sc = fillEFTauVsTruth(*tauItr, trigItem);
     	if(*tauItr) sc = fillEFTauVsOffline(*tauItr, trigItem, "basicVars");
     	if(*tauItr) sc = fillEFTauVsOffline(*tauItr, trigItem, "1p_NonCorr");
     	if(*tauItr) sc = fillEFTauVsOffline(*tauItr, trigItem, "mp_NonCorr");
     	if(sc.isFailure()){ ATH_MSG_WARNING("Failed to Fill histograms for fillEFTauVsOffline(). Exiting!"); return StatusCode::FAILURE;}
           }
         }
       }
     }
     }
   }
    
    if( m_turnOnCurves) {
      if(m_truth) sc = TruthTauEfficiency(trigItem, "Truth");
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill Truth eff curves. Exiting!"); return StatusCode::FAILURE;}
      if(m_truth) sc = TruthTauEfficiency(trigItem, "Truth+Reco");
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill Truth+Reco eff curves. Exiting!"); return StatusCode::FAILURE;}
      sc = TauEfficiency(trigItem,m_turnOnCurvesDenom);
      //sc = RecoTauEfficiency(trigItem);
      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill Reco eff curves. Exiting!"); return StatusCode::FAILURE;}
//      if(m_truth) sc = TauEfficiencyCombo(trigItem);
//      if(sc.isFailure()){ ATH_MSG_WARNING("Failed to fill combo eff curves. Exiting!"); return StatusCode::FAILURE;}
    }  

    if (!sc.isSuccess()) {
        ATH_MSG_WARNING("Could not fill histograms for given item.");
    }
    
    return sc;
    
}


StatusCode HLTTauMonTool::fillL1Tau(const xAOD::EmTauRoI * aL1Tau){

  ATH_MSG_DEBUG("HLTTauMonTool::fillL1Tau");

  if(!aL1Tau){
    ATH_MSG_WARNING("Invalid L1Tau pointer. Exiting");
    return StatusCode::FAILURE;
  }

  hist("hL1RoIEta")->Fill(aL1Tau->eta());
  hist("hL1RoIPhi")->Fill(aL1Tau->phi());
  hist2("hL1EtaVsPhi")->Fill(aL1Tau->eta(),aL1Tau->phi());
  hist("hL1RoIeT")->Fill(aL1Tau->eT()/CLHEP::GeV);

  uint8_t isoBit = aL1Tau->isol();
  if(isoBit/128) hist("hL1RoIisol")->Fill(8);
  if((isoBit/64)%2) hist("hL1RoIisol")->Fill(7);
  if((isoBit/32)%2) hist("hL1RoIisol")->Fill(6);
  if((isoBit/16)%2) hist("hL1RoIisol")->Fill(5);
  if((isoBit/8)%2) hist("hL1RoIisol")->Fill(4);
  if((isoBit/4)%2) hist("hL1RoIisol")->Fill(3);
  if((isoBit/2)%2) hist("hL1RoIisol")->Fill(2);
  if((isoBit/1)%2) hist("hL1RoIisol")->Fill(1);
  hist("hL1RoITauClus")->Fill(aL1Tau->tauClus()/CLHEP::GeV);
  //hist("hL1RoITauClus2")->Fill(aL1Tau->tauClus()/CLHEP::GeV);
  hist("hL1RoIEMIso")->Fill(aL1Tau->emIsol()/CLHEP::GeV);
  hist("hL1RoIHadCore")->Fill(aL1Tau->hadCore()/CLHEP::GeV);
  hist("hL1RoIHadIsol")->Fill(aL1Tau->hadIsol()/CLHEP::GeV);
  hist2("hL1RoITauClusEMIso")->Fill(aL1Tau->tauClus()/CLHEP::GeV,aL1Tau->emIsol()/CLHEP::GeV);
  hist2("hL1EtVsPhi")->Fill(aL1Tau->tauClus()/CLHEP::GeV,aL1Tau->phi());
  hist2("hL1EtVsEta")->Fill(aL1Tau->tauClus()/CLHEP::GeV,aL1Tau->eta());

  const xAOD::JetRoIContainer *l1jets = 0;
  if ( m_storeGate->retrieve( l1jets, "LVL1JetRoIs").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve LVL1JetRoIs container. Exiting.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_DEBUG("found LVL1JetRoIs in SG");
  }
  xAOD::JetRoIContainer::const_iterator itL1Jet = l1jets->begin();
  xAOD::JetRoIContainer::const_iterator itL1Jet_e = l1jets->end();
  for(; itL1Jet!=itL1Jet_e; ++itL1Jet){
   float dEta = aL1Tau->eta() - (*itL1Jet)->eta();
   float dPhi = deltaPhi(aL1Tau->phi(),(*itL1Jet)->phi());
   if(deltaR(aL1Tau->eta(), (*itL1Jet)->eta(), aL1Tau->phi(), (*itL1Jet)->phi()) > 0.3) continue;
   hist2("hL1RoITauVsJet")->Fill(aL1Tau->eT()/CLHEP::GeV,(*itL1Jet)->etLarge()/CLHEP::GeV);
   if(aL1Tau->eT()>(*itL1Jet)->etLarge()) {
	hist2("hL1RoITauVsJetMismatch")->Fill(dEta,dPhi);
	hist2("hL1RoITauVsJetDEt")->Fill(aL1Tau->eT()/CLHEP::GeV,aL1Tau->eT()/CLHEP::GeV-(*itL1Jet)->etLarge()/CLHEP::GeV);
   } 
  }

  return StatusCode::SUCCESS;

}

StatusCode HLTTauMonTool::fillL1Jet(const xAOD::JetRoI * aL1Jet){
	hist("hL1JetRoIEta")->Fill(aL1Jet->eta());
	hist("hL1JetRoIPhi")->Fill(aL1Jet->phi());
	hist2("hL1JetEtaVsPhi")->Fill(aL1Jet->eta(),aL1Jet->phi());
	hist("hL1JetRoIeT")->Fill(aL1Jet->etLarge()/CLHEP::GeV);
	hist2("hL1JetEtVsPhi")->Fill(aL1Jet->etLarge()/CLHEP::GeV,aL1Jet->phi());
	hist2("hL1JetEtVsEta")->Fill(aL1Jet->etLarge()/CLHEP::GeV,aL1Jet->eta());

	return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillPreselTau(const xAOD::TauJet *aEFTau){

    ATH_MSG_DEBUG ("HLTTauMonTool::fillPreselTau");

    if(!aEFTau) {
        ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
        return StatusCode::FAILURE;
    }
	
    //ATH_MSG_WARNING("Tau ROIWord: "<<aEFTau->ROIWord());

    int EFnTrack = aEFTau->nTracks();
    hist("hEFEt")->Fill(aEFTau->pt()/CLHEP::GeV);
    hist("hEFEt2")->Fill(aEFTau->pt()/CLHEP::GeV);
    hist("hFTFnTrack")->Fill(EFnTrack);
    hist("hEta")->Fill(aEFTau->eta());
    hist("hPhi")->Fill(aEFTau->phi());
    //float dRMax=-1; 
    //if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax)) hist("hdRmax")->Fill(dRMax); 
    hist2("hEFEtaVsPhi")->Fill(aEFTau->eta(),aEFTau->phi());
    hist2("hEtVsEta")->Fill(aEFTau->eta(),aEFTau->pt()/CLHEP::GeV);
    hist2("hEtVsPhi")->Fill(aEFTau->phi(),aEFTau->pt()/CLHEP::GeV);
    
    hist("hFTFnWideTrack")->Fill(aEFTau->nWideTracks());

    return StatusCode::SUCCESS;

}

StatusCode HLTTauMonTool::fillEFTau(const xAOD::TauJet *aEFTau, const std::string & trigItem, const std::string & BDTinput_type){
    
 ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTau");
  
  if(!aEFTau)
    {
      ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
      return StatusCode::FAILURE;
    }

  //ATH_MSG_WARNING("Tau ROIWord: "<<aEFTau->ROIWord());
 
  float etEM = 0;
  float etHad = 0;
//  float emRadius = 0;
//  float hadRadius = 0;
  float isoFrac = 0;
  float EMFrac = -1.0;
//  float PSSFraction = 0;
  float BDTJetScore = 0;
  float innerTrkAvgDist = 0;
  float etOverPtLeadTrk = 0;
  float ipSigLeadTrk = 0;
  float SumPtTrkFrac = 0;
  float ChPiEMEOverCaloEME = 0;
  float EMPOverTrkSysP = 0;
  float centFrac = 0;
  float trFlightPathSig = 0;
  float massTrkSys = 0;
  float dRmax = 0;
  float mEflowApprox = 0;
  float ptRatioEflowApprox = 0;

  float innerTrkAvgDistCorr = 0;
  float etOverPtLeadTrkCorr = 0;
  float ipSigLeadTrkCorr = 0;
  float SumPtTrkFracCorr = 0;
  float ChPiEMEOverCaloEMECorr = 0;
  float EMPOverTrkSysPCorr = 0;
  float centFracCorr = 0;
  float trFlightPathSigCorr = 0;
  float massTrkSysCorr = 0;
  float dRmaxCorr = 0;
  float mEflowApproxCorr = 0;
  float ptRatioEflowApproxCorr = 0;
  float mu = 0;

  int EFnTrack = aEFTau->nTracks();
  bool is1P(false), isMP(false);   
  if(EFnTrack==1) is1P = true;
  if(EFnTrack>1) isMP = true;
  //Pileup
  mu = mu_offline;

  if(BDTinput_type == "basicVars")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau");
      hist("hEFEt")->Fill(aEFTau->pt()/CLHEP::GeV);
      hist("hEFEta")->Fill(aEFTau->eta());
      int num_vxt = mu_online;
      //try {num_vxt = aEFTau->auxdata< int >("NUMVERTICES");}
      //catch(std::exception e) {ATH_MSG_DEBUG("HLTTauMonTool: NUMVERTICES decoration not found");}
      hist("hEFNUM")->Fill(num_vxt);
      hist2("hEFNUMvsmu")->Fill(num_vxt,mu);
      hist("hEFPhi")->Fill(aEFTau->phi());
      hist("hEFnTrack")->Fill(EFnTrack);
      hist("hEFnWideTrack")->Fill(aEFTau->nWideTracks());
      hist2("hEFEtaVsPhi")->Fill(aEFTau->eta(),aEFTau->phi());
      hist2("hEFEtVsPhi")->Fill(aEFTau->phi(),aEFTau->pt()/CLHEP::GeV);
      hist2("hEFEtVsEta")->Fill(aEFTau->eta(),aEFTau->pt()/CLHEP::GeV);
      if(aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEM) && aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHad))
	{
	  hist("hEFEtRaw")->Fill( (etEM + etHad)/CLHEP::GeV);
	  EMFrac = etEM / (etEM + etHad);
	  hist("hEFEMFraction")->Fill(EMFrac);
	}
      //if(aEFTau->detail(xAOD::TauJetParameters::EMRadius, emRadius)) hist("hEFEMRadius")->Fill(emRadius);
      //if(aEFTau->detail(xAOD::TauJetParameters::hadRadius, hadRadius)) hist("hEFHADRadius")->Fill(hadRadius);  
      if(aEFTau->detail(xAOD::TauJetParameters::isolFrac, isoFrac)) hist("hEFIsoFrac")->Fill(isoFrac);
      //if(aEFTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction)) hist("hEFPSSFraction")->Fill(PSSFraction);
	BDTJetScore = aEFTau->discriminant(xAOD::TauJetParameters::TauID::BDTJetScore);
      if( BDTJetScore )
	{
	  if(is1P) hist("hScore1p")->Fill(BDTJetScore);
	  if(isMP) hist("hScoremp")->Fill(BDTJetScore);
	}
    }
  else if(BDTinput_type == "1p_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/1p_nonCorrected");
      if(is1P)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist)) 
	    {
	      hist("hEFinnerTrkAvgDist1PNCorr")->Fill(innerTrkAvgDist);
	      profile("hEFinnerTrkAvgDist1PNCmu")->Fill(mu, innerTrkAvgDist);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk))
	    {
	      hist("hEFetOverPtLeadTrk1PNCorr")->Fill(etOverPtLeadTrk);
	      profile("hEFetOverPtLeadTrk1PNCmu")->Fill(mu, etOverPtLeadTrk);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk))
	    {
	      hist("hEFipSigLeadTrk1PNCorr")->Fill(ipSigLeadTrk);
	      profile("hEFipSigLeadTrk1PNCmu")->Fill(mu, ipSigLeadTrk);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac))
	    {
	      hist("hEFSumPtTrkFrac1PNCorr")->Fill(SumPtTrkFrac);
	      profile("hEFSumPtTrkFrac1PNCmu")->Fill(mu, SumPtTrkFrac);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME))
	    {
	      hist("hEFChPiEMEOverCaloEME1PNCorr")->Fill(ChPiEMEOverCaloEME);
	      profile("hEFChPiEMEOverCaloEME1PNCmu")->Fill(mu, ChPiEMEOverCaloEME);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP))
	    {
	      hist("hEFEMPOverTrkSysP1PNCorr")->Fill(EMPOverTrkSysP);
	      profile("hEFEMPOverTrkSysP1PNCmu")->Fill(mu, EMPOverTrkSysP);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac))
	    {
	      hist("hEFcentFrac1PNCorr")->Fill(centFrac);
	      profile("hEFcentFrac1PNCmu")->Fill(mu, centFrac);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox)) 
	    {
	      hist("hEFptRatioEflowApprox1PNCorr")->Fill(ptRatioEflowApprox);
	      profile("hEFptRatioEflowApprox1PNCmu")->Fill(mu, ptRatioEflowApprox);
	    }
        }
    }
  else if(BDTinput_type == "mp_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/mp_nonCorrected");
      if(isMP)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist))
	    {
	      hist("hEFinnerTrkAvgDistMPNCorr")->Fill(innerTrkAvgDist);
	      profile("hEFinnerTrkAvgDistMPNCmu")->Fill(mu, innerTrkAvgDist);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk))
	    {
	      hist("hEFetOverPtLeadTrkMPNCorr")->Fill(etOverPtLeadTrk);
	      profile("hEFetOverPtLeadTrkMPNCmu")->Fill(mu, etOverPtLeadTrk);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME))
	    {
	      hist("hEFChPiEMEOverCaloEMEMPNCorr")->Fill(ChPiEMEOverCaloEME);
	      profile("hEFChPiEMEOverCaloEMEMPNCmu")->Fill(mu, ChPiEMEOverCaloEME);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP))
	    {
	      hist("hEFEMPOverTrkSysPMPNCorr")->Fill(EMPOverTrkSysP);
	      profile("hEFEMPOverTrkSysPMPNCmu")->Fill(mu, EMPOverTrkSysP);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac))
	    {
	      hist("hEFcentFracMPNCorr")->Fill(centFrac);
	      profile("hEFcentFracMPNCmu")->Fill(mu, centFrac);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox))
	    {
	      hist("hEFptRatioEflowApproxMPNCorr")->Fill(ptRatioEflowApprox);
	      profile("hEFptRatioEflowApproxMPNCmu")->Fill(mu, ptRatioEflowApprox);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRmax))
	    {
	      hist("hEFdRmaxMPNCorr")->Fill(dRmax);      
	      profile("hEFdRmaxMPNCmu")->Fill(mu, dRmax);      
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys))
	    {
	      hist("hEFmassTrkSysMPNCorr")->Fill(massTrkSys/CLHEP::GeV);
	      profile("hEFmassTrkSysMPNCmu")->Fill(mu, massTrkSys/CLHEP::GeV);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig))
	    {
	      hist("hEFtrFlightPathSigMPNCorr")->Fill(trFlightPathSig);
	      profile("hEFtrFlightPathSigMPNCmu")->Fill(mu, trFlightPathSig);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::mEflowApprox, mEflowApprox))
	    {
	      hist("hEFmEflowApproxMPNCorr")->Fill(mEflowApprox);
	      profile("hEFmEflowApproxMPNCmu")->Fill(mu, mEflowApprox);
	    }
        }
    }
  else if(BDTinput_type == "1p_muCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/1p_Corrected");
      if(is1P)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDistCorrected, innerTrkAvgDistCorr)) 
	    {
	      hist("hEFinnerTrkAvgDist1PCorr")->Fill(innerTrkAvgDistCorr);
	      profile("hEFinnerTrkAvgDist1PCmu")->Fill(mu, innerTrkAvgDistCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrkCorrected, etOverPtLeadTrkCorr))
	    {
	      hist("hEFetOverPtLeadTrk1PCorr")->Fill(etOverPtLeadTrkCorr);
	      profile("hEFetOverPtLeadTrk1PCmu")->Fill(mu, etOverPtLeadTrkCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrkCorrected, ipSigLeadTrkCorr))
	    {
	      hist("hEFipSigLeadTrk1PCorr")->Fill(ipSigLeadTrkCorr);
	      profile("hEFipSigLeadTrk1PCmu")->Fill(mu, ipSigLeadTrkCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::SumPtTrkFracCorrected, SumPtTrkFracCorr)) 
	    {
	      hist("hEFSumPtTrkFrac1PCorr")->Fill(SumPtTrkFracCorr);
	      profile("hEFSumPtTrkFrac1PCmu")->Fill(mu, SumPtTrkFracCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEMECorrected, ChPiEMEOverCaloEMECorr))
	    {
	      hist("hEFChPiEMEOverCaloEME1PCorr")->Fill(ChPiEMEOverCaloEMECorr);
	      profile("hEFChPiEMEOverCaloEME1PCmu")->Fill(mu, ChPiEMEOverCaloEMECorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysPCorrected, EMPOverTrkSysPCorr)) 
	    {
	      hist("hEFEMPOverTrkSysP1PCorr")->Fill(EMPOverTrkSysPCorr);
	      profile("hEFEMPOverTrkSysP1PCmu")->Fill(mu, EMPOverTrkSysPCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::centFracCorrected, centFracCorr))
	    {
	      hist("hEFcentFrac1PCorr")->Fill(centFracCorr);
	      profile("hEFcentFrac1PCmu")->Fill(mu, centFracCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApproxCorrected, ptRatioEflowApproxCorr))
	    {
	      hist("hEFptRatioEflowApprox1PCorr")->Fill(ptRatioEflowApproxCorr);
	      profile("hEFptRatioEflowApprox1PCmu")->Fill(mu, ptRatioEflowApproxCorr);
	    }
        }
    }
  else if(BDTinput_type == "mp_muCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFTau/BDT/mp_Corrected");
      if(isMP)
        {
          if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDistCorrected, innerTrkAvgDistCorr)) 
	    {
	      hist("hEFinnerTrkAvgDistMPCorr")->Fill(innerTrkAvgDistCorr);
	      profile("hEFinnerTrkAvgDistMPCmu")->Fill(mu, innerTrkAvgDistCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrkCorrected, etOverPtLeadTrkCorr))
	    {
	      hist("hEFetOverPtLeadTrkMPCorr")->Fill(etOverPtLeadTrkCorr);
	      profile("hEFetOverPtLeadTrkMPCmu")->Fill(mu, etOverPtLeadTrkCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEMECorrected, ChPiEMEOverCaloEMECorr)) 
	    {
	      hist("hEFChPiEMEOverCaloEMEMPCorr")->Fill(ChPiEMEOverCaloEMECorr);
	      profile("hEFChPiEMEOverCaloEMEMPCmu")->Fill(mu, ChPiEMEOverCaloEMECorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysPCorrected, EMPOverTrkSysPCorr)) 
	    {
	      hist("hEFEMPOverTrkSysPMPCorr")->Fill(EMPOverTrkSysPCorr); 	   
	      profile("hEFEMPOverTrkSysPMPCmu")->Fill(mu, EMPOverTrkSysPCorr); 	   
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::centFracCorrected, centFracCorr))
	    {
	      hist("hEFcentFracMPCorr")->Fill(centFracCorr);
	      profile("hEFcentFracMPCmu")->Fill(mu, centFracCorr);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApproxCorrected, ptRatioEflowApproxCorr))
	    {
	      hist("hEFptRatioEflowApproxMPCorr")->Fill(ptRatioEflowApproxCorr);
	      profile("hEFptRatioEflowApproxMPCmu")->Fill(mu, ptRatioEflowApproxCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::dRmaxCorrected, dRmaxCorr))
	    {
	      hist("hEFdRmaxMPCorr")->Fill(dRmaxCorr);      
	      profile("hEFdRmaxMPCmu")->Fill(mu, dRmaxCorr);      
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::massTrkSysCorrected, massTrkSysCorr))
	    {
	      hist("hEFmassTrkSysMPCorr")->Fill(massTrkSysCorr/CLHEP::GeV);
	      profile("hEFmassTrkSysMPCmu")->Fill(mu, massTrkSysCorr/CLHEP::GeV);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::trFlightPathSigCorrected, trFlightPathSigCorr))
	    {
	      hist("hEFtrFlightPathSigMPCorr")->Fill(trFlightPathSigCorr);
	      profile("hEFtrFlightPathSigMPCmu")->Fill(mu, trFlightPathSigCorr);
	    }
          if(aEFTau->detail(xAOD::TauJetParameters::mEflowApproxCorrected, mEflowApproxCorr))
	    {
	      hist("hEFmEflowApproxMPCorr")->Fill(mEflowApproxCorr);
	      profile("hEFmEflowApproxMPCmu")->Fill(mu, mEflowApproxCorr);
	    }
        }
    }
  else
    {
      ATH_MSG_WARNING("Wrong BDT input type Failed to fill the histograms.Check fillEFTau(). Exiting.");
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillL1TauVsOffline(const xAOD::EmTauRoI *aL1Tau){

  ATH_MSG_DEBUG ("HLTTauMonTool::fillL1TauVsOffline");
    
  if(!aL1Tau) {
        ATH_MSG_WARNING("Invalid EmTauRoI pointer. Exiting");
        return StatusCode::FAILURE;
  }

  const xAOD::TauJet *aOfflineTau = 0;
  float tmpR = 0.3;
  const xAOD::TauJetContainer* tauCont = 0;
    
  if( m_storeGate->retrieve(tauCont, "TauJets").isFailure()  ){
        ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
        return StatusCode::FAILURE;
  }
    
  xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
  for(CI=tauCont->begin();CI!=offlinetau_end; ++CI){
        if((*CI)){
    
           TLorentzVector TauTLV = (*CI)->p4();
           double eta_Tau = TauTLV.Eta();
           if(fabs(eta_Tau) > 2.47) continue;
           double pt_Tau = TauTLV.Pt();
           if(pt_Tau<m_effOffTauPtCut) continue;
           int ntrack_TAU = (*CI)->nTracks();
           if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
           bool good_tau = (*CI)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
           //bool not_a_electron = !( (*CI)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
           //bool not_a_muon = !( (*CI)->isTau(xAOD::TauJetParameters::MuonVeto) );
           //bool best_tau = good_tau && not_a_electron && not_a_muon;
           bool best_tau = good_tau;
	   if(!best_tau) continue;
	   float eta = aL1Tau->eta();
	   float phi = aL1Tau->phi();
           double dR = deltaR(eta,TauTLV.Eta(),phi,TauTLV.Phi());
              if(dR < tmpR){
                tmpR = dR;
                aOfflineTau = (*CI);
            }
        }
  }

  if(aOfflineTau){
        ATH_MSG_DEBUG("Found an offline TauJet matching L1 EmTauRoI, dR = "<<tmpR);
  }else {
        ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
        return StatusCode::SUCCESS;
  }

  // plots of L1 vs Offline ....

    FillRelDiffHist(hist("hL1EtRatio"), aOfflineTau->pt(), aL1Tau->tauClus(), 0, 1);

  return StatusCode::SUCCESS;

}


StatusCode HLTTauMonTool::fillPreselTauVsOffline(const xAOD::TauJet *aEFTau){
    
    ATH_MSG_DEBUG ("HLTTauMonTool::fillPreselTauVsOffline");
    
    if(!aEFTau) {
        ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
        return StatusCode::FAILURE;
    }
    
    const xAOD::TauJet *aOfflineTau = 0;
    float tmpR = 0.2;
    const xAOD::TauJetContainer* tauCont = 0;
    
    if( m_storeGate->retrieve(tauCont, "TauJets").isFailure()  ){
        ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
        return StatusCode::FAILURE;
    }
    
    xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
    for(CI=tauCont->begin();CI!=offlinetau_end; ++CI){
        if((*CI)){
            
            TLorentzVector TauTLV = (*CI)->p4();
            double eta_Tau = TauTLV.Eta();
            if(fabs(eta_Tau) > 2.47) continue;
            double pt_Tau = TauTLV.Pt();
            if(pt_Tau<m_effOffTauPtCut) continue;
            int ntrack_TAU = (*CI)->nTracks();
            if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
            bool good_tau = (*CI)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
            //bool not_a_electron = !( (*CI)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
            //bool not_a_muon = !( (*CI)->isTau(xAOD::TauJetParameters::MuonVeto) );
            //bool best_tau = good_tau && not_a_electron && not_a_muon;
            bool best_tau = good_tau;
	    if(!best_tau) continue;
            double dR = TauTLV.DeltaR(aEFTau->p4());
            if(dR < tmpR){
                tmpR = dR;
                aOfflineTau = (*CI);
            }
        }
    }
    
    if(aOfflineTau){
        ATH_MSG_DEBUG("Found an offline TauJet matching EF tauJet, dR = "<<tmpR);
    }else {
        ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
        return StatusCode::SUCCESS;
    }

    int EFnTrack = aEFTau->nTracks(); 
    hist2("hPreselvsOffnTrks")->Fill(aOfflineTau->nTracks(), EFnTrack);
    hist2("hPreselvsOffnWideTrks")->Fill(aOfflineTau->nWideTracks(), aEFTau->nWideTracks());
    FillRelDiffHist(hist("hEFEtRatio"), aOfflineTau->pt(), aEFTau->pt(), 0, 1);
    FillRelDiffHist(hist("hEtaRatio"), aOfflineTau->eta(), aEFTau->eta(), 0, 2);
    hist("hPhiRatio")->Fill( deltaPhi(aOfflineTau->phi(), aEFTau->phi()));

    return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillEFTauVsTruth(const xAOD::TauJet *aEFTau, const std::string & trigItem)
{
  ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTauVsTruth");
  if(!aEFTau)
    {
      ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
      return StatusCode::FAILURE;
    }
  
  float mu(Pileup());
  float truthPt(-1.),truthPhi(-999.),truthEta(-999.);
  float tmpdR(0.2);
  //Truth Tau matching to EF(HLT Tau)
  TLorentzVector TruthTauTLV(0.,0.,0.,0.);
  const xAOD::TruthParticleContainer* truth_cont = 0;
  if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
    return StatusCode::FAILURE;
  }
  xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
  for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    { 
      if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) // || (*truthItr)->status() == 10902))
        {
	  examineTruthTau(**truthItr);
	  float pt  = (*truthItr)->auxdata<double>("pt_vis");
	  float eta = (*truthItr)->auxdata<double>("eta_vis");
	  float phi = (*truthItr)->auxdata<double>("phi_vis");
	  float m   = (*truthItr)->auxdata<double>("m_vis");
	 
          TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
          double dR = TruthTauTLV.DeltaR(aEFTau->p4());
          if(dR < tmpdR)
            {
              tmpdR = dR;
              truthPt = TruthTauTLV.Pt();
       	      truthEta = TruthTauTLV.Eta();
	      truthPhi = TruthTauTLV.Phi();
            }
        }
     }
  if(truthPt>0.){
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsTruth");
      FillRelDiffProfile<float>(profile("hEtRatiovspt"), truthPt, aEFTau->pt(), truthPt/1000., 0, 1); 
      FillRelDiffProfile<float>(profile("hEtRatiovsmu"), truthPt, aEFTau->pt(), mu, 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovsphi"), truthPt, aEFTau->pt(), truthPhi, 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovseta"), truthPt, aEFTau->pt(), truthEta, 0, 1);

  }

  return StatusCode::SUCCESS;
}

StatusCode HLTTauMonTool::fillEFTauVsOffline(const xAOD::TauJet *aEFTau, const std::string & trigItem, const std::string & BDTinput_type)
{
  ATH_MSG_DEBUG ("HLTTauMonTool::fillEFTauVsOffline");
  if(!aEFTau)
    {
      ATH_MSG_WARNING("Invalid TauJet pointer. Exiting");
      return StatusCode::FAILURE;
    }
 
  float innerTrkAvgDist = 0;
  float etOverPtLeadTrk = 0;
  float ipSigLeadTrk = 0;
  float SumPtTrkFrac = 0;
  float ChPiEMEOverCaloEME = 0;
  float EMPOverTrkSysP = 0;
  float centFrac = 0;
  float ptRatioEflowApprox = 0;
  float dRMax = 0;
  float topoInvMass = 0;
  float trFlightPathSig = 0;
  float massTrkSys = 0;
  float mEflowApprox = 0;

  float innerTrkAvgDistOff = 0;
  float etOverPtLeadTrkOff = 0;
  float ipSigLeadTrkOff = 0;
  float SumPtTrkFracOff = 0;
  float ChPiEMEOverCaloEMEOff = 0;
  float EMPOverTrkSysPOff = 0;
  float centFracOff = 0;
  float ptRatioEflowApproxOff = 0;
  float dRMaxOff = 0;
  float topoInvMassOff = 0;
  float trFlightPathSigOff = 0;
  float massTrkSysOff = 0;
  float mEflowApproxOff = 0;

  float isoFrac = 0;
//  float EMRadius = 0;
//  float hadRadius = 0;
//  float PSSFraction = 0;
  float EMFraction = 0;
  float EtRaw = 0;
  
  float isoFracOff = 0;
//  float EMRadiusOff = 0;
//  float hadRadiusOff = 0;
//  float PSSFractionOff = 0;
  float EMFractionOff = 0;
  float EtRawOff = 0;
  
  float etEMAtEMScale = 0;
  float etEMAtEMScaleOff = 0;
  float etHadAtEMScale = 0;
  float etHadAtEMScaleOff = 0;
  float mu = 0;

  int EFnTrack = aEFTau->nTracks();
  bool is1P(false), isMP(false);
  if(EFnTrack==1) is1P = true;
  if(EFnTrack>1) isMP = true;

  //Offline Tau matching to EF(HLT Tau)
  const xAOD::TauJet *aOfflineTau = 0;
  const xAOD::TauJetContainer* tauCont = 0;
  float tmpR = 0.2;
  
  if(m_storeGate->retrieve(tauCont, "TauJets").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
      return StatusCode::FAILURE;
    }
  
  xAOD::TauJetContainer::const_iterator CI,offlinetau_end = tauCont->end();
  for(CI=tauCont->begin();CI!=offlinetau_end; ++CI)
    {
      if((*CI))
	{
	  TLorentzVector TauTLV = (*CI)->p4();
	  double eta_Tau = TauTLV.Eta();
	  if(fabs(eta_Tau) > 2.47) continue;
	  double pt_Tau = TauTLV.Pt();
	  if(pt_Tau<m_effOffTauPtCut) continue;
	  int ntrack_TAU = (*CI)->nTracks();
	  if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
	  bool good_tau = (*CI)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
	  //bool not_a_electron = !( (*CI)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
	  //bool not_a_muon = !( (*CI)->isTau(xAOD::TauJetParameters::MuonVeto) );
	  //bool best_tau = good_tau && not_a_electron && not_a_muon;
	  bool best_tau = good_tau;
	  if(!best_tau) continue;
	  double dR = TauTLV.DeltaR(aEFTau->p4());
	  if(dR < tmpR)
	    { 
	      tmpR = dR;
	      aOfflineTau = (*CI);
	    }
        }
    }
  if(aOfflineTau)
    {
      ATH_MSG_DEBUG("Found an offline TauJet matching EF tauJet, dR = "<<tmpR);
    }
  else 
    {
      ATH_MSG_DEBUG("Not found an offline TauJet matching EF tauJet");
      return StatusCode::SUCCESS;
    }
  //Pileup
  mu = Pileup();

  if(BDTinput_type == "basicVars")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsOffline");
      //Basic Vars
      hist2("hEFvsOffnTrks")->Fill(aOfflineTau->nTracks(), aEFTau->nTracks());
      hist2("hEFvsOffnWideTrks")->Fill(aOfflineTau->nWideTracks(), aEFTau->nWideTracks());
      FillRelDiffHist(hist("hptRatio"), aOfflineTau->pt(), aEFTau->pt(), 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovspt"), aOfflineTau->pt(), aEFTau->pt(), aOfflineTau->pt()/1000., 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovseta"), aOfflineTau->pt(), aEFTau->pt(), aOfflineTau->eta(), 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovsphi"), aOfflineTau->pt(), aEFTau->pt(), aOfflineTau->phi(), 0, 1);
      FillRelDiffProfile<float>(profile("hEtRatiovsmu"), aOfflineTau->pt(), aEFTau->pt(), mu, 0, 1);

      FillRelDiffHist(hist("hetaRatio"), aOfflineTau->eta(), aEFTau->eta(), 0, 2);
      hist("hphiRatio")->Fill(deltaPhi(aOfflineTau->phi(), aEFTau->phi()));
   
//      if(aEFTau->detail(xAOD::TauJetParameters::EMRadius, EMRadius) && aOfflineTau->detail(xAOD::TauJetParameters::EMRadius, EMRadiusOff))
//	{
//	  FillRelDiffHist(hist("hEMRadiusRatio"), EMRadiusOff, EMRadius, 0, 1);
//          FillRelDiffProfile<float>(profile("hEMRadiusRatiovspt"), EMRadiusOff, EMRadius, aOfflineTau->pt()/1000., 0, 1);
//          FillRelDiffProfile<float>(profile("hEMRadiusRatiovseta"), EMRadiusOff, EMRadius, aOfflineTau->eta(), 0, 1);
//          FillRelDiffProfile<float>(profile("hEMRadiusRatiovsphi"), EMRadiusOff, EMRadius, aOfflineTau->phi(), 0, 1);
//          FillRelDiffProfile<float>(profile("hEMRadiusRatiovsmu"), EMRadiusOff, EMRadius, mu, 0, 1);
//	}
//      if(aEFTau->detail(xAOD::TauJetParameters::hadRadius, hadRadius) && aOfflineTau->detail(xAOD::TauJetParameters::hadRadius, hadRadiusOff))
//	{
//	  FillRelDiffHist(hist("hHadRadiusRatio"), hadRadiusOff, hadRadius, 0, 1);
//          FillRelDiffProfile<float>(profile("hHADRadiusRatiovspt"), hadRadiusOff, hadRadius, aOfflineTau->pt()/1000., 0, 1);
//          FillRelDiffProfile<float>(profile("hHADRadiusRatiovseta"), hadRadiusOff, hadRadius, aOfflineTau->eta(), 0, 1);
//          FillRelDiffProfile<float>(profile("hHADRadiusRatiovsphi"), hadRadiusOff, hadRadius, aOfflineTau->phi(), 0, 1);
//          FillRelDiffProfile<float>(profile("hHADRadiusRatiovsmu"), hadRadiusOff, hadRadius, mu, 0, 1);
//	}
      if(aEFTau->detail(xAOD::TauJetParameters::isolFrac, isoFrac) && aOfflineTau->detail(xAOD::TauJetParameters::isolFrac, isoFracOff))
	{
	  FillRelDiffHist(hist("hIsoFracRatio"), isoFracOff, isoFrac, 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovspt"), isoFracOff, isoFrac, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovseta"), isoFracOff, isoFrac, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovsphi"), isoFracOff, isoFrac, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hIsoFracRatiovsmu"), isoFracOff, isoFrac, mu, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff))
        {  
          FillRelDiffProfile<float>(profile("hCentFracRatiovspt"), centFracOff, centFrac, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hCentFracRatiovseta"), centFracOff, centFrac, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hCentFracRatiovsphi"), centFracOff, centFrac, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hCentFracRatiovsmu"), centFracOff, centFrac, mu, 0, 1);
        }
//      if(aEFTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction) && aOfflineTau->detail(xAOD::TauJetParameters::PSSFraction, PSSFractionOff))
//	{
//	  FillRelDiffHist(hist("hPSSFracRatio"), PSSFractionOff, PSSFraction, 0, 1);
//          FillRelDiffProfile<float>(profile("hPSSFracRatiovspt"), PSSFractionOff, PSSFraction, aOfflineTau->pt()/1000., 0, 1);
//          FillRelDiffProfile<float>(profile("hPSSFracRatiovseta"), PSSFractionOff, PSSFraction, aOfflineTau->eta(), 0, 1);
//          FillRelDiffProfile<float>(profile("hPSSFracRatiovsphi"), PSSFractionOff, PSSFraction, aOfflineTau->phi(), 0, 1);
//          FillRelDiffProfile<float>(profile("hPSSFracRatiovsmu"), PSSFractionOff, PSSFraction, mu, 0, 1);
//	}
      if(aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScale) && aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScale) && aOfflineTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScaleOff) && aOfflineTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScaleOff))
	{
	  EtRaw = (etEMAtEMScale + etHadAtEMScale)/CLHEP::GeV;
	  EtRawOff = (etEMAtEMScaleOff + etHadAtEMScaleOff)/CLHEP::GeV;
	  FillRelDiffHist(hist("hEtRawRatio"), EtRawOff, EtRaw, 0.1, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovspt"), EtRawOff, EtRaw, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovseta"), EtRawOff, EtRaw, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovsphi"), EtRawOff, EtRaw, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hEtRawRatiovsmu"), EtRawOff, EtRaw, mu, 0, 1);

          EMFraction = etEMAtEMScale / (etEMAtEMScale + etHadAtEMScale);
          EMFractionOff = etEMAtEMScaleOff / (etEMAtEMScaleOff + etHadAtEMScaleOff);
          FillRelDiffHist(hist("hEMFracRatio"), EMFractionOff, EMFraction, 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovspt"), EMFractionOff, EMFraction, aOfflineTau->pt()/1000., 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovseta"), EMFractionOff, EMFraction, aOfflineTau->eta(), 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovsphi"), EMFractionOff, EMFraction, aOfflineTau->phi(), 0, 1);
          FillRelDiffProfile<float>(profile("hEMFracRatiovsmu"), EMFractionOff, EMFraction, mu, 0, 1);

	}
      if(aEFTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScale) && aOfflineTau->detail(xAOD::TauJetParameters::etEMAtEMScale, etEMAtEMScaleOff))
	{
	  FillRelDiffHist(hist("hOffEFEMDiff"), etEMAtEMScaleOff, etEMAtEMScale, 0, 1);
	}
      if(aEFTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScale) && aOfflineTau->detail(xAOD::TauJetParameters::etHadAtEMScale, etHadAtEMScaleOff))
	{
	  FillRelDiffHist(hist("hOffEFHADDiff"), etHadAtEMScaleOff, etHadAtEMScale, 0, 1);
	}
    }
  else if(BDTinput_type == "1p_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsOffline/BDT/1p_nonCorrected");
      if(is1P)
	{
	  if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist) && aOfflineTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDistOff))
	    {
	      FillRelDiffHist(hist("hInnerTrkAvgDistRatio1P"), innerTrkAvgDistOff, innerTrkAvgDist, 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVspt1P"), innerTrkAvgDistOff, innerTrkAvgDist, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVsmu1P"),  innerTrkAvgDistOff, innerTrkAvgDist, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk) && aOfflineTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrkOff))
	    {
	      FillRelDiffHist(hist("hEtOverPtLeadTrkRatio1P"), etOverPtLeadTrkOff, etOverPtLeadTrk, 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVspt1P"), etOverPtLeadTrkOff, etOverPtLeadTrk, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVsmu1P"), etOverPtLeadTrkOff, etOverPtLeadTrk, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk) && aOfflineTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrkOff))
	    {
	      FillRelDiffHist(hist("hIpSigLeadTrkRatio1P"), ipSigLeadTrkOff, ipSigLeadTrk, 0., 2);
	      FillRelDiffProfile<float>(profile("hIpSigLeadTrkVspt1P"), ipSigLeadTrkOff, ipSigLeadTrk, aOfflineTau->pt()/1000., 0., 2);
	      FillRelDiffProfile<float>(profile("hIpSigLeadTrkVsmu1P"), ipSigLeadTrkOff, ipSigLeadTrk, mu, 0., 2);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac) && aOfflineTau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFracOff))
	    {
	      FillRelDiffHist(hist("hSumPtTrkFracRatio1P"), SumPtTrkFracOff, SumPtTrkFrac, 0., 1);
	      FillRelDiffProfile<float>(profile("hSumPtTrkFracVspt1P"), SumPtTrkFracOff, SumPtTrkFrac, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hSumPtTrkFracVsmu1P"), SumPtTrkFracOff, SumPtTrkFrac, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME) && aOfflineTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEMEOff))
	    {
	      FillRelDiffHist(hist("hChPiEMEOverCaloEMERatio1P"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVspt1P"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVsmu1P"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP) && aOfflineTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysPOff))
	    {
	      FillRelDiffHist(hist("hEMPOverTrkSysPRatio1P"), EMPOverTrkSysPOff, EMPOverTrkSysP, 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVspt1P"), EMPOverTrkSysPOff, EMPOverTrkSysP, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVsmu1P"), EMPOverTrkSysPOff, EMPOverTrkSysP, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff))
	    {
	      FillRelDiffHist(hist("hCentFracRatio1P"), centFracOff, centFrac, 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVspt1P"), centFracOff, centFrac, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVsmu1P"), centFracOff, centFrac, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox) && aOfflineTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApproxOff))
	    {
	      FillRelDiffHist(hist("hPtRatioEflowApproxRatio1P"), ptRatioEflowApproxOff, ptRatioEflowApprox, 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVspt1P"), ptRatioEflowApproxOff, ptRatioEflowApprox, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVsmu1P"), ptRatioEflowApproxOff, ptRatioEflowApprox, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax)&& aOfflineTau->detail(xAOD::TauJetParameters::dRmax, dRMaxOff))
	    {
	      FillRelDiffHist(hist("hDRmaxRatio1P"), dRMaxOff, dRMax, 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVspt1P"), dRMaxOff, dRMax, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVsmu1P"), dRMaxOff, dRMax, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMass) && aOfflineTau->detail(xAOD::TauJetParameters::topoInvMass, topoInvMassOff))
	    {
	      FillRelDiffHist(hist("hTopoInvMassRatio1P"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, 0., 1);
	      FillRelDiffProfile<float>(profile("hTopoInvMassVspt1P"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hTopoInvMassVsmu1P"), topoInvMassOff/CLHEP::GeV, topoInvMass/CLHEP::GeV, mu, 0., 1);
	    }
	}
    }
  else if(BDTinput_type == "mp_NonCorr")
    {
      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/EFVsOffline/BDT/mp_nonCorrected");
      if(isMP)
	{
	  if(aEFTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist) && aOfflineTau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDistOff))
	    {
	      FillRelDiffHist(hist("hInnerTrkAvgDistRatioMP"), innerTrkAvgDistOff, innerTrkAvgDist, 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVsptMP"), innerTrkAvgDistOff, innerTrkAvgDist, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hInnerTrkAvgDistVsmuMP"), innerTrkAvgDistOff, innerTrkAvgDist, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk) && aOfflineTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrkOff))
	    {
	      FillRelDiffHist(hist("hEtOverPtLeadTrkRatioMP"), etOverPtLeadTrkOff, etOverPtLeadTrk, 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVsptMP"), etOverPtLeadTrkOff, etOverPtLeadTrk,  aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEtOverPtLeadTrkVsmuMP"), etOverPtLeadTrkOff, etOverPtLeadTrk, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME) && aOfflineTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEMEOff))
	    {
	      FillRelDiffHist(hist("hChPiEMEOverCaloEMERatioMP"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVsptMP"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME,  aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hChPiEMEOvCaloEMEVsmuMP"), ChPiEMEOverCaloEMEOff, ChPiEMEOverCaloEME, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP) && aOfflineTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysPOff))
	    {
	      FillRelDiffHist(hist("hEMPOverTrkSysPRatioMP"), EMPOverTrkSysPOff, EMPOverTrkSysP, 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVsptMP"), EMPOverTrkSysPOff, EMPOverTrkSysP, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hEMPOverTrkSysPVsmuMP"), EMPOverTrkSysPOff, EMPOverTrkSysP, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::centFrac, centFrac) && aOfflineTau->detail(xAOD::TauJetParameters::centFrac, centFracOff))
	    {
	      FillRelDiffHist(hist("hCentFracRatioMP"), centFracOff, centFrac, 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVsptMP"), centFracOff, centFrac, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hCentFracVsmuMP"), centFracOff, centFrac, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApprox) && aOfflineTau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, ptRatioEflowApproxOff))
	    {
	      FillRelDiffHist(hist("hPtRatioEflowApproxRatioMP"), ptRatioEflowApproxOff, ptRatioEflowApprox, 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVsptMP"), ptRatioEflowApproxOff, ptRatioEflowApprox, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hPtRatioEflowApproxVsmuMP"), ptRatioEflowApproxOff, ptRatioEflowApprox, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::dRmax, dRMax)&& aOfflineTau->detail(xAOD::TauJetParameters::dRmax, dRMaxOff))
	    {
	      FillRelDiffHist(hist("hDRmaxRatioMP"), dRMaxOff, dRMax, 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVsptMP"), dRMaxOff, dRMax, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hDRmaxVsmuMP"), dRMaxOff, dRMax, mu, 0., 1);
	    }
	  if( aEFTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig) && aOfflineTau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSigOff))
	    {
	      FillRelDiffHist(hist("hTrFlightPathSigRatioMP"), trFlightPathSigOff, trFlightPathSig, 0., 2);
	      FillRelDiffProfile<float>(profile("hTrFlightPathSigVsptMP"), trFlightPathSigOff, trFlightPathSig, aOfflineTau->pt()/1000., 0., 2);
	      FillRelDiffProfile<float>(profile("hTrFlightPathSigVsmuMP"), trFlightPathSigOff, trFlightPathSig, mu, 0., 2);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys) && aOfflineTau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSysOff))
	    {
	      FillRelDiffHist(hist("hMassTrkSysRatioMP"), massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, 0., 1);
	      FillRelDiffProfile<float>(profile("hMassTrkSysVsptMP"),  massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hMassTrkSysVsmuMP"),  massTrkSysOff/CLHEP::GeV, massTrkSys/CLHEP::GeV, mu, 0., 1);
	    }
	  if(aEFTau->detail(xAOD::TauJetParameters::mEflowApprox, mEflowApprox) && aOfflineTau->detail(xAOD::TauJetParameters::mEflowApprox, mEflowApproxOff))
	    {
	      FillRelDiffHist(hist("hMEflowApproxRatioMP"), mEflowApproxOff, mEflowApprox, 0., 1);
	      FillRelDiffProfile<float>(profile("hMEflowApproxVsptMP"), mEflowApproxOff, mEflowApprox, aOfflineTau->pt()/1000., 0., 1);
	      FillRelDiffProfile<float>(profile("hMEflowApproxVsmuMP"), mEflowApproxOff, mEflowApprox, mu, 0., 1);
	    }
	}
    }
  else
    {
      ATH_MSG_WARNING("Wrong BDT input type Failed to fill the histograms.Check fillEFTauVsOffline(). Exiting.");
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------
// Testing methods
//------------------------------------------------------------------------------------
float HLTTauMonTool::PrescaleRetrieval(const std::string & trigItem, const std::string & level){
	std::string trig_item_EF = "HLT_"+trigItem;
	// trying to get the prescale value
	
	std::string l1_chain(LowerChain(trig_item_EF));
	int L1_PSCut = (int) getTDT()->getPrescale(l1_chain);
	float L1_PS = TrigConf::PrescaleSet::getPrescaleFromCut(L1_PSCut);
	float HLT_PS = getTDT()->getPrescale(trig_item_EF) / (float)L1_PSCut; // Remove the L1 cut from this
	float Total_PS = L1_PS * HLT_PS;
	ATH_MSG_DEBUG(trig_item_EF << ": L1 PS "<< L1_PS << ", HLT PS " << HLT_PS << ", Total PS " << Total_PS);
//	if(getTDT()->isPassedBits(trig_item_EF) & TrigDefs::EF_prescaled) ATH_MSG_WARNING(trig_item_EF << " is prescaled!!");
	//if(getTDT()->isPassedBits(l1_chain) & TrigDefs::L1_isPassedAfterPrescale) ATH_MSG_WARNING(l1_chain << " is prescaled!!");
	if(level=="L1") return L1_PS;
	else if(level=="HLT") return HLT_PS;
	else return Total_PS;
}


void HLTTauMonTool::testL1TopoNavigation(const std::string & trigItem){
	ATH_MSG_DEBUG("HLTTauMonTool::testL1TopoNavigation");
        std::string trig_item_EF = "HLT_"+trigItem;

	if (getTDT()->isPassed(trig_item_EF,m_HLTTriggerCondition)){
		ATH_MSG_WARNING("Passed "<<trig_item_EF);	
		Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,m_HLTTriggerCondition) );
		Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
		if(comb==combEnd){
			ATH_MSG_WARNING("No combination found!");
		} else if (f.getCombinations().size()>100) {
                	ATH_MSG_WARNING("Chain " << trig_item_EF << " has " << f.getCombinations().size() << " combinations. Looping over first 100");
               		combEnd = comb;
                	std::advance(combEnd,100);
                }

                const xAOD::EmTauRoIContainer* l1Tau_cont = 0;
                if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){
                                ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container");
                } else{
                                ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
                        }
                xAOD::EmTauRoIContainer::const_iterator itEMTau;
                xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();

		for(;comb!=combEnd;++comb){
		
			const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb->get<TrigRoiDescriptor>("initialRoI",m_L1TriggerCondition);
			std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();
			if(roi==roi_e){
				ATH_MSG_WARNING("Combination found with without RoIs!");
			}
			ATH_MSG_WARNING("Combination found with following RoIs:");
			for(; roi != roi_e; ++roi){
				if(roi->cptr()){
					ATH_MSG_WARNING("RoI Word " << roi->cptr()->roiWord() << ", eta " << roi->cptr()->eta() << ", phi " << roi->cptr()->phi() << ", roiId " << roi->cptr()->roiId() << ", l1Id " << roi->cptr()->l1Id() );
					for(itEMTau = l1Tau_cont->begin(); itEMTau!=itEMTau_e; ++itEMTau){
						if(roi->cptr()->roiWord()!=(*itEMTau)->roiWord()) continue;
						ATH_MSG_WARNING("EMTau RoI Word " << (*itEMTau)->roiWord() << ", eta " << (*itEMTau)->eta() << ", phi " << (*itEMTau)->phi() );
						const std::vector<std::string>& names = (*itEMTau)->thrNames();
						for(unsigned int i=0;i<names.size(); i++) ATH_MSG_WARNING("thrNames: " << names.at(i) );
					}
				}
			}
		}
//		Trig::FeatureContainer f_comp = ( getTDT()->features(trig_item_EF,TrigDefs::TrigComposite) );
//                Trig::FeatureContainer::combination_const_iterator comb_comp(f_comp.getCombinations().begin()), combEnd_comp(f_comp.getCombinations().end());
//                if(comb_comp==combEnd_comp){
//                        ATH_MSG_WARNING("No combination found with TrigDefs::TrigComposite!");
//                }
//                for(;comb_comp!=combEnd_comp;++comb_comp){
//                        const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb_comp->get<TrigRoiDescriptor>("initialRoI",TrigDefs::TrigComposite);
//                        std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();
//                        if(roi==roi_e){
//                                ATH_MSG_WARNING("Combination found with TrigDefs::TrigComposite without RoIs!");
//                        }
//                        ATH_MSG_WARNING("Combination found with TrigDefs::TrigComposite with following RoIs:");
//                        for(; roi != roi_e; ++roi){
//                                if(roi->cptr()){
//                                        ATH_MSG_WARNING("RoI type" << roi->cptr()->roiWord() << ", eta " << roi->cptr()->eta() << ", phi " << roi->cptr()->phi());    
//                                        const std::vector<std::string>& names = roi->cptr()->thrNames();
//                                        for(unsigned int i=0;i<names.size(); i++) ATH_MSG_WARNING("thrNames: " << names.at(i) );
//                                }
//                        }
//                }

	}
}

void  HLTTauMonTool::testClusterNavigation(const xAOD::TauJet *aEFTau){

  ATH_MSG_DEBUG("HLTTauMonTool::testClusterNavigation");

  const xAOD::Jet* pJetSeed = *(aEFTau->jetLink());  

  xAOD::JetConstituentVector::const_iterator clusItr  = pJetSeed->getConstituents().begin();
  xAOD::JetConstituentVector::const_iterator clusItrE = pJetSeed->getConstituents().end();
  if(clusItr==clusItrE) ATH_MSG_WARNING("Cluster vector Empty!!");
  for (int clusCount = 0; clusItr != clusItrE; ++clusItr, ++clusCount) {
    ATH_MSG_WARNING( "Tau Cluster " << clusCount << " pt = " << (*clusItr)->pt()    
		<< " eta = " << (*clusItr)->eta()
		<< " phi = " << (*clusItr)->phi() );
  }
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
const xAOD::EmTauRoI * HLTTauMonTool::findLVL1_ROI(const TrigRoiDescriptor * roiDesc){
    
    const xAOD::EmTauRoIContainer*  l1Tau_cont = 0;
    if ( m_storeGate->retrieve( l1Tau_cont, "LVL1EmTauRoIs").isFailure() ){ // retrieve arguments: container type, container key
        ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoI container. Exiting.");
        return 0;
    } else{
        ATH_MSG_DEBUG("found LVL1EmTauRoI in SG");
    }
    
    xAOD::EmTauRoIContainer::const_iterator itEMTau = l1Tau_cont->begin();
    xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1Tau_cont->end();
    
    unsigned int id = roiDesc->roiWord();
    for( xAOD::EmTauRoIContainer::const_iterator it=itEMTau; it!=itEMTau_e;++it){
        if(id==(*it)->roiWord()) return (*it);
    }
    return 0;
}


//-----------------------------------------------------
// Useful tools and matching methods
//-----------------------------------------------------

void HLTTauMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {
    
    quo->Divide(num, den, 1., 1., "b");
}

void HLTTauMonTool::divide(TH2 *num, TH2 *den, TH2 *quo) {

    quo->Divide(num, den, 1., 1., "b");
}

void HLTTauMonTool::divide(const std::string & h_name_num, const std::string & h_name_den, const std::string & h_name_div, const std::string & dir){
    
    divide(
           hist(h_name_num,dir),
           hist(h_name_den,dir),
           hist(h_name_div,dir)
           );
}

void HLTTauMonTool::divide2(const std::string & h_name_num, const std::string & h_name_den, const std::string & h_name_div, const std::string & dir){

    divide(
           hist2(h_name_num,dir),
           hist2(h_name_den,dir),
           hist2(h_name_div,dir)
           );
}

float  HLTTauMonTool::deltaEta(float eta1, float eta2)
{
    float dEta = eta1-eta2;
    return dEta;
}

float  HLTTauMonTool::deltaPhi(float phi1, float phi2)
{
    float PHI=fabs(phi1-phi2);
    return (PHI<=PI)? PHI : TWOPI-PHI;
}

float HLTTauMonTool::deltaR(float eta1, float eta2, float phi1, float phi2)
{
    
    float dEta = eta1 - eta2;
    float dPhi = deltaPhi(phi1, phi2);
    
    return sqrt(dEta*dEta + dPhi*dPhi);
    
}

float HLTTauMonTool::deltaR(const xAOD::TauJet* aEFTau, const xAOD::EmTauRoI* l1Tau)
{
    
    return deltaR(aEFTau->eta(), l1Tau->eta(), aEFTau->phi(), l1Tau->phi());
    
}

bool HLTTauMonTool::Match_Offline_L1(const xAOD::TauJet *aOfflineTau, const std::string & trigItem){
    
    bool matchedL1=false;
    float tmpR = 5.0;
    
    // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("EF_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
    const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFall = (getTDT()->features("HLT_"+trigItem,TrigDefs::alsoDeactivateTEs)).get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
    
    std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFall.begin();
    
    for (; CI!=vec_L1roi_EFall.end();++CI){
        if(!(CI->cptr())) continue;
        const xAOD::EmTauRoI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
        if(!aEmTau_ROI) continue;
        if ( deltaR(aOfflineTau, aEmTau_ROI ) < tmpR) {
            tmpR = deltaR(aOfflineTau, aEmTau_ROI);
        }
    }
    
    
    if (tmpR <0.2) matchedL1=true;
    return matchedL1;
    
}


bool HLTTauMonTool::Match_Offline_EF(const xAOD::TauJet *aOfflineTau, const std::string & trigItem){
    
    bool matchedEF=false;
    float tmpR = 5.0;
    
    // const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("EF_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
    const std::vector<Trig::Feature<TrigRoiDescriptor> > vec_L1roi_EFpass = (getTDT()->features("HLT_"+trigItem)).get<TrigRoiDescriptor>("initialRoI");
    std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator CI = vec_L1roi_EFpass.begin();
    
    for(; CI!=vec_L1roi_EFpass.end(); ++CI){
        if(!(CI->cptr())) continue;
        const xAOD::EmTauRoI *aEmTau_ROI = findLVL1_ROI(CI->cptr());
        if(!aEmTau_ROI) continue;
        if ( deltaR(aOfflineTau, aEmTau_ROI) < tmpR) {
            tmpR = deltaR(aOfflineTau, aEmTau_ROI);
        }
        
    }
    
    if (tmpR < 0.2) matchedEF = true;
    
    return matchedEF;
    
}

void HLTTauMonTool::plotUnderOverFlow(TH1* hist){
    
    float underContent = hist->GetBinContent(0);
    float underError = hist->GetBinError(0);
    float firstContent = hist->GetBinContent(1)+underContent;
    float firstError = sqrt(hist->GetBinError(1)*hist->GetBinError(1) + underError*underError);
    
    hist->SetBinContent(1,firstContent);
    hist->SetBinError(1,firstError);
    hist->SetBinContent(0,0);
    hist->SetBinError(0,0);
    
    float overContent = hist->GetBinContent(hist->GetNbinsX()+1);
    float overError = hist->GetBinError(hist->GetNbinsX()+1);
    float lastContent = hist->GetBinContent(hist->GetNbinsX())+overContent;
    float lastError = sqrt(hist->GetBinError(hist->GetNbinsX())*hist->GetBinError(hist->GetNbinsX()) + overError*overError);
    
    hist->SetBinContent(hist->GetNbinsX(),lastContent);
    hist->SetBinError(hist->GetNbinsX(),lastError);
    hist->SetBinContent(hist->GetNbinsX()+1,0);
    hist->SetBinError(hist->GetNbinsX()+1,0);  
    
}

bool HLTTauMonTool::Selection(const xAOD::TauJet *aTau){
  
   int nTrk = aTau->nTracks();
   float pt = aTau->pt();
   float eta = aTau->eta();
   float phi = aTau->phi();

   if(m_selection_nTrkMax>-1 && nTrk>m_selection_nTrkMax) return false;
   if(m_selection_nTrkMin>-1 && nTrk<m_selection_nTrkMin) return false;
   if(m_selection_ptMax>-1 && pt>m_selection_ptMax) return false;
   if(m_selection_ptMin>-1 && pt<m_selection_ptMin) return false;
   if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) return false;
   if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) return false;
   if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) return false;
   if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) return false;

   return true;
}

bool HLTTauMonTool::Selection(const xAOD::EmTauRoI *aTau){
  
   float pt = aTau->tauClus();
   float eta = aTau->eta();
   float phi = aTau->phi();

   if(m_selection_ptMax>-1 && pt>m_selection_ptMax) return false;
   if(m_selection_ptMin>-1 && pt<m_selection_ptMin) return false;
   if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) return false;
   if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) return false;
   if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) return false;
   if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) return false;

   return true;
}

//=================================
// Turn On Curve Methods
//=================================

void HLTTauMonTool::examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const
{
    xTruthParticle.auxdecor<bool>("IsLeptonicTau") = false;
    
    TLorentzVector VisSumTLV;
    xTruthParticle.auxdecor<double>("pt_vis") = 0;
    xTruthParticle.auxdecor<double>("eta_vis") = 0;
    xTruthParticle.auxdecor<double>("phi_vis") = 0;
    xTruthParticle.auxdecor<double>("m_vis") = 0;
    xTruthParticle.auxdecor<int>("childChargeSum") = 0;
    xTruthParticle.auxdecor<int>("nTracks") = 0;
   
    if(!xTruthParticle.hasDecayVtx()) return;
    
    const xAOD::TruthVertex* decayvtx = xTruthParticle.decayVtx();
    if(decayvtx)
    {
        const std::size_t nChildren = decayvtx->nOutgoingParticles();
        for ( std::size_t iChild = 0; iChild != nChildren; ++iChild )
        {
            const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
	    if((abs(child->pdgId()) == 12 || abs(child->pdgId()) == 14 || abs(child->pdgId()) == 16)) continue;
	    if(child->status()==3) continue;
	    ATH_MSG_DEBUG("child "<< child->pdgId() << ", status "<< child->status() << ", charge "<< child->charge());
            if ((abs(child->pdgId()) == 11 || abs(child->pdgId()) == 13 || abs(child->pdgId()) == 15)) xTruthParticle.auxdecor<bool>("IsLeptonicTau") = true;
            VisSumTLV += child->p4();
            xTruthParticle.auxdecor<int>("childChargeSum") += child->charge();
	    xTruthParticle.auxdecor<int>("nTracks") += abs(child->charge());
        }
    }

    xTruthParticle.auxdecor<double>("pt_vis")  = VisSumTLV.Pt();
    xTruthParticle.auxdecor<double>("eta_vis") = VisSumTLV.Eta();
    xTruthParticle.auxdecor<double>("phi_vis") = VisSumTLV.Phi();
    xTruthParticle.auxdecor<double>("m_vis")   = VisSumTLV.M();
    if(xTruthParticle.auxdecor<int>("childChargeSum")!=xTruthParticle.charge() || xTruthParticle.auxdecor<int>("nTracks")%2==0)
      { 
	ATH_MSG_WARNING("Strange tau: charge " << xTruthParticle.auxdecor<int>("childChargeSum") << " and " << xTruthParticle.auxdecor<int>("nTracks")  << " tracks");
	const std::size_t nChildren = decayvtx->nOutgoingParticles();
	for ( std::size_t iChild = 0; iChild != nChildren; ++iChild )
	  {
	    const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
	    ATH_MSG_WARNING("child "<< child->pdgId() << ", status "<< child->status() << ", charge "<< child->charge());
	  }
      }
}


StatusCode HLTTauMonTool::TauEfficiency(const std::string & trigItem, const std::string & TauDenom){
	ATH_MSG_DEBUG("Efficiency wrt "<< TauDenom << " for trigItem" << trigItem);
	if(trigItem == "Dump") {ATH_MSG_DEBUG("Not computing efficiencies for Dump"); return StatusCode::SUCCESS;};

	std::string l1_chain(LowerChain("HLT_"+trigItem));

	std::vector<TLorentzVector> tlv_TauDenom;
	std::vector<int> ntrk_TauDenom;
	std::vector<bool> good_TauDenom;
	std::vector<TLorentzVector> tlv_tmp;

	float mu(Pileup());
	int nvtx(PrimaryVertices());


	// attempt to get an unbiased fake tau efficiency with W->munu events?
	bool passed_muonTrigger = getTDT()->isPassed("HLT_mu26_imedium") || getTDT()->isPassed("HLT_mu24_imedium") || getTDT()->isPassed("HLT_mu24_iloose_L1MU15") || getTDT()->isPassed("HLT_mu20_iloose_L1MU15");
	const xAOD::MuonContainer * muon_cont = 0;
	TLorentzVector Muon_TLV(0.,0.,0.,0.);
	int nmuon(0);
	if(m_L1StringCondition=="allowResurrectedDecision" || m_HLTStringCondition=="allowResurrectedDecision"){ 
		if(!passed_muonTrigger) return StatusCode::SUCCESS;
		if(evtStore()->retrieve(muon_cont, "Muons").isFailure()){
			ATH_MSG_WARNING("Failed to retrieve Muons container. skipping");
			return StatusCode::SUCCESS;
		}
		xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
		for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr){
			Muon_TLV = (*muonItr)->p4();
			if(Muon_TLV.Pt()<20000.) continue;
			if((*muonItr)->author() != xAOD::Muon::MuidCo) continue;
			if(! (*muonItr)->passesIDCuts() ) continue;
			nmuon++;
		}
		if(nmuon>1) return StatusCode::SUCCESS;
	}


	// build vector of taus in denominator:
	if(TauDenom.find("Truth")!=std::string::npos && m_truth){
		const xAOD::TruthParticleContainer* truth_cont = 0;
		if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
    			ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
    			return StatusCode::FAILURE;
  		}
		xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
		for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    		 {
      			if(abs((*truthItr)->pdgId()) == 15) ATH_MSG_DEBUG("Tau with status " << (*truthItr)->status() << " and charge " << (*truthItr)->charge());
      			if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) // || (*truthItr)->status() == 10902))
        		 {
	  			examineTruthTau(**truthItr);
	  			double pt  = (*truthItr)->auxdata<double>("pt_vis");
	  			double eta = (*truthItr)->auxdata<double>("eta_vis");
	  			double phi = (*truthItr)->auxdata<double>("phi_vis");
	  			double m   = (*truthItr)->auxdata<double>("m_vis");
	  			bool lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
	  			int ntracks = (*truthItr)->auxdata<int>("nTracks");
	  			if(pt < m_effOffTauPtCut || lep || fabs(eta) > 2.47 ) continue;
				TLorentzVector TruthTauTLV;
				TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
				tlv_TauDenom.push_back(TruthTauTLV);
				ntrk_TauDenom.push_back(ntracks);
				good_TauDenom.push_back(true);
			 }
		 }
	}

	if(TauDenom.find("Reco")!=std::string::npos){

		bool addToDenom(false);
		if(TauDenom.find("Truth")==std::string::npos || !m_truth) addToDenom = true;
 
		const xAOD::TauJetContainer * reco_cont = 0;
		if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() ){
    			ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
    			return StatusCode::FAILURE;
  		}
		xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
		for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
            	 {
	      		if(!Selection(*recoItr)) continue;
              		TLorentzVector TauTLV = (*recoItr)->p4();
              		double eta_Tau = TauTLV.Eta();
              		if(fabs(eta_Tau) > 2.47) continue;
 	      		double pt_Tau = TauTLV.Pt();
	      		if(pt_Tau<m_effOffTauPtCut) continue;
	      		int ntrack_TAU = (*recoItr)->nTracks();
	      		if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
			if(TauDenom.find("ID")!=std::string::npos){
   	      			bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
              			//bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
              			//bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
				//bool best_tau = good_tau && not_a_electron && not_a_muon;
  	      			bool best_tau = good_tau;
				if(!best_tau) continue;		
			}
			if(addToDenom){	
				tlv_TauDenom.push_back(TauTLV);
                                ntrk_TauDenom.push_back(ntrack_TAU);
                                good_TauDenom.push_back(true);		
			}
			if(!addToDenom){
				tlv_tmp.push_back(TauTLV);
			}
		}
		
		if(!addToDenom){
			for(unsigned int i=0;i<tlv_TauDenom.size();i++){
				if(!good_TauDenom.at(i)) continue;
				bool matched(false);
				for(unsigned int j=0;j<tlv_tmp.size();j++){
					if(tlv_tmp.at(j).DeltaR(tlv_TauDenom.at(i))<0.2) matched = true;
				}
				if(!matched) good_TauDenom.at(i) = false; 
			}
			tlv_tmp.clear();
		}
	}

	if(TauDenom.find("Presel")!=std::string::npos){
		bool addToDenom(false);
                if( (TauDenom.find("Truth")==std::string::npos || !m_truth) && TauDenom.find("Reco")==std::string::npos) addToDenom = true;
		
		const xAOD::TauJetContainer * hlt_cont = 0;
		if( evtStore()->retrieve(hlt_cont, "HLT_xAOD__TauJetContainer_TrigTauRecPreselection").isFailure() ){
    			ATH_MSG_WARNING("Failed to retrieve  HLT_xAOD__TauJetContainer_TrigTauRecPreselection container. Exiting.");
    			return StatusCode::FAILURE;
  		}
		xAOD::TauJetContainer::const_iterator hltItr, hlt_cont_end = hlt_cont->end();
		for(hltItr=hlt_cont->begin(); hltItr!=hlt_cont_end; ++hltItr){
			TLorentzVector hltTLV = (*hltItr)->p4();
			int ntrack_TAU = (*hltItr)->nTracks();
			int nWideTrack_TAU = (*hltItr)->nWideTracks();	
			if( !L1TauMatching(l1_chain, hltTLV, 0.3) ) continue;
			if(ntrack_TAU>3) continue;		
			if(trigItem.find("perf")==std::string::npos) if(ntrack_TAU==0 || nWideTrack_TAU>1) continue;
			if(trigItem.find("25")!=std::string::npos) if(hltTLV.Pt()<25000.) continue;
			if(trigItem.find("35")!=std::string::npos) if(hltTLV.Pt()<35000.) continue;
			if(trigItem.find("80")!=std::string::npos) if(hltTLV.Pt()<80000.) continue;
			if(trigItem.find("125")!=std::string::npos) if(hltTLV.Pt()<125000.) continue;
			if(trigItem.find("160")!=std::string::npos) if(hltTLV.Pt()<160000.) continue;
			if(addToDenom){
                                tlv_TauDenom.push_back(hltTLV);
                                ntrk_TauDenom.push_back(ntrack_TAU);
                                good_TauDenom.push_back(true);
                        }
                        if(!addToDenom){
                                tlv_tmp.push_back(hltTLV);
                        }
		}

		if(!addToDenom){
                        for(unsigned int i=0;i<tlv_TauDenom.size();i++){
                                if(!good_TauDenom.at(i)) continue;
                                bool matched(false);
                                for(unsigned int j=0;j<tlv_tmp.size();j++){
                                        if(tlv_tmp.at(j).DeltaR(tlv_TauDenom.at(i))<0.2) matched = true;
                                }
                                if(!matched) good_TauDenom.at(i) = false;
                        }
                        tlv_tmp.clear();
                }
	}


	// loop over taus in denominator and match with L1 and HLT taus:
	for(unsigned int i=0;i<tlv_TauDenom.size();i++){
	  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/RecoEfficiency");
	  if(!good_TauDenom.at(i)) continue;
	  int ntracks = ntrk_TauDenom.at(i);
	  if(m_L1StringCondition=="allowResurrectedDecision" || m_HLTStringCondition=="allowResurrectedDecision"){
	    if(tlv_TauDenom.at(i).DeltaR(Muon_TLV)<0.4) continue;
	  }
	  double pt = tlv_TauDenom.at(i).Pt();
	  double eta = tlv_TauDenom.at(i).Eta();
	  double phi = tlv_TauDenom.at(i).Phi();
	  
	  if(m_selection_nTrkMax>-1 && ntracks>m_selection_nTrkMax) continue;
   	  if(m_selection_nTrkMin>-1 && ntracks<m_selection_nTrkMin) continue;
   	  if(m_selection_ptMax>-1 && pt>m_selection_ptMax) continue;
   	  if(m_selection_ptMin>-1 && pt<m_selection_ptMin) continue;
   	  if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) continue;
   	  if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) continue;
   	  if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) continue;
   	  if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) continue;
		
		//      		hist("hRecoTauPtDenom")->Fill(pt/1000.);
		//      		if(ntracks == 1) hist("hRecoTauPt1PDenom")->Fill(pt/1000.);
		//      		if(ntracks > 1)  hist("hRecoTauPt3PDenom")->Fill(pt/1000.);
		//		hist("hRecoTauEtaDenom")->Fill(eta);
		//      		hist("hRecoTauPhiDenom")->Fill(phi);
		//      		hist("hRecoTauNTrackDenom")->Fill(ntracks);
		//      		hist("hRecoTauNVtxDenom")->Fill(nvtx);
		//      		hist("hRecoTauMuDenom")->Fill(mu);
	  hist2("hRecoTauEtaVsPhiDenom")->Fill(eta,phi);				
		// get prescale wait:
		//float L1_PS = PrescaleRetrieval(trigItem,"L1");
		//float Tot_PS = PrescaleRetrieval(trigItem,"TOT");
		//ATH_MSG_WARNING(trigItem << " " << L1_PS << " " << Tot_PS);
	  if(m_bootstrap && !getTDT()->isPassed("L1_TAU12")) continue; 		
		//const unsigned int HLT_bits = getTDT()->isPassedBits("HLT_"+trigItem);
		//if( !(HLT_bits & TrigDefs::EF_resurrected) ) continue;

	  if( HLTTauMatching(trigItem, tlv_TauDenom.at(i), 0.2)  ){
		  //	hist("hRecoHLTPtNum")->Fill(pt/1000.);
		  //		  if(ntracks == 1) {hist("hRecoHLTPt1PNum")->Fill(pt/1000.);profile("TProfRecoHLTPt1PEfficiency")->Fill(pt/1000.,1);}
		  //	       	if(ntracks > 1) {hist("hRecoHLTPt3PNum")->Fill(pt/1000.);profile("TProfRecoHLTPt3PEfficiency")->Fill(pt/1000.,1);}
		  profile("TProfRecoHLTPtEfficiency")->Fill(pt/1000.,1);
		  if(ntracks == 1) {profile("TProfRecoHLTPt1PEfficiency")->Fill(pt/1000.,1);}
		  if(ntracks > 1) {profile("TProfRecoHLTPt3PEfficiency")->Fill(pt/1000.,1);}
		  profile("TProfRecoHLTHighPtEfficiency")->Fill(pt/1000.,1);
		  if(ntracks == 1) {profile("TProfRecoHLTHighPt1pEfficiency")->Fill(pt/1000.,1);}
		  if(ntracks > 1) {profile("TProfRecoHLTHighPt3pEfficiency")->Fill(pt/1000.,1);}
		  //	  		hist("hRecoHLTEtaNum")->Fill(eta);
		  //	  		hist("hRecoHLTPhiNum")->Fill(phi);
		  //	  		hist("hRecoHLTNTrackNum")->Fill(ntracks);
			//	  		hist("hRecoHLTNVtxNum")->Fill(nvtx);
			//	  		hist("hRecoHLTMuNum")->Fill(mu);
		  hist2("hRecoHLTEtaVsPhiNum")->Fill(eta,phi);
		  profile("TProfRecoHLTEtaEfficiency")->Fill(eta,1);
		  profile("TProfRecoHLTPhiEfficiency")->Fill(phi,1);
		  profile("TProfRecoHLTNTrackEfficiency")->Fill(ntracks,1);
		  profile("TProfRecoHLTNVtxEfficiency")->Fill(nvtx,1);
		  profile("TProfRecoHLTMuEfficiency")->Fill(mu,1);
	  }
		
	  if( !HLTTauMatching(trigItem, tlv_TauDenom.at(i), 0.2)  ){
		  if(ntracks == 1) profile("TProfRecoHLTPt1PEfficiency")->Fill(pt/1000.,0);
                  if(ntracks > 1) profile("TProfRecoHLTPt3PEfficiency")->Fill(pt/1000.,0);
		  profile("TProfRecoHLTHighPtEfficiency")->Fill(pt/1000.,0);
		  if(ntracks == 1) {profile("TProfRecoHLTHighPt1pEfficiency")->Fill(pt/1000.,0);}
		  if(ntracks > 1) {profile("TProfRecoHLTHighPt3pEfficiency")->Fill(pt/1000.,0);}
		  profile("TProfRecoHLTPtEfficiency")->Fill(pt/1000.,0);
		  profile("TProfRecoHLTEtaEfficiency")->Fill(eta,0);
		  profile("TProfRecoHLTPhiEfficiency")->Fill(phi,0);
		  profile("TProfRecoHLTNTrackEfficiency")->Fill(ntracks,0);
		  profile("TProfRecoHLTNVtxEfficiency")->Fill(nvtx,0);
		  profile("TProfRecoHLTMuEfficiency")->Fill(mu,0);
  
	  }

          if( L1TauMatching(l1_chain, tlv_TauDenom.at(i), 0.3)  ){
		  //                        hist("hRecoL1PtNum")->Fill(pt/1000.);
		  //                        if(ntracks == 1) {hist("hRecoL1Pt1PNum")->Fill(pt/1000.);profile("TProfRecoL1Pt1PEfficiency")->Fill(pt/1000.,1);}
		  //                        if(ntracks > 1) {hist("hRecoL1Pt3PNum")->Fill(pt/1000.);profile("TProfRecoL1Pt3PEfficiency")->Fill(pt/1000.,1);}
                        if(ntracks == 1) {profile("TProfRecoL1Pt1PEfficiency")->Fill(pt/1000.,1);}
                        if(ntracks > 1) {profile("TProfRecoL1Pt3PEfficiency")->Fill(pt/1000.,1);}
                        if(ntracks == 1) {profile("TProfRecoL1HighPt1PEfficiency")->Fill(pt/1000.,1);}
                        if(ntracks > 1) {profile("TProfRecoL1HighPt3PEfficiency")->Fill(pt/1000.,1);}
			//                        hist("hRecoL1EtaNum")->Fill(eta);
			//                        hist("hRecoL1PhiNum")->Fill(phi);
			//                        hist("hRecoL1NTrackNum")->Fill(ntracks);
			//                        hist("hRecoL1NVtxNum")->Fill(nvtx);
			//                        hist("hRecoL1MuNum")->Fill(mu);
			hist2("hRecoL1EtaVsPhiNum")->Fill(eta,phi);
			profile("TProfRecoL1PtEfficiency")->Fill(pt/1000.,1);
			profile("TProfRecoL1HighPtEfficiency")->Fill(pt/1000.,1);
			profile("TProfRecoL1EtaEfficiency")->Fill(eta,1);
			profile("TProfRecoL1PhiEfficiency")->Fill(phi,1);
			profile("TProfRecoL1NTrackEfficiency")->Fill(ntracks,1);
			profile("TProfRecoL1NVtxEfficiency")->Fill(nvtx,1);
			profile("TProfRecoL1MuEfficiency")->Fill(mu,1);
          }

	  if( !L1TauMatching(l1_chain, tlv_TauDenom.at(i), 0.3)  ){
		  if(ntracks == 1) profile("TProfRecoL1Pt1PEfficiency")->Fill(pt/1000.,0);
		  if(ntracks > 1) profile("TProfRecoL1Pt3PEfficiency")->Fill(pt/1000.,0);
		  if(ntracks == 1) {profile("TProfRecoL1HighPt1PEfficiency")->Fill(pt/1000.,0);}
		  if(ntracks > 1) {profile("TProfRecoL1HighPt3PEfficiency")->Fill(pt/1000.,0);}
		  profile("TProfRecoL1PtEfficiency")->Fill(pt/1000.,0);
		  profile("TProfRecoL1HighPtEfficiency")->Fill(pt/1000.,0);
		  profile("TProfRecoL1EtaEfficiency")->Fill(eta,0);
		  profile("TProfRecoL1PhiEfficiency")->Fill(phi,0);
		  profile("TProfRecoL1NTrackEfficiency")->Fill(ntracks,0);
		  profile("TProfRecoL1NVtxEfficiency")->Fill(nvtx,0);
		  profile("TProfRecoL1MuEfficiency")->Fill(mu,0);
	  }
	
	}

	const xAOD::TauJetContainer * reco_cont = 0;
	if(evtStore()->retrieve(reco_cont, "TauJets").isFailure())
	  {
	    ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
	    return StatusCode::FAILURE;
	  }
	xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
	for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
	  {
	    if(!Selection(*recoItr)) continue;
	    TLorentzVector TauTLV = (*recoItr)->p4();
	    double eta = TauTLV.Eta();
	    double phi = TauTLV.Phi();
	    if(fabs(eta) > 2.47) continue;
	    double pt = TauTLV.Pt();
	    int ntracks = (*recoItr)->nTracks();
	    if(ntracks!=1 && ntracks!=3 ) continue;
	    bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
	    if(good_tau){

	      //	      std::cout<<"HLT"<<trigItem<<std::endl;
	      if(trigItem=="tau160_idperf_tracktwo"){
                setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");
		if( HLTTauMatching("tau160_perf_tracktwo", TauTLV, 0.2)  ){
		  profile("TProfRecoHLT160PtEfficiency")->Fill(pt/1000.,1);}
		else if( HLTTauMatching("tau160_idperf_tracktwo", TauTLV, 0.2)  ){
		  profile("TProfRecoHLT160PtEfficiency")->Fill(pt/1000.,0);}
	      }


	      if(trigItem=="tau25_idperf_tracktwo"){
		setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");
		if( HLTTauMatching("tau25_idperf_tracktwo", TauTLV, 0.2)  ){
		  //		  hist("hRecoTau25PtDenom")->Fill(pt/1000.);
		  //		  if(ntracks == 1) hist("hRecoTau25Pt1PDenom")->Fill(pt/1000.);
		  //		  if(ntracks > 1)  hist("hRecoTau25Pt3PDenom")->Fill(pt/1000.);
		  //		  hist("hRecoTau25EtaDenom")->Fill(eta);
		  //		  hist("hRecoTau25PhiDenom")->Fill(phi);
		  //		  hist("hRecoTau25NTrackDenom")->Fill(ntracks);
		  //		  hist("hRecoTau25NVtxDenom")->Fill(nvtx);
		  //		  hist("hRecoTau25MuDenom")->Fill(mu);
		  hist2("hRecoTau25EtaVsPhiDenom")->Fill(eta,phi);
		}
		if( HLTTauMatching("tau25_perf_tracktwo", TauTLV, 0.2)  )
		  {		  
		    //		    hist("hRecoHLT25PtNum")->Fill(pt/1000.);
		    //		    if(ntracks == 1) hist("hRecoHLT25Pt1PNum")->Fill(pt/1000.);
		    //		    if(ntracks > 1) hist("hRecoHLT25Pt3PNum")->Fill(pt/1000.);
		    //		    hist("hRecoHLT25EtaNum")->Fill(eta);
		    //		    hist("hRecoHLT25PhiNum")->Fill(phi);
		    //		    hist("hRecoHLT25NTrackNum")->Fill(ntracks);
		    //		    hist("hRecoHLT25NVtxNum")->Fill(nvtx);
		    //		    hist("hRecoHLT25MuNum")->Fill(mu);
		    hist2("hRecoHLT25EtaVsPhiNum")->Fill(eta,phi);
		    profile("TProfRecoHLT25PtEfficiency")->Fill(pt/1000.,1);
		    if(ntracks==1)   profile("TProfRecoHLT25Pt1PEfficiency")->Fill(pt/1000.,1);
		    if(ntracks>1)   profile("TProfRecoHLT25Pt3PEfficiency")->Fill(pt/1000.,1);
		    profile("TProfRecoHLT25EtaEfficiency")->Fill(eta,1);
		    profile("TProfRecoHLT25PhiEfficiency")->Fill(phi,1);
		    profile("TProfRecoHLT25NTrackEfficiency")->Fill(ntracks,1);
		    profile("TProfRecoHLT25NVtxEfficiency")->Fill(nvtx,1);
		    profile("TProfRecoHLT25MuEfficiency")->Fill(mu,1);}
		else if( HLTTauMatching("tau25_idperf_tracktwo", TauTLV, 0.2)  ){
		  profile("TProfRecoHLT25PtEfficiency")->Fill(pt/1000.,0);
		  profile("TProfRecoHLT25EtaEfficiency")->Fill(eta,0);
		  if(ntracks==1)   profile("TProfRecoHLT25Pt1PEfficiency")->Fill(pt/1000.,0);
		  if(ntracks>1)   profile("TProfRecoHLT25Pt3PEfficiency")->Fill(pt/1000.,0);
		  profile("TProfRecoHLT25PhiEfficiency")->Fill(phi,0);
		  profile("TProfRecoHLT25NTrackEfficiency")->Fill(ntracks,0);
		  profile("TProfRecoHLT25NVtxEfficiency")->Fill(nvtx,0);
		  profile("TProfRecoHLT25MuEfficiency")->Fill(mu,0);		

		  
		}
	      }
	   
	      if(trigItem=="tau160_perf_tracktwo"){
		setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");
		if( HLTTauMatching("tau160_medium1_tracktwo", TauTLV, 0.2)  ){
		  profile("TProfRecoHLT160PtEfficiency_2")->Fill(pt/1000.,1);}
		else if( HLTTauMatching("tau160_perf_tracktwo", TauTLV, 0.2)  ){
		  profile("TProfRecoHLT160PtEfficiency_2")->Fill(pt/1000.,0);}
	      }
		
	      if(trigItem=="tau25_perf_tracktwo"){
		setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");
		  if( HLTTauMatching("tau25_perf_tracktwo", TauTLV, 0.2)  ){
		    //		    hist("hRecoTau25PtDenom_2")->Fill(pt/1000.);
		    //		    if(ntracks == 1) hist("hRecoTau25Pt1PDenom_2")->Fill(pt/1000.);
		    //		    if(ntracks > 1)  hist("hRecoTau25Pt3PDenom_2")->Fill(pt/1000.);
		    //		    hist("hRecoTau25EtaDenom_2")->Fill(eta);
		    //		    hist("hRecoTau25PhiDenom_2")->Fill(phi);
		    //		    hist("hRecoTau25NTrackDenom_2")->Fill(ntracks);
		    //		    hist("hRecoTau25NVtxDenom_2")->Fill(nvtx);
		    //		    hist("hRecoTau25MuDenom_2")->Fill(mu);
		    hist2("hRecoTau25EtaVsPhiDenom_2")->Fill(eta,phi);
		  }
		  
		  if( HLTTauMatching("tau25_medium1_tracktwo", TauTLV, 0.2)  ){
		    //		    hist("hRecoHLT25PtNum_2")->Fill(pt/1000.);
		    if(ntracks == 1) {
		      //		      hist("hRecoHLT25Pt1PNum_2")->Fill(pt/1000.);
		      profile("TProfRecoHLT25Pt1PEfficiency_2")->Fill(pt/1000.,1);}
		    if(ntracks > 1) {
		      //		      hist("hRecoHLT25Pt3PNum_2")->Fill(pt/1000.);
		      profile("TProfRecoHLT25Pt3PEfficiency_2")->Fill(pt/1000.,1);}
		    //		    hist("hRecoHLT25EtaNum_2")->Fill(eta);
		    //		    hist("hRecoHLT25PhiNum_2")->Fill(phi);
		    //		    hist("hRecoHLT25NTrackNum_2")->Fill(ntracks);
		    //		    hist("hRecoHLT25NVtxNum_2")->Fill(nvtx);
		    //		    hist("hRecoHLT25MuNum_2")->Fill(mu);
		    hist2("hRecoHLT25EtaVsPhiNum_2")->Fill(eta,phi);
		    profile("TProfRecoHLT25PtEfficiency_2")->Fill(pt/1000.,1);
		    profile("TProfRecoHLT25EtaEfficiency_2")->Fill(eta,1);
		    profile("TProfRecoHLT25PhiEfficiency_2")->Fill(phi,1);
		    profile("TProfRecoHLT25NTrackEfficiency_2")->Fill(ntracks,1);
		    profile("TProfRecoHLT25NVtxEfficiency_2")->Fill(nvtx,1);
		    profile("TProfRecoHLT25MuEfficiency_2")->Fill(mu,1);
		  }
		  else if( HLTTauMatching("tau25_perf_tracktwo", TauTLV, 0.2)  ){
		    if(ntracks==1)   profile("TProfRecoHLT25Pt1PEfficiency_2")->Fill(pt/1000.,0);
		    if(ntracks>1)   profile("TProfRecoHLT25Pt3PEfficiency_2")->Fill(pt/1000.,0);
		    profile("TProfRecoHLT25PtEfficiency_2")->Fill(pt/1000.,0);
		    profile("TProfRecoHLT25EtaEfficiency_2")->Fill(eta,0);
		    profile("TProfRecoHLT25PhiEfficiency_2")->Fill(phi,0);
		    profile("TProfRecoHLT25NTrackEfficiency_2")->Fill(ntracks,0);
		    profile("TProfRecoHLT25NVtxEfficiency_2")->Fill(nvtx,0);
		    profile("TProfRecoHLT25MuEfficiency_2")->Fill(mu,0);		
		  }	 
	      }   
	    }
	  }	

	  return StatusCode::SUCCESS;
}


//StatusCode HLTTauMonTool::TauEfficiencyCombo(const std::string & trigItem){ 
//
//  ATH_MSG_DEBUG("Combo Efficiency wrt for trigItem" << trigItem);
//  if(trigItem == "Dump") {ATH_MSG_DEBUG("Not computing efficiencies for Dump"); return StatusCode::SUCCESS;};
//  if(!m_truth) {ATH_MSG_DEBUG("Not computing efficiencies, doTruth=False"); return StatusCode::SUCCESS;};
//  
//  std::string l1_chain(LowerChain("HLT_"+trigItem));
//  std::string hlt_chain = "HLT_"+trigItem;
//  
//  ATH_MSG_DEBUG("+++++++++++++++++++");
//  ATH_MSG_DEBUG("HLT: "<< hlt_chain);
//  ATH_MSG_DEBUG("L1: "<< l1_chain);
//
//  std::vector<TLorentzVector> tlv_truthMatched;
//
//  //float offline_met(0.);
//  std::vector<TLorentzVector> tlv_OffJets;
//
//  std::vector<TLorentzVector> tlv_truthTau;
//  std::vector<bool> tlv_truthTauMatched;
//  std::vector<TLorentzVector> tlv_truthElectron;
//  std::vector<bool> tlv_truthElectronMatched;
//  std::vector<TLorentzVector> tlv_truthMuon;
//  std::vector<bool> tlv_truthMuonMatched;
//  
//  //loop over truth particles to get taus, electrons and muons
//  
//  const xAOD::TruthParticleContainer* truth_cont = 0;
//  if(evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure())
//    {
//      ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
//      return StatusCode::FAILURE;
//    }
//  xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
//  for(truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
//    {
//      // get truth taus
//      if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) 
//	{
//	  examineTruthTau(**truthItr);
//	  double pt  = (*truthItr)->auxdata<double>("pt_vis");
//	  double eta = (*truthItr)->auxdata<double>("eta_vis");
//	  double phi = (*truthItr)->auxdata<double>("phi_vis");
//	  double m   = (*truthItr)->auxdata<double>("m_vis");
//	  bool lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
//	  //int ntracks = (*truthItr)->auxdata<int>("nTracks");
//	  if(pt >= m_effOffTauPtCut && !lep && fabs(eta) < 2.47 )
//	    {
//	      TLorentzVector TruthTauTLV;
//	      TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
//	      tlv_truthTau.push_back(TruthTauTLV);
//	      tlv_truthTauMatched.push_back(false);
//	    }
//	}
//
//      // get truth electrons
//      if(abs((*truthItr)->pdgId()) == 11 && (*truthItr)->status() == 1)
//	{
//	  TLorentzVector TruthElTLV((*truthItr)->p4());
//	  tlv_truthElectron.push_back(TruthElTLV);
//	  tlv_truthElectronMatched.push_back(false);
//	}
//
//      // get truth muons
//      if(abs((*truthItr)->pdgId()) == 13 && (*truthItr)->status() == 1)
//	{
//	  TLorentzVector TruthMuonTLV((*truthItr)->p4());
//	  tlv_truthMuon.push_back(TruthMuonTLV);
//	  tlv_truthMuonMatched.push_back(false);
//	}
//    }
//
//  // truth tau matching with offline:
//  const xAOD::TauJetContainer * reco_cont = 0;
//  if(evtStore()->retrieve(reco_cont, "TauJets").isFailure())
//    {
//      ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
//      return StatusCode::FAILURE;
//    }
//  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
//  for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
//    {
//      if(!Selection(*recoItr)) continue;
//      TLorentzVector TauTLV = (*recoItr)->p4();
//      double eta_Tau = TauTLV.Eta();
//      if(fabs(eta_Tau) > 2.47) continue;
//      double pt_Tau = TauTLV.Pt();
//      if(pt_Tau<m_effOffTauPtCut) continue;
//      int ntrack_TAU = (*recoItr)->nTracks();
//      if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
//      bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
//      //bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
//      //bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
//      //bool best_tau = good_tau && not_a_electron && not_a_muon;
//      bool best_tau = good_tau;
//      if(!best_tau) continue;
//      
//      for(unsigned int truth=0;truth<tlv_truthTau.size();truth++)
//	{
//	  if(tlv_truthTau.at(truth).DeltaR(TauTLV)<0.2) tlv_truthTauMatched.at(truth)=true;
//	}
//    }
//
//  // truth electron matching with offline
//  const xAOD::ElectronContainer * elec_cont = 0; 
//  if(evtStore()->retrieve(elec_cont, "Electrons").isFailure())
//    {
//      ATH_MSG_WARNING("Failed to retrieve Electrons container. Exiting.");
//      return StatusCode::FAILURE;
//    }
//  xAOD::ElectronContainer::const_iterator elecItr, elec_cont_end = elec_cont->end();
//  for(elecItr=elec_cont->begin(); elecItr!=elec_cont_end; ++elecItr)
//    {
//      // selection for good electron??
//      TLorentzVector ElecTLV = (*elecItr)->p4();
//      for(unsigned int truth=0;truth<tlv_truthElectron.size();truth++)
//	{
//	  if(tlv_truthElectron.at(truth).DeltaR(ElecTLV)<0.1) tlv_truthElectronMatched.at(truth)=true;
//	}
//    }
//
//  // truth muon matching with offline
//  const xAOD::MuonContainer * muon_cont = 0;
//  if(evtStore()->retrieve(muon_cont, "Muons").isFailure())
//    {
//      ATH_MSG_WARNING("Failed to retrieve Muons container. Exiting.");
//      return StatusCode::FAILURE;
//    }
//  xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
//  for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr)
//    {
//      // selection for good muon??
//      TLorentzVector MuonTLV = (*muonItr)->p4();
//      for(unsigned int truth=0;truth<tlv_truthMuon.size();truth++)
//	{
//	  if(tlv_truthMuon.at(truth).DeltaR(MuonTLV)<0.1) tlv_truthMuonMatched.at(truth)=true;
//	}
//    }
//
//  // put all truth particles together
//  for(unsigned int truth=0;truth<tlv_truthTau.size();truth++) if(tlv_truthTauMatched.at(truth)) tlv_truthMatched.push_back(tlv_truthTau.at(truth));
//  for(unsigned int truth=0;truth<tlv_truthElectron.size();truth++) if(tlv_truthElectronMatched.at(truth)) tlv_truthMatched.push_back(tlv_truthElectron.at(truth));
//  for(unsigned int truth=0;truth<tlv_truthMuon.size();truth++) if(tlv_truthMuonMatched.at(truth)) tlv_truthMatched.push_back(tlv_truthMuon.at(truth));
//
//  // get offline jets, only the ones not overlapping with truth taus, electrons or muons
//  const xAOD::JetContainer * jet_cont = 0;
//  if(evtStore()->retrieve(jet_cont, "AntiKt4EMTopoJets").isFailure())
//    {
//      ATH_MSG_WARNING("Failed to retrieve AntiKt4EMTopoJets container. Exiting.");
//      return StatusCode::FAILURE;
//    } 
//  xAOD::JetContainer::const_iterator jetItr, jet_cont_end = jet_cont->end();
//  for(jetItr=jet_cont->begin(); jetItr!=jet_cont_end; ++jetItr)
//    {
//      // selection for good jets??
//      TLorentzVector JetTLV = (*jetItr)->p4();
//      bool truth_matched(false);
//      for(unsigned int truth=0;truth<tlv_truthMatched.size();truth++)
//	{
//	  if(tlv_truthMatched.at(truth).DeltaR(JetTLV)<0.2) truth_matched=true;
//	}
//      if(!truth_matched) tlv_OffJets.push_back(JetTLV);
//    }
//
// // get offline met
// const xAOD::MissingETContainer *m_off_met_cont = 0;
// StatusCode sc = m_storeGate->retrieve(m_off_met_cont, "MET_Reference_AntiKt4LCTopo");
// if (sc.isFailure() || !m_off_met_cont) 
//   {
//     ATH_MSG_WARNING("Could not retrieve Reconstructed MET term with Key MET_Reference_AntiKt4LCTopo : m_off_met_cont = 0");
//   }
// 
// const xAOD::MissingET  *m_off_met = 0;
// float off_met = -9e9;
// if (m_off_met_cont && m_off_met_cont->size())
//   {
//     m_off_met = m_off_met_cont->at(0);
//     float off_ex = (m_off_met_cont->at(0)->mpx())/CLHEP::GeV;
//     float off_ey = (m_off_met_cont->at(0)->mpy())/CLHEP::GeV;
//     off_met = sqrt(off_ex*off_ex+off_ey+off_ey);
//     ATH_MSG_DEBUG("m_off_met:" << m_off_met );
//   }
// //offline_met = off_met;
//
// // compute dR,dPhi,dEta between two leading truth-matched objects
// // compute efficiencies looking at TDT bit as funciton of leading truth tau, leading truth electron, leading truth muon, leading jet pt, met, dR/dEta/dPhi between the two leading objects
//
// setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/TauComboEfficiency");
// double pt_tau = -1., pt_el = -1., pt_mu = -1., pt_offJet = -1.;
// for(unsigned int t_tau=0;t_tau<tlv_truthTauMatched.size();t_tau++)
//   {
//     if(tlv_truthTauMatched.at(t_tau) && tlv_truthTau.at(t_tau).Pt()>25000.)
//       {
//	 if(pt_tau < tlv_truthTau.at(t_tau).Pt()) pt_tau = tlv_truthTau.at(t_tau).Pt();
//       }
//   }
// if(pt_tau>0.) hist("hCombTauPtDenom")->Fill(pt_tau/1000.);
// if(pt_tau>0.) {if(getTDT()->isPassed(l1_chain))  {hist("hCombL1TauPtNum")->Fill(pt_tau/1000.);profile("TProfCombL1TauPtEfficiency")->Fill(pt_tau/1000.,1);} if(!getTDT()->isPassed(l1_chain)) profile("TProfCombL1TauPtEfficiency")->Fill(pt_tau/1000.,0);}
// if(pt_tau>0.) {if(getTDT()->isPassed(hlt_chain)) {hist("hCombHLTTauPtNum")->Fill(pt_tau/1000.);profile("TProfCombHLTTauPtEfficiency")->Fill(pt_tau/1000.,1);} if(!getTDT()->isPassed(hlt_chain)) profile("TProfCombHLTTauPtEfficiency")->Fill(pt_tau/1000.,0);}
// 
// for(unsigned int t_el=0;t_el<tlv_truthElectronMatched.size();t_el++)
//   {
//     if(tlv_truthElectronMatched.at(t_el) && tlv_truthElectron.at(t_el).Pt()>15000.)
//       {
//	 if(pt_el < tlv_truthElectron.at(t_el).Pt()) pt_el = tlv_truthElectron.at(t_el).Pt();
//       }
//   }
// if(pt_el>0.) hist("hCombelPtDenom")->Fill(pt_el/1000.);
// if(pt_el>0.) {if(getTDT()->isPassed(l1_chain))  {hist("hCombL1elPtNum")->Fill(pt_el/1000.);profile("TProfCombL1elPtEfficiency")->Fill(pt_el/1000.,1);} if(!getTDT()->isPassed(l1_chain)) profile("TProfCombL1elPtEfficiency")->Fill(pt_el/1000.,0);}
// if(pt_el>0.) {if(getTDT()->isPassed(hlt_chain)) {hist("hCombHLTelPtNum")->Fill(pt_el/1000.);profile("TProfCombHLTelPtEfficiency")->Fill(pt_el/1000.,1);} if(!getTDT()->isPassed(hlt_chain)) profile("TProfCombHLTelPtEfficiency")->Fill(pt_el/1000.,0);}
//
// for(unsigned int t_mu=0;t_mu<tlv_truthMuonMatched.size();t_mu++)
//   {
//     if(tlv_truthMuonMatched.at(t_mu) && tlv_truthMuon.at(t_mu).Pt()>14000.)
//       {
//	 if(pt_mu < tlv_truthMuon.at(t_mu).Pt()) pt_mu = tlv_truthMuon.at(t_mu).Pt();
//       }
//   }
// if(pt_mu>0.) hist("hCombmuPtDenom")->Fill(pt_mu/1000.);
// if(pt_mu>0.) {if(getTDT()->isPassed(l1_chain))  {hist("hCombL1muPtNum")->Fill(pt_mu/1000.);profile("TProfCombL1muPtEfficiency")->Fill(pt_mu/1000.,1);} if(!getTDT()->isPassed(l1_chain)) profile("TProfCombL1muPtEfficiency")->Fill(pt_mu/1000.,0);}
// if(pt_mu>0.) {if(getTDT()->isPassed(hlt_chain)) {hist("hCombHLTmuPtNum")->Fill(pt_mu/1000.);profile("TProfCombHLTmuPtEfficiency")->Fill(pt_mu/1000.,1);} if(!getTDT()->isPassed(hlt_chain)) profile("TProfCombHLTmuPtEfficiency")->Fill(pt_mu/1000.,0);}
//
// for(unsigned int off_j=0; off_j<tlv_OffJets.size();off_j++)
//   {
//     if(pt_offJet < tlv_OffJets.at(off_j).Pt() && tlv_OffJets.at(off_j).Pt()>25000. ) pt_offJet = tlv_OffJets.at(off_j).Pt();
//   }
// if(pt_offJet>0.) hist("hCombOffjetPtDenom")->Fill(pt_offJet/1000.);
// if(pt_offJet>0.) {if(getTDT()->isPassed(l1_chain))  {hist("hCombL1OffjetPtNum")->Fill(pt_offJet/1000.);profile("TProfCombL1OffjetPtEfficiency")->Fill(pt_offJet/1000.,1);} if(!getTDT()->isPassed(l1_chain)) profile("TProfCombL1OffJetPtEfficiency")->Fill(pt_offJet/1000.,0);}
// if(pt_offJet>0.) {if(getTDT()->isPassed(hlt_chain)) {hist("hCombHLTOffjetPtNum")->Fill(pt_offJet/1000.);profile("TProfCombHLTOffjetPtEfficiency")->Fill(pt_offJet/1000.,1);} if(!getTDT()->isPassed(hlt_chain)) profile("TProfCombHLTOffjetPtEfficiency")->Fill(pt_offJet/1000.,0);}
//
// if(off_met>20000.) hist("hCombMETDenom")->Fill(off_met);
// if(off_met>20000.) {if(getTDT()->isPassed(l1_chain))  {hist("hCombL1METNum")->Fill(off_met);profile("TProfCombL1METEfficiency")->Fill(off_met/1000.,1);} if(!getTDT()->isPassed(l1_chain)) profile("TProfCombL1METEfficiency")->Fill(off_met/1000.,0);}
// if(off_met>20000.) {if(getTDT()->isPassed(hlt_chain)) {hist("hCombHLTMETNum")->Fill(off_met);profile("TProfCombHLTMETEfficiency")->Fill(off_met/1000.,1);} if(!getTDT()->isPassed(hlt_chain)) profile("TProfCombHLTMETEfficiency")->Fill(off_met/1000.,0);}
//
// double lead_pt =-1., sub_lead_pt=-1., dR=0, dEta=0, dPhi=0;
// unsigned int ind_1 = 0, ind_2 = 0; 
// if(tlv_truthMatched.size() >=2)
//   {
//     for(unsigned int i=0;i<tlv_truthMatched.size();i++)
//       {    
//	 if(lead_pt < tlv_truthMatched.at(i).Pt())
//	   {
//	     lead_pt = tlv_truthMatched.at(i).Pt();
//	     ind_1 = i;
//	   }
//       }
//     for(unsigned int j=0;j<tlv_truthMatched.size();j++)
//       {
//	 if(j == ind_1) continue;
//	 if(sub_lead_pt < tlv_truthMatched.at(j).Pt())
//	   {
//	     sub_lead_pt = tlv_truthMatched.at(j).Pt();
//	     ind_2 = j;
//	   }
//       }
//     dR = fabs(deltaR(tlv_truthMatched.at(ind_1).Eta(),tlv_truthMatched.at(ind_2).Eta(),tlv_truthMatched.at(ind_1).Phi(),tlv_truthMatched.at(ind_2).Phi()));
//     dEta = fabs(deltaEta(tlv_truthMatched.at(ind_1).Eta(),tlv_truthMatched.at(ind_2).Eta()));
//     dPhi = fabs(deltaPhi(tlv_truthMatched.at(ind_1).Phi(),tlv_truthMatched.at(ind_2).Phi()));
//     hist("hCombdRDenom")->Fill(dR);
//     hist("hCombdEtaDenom")->Fill(dEta);
//     hist("hCombdPhiDenom")->Fill(dPhi);
//     if(getTDT()->isPassed(l1_chain))
//       {
//	 hist("hCombL1dRNum")->Fill(dR);
//	 hist("hCombL1dEtaNum")->Fill(dEta);
//	 hist("hCombL1dPhiNum")->Fill(dPhi);
//	 profile("TProfCombL1dREfficiency")->Fill(dR,1);
//	 profile("TProfCombL1dEtaEfficiency")->Fill(dEta,1);
//	 profile("TProfCombL1dPhiEfficiency")->Fill(dPhi,1);
//       }
//     if(!getTDT()->isPassed(l1_chain))
//       {
//	 profile("TProfCombL1dREfficiency")->Fill(dR,0);
//	 profile("TProfCombL1dEtaEfficiency")->Fill(dEta,0);
//	 profile("TProfCombL1dPhiEfficiency")->Fill(dPhi,0);
//       }
//
//     if(getTDT()->isPassed(hlt_chain))
//       {
//	 hist("hCombHLTdRNum")->Fill(dR);
//	 hist("hCombHLTdEtaNum")->Fill(dEta);
//	 hist("hCombHLTdPhiNum")->Fill(dPhi);
//	 profile("TProfCombHLTdREfficiency")->Fill(dR,1);
//         profile("TProfCombHLTdEtaEfficiency")->Fill(dEta,1);
//         profile("TProfCombHLTdPhiEfficiency")->Fill(dPhi,1);
//
//       }
//     if(!getTDT()->isPassed(hlt_chain))
//       {
//	 profile("TProfCombHLTdREfficiency")->Fill(dR,0);
//         profile("TProfCombHLTdEtaEfficiency")->Fill(dEta,0);
//         profile("TProfCombHLTdPhiEfficiency")->Fill(dPhi,0);
//
//       }
//
//   }
//
// return StatusCode::SUCCESS;
//}

StatusCode HLTTauMonTool::TruthTauEfficiency(const std::string & trigItem, const std::string & TauCont_type)
{
  ATH_MSG_DEBUG("Truth Tau Matching to Offline and Online Taus for trigItem" << trigItem);
  
  std::string l1_chain(LowerChain("HLT_"+trigItem));

  double pt = 0, eta = 0, phi = 0, m = 0;
  int ntracks = 0, nvtx = 0;
  float mu = 0;
  bool lep;
  TLorentzVector TruthTauTLV(0.,0.,0.,0.);
  TLorentzVector TauTLV(0.,0.,0.,0.);
  
  const xAOD::TruthParticleContainer* truth_cont = 0;
  const xAOD::TauJetContainer * reco_cont = 0;
  const xAOD::TauJetContainer * hlt_cont = 0;
  const xAOD::EmTauRoIContainer * L1_cont = 0;

  if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
    return StatusCode::FAILURE;
  }

  if( evtStore()->retrieve(hlt_cont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve  HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting.");
    return StatusCode::FAILURE;
  }

  if( m_storeGate->retrieve(L1_cont, "LVL1EmTauRoIs").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoIs container. Exiting.");
    return StatusCode::FAILURE;
  }

  if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
    return StatusCode::FAILURE;
  } 
  
  xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
  xAOD::TauJetContainer::const_iterator hltItr, hlt_cont_end = hlt_cont->end();
  xAOD::EmTauRoIContainer::const_iterator L1Itr, L1_cont_end = L1_cont->end();

  std::vector<bool> truth_matched_to_L1;
  std::vector<bool> truth_matched_to_hlt;
  std::vector<bool> truth_matched_to_reco;
  std::vector<bool> truthReco_matched_to_L1;
  std::vector<bool> truthReco_matched_to_hlt;

  for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr)
    {
      if(abs((*truthItr)->pdgId()) == 15) ATH_MSG_DEBUG("Tau with status " << (*truthItr)->status() << " and charge " << (*truthItr)->charge());
      if(abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) // || (*truthItr)->status() == 10902))
        {
	  examineTruthTau(**truthItr);
	  pt  = (*truthItr)->auxdata<double>("pt_vis");
	  eta = (*truthItr)->auxdata<double>("eta_vis");
	  phi = (*truthItr)->auxdata<double>("phi_vis");
	  m   = (*truthItr)->auxdata<double>("m_vis");
	  lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
	  ntracks = (*truthItr)->auxdata<int>("nTracks");
	  if(pt < m_effOffTauPtCut || lep || fabs(eta) > 2.47 ) continue;
	  if(m_selection_nTrkMax>-1 && ntracks>m_selection_nTrkMax) continue;
   	  if(m_selection_nTrkMin>-1 && ntracks<m_selection_nTrkMin) continue;
   	  if(m_selection_ptMax>-1 && pt>m_selection_ptMax) continue;
   	  if(m_selection_ptMin>-1 && pt<m_selection_ptMin) continue;
   	  if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) continue;
   	  if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) continue;
   	  if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) continue;
   	  if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) continue;

          TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
	  
	  truth_matched_to_L1.push_back(false);
	  truth_matched_to_hlt.push_back(false);
	  truth_matched_to_reco.push_back(false);
          truthReco_matched_to_L1.push_back(false);
          truthReco_matched_to_hlt.push_back(false);

	  //Primary Vertices
          nvtx = PrimaryVertices();
          //Pile up
          mu = Pileup();
	  
	  // get list of truth-matched reco taus
          for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
            {
	      if(!Selection(*recoItr)) continue;
              TauTLV = (*recoItr)->p4();
              double eta_Tau = TauTLV.Eta();
              if(fabs(eta_Tau) > 2.47) continue;
 	      double pt_Tau = TauTLV.Pt();
	      if(pt_Tau<m_effOffTauPtCut) continue;
	      int ntrack_TAU = (*recoItr)->nTracks();
	      if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
   	      //bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
              //bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
              //bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
              //bool best_tau = good_tau && not_a_electron && not_a_muon;
  	      //if(!best_tau) continue;
              double dR = TauTLV.DeltaR(TruthTauTLV);
              if(dR <= 0.2) truth_matched_to_reco.back()=true;
            }
	  
	  // matching with HLT
	  if(trigItem == "Dump")
	    {
	      for(hltItr=hlt_cont->begin(); hltItr!=hlt_cont_end; ++hltItr)
		{
		  TauTLV = (*hltItr)->p4();
		  double dR = TauTLV.DeltaR(TruthTauTLV);
		  if(dR <= 0.2) truth_matched_to_hlt.back()=true;
		  if( dR <= 0.2 && truth_matched_to_reco.back() ) truthReco_matched_to_hlt.back()=true;
		}
	    }
	  else 
	    {
	      bool HLTTaumatched = HLTTauMatching(trigItem, TruthTauTLV, 0.2);
	      if(HLTTaumatched) truth_matched_to_hlt.back()=true;
	      if(HLTTaumatched && truth_matched_to_reco.back()) truthReco_matched_to_hlt.back()=true;
	      if(truth_matched_to_hlt.back()) ATH_MSG_DEBUG("HLT tau matched to truth taus");
	      if(truthReco_matched_to_hlt.back()) ATH_MSG_DEBUG("HLT tau matched to truth+reco taus");
	    }
	  
          // matching with L1
          if(trigItem=="Dump")
	    {
	      for(L1Itr=L1_cont->begin(); L1Itr!=L1_cont_end; ++L1Itr)
		{
		  float L1_eta = (*L1Itr)->eta();
		  float L1_phi = (*L1Itr)->phi();
		  float dR = deltaR(L1_eta,TruthTauTLV.Eta(),L1_phi,TruthTauTLV.Phi());
		  if(dR <= 0.3) truth_matched_to_L1.back()=true;
		  if( dR <= 0.3 && truth_matched_to_reco.back() ) truthReco_matched_to_L1.back()=true;
		}
	    } 
	  else 
	    {
	      bool L1Taumatched = L1TauMatching(l1_chain, TruthTauTLV, 0.3);
	      if(L1Taumatched) truth_matched_to_L1.back()=true;
	      if(L1Taumatched && truth_matched_to_reco.back() ) truthReco_matched_to_L1.back()=true;
	      if(truth_matched_to_L1.back()) ATH_MSG_DEBUG("L1 tau matched to truth tau");
	      if(truthReco_matched_to_L1.back()) ATH_MSG_DEBUG("L1 tau matched to truth+reco tau");     
	    }
	  
	  if(TauCont_type == "Truth")
            {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/TruthEfficiency");
	      
	      //	      hist("hTrueTauPtDenom")->Fill(pt/1000.);
	      //	      if(ntracks == 1) hist("hTrueTauPt1PDenom")->Fill(pt/1000.);
	      //	      if(ntracks > 1) hist("hTrueTauPt3PDenom")->Fill(pt/1000.);
	      //	      hist("hTrueTauEtaDenom")->Fill(eta);
	      //	      hist("hTrueTauPhiDenom")->Fill(phi);
	      //	      hist("hTrueTauNTrackDenom")->Fill(ntracks);
	      //	      hist("hTrueTauNVtxDenom")->Fill(nvtx);
	      //              hist("hTrueTauMuDenom")->Fill(mu);
	      hist2("hTrueTauEtaVsPhiDenom")->Fill(eta,phi);
	      
	      if(truth_matched_to_L1.back())
		{
		  //		  hist("hTrueL1PtNum")->Fill(pt/1000.);
		  //		  if(ntracks == 1) {hist("hTrueL1Pt1PNum")->Fill(pt/1000.);profile("TProfTrueL1Pt1PEfficiency")->Fill(pt/1000.,1);}
		  if(ntracks == 1) {profile("TProfTrueL1Pt1PEfficiency")->Fill(pt/1000.,1);}
		  //		  if(ntracks > 1) {hist("hTrueL1Pt3PNum")->Fill(pt/1000.);profile("TProfTrueL1Pt3PEfficiency")->Fill(pt/1000.,1);}
		  if(ntracks > 1) { profile("TProfTrueL1Pt3PEfficiency")->Fill(pt/1000.,1);}
		  //		  hist("hTrueL1EtaNum")->Fill(eta);
		  //		  hist("hTrueL1PhiNum")->Fill(phi);
		  //		  hist("hTrueL1NTrackNum")->Fill(ntracks);
		  //		  hist("hTrueL1NVtxNum")->Fill(nvtx);
		  //		  hist("hTrueL1MuNum")->Fill(mu);
		  hist2("hTrueL1EtaVsPhiNum")->Fill(eta,phi);
		  profile("TProfTrueL1PtEfficiency")->Fill(pt/1000.,1);
		  profile("TProfTrueL1EtaEfficiency")->Fill(eta,1);
		  profile("TProfTrueL1PhiEfficiency")->Fill(phi,1);
		  profile("TProfTrueL1NTrackEfficiency")->Fill(ntracks,1);
		  profile("TProfTrueL1NVtxEfficiency")->Fill(nvtx,1);
		  profile("TProfTrueL1MuEfficiency")->Fill(mu,1);
		}

	      if(!truth_matched_to_L1.back())
                {
                  if(ntracks == 1) profile("TProfTrueL1Pt1PEfficiency")->Fill(pt/1000.,0);
                  if(ntracks > 1) profile("TProfTrueL1Pt3PEfficiency")->Fill(pt/1000.,0);
		  profile("TProfTrueL1PtEfficiency")->Fill(pt/1000.,0);
                  profile("TProfTrueL1EtaEfficiency")->Fill(eta,0);
                  profile("TProfTrueL1PhiEfficiency")->Fill(phi,0);
                  profile("TProfTrueL1NTrackEfficiency")->Fill(ntracks,0);
                  profile("TProfTrueL1NVtxEfficiency")->Fill(nvtx,0);
                  profile("TProfTrueL1MuEfficiency")->Fill(mu,0);
                }

              if(truth_matched_to_hlt.back())
		{
		  //		  hist("hTrueHLTPtNum")->Fill(pt/1000.);
		  //		  if(ntracks == 1) {hist("hTrueHLTPt1PNum")->Fill(pt/1000.);profile("TProfTrueHLTPt1PEfficiency")->Fill(pt/1000.,1);}
		  //		  if(ntracks > 1) {hist("hTrueHLTPt3PNum")->Fill(pt/1000.);profile("TProfTrueHLTPt3PEfficiency")->Fill(pt/1000.,1);}
		  if(ntracks == 1) {profile("TProfTrueHLTPt1PEfficiency")->Fill(pt/1000.,1);}
		  if(ntracks > 1) { profile("TProfTrueHLTPt3PEfficiency")->Fill(pt/1000.,1);}
		  //		  hist("hTrueHLTEtaNum")->Fill(eta);
		  //		  hist("hTrueHLTPhiNum")->Fill(phi);
		  //		  hist("hTrueHLTNTrackNum")->Fill(ntracks);
		  //		  hist("hTrueHLTNVtxNum")->Fill(nvtx);
		  //		  hist("hTrueHLTMuNum")->Fill(mu);
		  hist2("hTrueHLTEtaVsPhiNum")->Fill(eta,phi);
		  profile("TProfTrueHLTPtEfficiency")->Fill(pt/1000.,1);
		  profile("TProfTrueHLTEtaEfficiency")->Fill(eta,1);
		  profile("TProfTrueHLTPhiEfficiency")->Fill(phi,1);
		  profile("TProfTrueHLTNTrackEfficiency")->Fill(ntracks,1);
		  profile("TProfTrueHLTNVtxEfficiency")->Fill(nvtx,1);
		  profile("TProfTrueHLTMuEfficiency")->Fill(mu,1);

		}
	      
	      if(!truth_matched_to_hlt.back())
                {
		  if(ntracks == 1) profile("TProfTrueHLTPt1PEfficiency")->Fill(pt/1000.,0);
                  if(ntracks > 1) profile("TProfTrueHLTPt3PEfficiency")->Fill(pt/1000.,0);
		  profile("TProfTrueHLTPtEfficiency")->Fill(pt/1000.,0);
                  profile("TProfTrueHLTEtaEfficiency")->Fill(eta,0);
                  profile("TProfTrueHLTPhiEfficiency")->Fill(phi,0);
                  profile("TProfTrueHLTNTrackEfficiency")->Fill(ntracks,0);
                  profile("TProfTrueHLTNVtxEfficiency")->Fill(nvtx,0);
                  profile("TProfTrueHLTMuEfficiency")->Fill(mu,0);

                }

	    }
	  else if(TauCont_type == "Truth+Reco")
            {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/TurnOnCurves/Truth+RecoEfficiency");
	      
	      if(truth_matched_to_reco.back())
		{	
		  //		  hist("hTruthRecoTauPtDenom")->Fill(pt/1000.);
		  //		  if(ntracks == 1) hist("hTruthRecoTauPt1PDenom")->Fill(pt/1000.);
		  //		  if(ntracks > 1) hist("hTruthRecoTauPt3PDenom")->Fill(pt/1000.);
		  //		  hist("hTruthRecoTauEtaDenom")->Fill(eta);
		  //		  hist("hTruthRecoTauPhiDenom")->Fill(phi);
		  //		  hist("hTruthRecoTauNTrackDenom")->Fill(ntracks);
		  //		  hist("hTruthRecoTauNVtxDenom")->Fill(nvtx);
		  //		  hist("hTruthRecoTauMuDenom")->Fill(mu);
		  hist2("hTruthRecoTauEtaVsPhiDenom")->Fill(eta,phi);

		  if(truthReco_matched_to_L1.back())
		    {
		      //  hist("hTruthRecoL1PtNum")->Fill(pt/1000.);
		      //		      if(ntracks == 1) {hist("hTruthRecoL1Pt1PNum")->Fill(pt/1000.);profile("TProfTruthRecoL1Pt1PEfficiency")->Fill(pt/1000.,1);}
		      //		      if(ntracks > 1) {hist("hTruthRecoL1Pt3PNum")->Fill(pt/1000.);profile("TProfTruthRecoL1Pt3PEfficiency")->Fill(pt/1000.,1);}
		      if(ntracks == 1) { profile("TProfTruthRecoL1Pt1PEfficiency")->Fill(pt/1000.,1);}
		      if(ntracks > 1) { profile("TProfTruthRecoL1Pt3PEfficiency")->Fill(pt/1000.,1);}
		      //		      hist("hTruthRecoL1EtaNum")->Fill(eta);
		      //		      hist("hTruthRecoL1PhiNum")->Fill(phi);
		      //		      hist("hTruthRecoL1NTrackNum")->Fill(ntracks);
		      //		      hist("hTruthRecoL1NVtxNum")->Fill(nvtx);
		      //		      hist("hTruthRecoL1MuNum")->Fill(mu);
		      hist2("hTruthRecoL1EtaVsPhiNum")->Fill(eta,phi);
		      profile("TProfTruthRecoL1PtEfficiency")->Fill(pt/1000.,1);
		      profile("TProfTruthRecoL1EtaEfficiency")->Fill(eta,1);
		      profile("TProfTruthRecoL1PhiEfficiency")->Fill(phi,1);
		      profile("TProfTruthRecoL1NTrackEfficiency")->Fill(ntracks,1);
		      profile("TProfTruthRecoL1NVtxEfficiency")->Fill(nvtx,1);
		      profile("TProfTruthRecoL1MuEfficiency")->Fill(mu,1);

		    }

		  if(!truthReco_matched_to_L1.back())
                    {
                      if(ntracks == 1) profile("TProfTruthRecoL1Pt1PEfficiency")->Fill(pt/1000.,0);
                      if(ntracks > 1) profile("TProfTruthRecoL1Pt3PEfficiency")->Fill(pt/1000.,0);
		      profile("TProfTruthRecoL1PtEfficiency")->Fill(pt/1000.,0);
                      profile("TProfTruthRecoL1EtaEfficiency")->Fill(eta,0);
                      profile("TProfTruthRecoL1PhiEfficiency")->Fill(phi,0);
                      profile("TProfTruthRecoL1NTrackEfficiency")->Fill(ntracks,0);
                      profile("TProfTruthRecoL1NVtxEfficiency")->Fill(nvtx,0);
                      profile("TProfTruthRecoL1MuEfficiency")->Fill(mu,0);

                    }

		  if(truthReco_matched_to_hlt.back())
		    {
		      //	hist("hTruthRecoHLTPtNum")->Fill(pt/1000.);
		      //		      if(ntracks == 1) {hist("hTruthRecoHLTPt1PNum")->Fill(pt/1000.);profile("TProfTruthRecoHLTPt1PEfficiency")->Fill(pt/1000.,1);}
		      //		      if(ntracks > 1) {hist("hTruthRecoHLTPt3PNum")->Fill(pt/1000.);profile("TProfTruthRecoHLTPt3PEfficiency")->Fill(pt/1000.,1);}
		      if(ntracks == 1) {profile("TProfTruthRecoHLTPt1PEfficiency")->Fill(pt/1000.,1);}
		      if(ntracks > 1) { profile("TProfTruthRecoHLTPt3PEfficiency")->Fill(pt/1000.,1);}
		      //		      hist("hTruthRecoHLTEtaNum")->Fill(eta);
		      //		      hist("hTruthRecoHLTPhiNum")->Fill(phi);
		      //		      hist("hTruthRecoHLTNTrackNum")->Fill(ntracks);
		      //		      hist("hTruthRecoHLTNVtxNum")->Fill(nvtx);
		      //		      hist("hTruthRecoHLTMuNum")->Fill(mu);
		      hist2("hTruthRecoHLTEtaVsPhiNum")->Fill(eta,phi);
		      profile("TProfTruthRecoHLTPtEfficiency")->Fill(pt/1000.,1);
		      profile("TProfTruthRecoHLTEtaEfficiency")->Fill(eta,1);
		      profile("TProfTruthRecoHLTPhiEfficiency")->Fill(phi,1);
		      profile("TProfTruthRecoHLTNTrackEfficiency")->Fill(ntracks,1);
		      profile("TProfTruthRecoHLTNVtxEfficiency")->Fill(nvtx,1);
		      profile("TProfTruthRecoHLTMuEfficiency")->Fill(mu,1);

		    }

		  if(!truthReco_matched_to_hlt.back())
                    {
		      if(ntracks == 1) profile("TProfTruthRecoHLTPt1PEfficiency")->Fill(pt/1000.,0);
                      if(ntracks > 1) profile("TProfTruthRecoHLTPt3PEfficiency")->Fill(pt/1000.,0);
		      profile("TProfTruthRecoHLTPtEfficiency")->Fill(pt/1000.,0);
                      profile("TProfTruthRecoHLTEtaEfficiency")->Fill(eta,0);
                      profile("TProfTruthRecoHLTPhiEfficiency")->Fill(phi,0);
                      profile("TProfTruthRecoHLTNTrackEfficiency")->Fill(ntracks,0);
                      profile("TProfTruthRecoHLTNVtxEfficiency")->Fill(nvtx,0);
                      profile("TProfTruthRecoHLTMuEfficiency")->Fill(mu,0);

                    }

		}
            }
	  else
	    {
	      ATH_MSG_WARNING("Failed to fill the histograms. Exiting.");
	      return StatusCode::FAILURE;
	    }
	}
    }//End of Truth Particle Container iter
    
  return StatusCode::SUCCESS;
}

bool HLTTauMonTool::PresTauMatching(const std::string & trigItem, const TLorentzVector & TLV, double DR){
	std::string trig_item_EF= "HLT_"+trigItem;
	if (getTDT()->isPassed(trig_item_EF,m_HLTTriggerCondition)) {

       Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,m_HLTTriggerCondition) );
       Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());

	if (f.getCombinations().size()>100) {
                ATH_MSG_WARNING("Chain " << trig_item_EF << " has " << f.getCombinations().size() << " combinations. Looping over first 100");
                combEnd = comb;
                std::advance(combEnd,100);
        }

       for(;comb!=combEnd;++comb){

       	 const std::vector< Trig::Feature<xAOD::TauJetContainer> >  vec_preseltau = comb -> get<xAOD::TauJetContainer>("HLT_xAOD__TauJetContainer_TrigTauRecPreselection", m_HLTTriggerCondition);
         std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator preselCI = vec_preseltau.begin(), preselCI_e = vec_preseltau.end();

         for(; preselCI != preselCI_e; ++preselCI){
             if(preselCI->cptr()){
                xAOD::TauJetContainer::const_iterator tauItr = preselCI->cptr()->begin();
                xAOD::TauJetContainer::const_iterator tauEnd = preselCI->cptr()->end();
		for(; tauItr != tauEnd; ++tauItr) {
                	double delR = (*tauItr)->p4().DeltaR(TLV);
			if(delR <= DR) return true;  
			}
	      }
	 }
	}
	}
	return true;
}     
            

bool HLTTauMonTool::HLTTauMatching(const std::string & trigItem, const TLorentzVector & TLV, double DR)
{
  if(trigItem == "Dump")
    {
      ATH_MSG_WARNING("No HLT matching for Dump Item return false");
      return false;
    }
  
  std::string trig_item_EF= "HLT_"+trigItem;
  //std::string l1_chain(LowerChain("HLT_"+trigItem));

/*  std::string testing_chain(trig_item_EF);
  if(testing_chain.find("emulate")!=std::string::npos){
	ATH_MSG_DEBUG("Emulating "<<testing_chain);
	// emulate new trk preselection
	// HLT_tau25_perf_tracktwo_ptmin_emulate -> start from HLT_tau_idperf_tracktwo and apply trk preselection
	// HLT_tau25_medium1_tracktwo_ptmin_emulate -> HLT_tau_idperf_tracktwo, apply trk preselection and match with HLT_tau25_medium1_mvonly
	// same for track
	if(testing_chain.find("track")!=std::string::npos) trig_item_EF = "HLT_tau25_idperf_track"; 
	if(testing_chain.find("tracktwo")!=std::string::npos) trig_item_EF = "HLT_tau25_idperf_tracktwo"; 
  }*/

  if(getTDT()->isPassed(trig_item_EF,TrigDefs::Physics | TrigDefs::allowResurrectedDecision)  )
  //if(getTDT()->isPassed(trig_item_EF,TrigDefs::Physics)  )
    {
      ATH_MSG_DEBUG("HTLTauMonTool::TruthTauEfficiency event passed " << trig_item_EF << ", obtaining list of features now");
      Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,TrigDefs::Physics | TrigDefs::allowResurrectedDecision) );
      Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
      if(comb==combEnd)
      	{
      	  ATH_MSG_DEBUG("No features for chain " << trig_item_EF << ", HLTMatching return false");
      	  return false;
      	} else if (f.getCombinations().size()>100) {
                ATH_MSG_WARNING("Chain " << trig_item_EF << " has " << f.getCombinations().size() << " combinations. Looping over first 100");
                combEnd = comb;
                std::advance(combEnd,100);
        }
      
      int nComb(0);
      for(;comb!=combEnd;++comb)
	{
	  ATH_MSG_DEBUG("-------- Trigger " << trigItem  << ", combination " << nComb << "-th, active? "<< comb->active() << ", TEs "<< comb->tes().size());
	  nComb++;
	  
	  const std::vector<Trig::Feature<xAOD::TauJetContainer> >  vec_tauEF = comb->get<xAOD::TauJetContainer>("TrigTauRecMerged",TrigDefs::Physics | TrigDefs::allowResurrectedDecision);
	  std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator Itr_tauEF = vec_tauEF.begin(), vec_tauEF_end = vec_tauEF.end();
 		  
	  ATH_MSG_DEBUG("vector of EF TauJetsContainer, size: " << vec_tauEF.size());
	  
	  for(;Itr_tauEF!=vec_tauEF_end;++Itr_tauEF)
	    {
	      if(Itr_tauEF->cptr())
		{
		  xAOD::TauJetContainer::const_iterator Itr_tauEFjet = Itr_tauEF->cptr()->begin(), Itr_tauEF_end = Itr_tauEF->cptr()->end();
		  ATH_MSG_DEBUG("TauJetsContainer, " << Itr_tauEF->cptr()->size() << " HLT taus passed");
		  
		  for(;Itr_tauEFjet!=Itr_tauEF_end;++Itr_tauEFjet)
		    {
 		      if(*Itr_tauEFjet)
			{
			  ATH_MSG_DEBUG("HLT tau: pt " << (*Itr_tauEFjet)->pt() << ", #trk " << (*Itr_tauEFjet)->nTracks());

			  /*if(testing_chain.find("emulate")!=std::string::npos){
				bool pass = false;
				bool do2step = false;
				bool doReject0trk = true;
				float track_pt_min = 0.;
				if(testing_chain.find("tracktwo")!=std::string::npos) do2step = true;
				if(testing_chain.find("ptmin")!=std::string::npos) track_pt_min = 1000.;
				if(testing_chain.find("no0reject")!=std::string::npos) doReject0trk = false;
				//				float RoI_eta = (*Itr_tauEFjet)->etaJetSeed();
				//				float RoI_phi = (*Itr_tauEFjet)->phiJetSeed();
				// ATH_MSG_DEBUG("Emulating "<<testing_chain<<" with do2step "<<do2step<<", doReject0trk "<<doReject0trk<<", track_pt_min "<<track_pt_min);
				//				pass = emulate2StepTracking(RoI_eta, RoI_phi, do2step, doReject0trk, track_pt_min);
				if(!pass) return false;
				ATH_MSG_DEBUG("Passed emulated trk preselection");
				pass = L1TauMatching("L1_TAU12IM", TLV, 0.3);
				if(!pass) return false;
				ATH_MSG_DEBUG("Passed L1_TAU12IM");
				if(testing_chain.find("medium")!=std::string::npos) pass = HLTTauMatching("HLT_tau25_medium1_mvonly", (*Itr_tauEFjet)->p4(), 0.2);
				if(!pass) return false;	
				ATH_MSG_DEBUG("Passed medium1");
			  }*/

			  double delR = (*Itr_tauEFjet)->p4().DeltaR(TLV);
			  if(delR <= DR) return true;
			}			
		    }
		}
	    }  
	}
    }

 return false;

}

bool HLTTauMonTool::L1TauMatching(const std::string & l1_item, const TLorentzVector & TLV, double DR)
{
  if(l1_item == "Dump")
    {
      ATH_MSG_DEBUG("No L1 matching for Dump Item return false");
      return false;
    }

  if(getTDT()->isPassed( l1_item,TrigDefs::Physics | TrigDefs::allowResurrectedDecision ))
    {     
      ATH_MSG_DEBUG("HTLTauMonTool::TruthTauEfficiency event passed " << l1_item << ", obtaining list of features now");
      
      Trig::FeatureContainer f = ( getTDT()->features(l1_item, TrigDefs::Physics | TrigDefs::allowResurrectedDecision ) );
      Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
      if(comb==combEnd)
	{
	  ATH_MSG_DEBUG("No features for chain " <<l1_item <<", L1Matching return false");
	  return false;
	} else if (f.getCombinations().size()>100) {
                ATH_MSG_WARNING("Chain " << l1_item << " has " << f.getCombinations().size() << " combinations. Looping over first 100");
                combEnd = comb;
                std::advance(combEnd,100);
       }

      for(;comb!=combEnd;++comb)
	{
	  const std::vector<Trig::Feature<TrigRoiDescriptor> >  vec_tauL1 = comb->get<TrigRoiDescriptor>("initialRoI",TrigDefs::Physics | TrigDefs::allowResurrectedDecision);
	  std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator Itr_tauL1 = vec_tauL1.begin(), vec_tauL1_end = vec_tauL1.end();
	  
	  ATH_MSG_DEBUG("HTLTauMonTool::TruthTauEfficiency " << vec_tauL1.size() << " L1 taus passed");
	  
	  for(;Itr_tauL1!=vec_tauL1_end;++Itr_tauL1)
	    {
	      if(Itr_tauL1->cptr())
		{
		  float L1_eta = Itr_tauL1->cptr()->eta();
		  float L1_phi = Itr_tauL1->cptr()->phi();
		  float delR = deltaR(L1_eta,TLV.Eta(),L1_phi,TLV.Phi());
		  if(delR <= DR) return true;
		}
	    }
	}
    }

  //int L1_PSCut = (int) getTDT()->getPrescale(l1_item);
  //float L1_PS = TrigConf::PrescaleSet::getPrescaleFromCut(L1_PSCut);
  //if(m_L1StringCondition=="allowResurrectedDecision"){
  //	const unsigned int TBP_bit = getTDT()->isPassedBits(l1_item);
  //	if(TBP_bit & TrigDefs::L1_isPassedBeforePrescale) return true;
  //}

  return false;

}

int HLTTauMonTool::PrimaryVertices(){
  int nGoodVtx(0);
  const xAOD::VertexContainer* primary = 0;
  if( evtStore()->retrieve(primary, "PrimaryVertices" ).isFailure() ){
    ATH_MSG_DEBUG("Failed to retrieve PrimaryVertices container, returning -1!");
    return -1;
    }
  
  xAOD::VertexContainer::const_iterator vtxIt = primary->begin(), vtxIt_e = primary->end();
  for(;vtxIt!=vtxIt_e;++vtxIt){
   /// if((*vtxIt)->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
   if((*vtxIt)->nTrackParticles() < 3 )  continue; 
   nGoodVtx++;
   }

  return nGoodVtx;
}

float HLTTauMonTool::Pileup(){
  float Pileup(0.);
  const xAOD::EventInfo* evtInfo = 0;
  if( evtStore()->retrieve(evtInfo, "EventInfo" ).isFailure() ){
    ATH_MSG_DEBUG("Failed to retrieve EventInfo container, returning -1!");
    return -1;
    }
  Pileup = evtInfo->averageInteractionsPerCrossing();
  return Pileup;
}

///////////////////////////////////////////////////////////
// Function to fill relative difference histograms
///////////////////////////////////////////////////////////
void HLTTauMonTool::FillRelDiffHist(TH1 * hist, float val1, float val2, float checkVal, int checkMode){
    if (val1 != Analysis::TauDetails::DEFAULT) {
        if (val2 != Analysis::TauDetails::DEFAULT) {
            if ( (checkMode == 1 && val1 > checkVal) || (checkMode == 2 && fabs(val1) > checkVal) ){
                val1 = (val2 - val1) / val1;
            
               // if (val1 >= hist->GetXaxis()->GetXmax()) {
               //  int nbins = hist->GetNbinsX();
               //   val1 = hist->GetBinLowEdge(nbins)+0.5*hist->GetBinWidth(nbins);
               // }
               // else if (val1 < hist->GetXaxis()->GetXmin())
               //    val1 = hist->GetXaxis()->GetXmin();
            
                hist->Fill(val1);
	  }
        }
        //else hist->Fill(hist->GetXaxis()->GetXmin()); // fill invalid values in the first visible bin instead of the underflow bin
    }
    //else hist->Fill(hist->GetXaxis()->GetXmin()); // fill invalid values in the first visible bin instead of the underflow bin
    
}

template <class T> void HLTTauMonTool::FillRelDiffProfile(TProfile *prof, float val1, float val2, T val3, float checkVal, int checkMode)
{
  if(val1 != Analysis::TauDetails::DEFAULT)
    {
      if(val2 != Analysis::TauDetails::DEFAULT)
        {
          if((checkMode == 1 && val1 > checkVal) || (checkMode == 2 && fabs(val1) > checkVal))
            {
              val1 = (val2 - val1) / val1;
              prof->Fill(val3, val1);
            }
        }
    }
}
