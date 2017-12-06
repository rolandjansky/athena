/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************
//
// NAME:     HLTBjetMonTool.cxx
// PACKAGE:  Trigger/TrigMonitoring/TrigBjetMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@cern.ch
//
// REVISED for RUN2: Elemer Nagy
// EMAIL:            Elemer.Nagy@cern.ch
//
// ***************************************************

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"   //EN

#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"

#include "JetUtils/JetCaloQualityUtils.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "Particle/TrackParticleContainer.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrigParticle/TrigEFBjet.h"
#include "TrigParticle/TrigEFBjetContainer.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include <vector>
#include <map>
/* Shaun Roe says it is alread included elsewhere so no need to be here
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
*/
// #include <math.h> // Shaun Roe says the use of all '.h' headers of the C++ standard library is deprecated
#include <cmath>

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

#include "xAODTracking/VertexContainer.h"

#include "xAODTracking/TrackParticle.h"

#include "xAODEventInfo/EventInfo.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"



#include "TrigBjetMonitoring/HLTBjetMonTool.h"

#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "GaudiKernel/ListItem.h"

namespace Analysis { class MuonContainer; }



//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::HLTBjetMonTool(const std::string & type, const std::string & name, const IInterface* parent) :
  IHLTMonTool(type, name, parent),
  m_trackJetFinderTool("TrigTrackJetFinderTool"),
  m_TriggerChainBjet{}, m_TriggerChainMujet{},
  m_TriggerChainBjet_x{}, m_TriggerChainMujet_x{},
  m_Chain2Dir{},
  m_Shifter_jSplit{}, m_Expert_jSplit{}, m_Shifter_jUnSplit{},  m_Expert_jUnSplit{}, m_Shifter_mujet{},m_Expert_mujet{},
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_etCut(10.), m_sv1_infosource("SV1")
{
  declareProperty ("monitoring_bjet",       m_TriggerChainBjet_x);
  declareProperty ("monitoring_mujet",      m_TriggerChainMujet_x);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::~HLTBjetMonTool() {


}

//** ----------------------------------------------------------------------------------------------------------------- **//

float HLTBjetMonTool::phiCorr(float phi) {
  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;
  return phi;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::init() {

  ATH_CHECK(m_trackJetFinderTool.retrieve());

  ATH_MSG_INFO("in HLTBjetMonTool::init - retrieved tool: " << m_trackJetFinderTool);

  m_etCut = 10.;
  ATH_MSG_INFO(" ===> in HLTBjetMonTool::init - Muon-jet parameters: m_etCut = " << m_etCut);

  m_sv1_infosource = "SV1";
  ATH_MSG_INFO(" ===> in HLTBjetMonTool::init - SV1  parameters: inputSV1SourceName = "  <<  m_sv1_infosource);


  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::proc(){
#else
  StatusCode HLTBjetMonTool::proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun){
#endif

    ATH_MSG_INFO("in HLTBjetMonTool::proc");

  return StatusCode::SUCCESS;

  }


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::book(){
#else
  StatusCode HLTBjetMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun){
#endif


    ATH_MSG_INFO("HLTBjetMonTool::book");
    ATH_MSG_INFO(" entered HLTBjetMonTool::book");

    addMonGroup(new MonGroup(this,"HLT/BjetMon", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN

    ATH_MSG_INFO("in HLTBjetMonTool::book added directory HLT/BjetMon, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED);

    ATH_MSG_INFO("in HLTBjetMonTool::book newRun: " << newRunFlag() );



    if(newRunFlag()){

      //#define TestOnTrigConf

      // Decode extended TriggerChain vectors from the python config file
      // Create conventional TriggerChain vectors
      // Verify if a chain is not configured, in that case discard it

      int size_TriggerChainBjet = m_TriggerChainBjet_x.size();
      int size_TriggerChainMujet = m_TriggerChainMujet_x.size();
      std::string chainName, folderName;

      ATH_MSG_INFO("         Extended Chain Name sizes - Bjet: " << size_TriggerChainBjet << " MuJet: " << size_TriggerChainMujet );
      for (int i =0; i<size_TriggerChainBjet; i++){
	chainName = m_TriggerChainBjet_x.at(i);
	ATH_MSG_INFO("         Chain number: " << i << " Bjet Chain Name: " << chainName );
	// Investigate by the 1st character which folder should be assigned
	if (chainName.substr(0,1) == "E") {
	  chainName = chainName.substr (2);
#ifdef TestOnTrigConf
	  if ( (m_trigDec->getListOfTriggers( chainName)).empty() ) continue;
#endif
	  m_TriggerChainBjet.push_back(chainName);
	  m_Chain2Dir[chainName] = "Expert";
	} else if (chainName.substr(0,1) == "S") {
	  chainName = chainName.substr (2);
#ifdef TestOnTrigConf
	  if ( (m_trigDec->getListOfTriggers( chainName)).empty() ) continue;
#endif
	  m_TriggerChainBjet.push_back(chainName);
	  m_Chain2Dir[chainName] = "Shifter";
	} else {
	  ATH_MSG_INFO("         Chain number: " << i << " Bjet Chain Name: " << chainName << " is not assigned to Expert or Shifter folders !!" );
	  return StatusCode::FAILURE;
	} // else
	ATH_MSG_DEBUG("         Chain number: " << i << " Bjet Chain Name: " << chainName );
      }
      for (int i =0; i<size_TriggerChainMujet; i++){
	chainName = m_TriggerChainMujet_x.at(i);
	ATH_MSG_INFO("         Chain number: " << i << " Mujet Chain Name: " << chainName );
	// Investigate by the 1st character which folder should be assigned
	if (chainName.substr(0,1) == "E") {
	  chainName = chainName.substr (2);
#ifdef TestOnTrigConf
	  if ( (m_trigDec->getListOfTriggers( chainName)).empty() ) continue;
#endif
	  m_TriggerChainMujet.push_back(chainName);
	  m_Chain2Dir[chainName] = "Expert";
	} else if (chainName.substr(0,1) == "S") {
	  chainName = chainName.substr (2);
#ifdef TestOnTrigConf
	  if ( (m_trigDec->getListOfTriggers( chainName)).empty() ) continue;
#endif
	  m_TriggerChainMujet.push_back(chainName);
	  m_Chain2Dir[chainName] = "Shifter";
	} else {
	  ATH_MSG_INFO("         Chain number: " << i << " Mujet Chain Name: " << chainName << " is not assigned to Expert or Shifter folders !!" );
	  return StatusCode::FAILURE;
	} // else
	ATH_MSG_DEBUG("         Chain number: " << i << " Mujet Chain Name: " << chainName );
      }


  // Look for the created TriggerChain vector - if it is empty and if the triggers are configured

      size_TriggerChainBjet = m_TriggerChainBjet.size();
      size_TriggerChainMujet = m_TriggerChainMujet.size();
      ATH_MSG_INFO("         Chain Name sizes - Bjet: " << size_TriggerChainBjet << " MuJet: " << size_TriggerChainMujet );
      if ( (size_TriggerChainBjet+size_TriggerChainMujet) <= 0) {
	ATH_MSG_INFO( " No trigger chain is configured for this run ==> Stop monitoring " );
	return StatusCode::FAILURE;
      }
      for (int i =0; i<size_TriggerChainBjet; i++){
	chainName = m_TriggerChainBjet.at(i);
	ATH_MSG_INFO("         Chain number: " << i << " Bjet Chain Name: " << chainName );
	std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
	ATH_MSG_DEBUG( " In HLTBjetMonTool::book(): Trigger chain " << chainName << " Size of selectChains " << selectChains.size());
      }
      for (int i =0; i<size_TriggerChainMujet; i++){
	chainName = m_TriggerChainMujet.at(i);
	ATH_MSG_INFO("         Chain number: " << i << " Mujet Chain Name: " << chainName );
	std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
	ATH_MSG_DEBUG( " In HLTBjetMonTool::book(): Trigger chain " << chainName << " Size of selectChains " << selectChains.size());
      }

      // Check if folder assignment is made for all chains - if not stop the program
      // Regroup chain names with identical histograms

      std::map<std::string,std::string>::iterator it = m_Chain2Dir.begin();
      while (it != m_Chain2Dir.end()) {
	ATH_MSG_DEBUG(it->first << " :: " << it->second );
	it++;
      }
      std::map<std::string,std::string>::iterator p;
      for (const auto & chainName:m_TriggerChainBjet){     // Shaun Roe 21/9/16
	p = m_Chain2Dir.find(chainName);
	if ( p != m_Chain2Dir.end() ) {
	  folderName = p->second;
	} else {
	  ATH_MSG_INFO(" Chain Name " << chainName << " has no folder name - verify HLTBjetMonTool::book() !!!" );
	  return StatusCode::FAILURE;
	}
	ATH_MSG_INFO( " Chain name " << chainName << " is in folder " << folderName);
	if ( (folderName != "Expert") && (folderName != "Shifter") ){
	  ATH_MSG_INFO( " Chain name " << chainName << " is neither in Expert nor in Shifter folder - verify HLTBjetMonTool::book() !!!");
	  return StatusCode::FAILURE;
	} //if
	std::size_t found = chainName.find("split");
	if (found!=std::string::npos) {
	  if (folderName == "Shifter") m_Shifter_jSplit.push_back(chainName);
	  if (folderName == "Expert") m_Expert_jSplit.push_back(chainName);
	} else {
	  if (folderName == "Shifter") m_Shifter_jUnSplit.push_back(chainName);
	  if (folderName == "Expert") m_Expert_jUnSplit.push_back(chainName);
	}
      } //i Bjet
      for (const auto & chainName:m_TriggerChainMujet){    // Shaun Roe 21/9/16
	p = m_Chain2Dir.find(chainName);
	if ( p != m_Chain2Dir.end() ) {
	  folderName = p->second;
	} else {
	  ATH_MSG_INFO(" Chain Name " << chainName << " has no folder name - verify HLTBjetMonTool::book() !!!" );
	  return StatusCode::FAILURE;
	}
	ATH_MSG_INFO( " Chain name " << chainName << " is in folder " << folderName);
	if (folderName == "Shifter") m_Shifter_mujet.push_back(chainName);
	else if (folderName == "Expert") m_Expert_mujet.push_back(chainName);
	else {
	  ATH_MSG_INFO( " Chain name " << chainName << " is neither in Expert nor in Shifter folder - verify HLTBjetMonTool::book() !!!");
	  return StatusCode::FAILURE;
	} //if
      } //i Mujet

      for (unsigned int i = 0; i< m_Shifter_mujet.size(); i++) {
	ATH_MSG_DEBUG(" m_Shifter_mujet: " << m_Shifter_mujet.at(i) );
      }
      for (unsigned int i = 0; i< m_Expert_mujet.size(); i++) {
	ATH_MSG_DEBUG(" m_Expert_mujet: " << m_Expert_mujet.at(i) );
      }
      for (unsigned int i = 0; i< m_Shifter_jUnSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Shifter_jUnSplit: " << m_Shifter_jUnSplit.at(i) );
      }
      for (unsigned int i = 0; i< m_Expert_jUnSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Expert_jUnSplit: " << m_Expert_jUnSplit.at(i) );
      }
      for (unsigned int i = 0; i< m_Shifter_jSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Shifter_jSplit: " << m_Shifter_jSplit.at(i) );
      }
      for (unsigned int i = 0; i< m_Expert_jSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Expert_jSplit: " << m_Expert_jSplit.at(i) );
      }


      // Shifter Folders

      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );

      //    Off-line
      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/Offline", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/Offline, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
      // PV
      addHistogram(new TH1F("nPV","Number of offline PV per event", 100, 0., 100.));
      addHistogram(new TH1F("PVx","offline xPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVy","offline yPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVz","offline zPV", 200, -200., 200.));

      //    On-line SplitChains
      //      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/Online_SplitChains", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      //      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/Online_SplitChains, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
      for (unsigned int i = 0; i< m_Shifter_jSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Shifter_jSplit: " << m_Shifter_jSplit.at(i) );
	addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/"+m_Shifter_jSplit.at(i), run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
	ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/" << m_Shifter_jSplit.at(i) << " run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
	// PV
	addHistogram(new TH1F(("nPV_tr_"+m_Shifter_jSplit.at(i)).c_str(),"Number of online PV per event for xPrimVtx", 101, -1., 100.));
	addHistogram(new TH1F(("PVx_tr_"+m_Shifter_jSplit.at(i)).c_str(),"online (1st) xPV for xPrimVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVy_tr_"+m_Shifter_jSplit.at(i)).c_str(),"online (1st) yPV for xPrimVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVz_tr_"+m_Shifter_jSplit.at(i)).c_str(),"online (1st) zPV for xPrimVtx", 200, -200., 200.));
	addHistogram(new TH1F(("nPV_tr_Hist_"+m_Shifter_jSplit.at(i)).c_str(),"Number of online PV per event for EFHistoPrmVtx", 101, -1., 100.));
	addHistogram(new TH1F(("PVx_tr_Hist_"+m_Shifter_jSplit.at(i)).c_str(),"online (1st) xPV for EFHistoPrmVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVy_tr_Hist_"+m_Shifter_jSplit.at(i)).c_str(),"online (1st) yPV for EFHistoPrmVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVz_tr_Hist_"+m_Shifter_jSplit.at(i)).c_str(),"online (1st) zPV for EFHistoPrmVtx", 200, -200., 200.));
	addHistogram(new TH1F(("diffzPV0offPVon_"+m_Shifter_jSplit.at(i)).c_str(),"z difference of the 1st offline and (1st) online PV", 200, -1., 1.));
	// Tracks
	addHistogram(new TH1F(("nTrack_"+m_Shifter_jSplit.at(i)).c_str(),"Number of tracks", 40, 0., 40.));
	addHistogram(new TH1F(("d0_"+m_Shifter_jSplit.at(i)).c_str(),"d0 of tracks", 200, -2., 2.));
	addHistogram(new TH1F(("z0_"+m_Shifter_jSplit.at(i)).c_str(),"z0 of tracks", 200, -200., 200.));
	addHistogram(new TH1F(("ed0_"+m_Shifter_jSplit.at(i)).c_str(),"err_d0 of tracks", 200, 0., 1.));
	addHistogram(new TH1F(("ez0_"+m_Shifter_jSplit.at(i)).c_str(),"err_z0 of tracks", 200, 0., 5.));
	addHistogram(new TH1F(("diffz0PV0_"+m_Shifter_jSplit.at(i)).c_str(),"z0 of tracks wrt 1st offline PV", 200, -10., 10.));
	addHistogram(new TH1F(("sigz0PV_"+m_Shifter_jSplit.at(i)).c_str(),"z0-PV/errz0", 200, -20., 20.));
	addHistogram(new TH1F(("trkPt_"+m_Shifter_jSplit.at(i)).c_str(),"Pt of tracks", 100, 0., 50.));
	addHistogram(new TH2F(("trkEtaPhi_"+m_Shifter_jSplit.at(i)).c_str(),"Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
	//   Jets
	addHistogram(new TH1F(("nJet_"+m_Shifter_jSplit.at(i)).c_str(),"Number of jets", 40, 0., 40.));
	addHistogram(new TH1F(("jetPt_"+m_Shifter_jSplit.at(i)).c_str(),"Pt of jets", 100, 0., 750.));
	addHistogram(new TH2F(("jetEtaPhi_"+m_Shifter_jSplit.at(i)).c_str(),"Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
	//   B-jets
	addHistogram(new TH1F(("IP3D_pu_tr_"+m_Shifter_jSplit.at(i)).c_str(),"IP3D_pu probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pb_tr_"+m_Shifter_jSplit.at(i)).c_str(),"IP3D_pb probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pc_tr_"+m_Shifter_jSplit.at(i)).c_str(),"IP3D_pc probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("wIP3D_Rbu_tr_"+m_Shifter_jSplit.at(i)).c_str(),"LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wSV1_Rbu_tr_"+m_Shifter_jSplit.at(i)).c_str(),"LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wCOMB_Rbu_tr_"+m_Shifter_jSplit.at(i)).c_str(),"LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wMV2c00_tr_"+m_Shifter_jSplit.at(i)).c_str(),"MV2c00 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c10_tr_"+m_Shifter_jSplit.at(i)).c_str(),"MV2c10 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c20_tr_"+m_Shifter_jSplit.at(i)).c_str(),"MV2c20 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("xMVtx_tr_"+m_Shifter_jSplit.at(i)).c_str(),"SV1 mass - all SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_tr_"+m_Shifter_jSplit.at(i)).c_str(),"SV1 E-fraction - all SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_tr_"+m_Shifter_jSplit.at(i)).c_str(),"Number of 2-track SV1 - all SV1", 40, 0., 40.));
	addHistogram(new TH1F(("xMVtx_trv_"+m_Shifter_jSplit.at(i)).c_str(),"SV1 mass - valid SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_trv_"+m_Shifter_jSplit.at(i)).c_str(),"SV1 E-fraction - valid SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_trv_"+m_Shifter_jSplit.at(i)).c_str(),"Number of 2-track SV1 - valid SV1", 40, 0., 40.));
      }

      //    On-line UnSplitChains
      //      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/Online_UnSplitChains", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      //      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/Online_UnSplitChains, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
      for (unsigned int i = 0; i< m_Shifter_jUnSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Shifter_jUnSplit: " << m_Shifter_jUnSplit.at(i) );
	addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/"+m_Shifter_jUnSplit.at(i), run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
	ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/" << m_Shifter_jUnSplit.at(i) << " run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
	// PV
	addHistogram(new TH1F(("nPV_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"Number of online PV per event", 101, -1., 100.));
	addHistogram(new TH1F(("PVx_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"online (1st) xPV", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVy_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"online (1st) yPV", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVz_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"online (1st) zPV", 200, -200., 200.));
	addHistogram(new TH1F(("diffzPV0offPVon_"+m_Shifter_jUnSplit.at(i)).c_str(),"z difference of the 1st offline and (1st) online PV", 200, -1., 1.));
	// Tracks
	addHistogram(new TH1F(("nTrack_"+m_Shifter_jUnSplit.at(i)).c_str(),"Number of tracks", 40, 0., 40.));
	addHistogram(new TH1F(("d0_"+m_Shifter_jUnSplit.at(i)).c_str(),"d0 of tracks", 200, -2., 2.));
	addHistogram(new TH1F(("z0_"+m_Shifter_jUnSplit.at(i)).c_str(),"z0 of tracks", 200, -200., 200.));
	addHistogram(new TH1F(("ed0_"+m_Shifter_jUnSplit.at(i)).c_str(),"err_d0 of tracks", 200, 0., 1.));
	addHistogram(new TH1F(("ez0_"+m_Shifter_jUnSplit.at(i)).c_str(),"err_z0 of tracks", 200, 0., 5.));
	addHistogram(new TH1F(("diffz0PV0_"+m_Shifter_jUnSplit.at(i)).c_str(),"z0 of tracks wrt 1st offline PV", 200, -10., 10.));
	addHistogram(new TH1F(("sigz0PV_"+m_Shifter_jUnSplit.at(i)).c_str(),"z0-PV/errz0", 200, -20., 20.));
	addHistogram(new TH1F(("trkPt_"+m_Shifter_jUnSplit.at(i)).c_str(),"Pt of tracks", 100, 0., 50.));
	addHistogram(new TH2F(("trkEtaPhi_"+m_Shifter_jUnSplit.at(i)).c_str(),"Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
	//   Jets
	addHistogram(new TH1F(("nJet_"+m_Shifter_jUnSplit.at(i)).c_str(),"Number of jets", 40, 0., 40.));
	addHistogram(new TH1F(("jetPt_"+m_Shifter_jUnSplit.at(i)).c_str(),"Pt of jets", 100, 0., 750.));
	addHistogram(new TH2F(("jetEtaPhi_"+m_Shifter_jUnSplit.at(i)).c_str(),"Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
	//   B-jets
	addHistogram(new TH1F(("IP3D_pu_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"IP3D_pu probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pb_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"IP3D_pb probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pc_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"IP3D_pc probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("wIP3D_Rbu_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wSV1_Rbu_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wCOMB_Rbu_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wMV2c00_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"MV2c00 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c10_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"MV2c10 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c20_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"MV2c20 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("xMVtx_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"SV1 mass - all SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"SV1 E-fraction - all SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_tr_"+m_Shifter_jUnSplit.at(i)).c_str(),"Number of 2-track SV1 - all SV1", 40, 0., 40.));
	addHistogram(new TH1F(("xMVtx_trv_"+m_Shifter_jUnSplit.at(i)).c_str(),"SV1 mass - valid SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_trv_"+m_Shifter_jUnSplit.at(i)).c_str(),"SV1 E-fraction - valid SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_trv_"+m_Shifter_jUnSplit.at(i)).c_str(),"Number of 2-track SV1 - valid SV1", 40, 0., 40.));
      }
      //    On-line MuChains
      for (unsigned int i = 0; i< m_Shifter_mujet.size(); i++) {
	ATH_MSG_DEBUG(" m_Shifter_mujet: " << m_Shifter_mujet.at(i) );
	addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/"+m_Shifter_mujet.at(i), run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
	ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/" << m_Shifter_mujet.at(i) << " run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
	//   Jets
	addHistogram(new TH1F(("nJet_"+m_Shifter_mujet.at(i)).c_str(),"Number of jets", 40, 0., 40.));
	addHistogram(new TH1F(("jetPt_"+m_Shifter_mujet.at(i)).c_str(),"Pt of jets", 100, 0., 750.));
	addHistogram(new TH2F(("jetEtaPhi_"+m_Shifter_mujet.at(i)).c_str(),"Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
	// Muons
	addHistogram(new TH1F(("nMuon_"+m_Shifter_mujet.at(i)).c_str(),"Number of muons", 20, 0., 20.));
	addHistogram(new TH1F(("muonPt_"+m_Shifter_mujet.at(i)).c_str(),"Pt of muons", 100, 0., 250.));
	addHistogram(new TH2F(("muonEtaPhi_"+m_Shifter_mujet.at(i)).c_str(),"Phi vs Eta of muons", 20, -5., 5., 20, -3.1416, 3.1416));
	addHistogram(new TH1F(("DeltaRAll_"+m_Shifter_mujet.at(i)).c_str(),"DeltaR between muon and any jet", 100, 0., 6.));
	addHistogram(new TH1F(("DeltaZAll_"+m_Shifter_mujet.at(i)).c_str(),"DeltaZ between muon and any jet", 100, 0., 10.));
      }

      // Expert Folders

      addMonGroup(new MonGroup(this,"HLT/BjetMon/Expert", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Expert, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );


      //    On-line SplitChains
      for (unsigned int i = 0; i< m_Expert_jSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Expert_jSplit: " << m_Expert_jSplit.at(i) );
	addMonGroup(new MonGroup(this,"HLT/BjetMon/Expert/"+m_Expert_jSplit.at(i), run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
	ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Expert/" << m_Expert_jSplit.at(i) << " run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
	// PV
	addHistogram(new TH1F(("nPV_tr_"+m_Expert_jSplit.at(i)).c_str(),"Number of online PV per event for xPrimVtx", 101, -1., 100.));
	addHistogram(new TH1F(("PVx_tr_"+m_Expert_jSplit.at(i)).c_str(),"online (1st) xPV for xPrimVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVy_tr_"+m_Expert_jSplit.at(i)).c_str(),"online (1st) yPV for xPrimVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVz_tr_"+m_Expert_jSplit.at(i)).c_str(),"online (1st) zPV for xPrimVtx", 200, -200., 200.));
	addHistogram(new TH1F(("nPV_tr_Hist_"+m_Expert_jSplit.at(i)).c_str(),"Number of online PV per event for EFHistoPrmVtx", 101, -1., 100.));
	addHistogram(new TH1F(("PVx_tr_Hist_"+m_Expert_jSplit.at(i)).c_str(),"online (1st) xPV for EFHistoPrmVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVy_tr_Hist_"+m_Expert_jSplit.at(i)).c_str(),"online (1st) yPV for EFHistoPrmVtx", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVz_tr_Hist_"+m_Expert_jSplit.at(i)).c_str(),"online (1st) zPV for EFHistoPrmVtx", 200, -200., 200.));
	addHistogram(new TH1F(("diffzPV0offPVon_"+m_Expert_jSplit.at(i)).c_str(),"z difference of the 1st offline and (1st) online PV", 200, -1., 1.));
	// Tracks
	addHistogram(new TH1F(("nTrack_"+m_Expert_jSplit.at(i)).c_str(),"Number of tracks", 40, 0., 40.));
	addHistogram(new TH1F(("d0_"+m_Expert_jSplit.at(i)).c_str(),"d0 of tracks", 200, -2., 2.));
	addHistogram(new TH1F(("z0_"+m_Expert_jSplit.at(i)).c_str(),"z0 of tracks", 200, -200., 200.));
	addHistogram(new TH1F(("ed0_"+m_Expert_jSplit.at(i)).c_str(),"err_d0 of tracks", 200, 0., 1.));
	addHistogram(new TH1F(("ez0_"+m_Expert_jSplit.at(i)).c_str(),"err_z0 of tracks", 200, 0., 5.));
	addHistogram(new TH1F(("diffz0PV0_"+m_Expert_jSplit.at(i)).c_str(),"z0 of tracks wrt 1st offline PV", 200, -10., 10.));
	addHistogram(new TH1F(("sigz0PV_"+m_Expert_jSplit.at(i)).c_str(),"z0-PV/errz0", 200, -20., 20.));
	addHistogram(new TH1F(("trkPt_"+m_Expert_jSplit.at(i)).c_str(),"Pt of tracks", 100, 0., 50.));
	addHistogram(new TH2F(("trkEtaPhi_"+m_Expert_jSplit.at(i)).c_str(),"Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
	//   Jets
	addHistogram(new TH1F(("nJet_"+m_Expert_jSplit.at(i)).c_str(),"Number of jets", 40, 0., 40.));
	addHistogram(new TH1F(("jetPt_"+m_Expert_jSplit.at(i)).c_str(),"Pt of jets", 100, 0., 750.));
	addHistogram(new TH2F(("jetEtaPhi_"+m_Expert_jSplit.at(i)).c_str(),"Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
	//   B-jets
	addHistogram(new TH1F(("IP3D_pu_tr_"+m_Expert_jSplit.at(i)).c_str(),"IP3D_pu probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pb_tr_"+m_Expert_jSplit.at(i)).c_str(),"IP3D_pb probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pc_tr_"+m_Expert_jSplit.at(i)).c_str(),"IP3D_pc probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("wIP3D_Rbu_tr_"+m_Expert_jSplit.at(i)).c_str(),"LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wSV1_Rbu_tr_"+m_Expert_jSplit.at(i)).c_str(),"LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wCOMB_Rbu_tr_"+m_Expert_jSplit.at(i)).c_str(),"LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wMV2c00_tr_"+m_Expert_jSplit.at(i)).c_str(),"MV2c00 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c10_tr_"+m_Expert_jSplit.at(i)).c_str(),"MV2c10 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c20_tr_"+m_Expert_jSplit.at(i)).c_str(),"MV2c20 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("xMVtx_tr_"+m_Expert_jSplit.at(i)).c_str(),"SV1 mass - all SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_tr_"+m_Expert_jSplit.at(i)).c_str(),"SV1 E-fraction - all SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_tr_"+m_Expert_jSplit.at(i)).c_str(),"Number of 2-track SV1 - all SV1", 40, 0., 40.));
	addHistogram(new TH1F(("xMVtx_trv_"+m_Expert_jSplit.at(i)).c_str(),"SV1 mass - valid SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_trv_"+m_Expert_jSplit.at(i)).c_str(),"SV1 E-fraction - valid SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_trv_"+m_Expert_jSplit.at(i)).c_str(),"Number of 2-track SV1 - valid SV1", 40, 0., 40.));
      }

      //    On-line UnSplitChains
      for (unsigned int i = 0; i< m_Expert_jUnSplit.size(); i++) {
	ATH_MSG_DEBUG(" m_Expert_jUnSplit: " << m_Expert_jUnSplit.at(i) );
	addMonGroup(new MonGroup(this,"HLT/BjetMon/Expert/"+m_Expert_jUnSplit.at(i), run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
	ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Expert/" << m_Expert_jUnSplit.at(i) << " run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
	// PV
	addHistogram(new TH1F(("nPV_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"Number of online PV per event", 101, -1., 100.));
	addHistogram(new TH1F(("PVx_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"online (1st) xPV", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVy_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"online (1st) yPV", 200, -1.0, 1.0));
	addHistogram(new TH1F(("PVz_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"online (1st) zPV", 200, -200., 200.));
	addHistogram(new TH1F(("diffzPV0offPVon_"+m_Expert_jUnSplit.at(i)).c_str(),"z difference of the 1st offline and (1st) online PV", 200, -1., 1.));
	// Tracks
	addHistogram(new TH1F(("nTrack_"+m_Expert_jUnSplit.at(i)).c_str(),"Number of tracks", 40, 0., 40.));
	addHistogram(new TH1F(("d0_"+m_Expert_jUnSplit.at(i)).c_str(),"d0 of tracks", 200, -2., 2.));
	addHistogram(new TH1F(("z0_"+m_Expert_jUnSplit.at(i)).c_str(),"z0 of tracks", 200, -200., 200.));
	addHistogram(new TH1F(("ed0_"+m_Expert_jUnSplit.at(i)).c_str(),"err_d0 of tracks", 200, 0., 1.));
	addHistogram(new TH1F(("ez0_"+m_Expert_jUnSplit.at(i)).c_str(),"err_z0 of tracks", 200, 0., 5.));
	addHistogram(new TH1F(("diffz0PV0_"+m_Expert_jUnSplit.at(i)).c_str(),"z0 of tracks wrt 1st offline PV", 200, -10., 10.));
	addHistogram(new TH1F(("sigz0PV_"+m_Expert_jUnSplit.at(i)).c_str(),"z0-PV/errz0", 200, -20., 20.));
	addHistogram(new TH1F(("trkPt_"+m_Expert_jUnSplit.at(i)).c_str(),"Pt of tracks", 100, 0., 50.));
	addHistogram(new TH2F(("trkEtaPhi_"+m_Expert_jUnSplit.at(i)).c_str(),"Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
	//   Jets
	addHistogram(new TH1F(("nJet_"+m_Expert_jUnSplit.at(i)).c_str(),"Number of jets", 40, 0., 40.));
	addHistogram(new TH1F(("jetPt_"+m_Expert_jUnSplit.at(i)).c_str(),"Pt of jets", 100, 0., 750.));
	addHistogram(new TH2F(("jetEtaPhi_"+m_Expert_jUnSplit.at(i)).c_str(),"Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
	//   B-jets
	addHistogram(new TH1F(("IP3D_pu_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"IP3D_pu probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pb_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"IP3D_pb probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("IP3D_pc_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"IP3D_pc probability distribution", 200, 0., 1.));
	addHistogram(new TH1F(("wIP3D_Rbu_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wSV1_Rbu_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wCOMB_Rbu_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
	addHistogram(new TH1F(("wMV2c00_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"MV2c00 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c10_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"MV2c10 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("wMV2c20_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"MV2c20 discriminant", 200, -1., 1.));
	addHistogram(new TH1F(("xMVtx_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"SV1 mass - all SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"SV1 E-fraction - all SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_tr_"+m_Expert_jUnSplit.at(i)).c_str(),"Number of 2-track SV1 - all SV1", 40, 0., 40.));
	addHistogram(new TH1F(("xMVtx_trv_"+m_Expert_jUnSplit.at(i)).c_str(),"SV1 mass - valid SV1", 50, 0., 10.));
	addHistogram(new TH1F(("xEVtx_trv_"+m_Expert_jUnSplit.at(i)).c_str(),"SV1 E-fraction - valid SV1", 50, 0., 1.));
	addHistogram(new TH1F(("xNVtx_trv_"+m_Expert_jUnSplit.at(i)).c_str(),"Number of 2-track SV1 - valid SV1", 40, 0., 40.));
      }

      //    On-line MuChains
      for (unsigned int i = 0; i< m_Expert_mujet.size(); i++) {
	ATH_MSG_DEBUG(" m_Expert_mujet: " << m_Expert_mujet.at(i) );
	addMonGroup(new MonGroup(this,"HLT/BjetMon/Expert/"+m_Expert_mujet.at(i), run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
	ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Expert/" << m_Expert_mujet.at(i) << " run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
	//   Jets
	addHistogram(new TH1F(("nJet_"+m_Expert_mujet.at(i)).c_str(),"Number of jets", 40, 0., 40.));
	addHistogram(new TH1F(("jetPt_"+m_Expert_mujet.at(i)).c_str(),"Pt of jets", 100, 0., 750.));
	addHistogram(new TH2F(("jetEtaPhi_"+m_Expert_mujet.at(i)).c_str(),"Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
	// Muons
	addHistogram(new TH1F(("nMuon_"+m_Expert_mujet.at(i)).c_str(),"Number of muons", 20, 0., 20.));
	addHistogram(new TH1F(("muonPt_"+m_Expert_mujet.at(i)).c_str(),"Pt of muons", 100, 0., 250.));
	addHistogram(new TH2F(("muonEtaPhi_"+m_Expert_mujet.at(i)).c_str(),"Phi vs Eta of muons", 20, -5., 5., 20, -3.1416, 3.1416));
	addHistogram(new TH1F(("DeltaRAll_"+m_Expert_mujet.at(i)).c_str(),"DeltaR between muon and any jet", 100, 0., 6.));
	addHistogram(new TH1F(("DeltaZAll_"+m_Expert_mujet.at(i)).c_str(),"DeltaZ between muon and any jet", 100, 0., 10.));
      }

    } // if newRunFlag



    return StatusCode::SUCCESS;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  StatusCode HLTBjetMonTool::fill() {

    ATH_MSG_DEBUG("====> Entering HLTBjetMonTool::fill()" );


  ////////////////////////////////////
  //setCurrentMonGroup("HLT/BjetMon/Shifter");
  ////////////////////////////////////


  // Trigger (chain) name definition

  int size_TriggerChainBjet =m_TriggerChainBjet.size();
  int size_TriggerChainMujet =m_TriggerChainMujet.size();
  ATH_MSG_DEBUG( " Size of Bjet Trigger chain " << size_TriggerChainBjet );
  ATH_MSG_DEBUG( " Size of Mujet Trigger chain " << size_TriggerChainMujet );
  if ( (size_TriggerChainBjet <= 0) && (size_TriggerChainMujet <= 0) ) {
    return StatusCode::SUCCESS;
  }
  std::string chainName = "Not fired";
  std::vector<std::string> FiredChainNames;

  for (int i =0; i<size_TriggerChainBjet; i++){
    if (!getTDT()->isPassed(m_TriggerChainBjet.at(i))){
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << m_TriggerChainBjet.at(i) << " not fired." );
    } else {
      chainName = m_TriggerChainBjet.at(i);
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << chainName << " fired." );
      std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
      ATH_MSG_DEBUG( " In HLTBjetMonTool::fill(): Trigger chain " << chainName << " fired. Size of selectChains " << selectChains.size());
      if ( not selectChains.empty() ) FiredChainNames.push_back(chainName); // test if chain is configured
    } // else
  } //i Bjet

  for (int i =0; i<size_TriggerChainMujet; i++){
    if (!getTDT()->isPassed(m_TriggerChainMujet.at(i))){
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << m_TriggerChainMujet.at(i) << " not fired." );
    } else {
      chainName = m_TriggerChainMujet.at(i);
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << chainName << " fired." );
      std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
      ATH_MSG_DEBUG( " In HLTBjetMonTool::fill(): Trigger chain " << chainName << " fired. Size of selectChains " << selectChains.size());
      if ( not selectChains.empty() ) FiredChainNames.push_back(chainName); // test if chain is configured
    } // else
  } // i Mujet

  if ( not  FiredChainNames.empty() ) ATH_MSG_DEBUG( " " << FiredChainNames.size() << " trigger chains fired: " );
  for (unsigned int i = 0; i< FiredChainNames.size(); i++) {
    ATH_MSG_DEBUG( FiredChainNames.at(i) );
  }

  if ( FiredChainNames.empty() ) {

    ATH_MSG_INFO(" ===> No trigger fired neither for TriggerChainBjet of size: " << size_TriggerChainBjet
		 << " nor for TriggerChainMujet of size: " << size_TriggerChainMujet << " RETURN from HLTBjetMonTool::fill() ! " );
    return StatusCode::SUCCESS;
  } else {

    //    return StatusCode::SUCCESS;

    const xAOD::EventInfo* eventInfo = 0;
    CHECK( evtStore()->retrieve( eventInfo, "EventInfo") );
    bool MCflag = true;
    if ( !eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) MCflag = false;
    ATH_MSG_DEBUG( " MCflag: " << MCflag );

    /* This sequence contains a now obsolate jetcontainer, namely "AntiKt4TruthJets" so it has to be taken out
    // Since truthjets may be used later, we leave here the sequence commented out, waiting for another
    //      container name which will replace "AntiKt4TruthJets" for truthjets
    // Get truth jets
    if (MCflag) {
      const xAOD::JetContainer* truthjets = 0;
      ATH_CHECK( evtStore()->retrieve(truthjets,"AntiKt4TruthJets") );
      ATH_MSG_DEBUG("RETRIEVED TRUTH JETS  - size: " << truthjets->size());
    } // MCflag
    */

    ATH_MSG_DEBUG("  ========= Start retrival of Offline histograms  ========== ");

    std::string HistDir = "/Offline";
    std::string HistExt = "";
    // Get offline PV - non FTK
    float offlinepvz(-1.e6);
    bool Eofflinepv(false);
    const xAOD::VertexContainer* offlinepv = 0;
    if ( evtStore()->contains<xAOD::VertexContainer>("PrimaryVertices") ) {
      ATH_CHECK( evtStore()->retrieve(offlinepv, "PrimaryVertices") );
      ATH_MSG_DEBUG("RETRIEVED OFFLINE standard PV  - size: " << offlinepv->size());
      if ( offlinepv->size() ) {
	Eofflinepv = true;
	offlinepvz = offlinepv->front()->z();
	ATH_MSG_DEBUG(" 1st zPV a la Carlo: " << offlinepvz);
	hist("nPV"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(offlinepv->size());
	for (unsigned int j = 0; j<offlinepv->size(); j++){
	  hist("PVx"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepv))[j]->x());
	  hist("PVy"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepv))[j]->y());
	  hist("PVz"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepv))[j]->z());
	} // j
      } // if size
    } // evtStore
    //
    // Get offline PV - FTK
    float offlinepvzFTK(-1.e6);
    bool EofflinepvFTK(false);
    const xAOD::VertexContainer* offlinepvFTK = 0;
    if ( evtStore()->contains<xAOD::VertexContainer>("FTK_VertexContainer") ) {
      ATH_CHECK( evtStore()->retrieve(offlinepvFTK, "FTK_VertexContainer") );
      ATH_MSG_DEBUG("RETRIEVED OFFLINE FTK PV  - size: " << offlinepvFTK->size());
      if ( offlinepvFTK->size() ) {
	EofflinepvFTK = true;
	offlinepvzFTK = offlinepvFTK->front()->z();
	if (EofflinepvFTK) ATH_MSG_DEBUG(" 1st zPV FTK a la Carlo: " << offlinepvzFTK);
	/* for the moment FTK offline vertices are not moniored
	hist("nPV"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(offlinepvFTK->size());
	for (unsigned int j = 0; j<offlinepvFTK->size(); j++){
	  hist("PVxFTK"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepvFTK))[j]->x());
	  hist("PVyFTK"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepvFTK))[j]->y());
	  hist("PVzFTK"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepvFTK))[j]->z());
	} // j
	*/
      } // if size
    } // evtStore
    //

    ATH_MSG_DEBUG(" ======== Offline histograms are retrieved successfully ! ==== Start online histograms to retrieve ====== ");

    // Get online combinations

    bool SplitKey;
    bool FTKchain;

    //  unsigned int ichain = 0;
    // Loop on trigger items
    for (const auto & trigItem:FiredChainNames){        // Shaun Roe 21/9/16
      // Set directory name (Expert or Shifter)
      std::map<std::string,std::string>::iterator p;
      p = m_Chain2Dir.find(trigItem);
      if ( p != m_Chain2Dir.end() ) {
	HistDir = p->second+"/"+trigItem;
      } else {
	ATH_MSG_INFO(" Fired Trigger " << trigItem << " has no folder name - verify HLTBjetMonTool::init() !!!" );
	continue;
      }
      ATH_MSG_DEBUG(" Fired Trigger " << trigItem << " will be placed in folder " << p->second );
      HistExt = "_"+trigItem;
      ATH_MSG_DEBUG(" HistDir " << HistDir << " HistExt " << HistExt );
      // Set container names (2016/03/03) see TrigBtagEmulation.cxx
      // Set keys
      // HLT non split input chaines
      SplitKey = false;
      std::string m_jetKey = "";
      std::string m_priVtxKey = "EFHistoPrmVtx";
      std::string m_trackKey  = "";
      // HLT split input chaines
      std::size_t found = trigItem.find("split");
      if (found!=std::string::npos) {
	SplitKey =true;
	m_jetKey = "SplitJet";
	m_priVtxKey = "xPrimVx";
	m_trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
      }
      // FTK input chains: John Baines and Julie Hart 
      FTKchain = false;
      std::size_t found1 = trigItem.find("FTK");
      if (found1!=std::string::npos) {
	FTKchain = true;
	//	m_priVtxKey = "HLT_PrimVertexFTK";
	m_priVtxKey = "PrimVertexFTK";  // Mark Sutton 17.09.17 
	m_trackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig";
	std::size_t found2 = trigItem.find("FTKRefit");
        if (found2!=std::string::npos) {
          m_trackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig";
        }//found2
	std::size_t found3 = trigItem.find("FTKVtx");
        if (found3!=std::string::npos) {
          m_trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
        }//found3
      }//found1
      // gsc input chains: Ruch Gupta 2017/08/02 
      std::size_t found4 = trigItem.find("gsc");
      if (found4!=std::string::npos) {
	m_jetKey = "GSCJet";
      } // if found4
      ATH_MSG_DEBUG( " Trigger chain name: " << trigItem << " m_jetKey: " << m_jetKey << " m_priVtxKey: " << m_priVtxKey 
		    << " m_trackKey: " << m_trackKey << " SplitKey: " << SplitKey << " FTKchain: " << FTKchain);
      ATH_MSG_DEBUG("PROCESSING TRIGITEM  -  " << trigItem);
      // Set flag MuJet
      bool MuJet = false;
      found = trigItem.find("HLT_mu");
      if (found!=std::string::npos) MuJet = true;
      // Set flags for histograms
      bool HistPV = !MuJet;
      bool HistJet = true;
      bool HistMuJet = MuJet;
      bool HistTrack = !MuJet;
      bool HistBjet = !MuJet;

      float m_zPrmVtx = 0.; // used for muon-jets

      // Temporary work-around to retrieve online PV for FTK chains suggested by John Baines 2017/09/12
      // Dummy vertices not treated here for the moment

      /*
      if (FTKchain) {
        const xAOD::VertexContainer * onlinepvFTK = 0;
        if ( evtStore()->contains<xAOD::VertexContainer>("HLT_xAOD__VertexContainer_PrimVertexFTK") ) {
          ATH_CHECK( evtStore()->retrieve(onlinepvFTK, "HLT_xAOD__VertexContainer_PrimVertexFTK") );
          ATH_MSG_DEBUG("RETRIEVED FTK PV from evtStore -   size: " << onlinepvFTK->size());
          if( not onlinepvFTK->empty()) {
	    int iVsize = onlinepvFTK->size();
	    for (int iV = 0; iV < iVsize; iV++){
	      ATH_MSG_DEBUG("    FTK evtStore Online PV -  iV: " << iV << " z[iV]: " << (*(onlinepvFTK))[iV]->z() << " type: " << (*(onlinepvFTK))[iV]->vertexType() );
	    } // for iV
	    if(HistPV) hist("PVx_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepvFTK))[0]->x());
	    if(HistPV) hist("PVy_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepvFTK))[0]->y());
	    if(HistPV) hist("PVz_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepvFTK))[0]->z());
	    if(HistPV) hist("nPV_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(onlinepvFTK->size());
	    m_zPrmVtx = (*(onlinepvFTK))[0]->z();
	    if (EofflinepvFTK && HistPV) hist("diffzPV0offPVon"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepvFTK))[0]->z()-offlinepvzFTK);
          } // not onlinepvFTK->empty()
	} // if contains
      } // FTK chain
      */  

      Trig::FeatureContainer fc = m_trigDec->features(trigItem);
      const std::vector< Trig::Combination >& bjetCombs = fc.getCombinations();
      ATH_MSG_DEBUG("RETRIEVED " << bjetCombs.size() << " COMBINATIONS FOR "  << trigItem);

      // Loop on combinations
      std::vector< Trig::Combination >::const_iterator bjetComb;
      for( bjetComb = bjetCombs.begin(); bjetComb != bjetCombs.end(); ++bjetComb ) {
	const Trig::Combination& comb = *bjetComb;
	ATH_MSG_DEBUG("------------ NEW COMBINATION ------------");
	m_zPrmVtx = 0.; // used for muon-jets

	//	if (!FTKchain) {

	  // Get online PV
	bool DummyVtx = false;
	const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs = comb.get<xAOD::VertexContainer>(m_priVtxKey);
	ATH_MSG_DEBUG("RETRIEVED PV  -   size: " << onlinepvs.size());
	if ( not onlinepvs.empty() ) {
	  const xAOD::VertexContainer* onlinepv = onlinepvs[0].cptr();
	  ATH_MSG_DEBUG("   for VertexContainer: " << m_priVtxKey << " nVert: " << onlinepv->size());
	  if( not onlinepv->empty()) {
	    if ( (*(onlinepv))[0]->vertexType() == xAOD::VxType::VertexType:: PriVtx ) { // test that PriVtx is not dummy (JA)
	      if(HistPV) hist("PVx_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->x());
	      if(HistPV) hist("PVy_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->y());
	      if(HistPV) hist("PVz_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->z());
	      m_zPrmVtx = (*(onlinepv))[0]->z();
	      if (Eofflinepv && HistPV) hist("diffzPV0offPVon"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->z()-offlinepvz);
	      ATH_MSG_DEBUG("         Online PV -   z[0]: " << (*(onlinepv))[0]->z());
	    } // if PV not dummy
	    else {
	      DummyVtx = true;
	      ATH_MSG_DEBUG("  Dummy Vertex found: DummyVtx = " << DummyVtx << " m_jetKey = " << m_jetKey << " HistExt = " << HistExt << " m_priVtxKey " << m_priVtxKey );
	      ATH_MSG_DEBUG(" Online dummy PV - type: " << (*(onlinepv))[0]->vertexType() << " x[0]: " << (*(onlinepv))[0]->x()
			   << " y[0]: " << (*(onlinepv))[0]->y() <<  " z[0]: " << (*(onlinepv))[0]->z() );
	      int dummyflag = -1;
	      if(HistPV) hist("nPV_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(dummyflag);
	      //	      continue; // if vertex is dummy skip reading out the other quntities for this trigger combination (EN)
	    } // else
	    if (!DummyVtx && HistPV) hist("nPV_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(onlinepv->size());
	  }  // if onlinepv not empty
	} // if onlinepvs not empty
	if ( SplitKey && DummyVtx ) {
	  // for SplitJets and DummyVtx monitor Vtx with Histogram algorithm
	  const std::vector< Trig::Feature<xAOD::VertexContainer> >onlinepvsd = comb.get<xAOD::VertexContainer>("EFHistoPrmVtx");
	  ATH_MSG_DEBUG("RETRIEVED PV with Histo algo for Split chains when Dummy vtx found with xPrimVx algo-   size: " << onlinepvsd.size());
	  if ( not onlinepvsd.empty() ) {
	    const xAOD::VertexContainer* onlinepv = onlinepvsd[0].cptr();
	    ATH_MSG_DEBUG(" for VertexContainer EFHistoPrmVtx  - nVert: " << onlinepv->size());
	    if( not onlinepv->empty()) {
	      if ( (*(onlinepv))[0]->vertexType() == xAOD::VxType::VertexType:: PriVtx ) { // test that PriVtx is not dummy (JA)
		if(HistPV) hist("PVx_tr_Hist"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->x());
		if(HistPV) hist("PVy_tr_Hist"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->y());
		if(HistPV) hist("PVz_tr_Hist"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((*(onlinepv))[0]->z());
		if(HistPV) hist("nPV_tr_Hist"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(onlinepv->size());
		ATH_MSG_DEBUG(" Dummy PV using Histogram algorithm - type: " << (*(onlinepv))[0]->vertexType() << " x[0]: " << (*(onlinepv))[0]->x()
			     << " y[0]: " << (*(onlinepv))[0]->y() <<  " z[0]: " << (*(onlinepv))[0]->z() );
	      } // if VertexType
	    } // if not onlinepv
	  } // if not onlinepvs
	  continue; // if vertex is dummy skip reading out the other quntities for this trigger combination (EN)
	} // if DummyVtx
	  
	  //	} // if (!FTKchain)

	ATH_MSG_DEBUG(" ======== End of retrival of PV histograms  ==============  ");

	// Get online jet
	const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>(m_jetKey);
	ATH_MSG_DEBUG("RETRIEVED JETS   -   size: " << onlinejets.size());
	if( not onlinejets.empty()) {  // SR
	  const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
	  ATH_MSG_DEBUG("                 -   nJet: " << onlinejet->size());
	  int nJet = onlinejet->size();
	  if(HistJet) hist("nJet"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(nJet);
	  for(const auto* jet : *onlinejet) {
	    ATH_MSG_DEBUG("                 -   pt/eta/phi: " << (jet->pt())*1.e-3 << " / " << jet->eta() << " / " << jet->phi());
	    if(HistJet) hist("jetPt"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((jet->pt())*1.e-3);
	    if(HistJet) hist2("jetEtaPhi"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(jet->eta(),jet->phi());
	  } // for online jet
	}//onlinejets.size

	ATH_MSG_DEBUG(" Jets histograms are stored successfully !");

	// Get online muon
	const std::vector< Trig::Feature<xAOD::MuonContainer> > onlinemuons = comb.get<xAOD::MuonContainer>();
	ATH_MSG_DEBUG("RETRIEVED MUONS   -   size: " << onlinemuons.size());
	if( not onlinemuons.empty()) {  // SR
	  const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	  ATH_MSG_DEBUG("                 -   nMuon: " << onlinemuon->size());
	  int nMuon = onlinemuon->size();
	  if(HistMuJet) hist("nMuon"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(nMuon);
	  for(const auto* muon : *onlinemuon) {
	    ATH_MSG_DEBUG("                 -   pt/eta/phi: " << (muon->pt())*1.e-3 << " / " << muon->eta() << " / " << muon->phi());
	    if(HistMuJet) hist("muonPt"+HistExt,"HLT/BjetMon/"+HistDir)->Fill((muon->pt())*1.e-3);
	    if(HistMuJet) hist2("muonEtaPhi"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(muon->eta(),muon->phi());
	  } // for online muon
	}//onlinemuons.size

	// Loop over muons and jets to monitor muon-jets m_deltaZ and m_dR
	float muonEta=0, muonPhi=0, muonZ=0;
	float jetEta=0,  jetPhi=0, jetZ=0;
	float m_deltaEta=0, m_deltaPhi=0, m_deltaZ=0;
	double m_dR = 0.;
	if( not onlinemuons.empty()) { // SR
	  const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	  for(const auto* muon : *onlinemuon) {
	    //	  if(onlinejets.size()) {
	    if( not onlinejets.empty()) { // SR
	      const xAOD::Muon::MuonType muontype = muon->muonType();
	      if( muontype != xAOD::Muon::MuonType::Combined ) continue; // to correct coverity issue - see next commented line
	      //	    if(!(muontype == xAOD::Muon::MuonType::Combined) ) continue;
	      muonEta = muon->eta();
	      muonPhi = muon->phi();
	      muonZ=0;
	      //	    muonZ= (*(muon->combinedTrackParticleLink()))->z0();
	      muonZ = (*(muon->combinedTrackParticleLink()))->z0()  + (*(muon->combinedTrackParticleLink()))->vz(); //LZ 131115
	      const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
	      for(const auto* jet : *onlinejet) {
		if((jet)->p4().Et() < m_etCut) continue;
		jetEta = (jet)->eta();
		jetPhi = (jet)->phi();
		jetZ=m_zPrmVtx;
		m_deltaEta = muonEta - jetEta;
		m_deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
		m_deltaZ   = fabs(muonZ-jetZ);
		m_dR = sqrt(m_deltaEta*m_deltaEta + m_deltaPhi*m_deltaPhi);
		if(HistMuJet) hist("DeltaZAll"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(m_deltaZ);
		if(HistMuJet) hist("DeltaRAll"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(m_dR);
	      } // for online jet
	    }//onlinejets.size
	  } // for online muon
	}//onlinemuons.size

	ATH_MSG_DEBUG(" Muon histograms are stored successfully !");

	// Get online track particles
	const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>(m_trackKey);
	ATH_MSG_DEBUG("RETRIEVED TRACKS -   size: " << onlinetracks.size());
	if ( not onlinetracks.empty() ) { // SR
	  const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
	  ATH_MSG_DEBUG("                 -   nTrack: " << onlinetrack->size());
	  int nTrack = onlinetrack->size();
	  if (HistTrack) hist("nTrack"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(nTrack);
	  for(const auto* trk : *onlinetrack) {
	    ATH_MSG_DEBUG("     pT: " << (trk->pt())*1.e-3 << " Eta: " << trk->eta() << " Phi: " << trk->phi() << " d0: " << trk->d0() );
	    if (HistTrack) hist("d0"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(trk->d0());
	    if (HistTrack) hist("z0"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(trk->z0());
	    if (HistTrack) hist("ed0"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(Amg::error(trk->definingParametersCovMatrix(), 0));
	    if (HistTrack) hist("ez0"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(Amg::error(trk->definingParametersCovMatrix(), 1));
	    if (HistTrack) hist("trkPt"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( (trk->pt())*1.e-3 );
	    if (HistTrack) hist2("trkEtaPhi"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(trk->eta(),trk->phi());
	    float errz0 = Amg::error(trk->definingParametersCovMatrix(), 1);
	    if (FTKchain) {
	      if (EofflinepvFTK) ATH_MSG_DEBUG(" z0 - zPVoffl: " << trk->z0()+trk->vz()-offlinepvzFTK ); // John A
	      if (EofflinepvFTK && HistTrack) hist("diffz0PV0"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(trk->z0()+trk->vz()-offlinepvzFTK); // John Alison
	      if ( errz0 >0. && EofflinepvFTK && HistTrack) hist("sigz0PV"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( (trk->z0()+trk->vz()-offlinepvzFTK)/errz0 ); // John Alison
	    } else { // if FTKchain
	      if (Eofflinepv) ATH_MSG_DEBUG(" z0 - zPVoffl: " << trk->z0()+trk->vz()-offlinepvz ); // John A
	      if (Eofflinepv && HistTrack) hist("diffz0PV0"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(trk->z0()+trk->vz()-offlinepvz); // John Alison
	      if ( errz0 >0. && Eofflinepv && HistTrack) hist("sigz0PV"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( (trk->z0()+trk->vz()-offlinepvz)/errz0 ); // John Alison
	    } // else
	  } // for online track particles
	} // onlinetracks.size

	ATH_MSG_DEBUG(" Track histograms are stored successfully !");

	// Get online bjet from xAOD BTaggingContainer
	const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
	ATH_MSG_DEBUG("RETRIEVED BJETS  from xAOD BTaggingContainer -   size: " << onlinebjets.size());
	if(not onlinebjets.empty()) { // SR
	  const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
	  ATH_MSG_DEBUG("                 -   nBjet: " << onlinebjet->size());
	  for(const auto* bjet : *onlinebjet) {
	    double wIP3D, wSV1, wCOMB, wMV2c00, wMV2c10, wMV2c20  = 0.; // discriminant variables
	    //	  double wMV1  = 0.;
	    float svp_efrc, svp_mass = -1.; int svp_n2t = -1; // SV1 variables
	    bjet->loglikelihoodratio("IP3D", wIP3D);
	    bjet->loglikelihoodratio("SV1", wSV1);
	    double SV1_loglikelihoodratioLZ = bjet->SV1_loglikelihoodratio();
	    wCOMB = wIP3D+wSV1;
	    wMV2c00 = bjet->auxdata<double>("MV2c00_discriminant");
	    wMV2c10 = bjet->auxdata<double>("MV2c10_discriminant");
	    wMV2c20 = bjet->auxdata<double>("MV2c20_discriminant");
	    //	  wMV1 = bjet->MV1_discriminant();
	    // Suggestion of LZ
	    bjet->variable<float>("SV1", "masssvx", svp_mass);
	    bjet->variable<float>("SV1", "efracsvx", svp_efrc);
	    bjet->variable<int>("SV1", "N2Tpair", svp_n2t);
	    ATH_MSG_DEBUG("                 -   Before SV1 check - MVTX / EVTX / NVTX: " << svp_mass << " / " << svp_efrc << " / " << svp_n2t ) ;
	    if (HistBjet) hist("xNVtx_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(svp_n2t);
	    if ( svp_n2t > 0 ) {
	      if (HistBjet) hist("xMVtx_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( svp_mass * 1.e-3 );
	      if (HistBjet) hist("xEVtx_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( svp_efrc );
	    } // if svp_n2t
	    // end of suggestion of LZ
	    ATH_MSG_DEBUG("                 -   IP3Dpu / IP3Dpb / IP3Dpc: " << bjet->IP3D_pu() << " / " << bjet->IP3D_pb() << " / " << bjet->IP3D_pc() );
	    if (HistBjet) hist("IP3D_pu_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(bjet->IP3D_pu());
	    if (HistBjet) hist("IP3D_pb_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(bjet->IP3D_pb());
	    if (HistBjet) hist("IP3D_pc_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(bjet->IP3D_pc());
	    ATH_MSG_DEBUG("                 -   IP3D / SV1 / IP3D+SV1: " << wIP3D << " / " << wSV1 << " / " << wCOMB );
	    ATH_MSG_DEBUG("                 -   SV1 LZ: " << SV1_loglikelihoodratioLZ );
	    ATH_MSG_DEBUG("                 -   MV2c00 / MV2c10 / MV2c20: " << wMV2c00 << " / " << wMV2c10 << " / " << wMV2c20);
	    if (HistBjet) hist("wIP3D_Rbu_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(wIP3D);
	    if (HistBjet) hist("wSV1_Rbu_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(wSV1);
	    if (HistBjet) hist("wCOMB_Rbu_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(wCOMB);
	    if (HistBjet) hist("wMV2c00_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(wMV2c00);
	    if (HistBjet) hist("wMV2c10_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(wMV2c10);
	    if (HistBjet) hist("wMV2c20_tr"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(wMV2c20);

	    // Get SV1 secondary vtx information, see:
	    // /PhysicsAnalysis/JetTagging/JetTagTools/src/MV2Tag.cxx#0486 and
	    // /PhysicsAnalysis/JetTagging/JetTagTools/src/GaiaNNTool.cxx#0349
	    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
	    ATH_MSG_DEBUG("    SV1 info source name before calling VertexContainer: " << m_sv1_infosource ) ;
	    bjet->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices);
	    ATH_MSG_DEBUG("    SV1 info source name after calling VertexContainer: " << m_sv1_infosource ) ;
	    if ( myVertices.size() > 0 && myVertices[0].isValid() ) {
	      ATH_MSG_DEBUG("    SV1 vertex size: " << myVertices.size() << " is it valid? " << myVertices[0].isValid() ) ;
	      bjet->variable<float>(m_sv1_infosource, "masssvx", svp_mass);
	      bjet->variable<float>(m_sv1_infosource, "efracsvx", svp_efrc);
	      bjet->variable<int>(m_sv1_infosource, "N2Tpair", svp_n2t);
	      ATH_MSG_DEBUG("                 -   MVTX / EVTX / NVTX: " << svp_mass << " / " << svp_efrc << " / " << svp_n2t ) ;
	      if (HistBjet) hist("xNVtx_trv"+HistExt,"HLT/BjetMon/"+HistDir)->Fill(svp_n2t);
	      if ( svp_n2t > 0 ) {
		if (HistBjet) hist("xMVtx_trv"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( svp_mass );
		if (HistBjet) hist("xEVtx_trv"+HistExt,"HLT/BjetMon/"+HistDir)->Fill( svp_efrc );
	      } // if svp_n2t
	    } else {
	      ATH_MSG_DEBUG("  No valid SV1 vertex found --  SV1 vertex size: " << myVertices.size() );
	      if ( myVertices.size() > 0 ) ATH_MSG_DEBUG("  No valid SV1 vertex found -- myVertices[0].isValid(): " << myVertices[0].isValid() ) ;
	    } // if vertex valid
	  } // for online bjet
	} // onlinebjets.size

	ATH_MSG_DEBUG(" Bjet histograms are stored successfully !");

      } // for bjetComb
    } // ichain

  } // FiredChainNames.empty()

  ATH_MSG_DEBUG("====> Ended successfully HLTBjetMonTool::fill()" );



  return StatusCode::SUCCESS;

}




