/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile MuZTPMon.cxx
 *   
 *    authors: Marilyn Marx (marx@cern.ch), Mark Owen (markowen@cern.ch)
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TagInfo.h"
#include "EventInfo/EventID.h"

// To be modified when L2MuonSA / muComb move to xAOD storage
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"

// #include "TrigMuonEvent/IsoMuonFeatureContainer.h"
// #include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
// #include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
// #include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
// #include "TrigMuonEvent/TrigMuonEF.h"
// #include "TrigMuonEvent/TrigMuonEFInfo.h"
// #include "TrigMuonEvent/TrigMuonEFTrack.h"
// #include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigSteeringEvent/TrigPassBits.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTracking/Vertex.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "VxVertex/VxContainer.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"

// #include "muonEvent/Muon.h"
// #include "muonEvent/MuonContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "TrigMuonMonitoring/HLTMuonMonTool.h"
#include "GaudiKernel/ToolHandle.h"

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

class TrigMatchTool;

using namespace std;


bool CheckMuonTriggerMatch(float off_eta, float off_phi, const std::vector<float>& on_eta, const std::vector<float>& on_phi);
float CalcdeltaR(float off_eta, float off_phi,float on_eta, float on_phi);


StatusCode HLTMuonMonTool::initMuZTPDQA()
{
  m_ztp_newrun=true;

  for(map<string,int>::const_iterator itstr=m_ztp_isomap.begin(); itstr!=m_ztp_isomap.end(); ++itstr) {
    m_ztp_l1map[itstr->first] = ""; //load the L1 seed later
  }

  ATH_MSG_DEBUG("N(T&P) chains = " << m_ztpmap.size());
  ATH_MSG_DEBUG("Offline isolation cut (sumpt 0.1 / pt) = " << m_ztp_ptcone30rel_cut);

  // automatically configure the pT cuts for the chains
  for(std::map<std::string, std::string>::const_iterator it=m_ztpmap.begin(); it!=m_ztpmap.end(); ++it) {

    // if(msgLvl(MSG::INFO)) {
    //  msg(MSG::INFO) << "Configuring Z T&P code for chain " << it->first << ", isolation flag = " << m_ztp_isomap[it->first] << endmsg;
    // }

    if(it->first.find("24")!=string::npos) {
      m_ztpptcut[it->first] = 26.;
    } else if(it->first.find("26")!=string::npos) {
      m_ztpptcut[it->first] = 28.;
    } else if(it->first.find("40")!=string::npos) {
      m_ztpptcut[it->first] = 42.;
    } else if(it->first.find("50")!=string::npos) {
      m_ztpptcut[it->first] = 52.;
    } else if(it->first.find("60")!=string::npos) {
      m_ztpptcut[it->first] = 65.;
    } else if(it->first.find("14")!=string::npos) {
      m_ztpptcut[it->first] = 16.;
    } else if(it->first.find("10")!=string::npos){
      m_ztpptcut[it->first] = 12.;
    }else {
      ATH_MSG_DEBUG("Did not auto-configure pT cut for Z TP trigger " << it->first << ", set to 20 GeV");
      m_ztpptcut[it->first] = 20.0;
    }
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Using pT cut " << m_ztpptcut[it->first] << endmsg;
    }
  }//map loop    

  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::bookMuZTPDQA()
{
  if( newRunFlag() ){
    m_ztp_newrun=true;
    for(std::map<std::string, std::string>::iterator itmap=m_ztpmap.begin();itmap!=m_ztpmap.end();++itmap){
      
      const bool isefisochain = (m_ztp_isomap[itmap->first] > 0);

      std::string histdirmuztp="HLT/MuonMon/MuZTP/"+itmap->second;
      double xbins[3] = {0.,1.05,2.7};
      double ptbins[5];
      if(!m_HI_pp_mode){
	ptbins[0] = 0;
	ptbins[1] = 15;
	ptbins[2] = 20;
	ptbins[3] = 25;
	ptbins[4] = 50;
      }else{
	ptbins[0] = 0;
	ptbins[1] = 30;
	ptbins[2] = 50;
	ptbins[3] = 100;
	ptbins[4] = 500;
      }	
      //mass
      if(!m_HI_pp_mode){
	addHistogram( new TH1F(("muZTP_invmass_nomasswindow_" + itmap->second).c_str(), "Invariant mass", 20, 0.0, 5000.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_invmass_" + itmap->second).c_str(), "Invariant mass", 20, 0.0, 5000.0 ), histdirmuztp );
	//all probes
	addHistogram( new TH1F(("muZTP_Pt_" + itmap->second).c_str(), "Probe muon p_{T}", 25, 0.0, 25.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_EC_" + itmap->second).c_str(), "Probe muon p_{T} EC", 25, 0.0, 25.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_B_" + itmap->second).c_str(), "Probe muon p_{T} B", 25, 0.0, 25.0 ), histdirmuztp );
      }else{
	addHistogram( new TH1F(("muZTP_invmass_nomasswindow_" + itmap->second).c_str(), "Invariant mass", 20, 0.0, 120000.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_invmass_" + itmap->second).c_str(), "Invariant mass", 20, 0.0, 120000.0 ), histdirmuztp );
	//all probes
	addHistogram( new TH1F(("muZTP_Pt_" + itmap->second).c_str(), "Probe muon p_{T}", 20, 0.0, 100.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_EC_" + itmap->second).c_str(), "Probe muon p_{T} EC", 20, 0.0, 100.0 ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Pt_B_" + itmap->second).c_str(), "Probe muon p_{T} B", 20, 0.0, 100.0 ), histdirmuztp );
}
      addHistogram( new TH1F(("muZTP_Pt_4bins_" + itmap->second).c_str(), "Probe muon p_{T}", 4, ptbins ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Pt_B_4bins_" + itmap->second).c_str(), "Probe muon p_{T}", 4, ptbins ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Eta_" + itmap->second).c_str(), "Probe muon #eta", 20, -2.7, 2.7 ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Eta_1bin_" + itmap->second).c_str(), "Probe muon #eta", 1, 0.0, 2.7 ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Eta_2bins_" + itmap->second).c_str(), "Probe muon #eta", 2, xbins), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Eta_1bin_cut_" + itmap->second).c_str(), "Probe muon #eta", 1, 0.0, 2.7 ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Eta_2bins_cut_" + itmap->second).c_str(), "Probe muon #eta", 2, xbins), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Phi_" + itmap->second).c_str(), "Probe muon #phi", 20, -CLHEP::pi, CLHEP::pi), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Phi_EC_" + itmap->second).c_str(), "Probe muon #phi", 20, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      addHistogram( new TH1F(("muZTP_Phi_B_" + itmap->second).c_str(), "Probe muon #phi", 20, -CLHEP::pi, CLHEP::pi), histdirmuztp );
      //fired probes
      std::vector<std::string> level;
      level.push_back("L1");
      level.push_back("L2");
      level.push_back("EF");
      level.push_back("EFL2");
      if(isefisochain) level.push_back("EFIso");
      for(unsigned int j=0;j<level.size();j++){
      	if(!m_HI_pp_mode){
	addHistogram( new TH1F(("muZTP_Pt_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} (fired "+level[j]+")").c_str(), 25, 0.0, 25.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_EC_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} EC (fired "+level[j]+")").c_str(), 25, 0.0, 25.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_B_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} B (fired "+level[j]+")").c_str(), 25, 0.0, 25.0 ), histdirmuztp );
	}else{
	addHistogram( new TH1F(("muZTP_Pt_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} (fired "+level[j]+")").c_str(), 20, 0.0, 100.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_EC_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} EC (fired "+level[j]+")").c_str(), 20, 0.0, 100.0 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_B_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} B (fired "+level[j]+")").c_str(), 20, 0.0, 100.0 ), histdirmuztp );
	}
	addHistogram( new TH1F(("muZTP_Pt_4bins_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} (fired "+level[j]+")").c_str(), 4, ptbins ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Pt_B_4bins_"+level[j]+"fired_" + itmap->second).c_str(), ("p_{T} (fired "+level[j]+")").c_str(), 4, ptbins ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Eta_"+level[j]+"fired_" + itmap->second).c_str(), ("#eta (fired "+level[j]+")").c_str(), 20, -2.7, 2.7 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Eta_1bin_"+level[j]+"fired_" + itmap->second).c_str(), ("#eta (fired "+level[j]+")").c_str(), 1, 0.0, 2.7 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Eta_2bins_"+level[j]+"fired_" + itmap->second).c_str(), ("#eta (fired "+level[j]+")").c_str(), 2, xbins), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Eta_1bin_cut_"+level[j]+"fired_" + itmap->second).c_str(), ("#eta (fired "+level[j]+") with p_{T} cut").c_str(), 1, 0.0, 2.7 ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Eta_2bins_cut_"+level[j]+"fired_" + itmap->second).c_str(), ("#eta (fired "+level[j]+")with p_{T} cut").c_str(), 2, xbins), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Phi_"+level[j]+"fired_" + itmap->second).c_str(), ("#phi (fired "+level[j]+")").c_str(), 20, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Phi_EC_"+level[j]+"fired_" + itmap->second).c_str(), ("#phi EC (fired "+level[j]+")").c_str(), 20, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
	addHistogram( new TH1F(("muZTP_Phi_B_"+level[j]+"fired_" + itmap->second).c_str(), ("#phi B (fired "+level[j]+")").c_str(), 20, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      }
      //2D eta-phi
      addHistogram( new TH2F(("muZTP_EtaPhi_all_" + itmap->second).c_str(), "Eta/phi all", 16, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      addHistogram( new TH2F(("muZTP_EtaPhi_L1_" + itmap->second).c_str(), "Eta/phi L1", 16, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      addHistogram( new TH2F(("muZTP_EtaPhi_L2_" + itmap->second).c_str(), "Eta/phi L2", 16, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      addHistogram( new TH2F(("muZTP_EtaPhi_EF_" + itmap->second).c_str(), "Eta/phi EF", 16, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      if(isefisochain) addHistogram( new TH2F(("muZTP_EtaPhi_EFIso_" + itmap->second).c_str(), "Eta/phi EF", 16, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi ), histdirmuztp );
      //2D eta-phi efficiency
      TH2F *muZTP_eff_EtaPhi_L1 = (TH2F *)hist2(("muZTP_EtaPhi_L1_" + itmap->second).c_str(), histdirmuztp)->Clone();
      muZTP_eff_EtaPhi_L1->SetName(("muZTP_eff_EtaPhi_L1_"+ itmap->second).c_str());
      muZTP_eff_EtaPhi_L1->Sumw2();
      addHistogram(muZTP_eff_EtaPhi_L1, histdirmuztp );
      TH2F *muZTP_eff_EtaPhi_L2 = (TH2F *)hist2(("muZTP_EtaPhi_L2_" + itmap->second).c_str(), histdirmuztp)->Clone();
      muZTP_eff_EtaPhi_L2->SetName(("muZTP_eff_EtaPhi_L2_"+ itmap->second).c_str());
      muZTP_eff_EtaPhi_L2->Sumw2();
      addHistogram(muZTP_eff_EtaPhi_L2, histdirmuztp );
      TH2F *muZTP_eff_EtaPhi_EF = (TH2F *)hist2(("muZTP_EtaPhi_EF_" + itmap->second).c_str(), histdirmuztp)->Clone();
      muZTP_eff_EtaPhi_EF->SetName(("muZTP_eff_EtaPhi_EF_"+ itmap->second).c_str());
      muZTP_eff_EtaPhi_EF->Sumw2();
      addHistogram(muZTP_eff_EtaPhi_EF, histdirmuztp );
      if(isefisochain) {
	TH2F *muZTP_eff_EtaPhi_EFIso = (TH2F *)hist2(("muZTP_EtaPhi_EFIso_" + itmap->second).c_str(), histdirmuztp)->Clone();
	muZTP_eff_EtaPhi_EFIso->SetName(("muZTP_eff_EtaPhi_EFIso_"+ itmap->second).c_str());
	muZTP_eff_EtaPhi_EFIso->Sumw2();
	addHistogram(muZTP_eff_EtaPhi_EFIso, histdirmuztp );
      }
      //BayesDivide
      std::vector<std::string> var;
      var.push_back("_Pt_");
      var.push_back("_Pt_EC_");
      var.push_back("_Pt_B_");
      var.push_back("_Pt_4bins_");
      var.push_back("_Pt_B_4bins_");
      var.push_back("_Eta_");
      var.push_back("_Phi_");
      var.push_back("_Phi_EC_");
      var.push_back("_Phi_B_");
      var.push_back("_Eta_1bin_");
      var.push_back("_Eta_2bins_");
      var.push_back("_Eta_1bin_cut_");
      var.push_back("_Eta_2bins_cut_");
      for(int m=0;m<9;m++){
	std::vector<std::string> ratio;
	ratio.push_back("EFwrtL2");
	ratio.push_back("EFwrtL1");
	ratio.push_back("L2wrtL1");
	if(isefisochain) ratio.push_back("EFIsowrtEF");
      }//var
    }//trigger vector
    
  }
  //else if( newLumiBlockFlag() ){  }
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::fillMuZTPDQA_wrapper()
{
  StatusCode sc;
  StoreGateSvc* p_detStore;

  //Set pointer on DetectorStore
  sc = service("DetectorStore", p_detStore);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DetectorStore service not found !" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Found DetectorStore ") ;

  const TagInfo* tagInfo = 0;
  sc = p_detStore->retrieve( tagInfo );
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not get TagInfo");
    return StatusCode::RECOVERABLE;
  } else {
    string tag;
    tagInfo->findTag("triggerStreamOfFile",tag);
    ATH_MSG_DEBUG("triggerStreamOfFile tag: " << tag);

    // if expresss, check ES bit
    if (tag == "express") { // passing standard chains: mu18it and mu24i
      ATH_MSG_DEBUG("ZTP: express stream");
      // check if the event was indeed taken by a single muon menu item
      if( m_passedES[ESSTD] )  sc = fillMuZTPDQA();
    } else {
      // if non_express stream, run on all events
      sc = fillMuZTPDQA();
    }
  }
  
  return sc;
}

StatusCode HLTMuonMonTool::fillMuZTPDQA()
{
  hist("Common_Counter", m_histdir )->Fill((float)MUZTP);

  //RETRIEVE Vertex Container
  const xAOD::VertexContainer* VertexContainer=0;
  //StatusCode sc_ztp = evtStore()->retrieve(VertexContainer,"VxPrimaryCandidate");
  StatusCode sc_ztp = evtStore()->retrieve(VertexContainer,"HLT_xAOD__VertexContainer_xPrimVx");
  if(sc_ztp.isFailure()) {
    ATH_MSG_DEBUG("VxPrimaryCandidate" << " Container Unavailable");
    //return StatusCode::SUCCESS;
  }

  //REQUIREMENTS FOR GOOD PRIMARY VERTEX   
  bool HasGoodPV = false;
  xAOD::VertexContainer::const_iterator vertexIter;
  for(vertexIter=VertexContainer->begin(); vertexIter!=VertexContainer->end(); ++vertexIter){
    if ((*vertexIter)->nTrackParticles()>2 && fabs((*vertexIter)->z()) < 150) HasGoodPV = true;                                             
    ////if ((*vertexIter)->vxTrackAtVertex().size()>2 && fabs((*vertexIter)->z()) < 150) HasGoodPV = true;                                             
  }
  if (HasGoodPV == false){ 
  ATH_MSG_DEBUG(" ===== HLT Muon has no goodpv ===== "); 
  //  return StatusCode::SUCCESS;
  }

  // ---------------------------------------------------------------

  ATH_MSG_DEBUG(" ===== START HLTMuon muZTP MonTool ===== "); 
  
  // ---------------------------------------------------------------
  // load the L1 seeds for the triggers we are studying
  if(m_ztp_newrun) {
    map<string, string> newmap(m_ztpmap); // copy the map
    for(std::map<std::string, std::string>::iterator itmap=m_ztpmap.begin();itmap!=m_ztpmap.end();++itmap){
      string chainname;
      chainname += itmap->first;
      const TrigConf::HLTChain* chain = getTDT()->ExperimentalAndExpertMethods()->getChainConfigurationDetails(chainname);
      
      if(chain) {
	
	//	ATH_MSG_DEBUG( chainname << ", L2 chain = " << chain->lower_chain_name() );
	//	m_ztp_l2map[itmap->first] = chain->lower_chain_name();

	// const TrigConf::HLTChain* l2chain = getTDT()->ExperimentalAndExpertMethods()->getChainConfigurationDetails(chain->lower_chain_name());
	// if(l2chain) {

	ATH_MSG_DEBUG( "L1 seed = " << chain->lower_chain_name() );
	// newmap[itmap->first] = l2chain->lower_chain_name();
	newmap[itmap->first] = chain->lower_chain_name();

	//	} else {
	//	  msg(MSG::INFO) << "Could not get TrigConf::HLTChain for " << chain->lower_chain_name() << ", won't process this event" << endmsg;
	//	  return StatusCode::SUCCESS;
	//      }

      } else {
	msg(MSG::INFO) << "Could not get TrigConf::HLTChain for " << chainname << ", won't process this event" << endmsg;
	return StatusCode::SUCCESS;
      }
    }
    // copy new map to the private one
    m_ztp_l1map = newmap;
    m_ztp_newrun = false;
  }

  //LOOP OVER ALL TRIGGER CHAINS
  for(std::map<std::string, std::string>::iterator itmap=m_ztpmap.begin();itmap!=m_ztpmap.end();++itmap){

    ATH_MSG_DEBUG("Starting chain " << itmap->first << " => " << itmap->second);

    std::string histdirmuztp="HLT/MuonMon/MuZTP/"+itmap->second;
    double ptcut=999999.;
    map<std::string, double>::iterator itptcut = m_ztpptcut.find(itmap->first);
    if(itptcut!=m_ztpptcut.end())ptcut=itptcut->second;
    
    bool isMSonlychain = false;
    size_t found;
    found = itmap->first.find("msonly");
    if(found != string::npos) isMSonlychain = true;

    const bool isefIsochain = (m_ztp_isomap[itmap->first] > 0);
    //    const bool ismuIsochain = (m_ztp_isomap[itmap->first] > 1);
    
    //    std::string L2_chainName = m_ztp_l2map[itmap->first];

    //CHECK IF TRIGGER HAS FIRED EVENT TO START
    bool isTriggered_L1 = false;
    const Trig::ChainGroup* ChainGroupL1 = getTDT()->getChainGroup(m_ztp_l1map[itmap->first]);
    isTriggered_L1 = ChainGroupL1->isPassed();
    //ATH_MSG_WARNING("check if Fired L1 trigger or not: "<<isTriggered_L1); // attention
    //if (isTriggered_L1 == false){
    //  return StatusCode::SUCCESS;
    //} 
    bool isTriggered_L2 = false;
    // bool isTriggered_L2 = true;
    // YY 2 Oct 2014 - L2 decision should be obtained from whether the muComb trigger element is active or not?
    // perhaps OK if a matched active TE is found - although this may not be exactly 100% correct.
    
    // const Trig::ChainGroup* ChainGroupL2 = getTDT()->getChainGroup(L2_chainName);
    // isTriggered_L2 = ChainGroupL2->isPassed();
    
    bool isTriggered_EF = false;
    const Trig::ChainGroup* ChainGroupEF = getTDT()->getChainGroup(itmap->first);
    isTriggered_EF = ChainGroupEF->isPassed();

    

    ///////////////////////////////////// get ONLINE Objects //////////////////////////////////////////

    Trig::FeatureContainer fL1 = getTDT()->features(m_ztp_l1map[itmap->first]);
    Trig::FeatureContainer fHLT = getTDT()->features(itmap->first,TrigDefs::alsoDeactivateTEs);
    
    std::vector<Trig::Combination>::const_iterator jL1;    
    std::vector<Trig::Combination>::const_iterator jL2;    
    std::vector<Trig::Combination>::const_iterator jEF;    
        
    //    int iroiL1=1;
    int iroiL2=1;
    int iroiEF=1;

    std::vector<float> EFIsopt, EFIsoeta, EFIsophi;
    std::vector<float> EFCbpt, EFCbeta, EFCbphi;
    std::vector<float> EFExpt, EFExeta, EFExphi;
    std::vector<float> L2Cbpt, L2Cbeta, L2Cbphi;
    //    std::vector<float> L2Isopt, L2Isoeta, L2Isophi;
    std::vector<float> L2Expt, L2Exeta, L2Exphi;
    std::vector<float> L1pt, L1eta, L1phi;

    /////// L1 /////////////

    // add by Yuan
    // YY moved to xAOD 2 Oct 2014
    // const LVL1_ROI* lvl1Roi;
    // StatusCode sc = evtStore()->retrieve(lvl1Roi,"LVL1_ROI");
    const xAOD::MuonRoIContainer* lvl1Roi;
    std::string muonKeyL1 = "LVL1MuonRoIs"; // MuidMuonCollection

    StatusCode sc = evtStore()->retrieve(lvl1Roi, muonKeyL1);
    if (sc.isFailure()) {
      cout<< " can not retrieve LVL1 ROI"<<endl;
    } else {
      if (lvl1Roi) {
	//	LVL1_ROI::muons_type::const_iterator it = (lvl1Roi->getMuonROIs()).begin();
	//	LVL1_ROI::muons_type::const_iterator it_end = (lvl1Roi->getMuonROIs()).end();
	xAOD::MuonRoIContainer::const_iterator it = lvl1Roi->begin(); 
	xAOD::MuonRoIContainer::const_iterator it_end = lvl1Roi->end(); 
	for ( ; it != it_end ; ++it ) {
	  if(!m_HI_pp_mode){
	    if (
		("L1_MU0"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU0" || (*it)->thrName() == "MU4" || (*it)->thrName() == "MU6"|| (*it)->thrName() == "MU10"|| (*it)->thrName() == "MU11"|| (*it)->thrName() == "MU15"|| (*it)->thrName() == "MU20")) ||
		("L1_MU4"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU4"|| (*it)->thrName() == "MU6"|| (*it)->thrName() == "MU10"|| (*it)->thrName() == "MU11"|| (*it)->thrName() == "MU15"|| (*it)->thrName() == "MU20" )) ||
		("L1_MU6"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU6"|| (*it)->thrName() == "MU10"|| (*it)->thrName() == "MU11"|| (*it)->thrName() == "MU15"|| (*it)->thrName() == "MU20"))||
		("L1_MU10"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU10"|| (*it)->thrName() == "MU11"|| (*it)->thrName() == "MU15"|| (*it)->thrName() == "MU20"))||
		("L1_MU11"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU11"|| (*it)->thrName() == "MU15"|| (*it)->thrName() == "MU20"))
	       ) {
	      L1pt.push_back((*it)->thrValue());
	      L1eta.push_back((*it)->eta());
	      L1phi.push_back((*it)->phi());
	    }
	  }else{

	    if (
		("L1_MU10"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU10" || (*it)->thrName() == "MU15" || (*it)->thrName() == "MU20")) || 
		("L1_MU15"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU15" || (*it)->thrName() == "MU20")) || 
		("L1_MU20,L1_MU21"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU21")) ||
		("L1_MU21"==m_ztp_l1map[itmap->first] && ((*it)->thrName() == "MU21"))
	       ) {
	      L1pt.push_back((*it)->thrValue());
	      L1eta.push_back((*it)->eta());
	      L1phi.push_back((*it)->phi());
	    }  
	  }
	}
      }
    }
    // end the code add by Yuan

    // 2 Oct 2014 YY: not simple to find an active TE? think about it
    // bool L2CBActive = false;
    // bool L2SAActive = false;
    
    /////// L2 /////////////
    // 2 Oct 2014 - YY: getting CombinedMuonFeature from HLT combinations
    for (jL2 = fHLT.getCombinations().begin(); jL2 != fHLT.getCombinations().end(); ++jL2, ++iroiL2) {
      if (!isMSonlychain) {
	//muComb
	//	if(!ismuIsochain)
	std::vector<Trig::Feature<xAOD::L2CombinedMuonContainer> >
	  muCombL2Feature = (*jL2).get<xAOD::L2CombinedMuonContainer>("MuonL2CBInfo",TrigDefs::alsoDeactivateTEs); 
	if (muCombL2Feature.size()!=1) {
	  ATH_MSG_DEBUG( "Vector of L2 muComb InfoContainers size is not 1" );	
	} else {
	  const xAOD::L2CombinedMuonContainer* muCombL2 = muCombL2Feature[0];
	  if (!muCombL2) {
	    ATH_MSG_DEBUG( "No muComb track found" );
	  } else {
	    ATH_MSG_DEBUG( " muComb muon exists " );
	    //    L2CBActive = muCombL2.te()->getActiveState();
	    L2Cbpt.push_back(muCombL2->at(0)->pt());
	    L2Cbeta.push_back(muCombL2->at(0)->eta());
	    L2Cbphi.push_back(muCombL2->at(0)->phi());
	  }
	}
      }//!isMSonlychain

      //muFast
      std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > muFastL2Feature = (*jL2).get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);
      if (muFastL2Feature.size() != 1) {
	ATH_MSG_DEBUG( "Vector of L2 muFast InfoContainers size is not 1" );	
      } else {
	const xAOD::L2StandAloneMuonContainer* muFastL2 = muFastL2Feature[0];
	if (!muFastL2) {
	  ATH_MSG_DEBUG( "No mufast track found" );
	} else {
	  ATH_MSG_DEBUG( " muFast muon exists " );
	  //	  L2SAActive = muFastL2.te()->getActiveState();
	  L2Expt.push_back( muFastL2->at(0)->pt());
	  L2Exeta.push_back(muFastL2->at(0)->eta());
	  L2Exphi.push_back(muFastL2->at(0)->phi());
	}
      }
    }//jL2
    
    ATH_MSG_DEBUG("Starting to extract EF objects, N(EF) Combinations = " << fHLT.getCombinations().size());

    /////// EF /////////////
    for (jEF=fHLT.getCombinations().begin() ; jEF!=fHLT.getCombinations().end() ; ++jEF, ++iroiEF){


      // make sure this ROI triggered the event
      if(jEF->active()) {

	std::vector< Trig::Feature<xAOD::MuonContainer> > efarr[2];
	efarr[0] = jEF->get<xAOD::MuonContainer>("MuonEFInfo",TrigDefs::alsoDeactivateTEs);
	ATH_MSG_DEBUG("...MuonEFInfo size=" << efarr[0].size() );
	efarr[1] = jEF->get<xAOD::MuonContainer>("eMuonEFInfo",TrigDefs::alsoDeactivateTEs);
	ATH_MSG_DEBUG("...eMuonEFInfo size=" << efarr[1].size() );
	for (int ief = 0; ief < 2; ief++) {
	  if( efarr[ief].size() >= 1 ) {
	    if(ief == 0) {
	      ATH_MSG_DEBUG("MuonEFInfo container");
	    } else if (ief == 1) {
	      ATH_MSG_DEBUG("eMuonEFInfo container");
	    }
	    std::vector< Trig::Feature<xAOD::MuonContainer> > ef = efarr[ief]; 
	    ATH_MSG_DEBUG("ZTP...EFInfo: label/active=" << Trig::getTEName(*ef[0].te()) << " / " << ef[0].te()->getActiveState());

	    //// extracting EF track properties ////
	      
	    std::vector< Trig::Feature<xAOD::MuonContainer> >::const_iterator fIt = ef.begin();
	    for ( ; fIt != ef.end() ; ++fIt) {
	      const xAOD::MuonContainer* ef_cont = (*fIt);
	      if (ef_cont) {
		ATH_MSG_DEBUG("ZTP ef_contContainter size:"<<ef_cont->size());

		for(int iCont=0; iCont<(int)ef_cont->size(); iCont++) {


		  if (!isMSonlychain) {
		    const xAOD::TrackParticle *ef_cb_trk
		      = ef_cont->at(iCont)->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
		    if (ef_cb_trk) {
		      ATH_MSG_DEBUG("ZTP EFCB " << ef_cb_trk->pt() << " eta " << ef_cb_trk->eta() << " phi " << ef_cb_trk->phi());  
		      EFCbpt.push_back(fabs(ef_cb_trk->pt()) / CLHEP::GeV * ef_cb_trk->charge());
		      EFCbeta.push_back(ef_cb_trk->eta());
		      EFCbphi.push_back(ef_cb_trk->phi());

		      if(isefIsochain) {
			  float ptcone30;
			  ef_cont->at(iCont)->isolation(ptcone30, xAOD::Iso::ptcone30);
			  if(ptcone30/ef_cont->at(iCont)->pt() < m_ztp_EF_ptcone30rel_cut ) {
			      EFIsopt.push_back(fabs(ef_cb_trk->pt()) / CLHEP::GeV * ef_cb_trk->charge());
			      EFIsoeta.push_back(ef_cb_trk->eta());
			      EFIsophi.push_back(ef_cb_trk->phi());
			  }
		      }
		    } else {
		      ATH_MSG_DEBUG("ZTP EFcb none");
		    }
		  }

		  const xAOD::TrackParticle *ef_sa_trk
		    = ef_cont->at(iCont)->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
		  if (ef_sa_trk && ef_cont->at(iCont)->muonType()==xAOD::Muon::MuonType::MuonStandAlone) {
		    ATH_MSG_DEBUG("ZTP EFSA " << ef_sa_trk->pt() << " eta " << ef_sa_trk->eta() << " phi " << ef_sa_trk->phi());  
		    EFExpt.push_back(fabs(ef_sa_trk->pt()) / CLHEP::GeV * ef_sa_trk->charge());
		    EFExeta.push_back(ef_sa_trk->eta());
		    EFExphi.push_back(ef_sa_trk->phi());
		  } else {
		      ATH_MSG_DEBUG("ZTP EFsa none");
		  }

		
		}

	      }
	    }
	  }
	    
	} // ief
      } //active ROI
    } // jEF
	  

    //  check if there is at least one activated L2 object to determine L2 trigger decision  
    if(isMSonlychain) isTriggered_L2 = ( L2Expt.size() > 0) ;
    else isTriggered_L2 = (L2Cbpt.size() > 0);


    ///////////////////////////////////// get OFFLINE Objects //////////////////////////////////////////
   
    int n_RecCBmuon=0;
    std::vector<float> RecCBpt, RecCBeta, RecCBphi;
    std::vector<float> RecCBpx, RecCBpy, RecCBpz, RecCBe, RecCBcharge;
    std::vector<bool> passedchainL1;
    std::vector<bool> passedSAchainL2;
    std::vector<bool> passedCBchainL2;
    std::vector<bool> passedisochainL2;
    std::vector<bool> passedSAchainEF;
    std::vector<bool> passedCBchainEF;
    std::vector<bool> passedisochainEF;

    // Retrieve Muon Offline Reco MuidMuonCollection
    //const Analysis::MuonContainer* muonCont=0;
    const xAOD::MuonContainer* muonCont;
    
    std::string muonKey = "Muons";  /// default MuidMuonCollection
    sc = evtStore()->retrieve( muonCont, muonKey); 
    if(sc.isFailure() || !muonCont){
      ATH_MSG_WARNING( "Container of muon particle with key " << muonKey << " not found in Store Gate with size " );
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG(  "Container of muon particle with key found in Store Gate with size " << muonCont->size() );
      xAOD::MuonContainer::const_iterator contItr  = muonCont->begin();
      xAOD::MuonContainer::const_iterator contItrE = muonCont->end();
      
      for (; contItr != contItrE; contItr++){
	
	const xAOD::Muon* recMuon = *contItr;
	float pt  = - 999999.;
	float px  = - 999999.;
	float py  = - 999999.;
	float pz  = - 999999.;
	float e = -999.;
	float eta = -999.;
	float phi = -999.;
	float charge = -999.;
	
	ATH_MSG_DEBUG(  "check CB muon " );
	if (recMuon->combinedTrackParticleLink()!=0 ){
	  
	  ATH_MSG_DEBUG(  "CB muon found" );
	if(recMuon->muonType() != xAOD::Muon::MuonType::Combined) ATH_MSG_WARNING( " Combined track link found, but not a combined muon!!!!");
	  
	  n_RecCBmuon++;

	  const xAOD::TrackParticle* muidCBMuon =0;
	  muidCBMuon = *((*contItr)->combinedTrackParticleLink());
	  pt = muidCBMuon->pt()/CLHEP::GeV;
	  eta = muidCBMuon->eta();
	  phi = muidCBMuon->phi();
	  charge = muidCBMuon->charge();
	  e = muidCBMuon->e();
	  px = (muidCBMuon->p4()).Px();
	  py = (muidCBMuon->p4()).Py();
	  pz = (muidCBMuon->p4()).Pz();

	  if(!m_HI_pp_mode){
	    if( pt < 1.0 ) continue;
	  }else{
	    if(pt < 10.0) continue;
	  }
	  // isolation cut (sumpt / pt)
	  float ptcone30;
	  (*contItr)->isolation(ptcone30, xAOD::Iso::ptcone30); 

	  if(ptcone30 / muidCBMuon->pt() > m_ztp_ptcone30rel_cut) continue;

	  RecCBpt.push_back(pt);
	  RecCBpx.push_back(px);
	  RecCBpy.push_back(py);
	  RecCBpz.push_back(pz);
	  RecCBe.push_back(e);
	  RecCBcharge.push_back(charge);
	  RecCBeta.push_back(eta);
	  RecCBphi.push_back(phi);
	
	  //match offline object to online trigger object
	  passedchainL1.push_back(CheckMuonTriggerMatch(eta,phi, L1eta, L1phi));  
	  passedSAchainL2.push_back(CheckMuonTriggerMatch(eta,phi, L2Exeta, L2Exphi));
	  passedCBchainL2.push_back(CheckMuonTriggerMatch(eta,phi, L2Cbeta, L2Cbphi));
	  //	  passedisochainL2.push_back(CheckMuonTriggerMatch(eta,phi, L2Isoeta, L2Isophi));
	  passedSAchainEF.push_back(CheckMuonTriggerMatch(eta,phi, EFExeta, EFExphi));
	  passedCBchainEF.push_back(CheckMuonTriggerMatch(eta,phi, EFCbeta, EFCbphi));
	  passedisochainEF.push_back(CheckMuonTriggerMatch(eta,phi, EFIsoeta, EFIsophi));
	}
      }	      
    }///offline loop
    
    if(n_RecCBmuon<2) continue;
    
    //loop over TAG muons       
    for(unsigned int tag=0;tag<RecCBpt.size();++tag){
      // combined chains - tag must match combined EF muon
      if(!isMSonlychain && !passedCBchainEF[tag]) continue;
      // MS only chain - tag must match SA EF muon
      if(isMSonlychain && !passedSAchainEF[tag]) continue;
      // isolated chain - tag must match muon from isolation object
      if(isefIsochain && !passedisochainEF[tag]) continue;  

      //loop over PROBE muons
      for(unsigned int probe=0;probe<RecCBpt.size();++probe){
	if(tag==probe) continue;
	if(RecCBcharge[tag]==RecCBcharge[probe]) continue;
	
	float probept = RecCBpt[probe];
	float probeeta = RecCBeta[probe];
	float fabsprobeeta = fabs(probeeta);
	float probephi = RecCBphi[probe];
	float pxSum = RecCBpx[tag] + RecCBpx[probe]; 
	float pySum =  RecCBpy[tag] + RecCBpy[probe]; 
	float pzSum =  RecCBpz[tag] + RecCBpz[probe]; 
	float eSum =  RecCBe[tag] + RecCBe[probe]; 
	float invMass = sqrt(eSum*eSum - pxSum*pxSum - pySum*pySum - pzSum*pzSum);
	hist(("muZTP_invmass_nomasswindow_" + itmap->second).c_str(), histdirmuztp)->Fill(invMass);

	bool isEndcap = false;
	if(fabs(probeeta) > 1.05) isEndcap = true;
	bool masswindow = false;
	if(!m_HI_pp_mode){
	if(fabs(invMass - 3097) < 300.0) masswindow = true;
	}else{
	  if(fabs(invMass - 91187.6) < 12000.0) masswindow = true;
	}
	if(masswindow){
	  //all probes
	  hist(("muZTP_invmass_" + itmap->second).c_str(), histdirmuztp)->Fill(invMass);
	  hist(("muZTP_Pt_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  hist(("muZTP_Pt_4bins_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  if( isEndcap ) hist(("muZTP_Pt_EC_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  if( !isEndcap ){
	    hist(("muZTP_Pt_B_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_B_4bins_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  }
	  hist(("muZTP_Eta_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	  hist(("muZTP_Eta_1bin_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  hist(("muZTP_Eta_2bins_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  if(RecCBpt[probe] > ptcut){
	    hist(("muZTP_Eta_1bin_cut_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_cut_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  }	 
	  hist(("muZTP_Phi_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  if( isEndcap ) hist(("muZTP_Phi_EC_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  if( !isEndcap ) hist(("muZTP_Phi_B_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  hist2(("muZTP_EtaPhi_all_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  //L1
	  if(isTriggered_L1 && passedchainL1[probe]) {
	    hist(("muZTP_Pt_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_L1fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    hist2(("muZTP_EtaPhi_L1_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  }
	  //L2
	  if(isTriggered_L2 && isMSonlychain && passedSAchainL2[probe] && passedchainL1[probe]) { //muFast
	    hist(("muZTP_Pt_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    hist2(("muZTP_EtaPhi_L2_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  }
	  //	  if(isTriggered_L2 && !isMSonlychain && !ismuIsochain && passedCBchainL2[probe])  //muComb
	  if(isTriggered_L2 && !isMSonlychain && passedCBchainL2[probe] && passedchainL1[probe]) { //muComb
	    hist(("muZTP_Pt_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    hist2(("muZTP_EtaPhi_L2_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  }
	  // if(isTriggered_L2 && !isMSonlychain && ismuIsochain && passedisochainL2[probe]) { //muIso
	  //   hist(("muZTP_Pt_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //   hist(("muZTP_Pt_4bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //   if( isEndcap ) hist(("muZTP_Pt_EC_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //   if( !isEndcap ){
	  //     hist(("muZTP_Pt_B_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //     hist(("muZTP_Pt_B_4bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //   }
	  //   hist(("muZTP_Eta_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	  //   hist(("muZTP_Eta_1bin_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //   hist(("muZTP_Eta_2bins_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //   if(RecCBpt[probe] > ptcut){
	  //     hist(("muZTP_Eta_1bin_cut_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //     hist(("muZTP_Eta_2bins_cut_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //   }
	  //   hist(("muZTP_Phi_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  //   if( isEndcap ) hist(("muZTP_Phi_EC_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  //   if( !isEndcap ) hist(("muZTP_Phi_B_L2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  //   hist2(("muZTP_EtaPhi_L2_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  // }
	  //EF
	  if(isTriggered_EF && isMSonlychain && passedSAchainEF[probe] && passedchainL1[probe]){
	    hist(("muZTP_Pt_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    hist2(("muZTP_EtaPhi_EF_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  }
	  if(isTriggered_EF && !isMSonlychain && passedCBchainEF[probe] && passedchainL1[probe]) {
	    hist(("muZTP_Pt_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_EFfired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    hist2(("muZTP_EtaPhi_EF_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  }
	  if(isTriggered_EF && !isMSonlychain && isefIsochain && passedisochainEF[probe] && passedchainL1[probe]) {
	    hist(("muZTP_Pt_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_EFIsofired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    hist2(("muZTP_EtaPhi_EFIso_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta, probephi);
	  }
	  //EF && L2 - include the isolation if needed
	  if(isTriggered_EF && isMSonlychain && passedSAchainEF[probe] && passedSAchainL2[probe] && passedchainL1[probe]){
	    hist(("muZTP_Pt_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    hist(("muZTP_Pt_4bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( isEndcap ) hist(("muZTP_Pt_EC_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    if( !isEndcap ){
	      hist(("muZTP_Pt_B_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_B_4bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	    }
	    hist(("muZTP_Eta_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	    hist(("muZTP_Eta_1bin_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    hist(("muZTP_Eta_2bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    if(RecCBpt[probe] > ptcut){
	      hist(("muZTP_Eta_1bin_cut_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_cut_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	    }
	    hist(("muZTP_Phi_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( isEndcap ) hist(("muZTP_Phi_EC_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    if( !isEndcap ) hist(("muZTP_Phi_B_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  }
	  //	  if(isTriggered_EF && !isMSonlychain && !ismuIsochain && passedCBchainEF[probe] && passedCBchainL2[probe])  // !ismuIsochain
	  if(isTriggered_EF && !isMSonlychain && passedCBchainEF[probe] && passedCBchainL2[probe] && passedchainL1[probe]) { // !ismuIsochain
	    if( (!isefIsochain || passedisochainEF[probe]) ) {
	      hist(("muZTP_Pt_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      hist(("muZTP_Pt_4bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      if( isEndcap ) hist(("muZTP_Pt_EC_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      if( !isEndcap ){
		hist(("muZTP_Pt_B_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
		hist(("muZTP_Pt_B_4bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	      }
	      hist(("muZTP_Eta_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	      hist(("muZTP_Eta_1bin_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      hist(("muZTP_Eta_2bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      if(RecCBpt[probe] > ptcut){
		hist(("muZTP_Eta_1bin_cut_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
		hist(("muZTP_Eta_2bins_cut_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	      }
	      hist(("muZTP_Phi_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	      if( isEndcap ) hist(("muZTP_Phi_EC_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	      if( !isEndcap ) hist(("muZTP_Phi_B_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	    }
	  }
	  // if(isTriggered_EF && !isMSonlychain && ismuIsochain && passedCBchainEF[probe] && passedisochainL2[probe]) {
	  //   if( (!isefIsochain || passedisochainEF[probe]) ) {
	  //     hist(("muZTP_Pt_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //     hist(("muZTP_Pt_4bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //     if( isEndcap ) hist(("muZTP_Pt_EC_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //     if( !isEndcap ){
	  // 	hist(("muZTP_Pt_B_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  // 	hist(("muZTP_Pt_B_4bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probept);
	  //     }
	  //     hist(("muZTP_Eta_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probeeta);
	  //     hist(("muZTP_Eta_1bin_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //     hist(("muZTP_Eta_2bins_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //     if(RecCBpt[probe] > ptcut){
	  // 	hist(("muZTP_Eta_1bin_cut_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  // 	hist(("muZTP_Eta_2bins_cut_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(fabsprobeeta);
	  //     }
	  //     hist(("muZTP_Phi_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  //     if( isEndcap ) hist(("muZTP_Phi_EC_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  //     if( !isEndcap ) hist(("muZTP_Phi_B_EFL2fired_" + itmap->second).c_str(), histdirmuztp)->Fill(probephi);
	  //   }
	  // }
	} // masswindow
      } // loop over probes
    } // loop over tags 
  } // loop over trigger vector 

  return StatusCode::SUCCESS;
} // end of fillMuZTPDQA

StatusCode HLTMuonMonTool::procMuZTPDQA()
{
  if( endOfRunFlag() ){

    for(std::map<std::string, std::string>::iterator itmap=m_ztpmap.begin();itmap!=m_ztpmap.end();++itmap){

      std::string histdirmuztp="HLT/MuonMon/MuZTP/"+itmap->second;

      //efficiency histograms
      std::vector<std::string> var;
      var.push_back("_Pt_");
      var.push_back("_Pt_EC_");
      var.push_back("_Pt_B_");
      var.push_back("_Pt_4bins_");
      var.push_back("_Pt_B_4bins_");
      var.push_back("_Eta_");
      var.push_back("_Phi_");
      var.push_back("_Phi_EC_");
      var.push_back("_Phi_B_");
      for(unsigned int k=0;k<var.size();k++){
	std::vector<std::string> level;
	level.push_back("L1");
	level.push_back("L2");
	level.push_back("EF");
	if(m_ztp_isomap[itmap->first] > 0) level.push_back("EFIso");
	for(unsigned int j=0;j<level.size();j++){
	  ATH_MSG_DEBUG(itmap->first << " " << level[j] << " " << var[k]);
	  //ABSOLUTE
	  //2D ETA_PHI
	  hist2(("muZTP_eff_EtaPhi_"+level[j]+"_" + itmap->second).c_str(), histdirmuztp)->Divide( hist2(("muZTP_EtaPhi_"+level[j]+"_" + itmap->second).c_str(), histdirmuztp), hist2(("muZTP_EtaPhi_all_"+ itmap->second).c_str(), histdirmuztp), 1, 1, "B");
	}//level
	//RELATIVE
      }//var
    }
    
  }
  //else if( endOfLumiBlockFlag() ){  }
  return StatusCode::SUCCESS;
}


///////////////////////////////////// FUNCTION DEFINITIONS //////////////////////////////////////////

bool CheckMuonTriggerMatch(float off_eta, float off_phi, const std::vector<float>& v_on_eta, const std::vector<float>& v_on_phi)
{

  float deltaRcut = 0.15; 
  float deltaR= 999999.;
  for(unsigned int i=0; i< v_on_eta.size();++i){
    float dr = CalcdeltaR(off_eta,off_phi,v_on_eta[i],v_on_phi[i]);
    if(dr<deltaR){
      deltaR=dr;
    }
  }
  
  if(deltaR<deltaRcut)return true;
  else return false;
}
///////////////////////////////////////////////////////////////////////////////

float CalcdeltaR(float off_eta, float off_phi,float on_eta, float on_phi){

  float deta = off_eta - on_eta;
  float dphi = off_phi - on_phi;
  float dR=0;
  if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
  if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
  dR = sqrt( deta*deta + dphi*dphi ) ;

  return dR;
}
///////////////////////////////////////////////////////////////////////////////
