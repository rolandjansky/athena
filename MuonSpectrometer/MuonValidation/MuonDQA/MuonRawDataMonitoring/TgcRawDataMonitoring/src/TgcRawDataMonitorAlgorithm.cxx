/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRawDataMonitorAlgorithm.h"

#include "TObjArray.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "StoreGate/ReadDecorHandle.h"
#include "MuonReadoutGeometry/TgcDetectorElement.h"

namespace {
  // Cut values on pt bein exploited throughout the monitoring
  constexpr double pt_30_cut = 30. * Gaudi::Units::GeV;
  constexpr double pt_15_cut = 15. * Gaudi::Units::GeV;
  constexpr double pt_10_cut = 10. * Gaudi::Units::GeV;
  constexpr double pt_4_cut = 4. * Gaudi::Units::GeV;

  /// End of the barrel region
  constexpr double barrel_end = 1.05;
  constexpr double trigger_end = 2.4;

  // offset for better drawing
  constexpr double tgc_coin_phi_small_offset = 0.0001;
}
TgcRawDataMonitorAlgorithm::TgcRawDataMonitorAlgorithm(const std::string &name, ISvcLocator *pSvcLocator) :
  AthMonitorAlgorithm(name, pSvcLocator) {
}

StatusCode TgcRawDataMonitorAlgorithm::initialize() {
  ATH_MSG_DEBUG("initialize()");
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(m_MuonContainerKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_MuonRoIContainerKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_TgcPrepDataContainerKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_TgcCoinDataContainerCurrBCKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_TgcCoinDataContainerNextBCKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_TgcCoinDataContainerPrevBCKey.initialize(SG::AllowEmpty));

  if( !m_extrapolator.empty() ) ATH_CHECK(m_extrapolator.retrieve());
  else m_extrapolator.disable();
  if( !m_tgcMonTool.empty() ) ATH_CHECK(m_tgcMonTool.retrieve());
  else m_tgcMonTool.disable();
  if( !m_muonSelectionTool.empty() ) ATH_CHECK(m_muonSelectionTool.retrieve());
  else m_muonSelectionTool.disable();

  ATH_CHECK(m_L1MenuKey.initialize()); // ReadHandleKey, but DetStore (so renounce)
  renounce(m_L1MenuKey);
  ATH_CHECK(m_thresholdPatternsKey.initialize(!m_thresholdPatternsKey.empty()));

  m_extZposition.push_back(m_M1_Z.value());
  m_extZposition.push_back(m_M2_Z.value());
  m_extZposition.push_back(m_M3_Z.value());
  m_extZposition.push_back(m_EI_Z.value());
  m_extZposition.push_back(m_FI_Z.value());
  m_extZposition.push_back(-m_M1_Z.value());
  m_extZposition.push_back(-m_M2_Z.value());
  m_extZposition.push_back(-m_M3_Z.value());
  m_extZposition.push_back(-m_EI_Z.value());
  m_extZposition.push_back(-m_FI_Z.value());
  

  if(m_ctpDecMonList.value()!=""){
    m_CtpDecMonObj.clear();
    TString Str = m_ctpDecMonList.value();// format="Tit:L1_MU20_Run3,Mul:1,HLT:HLT_mu26_ivarmedium_L1MU20,RPC:6,TGC:12FCH;"
    std::unique_ptr<TObjArray> monTrigs( Str.Tokenize(";") );
    for(int i = 0 ; i < monTrigs->GetEntries() ; i++){
      TString monTrig = monTrigs->At(i)->GetName();
      if(monTrig=="")continue;
      CtpDecMonObj monObj;
      monObj.trigItem = monObj.title = "dummy";
      monObj.rpcThr=monObj.tgcThr=monObj.multiplicity=0;
      monObj.tgcF=monObj.tgcC=monObj.tgcH=monObj.rpcR=monObj.rpcM=false;
      std::unique_ptr<TObjArray> monElement( monTrig.Tokenize(",") );
      for(int j = 0 ; j < monElement->GetEntries() ; j++){
	std::string sysItem = monElement->At(j)->GetName();
	if(sysItem=="")continue;
	std::string item = sysItem.substr(4,sysItem.size());// remove "Tit:", "CTP:", "HLT:", "RPC:", "TGC:"
	if(sysItem.find("Tit")==0){
	  monObj.title = item;
	}else if(sysItem.find("Mul")==0){
	  monObj.multiplicity = std::atoi(item.data());
	}else if(sysItem.find("CTP")==0 || sysItem.find("HLT")==0){
	  monObj.trigItem = item;
	}else if(sysItem.find("RPC")==0){
	  monObj.rpcThr = std::atoi(item.data());
	  monObj.rpcR = (item.find("R")!=std::string::npos);
	  monObj.rpcM = (item.find("M")!=std::string::npos);
	}else if(sysItem.find("TGC")==0){
	  monObj.tgcThr = std::atoi(item.data());
	  monObj.tgcF = (item.find("F")!=std::string::npos);
	  monObj.tgcC = (item.find("C")!=std::string::npos);
	  monObj.tgcH = (item.find("H")!=std::string::npos);
	}
      }
      m_CtpDecMonObj.push_back(monObj);
    }
  }

  if(m_thrPatternList.value()!=""){
    m_thrMonList.clear();
    TString Str = m_thrPatternList.value();
    std::unique_ptr<TObjArray> arr( Str.Tokenize(",") );
    for(int i = 0 ; i < arr->GetEntries() ; i++){
      std::string name = arr->At(i)->GetName();
      if(name.find("MU")!=0)continue;
      m_thrMonList.insert(name);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TgcRawDataMonitorAlgorithm::fillHistograms(const EventContext &ctx) const {
  ATH_MSG_DEBUG("fillHistograms()");

  // Print out all available muon triggers
  // This is to be used when making a list of triggers
  // to be monitored, and writted in .py config file
  // The defult should be FALSE
  if( m_printAvailableMuonTriggers.value() ){
    ATH_MSG_DEBUG("printAvailableMuonTriggers");
    if( getTrigDecisionTool().empty() ){
      ATH_MSG_ERROR("TDT is not availeble");
      return StatusCode::FAILURE;
    }else{
      std::set<std::string> available_muon_triggers;
      auto chainGroup = getTrigDecisionTool()->getChainGroup(".*");
      if( chainGroup != nullptr ){
	auto triggerList = chainGroup->getListOfTriggers();
	if( !triggerList.empty() ){
	  for(const auto &trig : triggerList) {
	    std::string thisTrig = trig;
	    if( thisTrig.find("mu")==std::string::npos && thisTrig.find("MU")==std::string::npos)continue;
	    if(getTrigDecisionTool()->getNavigationFormat() == "TriggerElement") { // run 2 access
	      auto fc = getTrigDecisionTool()->features(thisTrig.data(),TrigDefs::alsoDeactivateTEs);
	      for(const auto& comb : fc.getCombinations()){
		auto initRoIs = comb.get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
		for(const auto& roi : initRoIs){
		  if( roi.empty() )continue;
		  if( roi.cptr()==nullptr ) continue;
		  // found an available muon trigger here
		  available_muon_triggers.insert(thisTrig);
		}
	      }
	    }else{ // run 3 access
	      auto initialRoIs = getTrigDecisionTool()->features<TrigRoiDescriptorCollection>(thisTrig.data(), TrigDefs::includeFailedDecisions, "", TrigDefs::lastFeatureOfType, "initialRoI");
	      for(const auto& roiLinkInfo : initialRoIs) {
		if( !roiLinkInfo.isValid() )continue;
		auto roiEL = roiLinkInfo.link;
		if( !roiEL.isValid() )continue;
		auto roi = *roiEL;
		if( roi==nullptr ) continue;
		// found an available muon trigger here
		available_muon_triggers.insert(thisTrig);
	      }
	    }
	  }
	}
      }
      for(const auto& trig : available_muon_triggers){
	ATH_MSG_INFO("Available Muon Trigger: " << trig);
      }
      return StatusCode::SUCCESS;
    }
  } ///////////////End of printing out available muon triggers


  ///////////////// Preparation: check trigger information /////////////////////
  ATH_MSG_DEBUG("Preparing trigger information");
  bool nonMuonTriggerFired = false;
  std::set<std::string> list_of_single_muon_triggers;
  if ( !getTrigDecisionTool().empty() ){
    auto chainGroup = getTrigDecisionTool()->getChainGroup("HLT_.*");
    if( chainGroup != nullptr ){
      auto triggerList = chainGroup->getListOfTriggers();
      if( !triggerList.empty() ){
	for(const auto &trig : triggerList) {
	  std::string thisTrig = trig;
	  if( thisTrig.find("HLT_mu")==0 ){ // muon triggers
	    // look for only single-muon triggers
	    if ( thisTrig.find("-")!=std::string::npos ) continue; // skipping L1Topo item
	    if ( thisTrig.find("msonly")!=std::string::npos ) continue; // only combined muon
	    if ( thisTrig.find("2MU")!=std::string::npos ) continue; // no L12MU
	    if ( thisTrig.find("3MU")!=std::string::npos ) continue; // no L13MU
	    if ( thisTrig.find("4MU")!=std::string::npos ) continue; // no L14MU
	    if ( thisTrig.find("mu")!=std::string::npos ) {
	      TString tmp = thisTrig;
	      tmp.ReplaceAll("mu","ZZZ");
	      std::unique_ptr<TObjArray> arr( tmp.Tokenize("ZZZ") );
	      auto n = arr->GetEntries();
	      if ( n != 2 ) continue; // exact one 'mu' letter
	    }
	    if ( thisTrig.find("MU")!=std::string::npos ) {
	      TString tmp = thisTrig;
	      tmp.ReplaceAll("MU","ZZZ");
	      std::unique_ptr<TObjArray> arr( tmp.Tokenize("ZZZ") );
	      auto n = arr->GetEntries();
	      if ( n > 2 ) continue; // only one 'MU' letter if exists
	    }
	    list_of_single_muon_triggers.insert( thisTrig );
	  }else{
	    if( m_useNonMuonTriggers.value() == false ) continue;
	    // look for muon-orthogonal triggers and if they fired
	    if(nonMuonTriggerFired)continue; // already checked, skipping
	    if(thisTrig.find("mu")!=std::string::npos) continue;
	    if(thisTrig.find("MU")!=std::string::npos) continue;
	    if(getTrigDecisionTool()->isPassed(thisTrig.data(),TrigDefs::Physics)) nonMuonTriggerFired = true;
	  }
	}
      }
    }
  }
  ///////////////// End preparation: check trigger information /////////////////////

  // define common monitoring variables //
  auto mon_bcid = Monitored::Scalar<int>("mon_bcid", GetEventInfo(ctx)->bcid());
  auto mon_pileup = Monitored::Scalar<int>("mon_pileup", lbAverageInteractionsPerCrossing(ctx));
  auto mon_lb = Monitored::Scalar<int>("mon_lb", GetEventInfo(ctx)->lumiBlock());
  fill(m_packageName+"_Common", mon_bcid, mon_pileup, mon_lb);





  
  ///////////////// Extract MuonRoI /////////////////
  const xAOD::MuonRoIContainer *rois = nullptr;
  if (!m_MuonRoIContainerKey.empty() && m_anaMuonRoI.value()) {
    ATH_MSG_DEBUG("Getting MuonRoI pointer");
    /* raw LVL1MuonRoIs distributions */
    SG::ReadHandle<xAOD::MuonRoIContainer > handle( m_MuonRoIContainerKey, ctx);
    if(handle.isValid()) {
      rois = handle.cptr();
    }
  }
  ///////////////// Filling MuonRoI-only histograms  /////////////////
  if(rois != nullptr){
    ATH_MSG_DEBUG("Filling MuonRoI-only histograms");
    MonVariables  roi_variables;
    auto roi_bcid = Monitored::Scalar<int>("roi_bcid", GetEventInfo(ctx)->bcid());
    roi_variables.push_back(roi_bcid);
    auto roi_pileup = Monitored::Scalar<int>("roi_pileup", lbAverageInteractionsPerCrossing(ctx));
    roi_variables.push_back(roi_pileup);
    auto roi_lumiBlock = Monitored::Scalar<int>("roi_lumiBlock", GetEventInfo(ctx)->lumiBlock());
    roi_variables.push_back(roi_lumiBlock);
    auto roi_eta = Monitored::Collection("roi_eta", *rois, [](const xAOD::MuonRoI *m) {
	return m->eta();
      });
    roi_variables.push_back(roi_eta);
    auto roi_eta_rpc = Monitored::Collection("roi_eta_rpc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_rpc);
    auto roi_eta_tgc = Monitored::Collection("roi_eta_tgc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_tgc);
    auto roi_eta_wInnerCoin = Monitored::Collection("roi_eta_wInnerCoin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wInnerCoin);
    auto roi_eta_wBW3Coin = Monitored::Collection("roi_eta_wBW3Coin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wBW3Coin);
    auto roi_eta_wInnerCoinVeto = Monitored::Collection("roi_eta_wInnerCoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wInnerCoinVeto);
    auto roi_eta_wBW3CoinVeto = Monitored::Collection("roi_eta_wBW3CoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wBW3CoinVeto);
    auto roi_phi = Monitored::Collection("roi_phi", *rois, [](const xAOD::MuonRoI *m) {
	return m->phi();
      });
    roi_variables.push_back(roi_phi);
    auto roi_phi_rpc = Monitored::Collection("roi_phi_rpc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Barrel) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_rpc);
    auto roi_phi_tgc = Monitored::Collection("roi_phi_tgc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() != xAOD::MuonRoI::Barrel) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_tgc);
    auto roi_phi_wInnerCoin = Monitored::Collection("roi_phi_wInnerCoin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wInnerCoin);
    auto roi_phi_wBW3Coin = Monitored::Collection("roi_phi_wBW3Coin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wBW3Coin);
    auto roi_phi_wInnerCoinVeto = Monitored::Collection("roi_phi_wInnerCoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wInnerCoinVeto);
    auto roi_phi_wBW3CoinVeto = Monitored::Collection("roi_phi_wBW3CoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wBW3CoinVeto);
    auto roi_thr = Monitored::Collection("roi_thr", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber();
      });
    roi_variables.push_back(roi_thr);
    auto roi_rpc = Monitored::Collection("roi_rpc", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_rpc);
    auto roi_tgc = Monitored::Collection("roi_tgc", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() != xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_tgc);
    auto roi_barrel = Monitored::Collection("roi_barrel", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_barrel);
    auto roi_endcap = Monitored::Collection("roi_endcap", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Endcap;
      });
    roi_variables.push_back(roi_endcap);
    auto roi_forward = Monitored::Collection("roi_forward", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Forward;
      });
    roi_variables.push_back(roi_forward);
    auto roi_phi_barrel = Monitored::Collection("roi_phi_barrel", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Barrel) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_barrel);
    auto roi_phi_endcap = Monitored::Collection("roi_phi_endcap", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Endcap) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_endcap);
    auto roi_phi_forward = Monitored::Collection("roi_phi_forward", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Forward) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_forward);
    auto roi_sideA = Monitored::Collection("roi_sideA", *rois, [](const xAOD::MuonRoI *m) {
	return m->getHemisphere() == xAOD::MuonRoI::Positive;
      });
    roi_variables.push_back(roi_sideA);
    auto roi_sideC = Monitored::Collection("roi_sideC", *rois, [](const xAOD::MuonRoI *m) {
	return m->getHemisphere() == xAOD::MuonRoI::Negative;
      });
    roi_variables.push_back(roi_sideC);
    auto thrmask1 = Monitored::Collection("thrmask1", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 1;
      });
    roi_variables.push_back(thrmask1);
    auto thrmask2 = Monitored::Collection("thrmask2", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 2;
      });
    roi_variables.push_back(thrmask2);
    auto thrmask3 = Monitored::Collection("thrmask3", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 3;
      });
    roi_variables.push_back(thrmask3);
    auto thrmask4 = Monitored::Collection("thrmask4", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 4;
      });
    roi_variables.push_back(thrmask4);
    auto thrmask5 = Monitored::Collection("thrmask5", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 5;
      });
    roi_variables.push_back(thrmask5);
    auto thrmask6 = Monitored::Collection("thrmask6", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 6;
      });
    roi_variables.push_back(thrmask6);
    auto thrmask7 = Monitored::Collection("thrmask7", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 7;
      });
    roi_variables.push_back(thrmask7);
    auto thrmask8 = Monitored::Collection("thrmask8", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 8;
      });
    roi_variables.push_back(thrmask8);
    auto thrmask9 = Monitored::Collection("thrmask9", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 9;
      });
    roi_variables.push_back(thrmask9);
    auto thrmask10 = Monitored::Collection("thrmask10", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 10;
      });
    roi_variables.push_back(thrmask10);
    auto thrmask11 = Monitored::Collection("thrmask11", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 11;
      });
    roi_variables.push_back(thrmask11);
    auto thrmask12 = Monitored::Collection("thrmask12", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 12;
      });
    roi_variables.push_back(thrmask12);
    auto thrmask13 = Monitored::Collection("thrmask13", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 13;
      });
    roi_variables.push_back(thrmask13);
    auto thrmask14 = Monitored::Collection("thrmask14", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 14;
      });
    roi_variables.push_back(thrmask14);
    auto thrmask15 = Monitored::Collection("thrmask15", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 15;
      });
    roi_variables.push_back(thrmask15);
    auto roi_charge = Monitored::Collection("roi_charge", *rois, [](const xAOD::MuonRoI* m) {
	return (m->getCharge()==xAOD::MuonRoI::Neg)?(-1):((m->getCharge()==xAOD::MuonRoI::Pos)?(+1):(0));
      });
    roi_variables.push_back(roi_charge);
    auto roi_bw3coin = Monitored::Collection("roi_bw3coin",*rois,[](const xAOD::MuonRoI* m) {
	return m->getBW3Coincidence();
      });
    roi_variables.push_back(roi_bw3coin);
    auto roi_bw3coinveto = Monitored::Collection("roi_bw3coinveto",*rois,[](const xAOD::MuonRoI* m) {
	return !m->getBW3Coincidence() && m->getSource()!=xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_bw3coinveto);
    auto roi_innercoin = Monitored::Collection("roi_innercoin",*rois,[](const xAOD::MuonRoI* m) {
	return m->getInnerCoincidence();
      });
    roi_variables.push_back(roi_innercoin);
    auto roi_innveto = Monitored::Collection("roi_innveto",*rois,[](const xAOD::MuonRoI* m) {
	return !m->getInnerCoincidence() && m->getSource()!=xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_innveto);
    auto roi_goodmf = Monitored::Collection("roi_goodmf",*rois,[](const xAOD::MuonRoI* m) {
	return m->getGoodMF();
      });
    roi_variables.push_back(roi_goodmf);
    auto roi_badmf = Monitored::Collection("roi_badmf",*rois,[](const xAOD::MuonRoI* m){
	return !m->getGoodMF() && m->getSource()!=xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_badmf);
    auto roi_ismorecand = Monitored::Collection("roi_ismorecand",*rois,[](const xAOD::MuonRoI* m){
	return m->isMoreCandInRoI();
      });
    roi_variables.push_back(roi_ismorecand);
    auto roi_posCharge = Monitored::Collection("roi_posCharge",*rois,[](const xAOD::MuonRoI* m){
	return m->getCharge()==xAOD::MuonRoI::Pos;
      });
    roi_variables.push_back(roi_posCharge);
    auto roi_negCharge = Monitored::Collection("roi_negCharge",*rois,[](const xAOD::MuonRoI* m){
	return m->getCharge()==xAOD::MuonRoI::Neg;
      });
    roi_variables.push_back(roi_negCharge);
    fill(m_packageName, roi_variables);
    ATH_MSG_DEBUG("End filling MuonRoI-only histograms");
  }
  ///////////////// End filling MuonRoI-only histograms  /////////////////


  ///////////////// Filling histograms for MuonRoIs after trigger decision /////////////////
  if ( !getTrigDecisionTool().empty() && rois != nullptr && m_monitorTriggerMultiplicity.value() ) {
    ATH_MSG_DEBUG("Filling histograms for MuonRoIs after trigger decision");
    for(const auto& monObj : m_CtpDecMonObj){
      std::set<unsigned int> allCands;
      std::set<unsigned int> ctpMuonCands;
      std::set<unsigned int> inputMuonCands;
      // collecting roiWords out of the CTP decision
      bool isRun2Legacy = false;
      if(getTrigDecisionTool()->getNavigationFormat() == "TriggerElement") { // run 2 access
	isRun2Legacy = true;
	if(monObj.title.find("Run2Legacy")==std::string::npos) continue;
	auto fc = getTrigDecisionTool()->features(monObj.trigItem.data(),TrigDefs::alsoDeactivateTEs);
	for(const auto& comb : fc.getCombinations()){
	  auto initRoIs = comb.get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	  for(const auto& roi : initRoIs){
	    if( roi.empty() )continue;
	    if( roi.cptr()==nullptr ) continue;
	    ctpMuonCands.insert(roi.cptr()->roiWord());
	    allCands.insert(roi.cptr()->roiWord());
	  }
	}
      }else{ // run 3 access
	auto initialRoIs = getTrigDecisionTool()->features<TrigRoiDescriptorCollection>(monObj.trigItem.data(), TrigDefs::includeFailedDecisions, "", TrigDefs::lastFeatureOfType, "initialRoI");
	for(const auto& roiLinkInfo : initialRoIs) {
	  if( !roiLinkInfo.isValid() )continue;
	  auto roiEL = roiLinkInfo.link;
	  if( !roiEL.isValid() )continue;
	  auto roi = *roiEL;
	  if( roi==nullptr ) continue;
	  ctpMuonCands.insert(roi->roiWord());
	  allCands.insert(roi->roiWord());
	}
      }
      // collecting roiWords out of RPC/TGC
      bool isRun3 = false;
      for(const auto roi : *rois){
	isRun3 = roi->isRun3();
	if(roi->getSource()==xAOD::MuonRoI::Barrel){
	  if(roi->getThrNumber()<monObj.rpcThr)continue;
	  if(monObj.rpcM && !roi->isMoreCandInRoI())continue;
	}else{
	  if(roi->getThrNumber()<monObj.tgcThr)continue;
	  if(monObj.tgcF && !roi->getBW3Coincidence())continue;
	  if(monObj.tgcC && !roi->getInnerCoincidence())continue;
	  if(monObj.tgcH && !roi->getGoodMF())continue;
	}
	inputMuonCands.insert(roi->roiWord());
	allCands.insert(roi->roiWord());
      }
      if(!isRun3 && isRun2Legacy && monObj.title.find("Run2Legacy")==std::string::npos)continue; // Run2Legacy
      if(!isRun3 && !isRun2Legacy && (monObj.title.find("Run2Legacy")!=std::string::npos||monObj.title.find("Run3")!=std::string::npos))continue; // Run2
      if(isRun3 && monObj.title.find("Run3")==std::string::npos)continue; // Run3

      if(ctpMuonCands.size()==0 && inputMuonCands.size()<monObj.multiplicity)continue;

      std::vector<int> roiMatching_CTPin;
      std::vector<int> roiMatching_CTPout;

      std::vector<double> roi_Eta;
      std::vector<double> roi_Phi;
      std::vector<double> roi_dRmin;
      std::vector<double> roi_pTdiff;
      std::vector<int> roi_ThrNum;
      std::vector<int> roi_Charge;
      std::vector<int> roi_BW3Coin;
      std::vector<int> roi_InnerCoin;
      std::vector<int> roi_GoodMF;
      std::vector<int> roi_IsMoreCandInRoI;
      std::vector<int> roi_PhiOverlap;
      std::vector<int> roi_EtaOverlap;
      std::vector<int> roi_isVetoed;
      std::vector<bool> roi_inOk_outOk;
      std::vector<bool> roi_inOk_outNg;
      std::vector<bool> roi_inNg_outOk;

      for(const auto roi : *rois){ // scan all MuonRoIs
	bool ctp_in  = inputMuonCands.find(roi->roiWord())!=inputMuonCands.end();
	bool ctp_out = ctpMuonCands.find(roi->roiWord())!=ctpMuonCands.end();
	if(!ctp_in && !ctp_out)continue;
	roiMatching_CTPin.push_back(ctp_in?1:0);
	roiMatching_CTPout.push_back(ctp_out?1:0);
	double dRmin = 1000;
	double pTdiff = -15;
	for(const auto roi2 : *rois){ // scan all the other MuonRoIs to check the isolation
	  if(roi == roi2)continue;
	  double dphi = xAOD::P4Helpers::deltaPhi(roi->phi(),roi2->phi());
	  double deta = roi->eta() - roi2->eta();
	  double dr = std::sqrt(dphi*dphi + deta*deta);
	  if(dr < dRmin){
	    dRmin = dr;
	    pTdiff = roi2->getThrNumber() - roi->getThrNumber();
	  }
	}
	// adjust the value so that the value can be in the histgram range
	if(dRmin>999) dRmin = -0.05;
	else if(dRmin>1.0) dRmin = 0.95;
	roi_Eta.push_back(roi->eta());
	roi_Phi.push_back(roi->phi());
	roi_dRmin.push_back(dRmin);
	roi_pTdiff.push_back(pTdiff);
	roi_ThrNum.push_back((roi->getSource()==xAOD::MuonRoI::Barrel)?(roi->getThrNumber()):(-roi->getThrNumber()));
	roi_Charge.push_back((roi->getCharge()==xAOD::MuonRoI::Neg)?(-1):((roi->getCharge()==xAOD::MuonRoI::Pos)?(+1):(0)));
	roi_BW3Coin.push_back((roi->getSource()!=xAOD::MuonRoI::Barrel)?roi->getBW3Coincidence():-1);
	roi_InnerCoin.push_back((roi->getSource()!=xAOD::MuonRoI::Barrel)?roi->getInnerCoincidence():-1);
	roi_GoodMF.push_back((roi->getSource()!=xAOD::MuonRoI::Barrel)?roi->getGoodMF():-1);
	roi_IsMoreCandInRoI.push_back((roi->getSource()==xAOD::MuonRoI::Barrel)?roi->isMoreCandInRoI():-1);
	roi_PhiOverlap.push_back((roi->getSource()==xAOD::MuonRoI::Barrel)?roi->getPhiOverlap():-1);
	roi_EtaOverlap.push_back(roi->getEtaOverlap());
	roi_isVetoed.push_back(roi->isVetoed());
	roi_inOk_outOk.push_back( ctp_in && ctp_out );
	roi_inOk_outNg.push_back( ctp_in && !ctp_out );
	roi_inNg_outOk.push_back( !ctp_in && ctp_out );
      }

      MonVariables  ctpMonVariables;
      auto val_roiMatching_CTPin = Monitored::Collection("roiMatching_CTPin", roiMatching_CTPin);
      auto val_roiMatching_CTPout = Monitored::Collection("roiMatching_CTPout", roiMatching_CTPout);

      auto val_ctpMultiplicity = Monitored::Scalar<int>("ctpMultiplicity",ctpMuonCands.size());
      auto val_rawMultiplicity = Monitored::Scalar<int>("rawMultiplicity",inputMuonCands.size());
      auto val_countDiff = Monitored::Scalar<int>("countDiff",ctpMuonCands.size()-inputMuonCands.size());

      auto val_roi_Eta = Monitored::Collection("Eta",roi_Eta);
      auto val_roi_Phi = Monitored::Collection("Phi",roi_Phi);
      auto val_roi_dRmin = Monitored::Collection("dRmin",roi_dRmin);
      auto val_roi_pTdiff = Monitored::Collection("pTdiff",roi_pTdiff);
      auto val_roi_ThrNum = Monitored::Collection("ThrNum",roi_ThrNum);
      auto val_roi_Charge = Monitored::Collection("Charge",roi_Charge);
      auto val_roi_BW3Coin = Monitored::Collection("BW3Coin",roi_BW3Coin);
      auto val_roi_InnerCoin = Monitored::Collection("InnerCoin",roi_InnerCoin);
      auto val_roi_GoodMF = Monitored::Collection("GoodMF",roi_GoodMF);
      auto val_roi_IsMoreCandInRoI = Monitored::Collection("IsMoreCandInRoI",roi_IsMoreCandInRoI);
      auto val_roi_PhiOverlap = Monitored::Collection("PhiOverlap",roi_PhiOverlap);
      auto val_roi_EtaOverlap = Monitored::Collection("EtaOverlap",roi_EtaOverlap);
      auto val_roi_isVetoed = Monitored::Collection("isVetoed",roi_isVetoed);
      auto val_roi_inOk_outOk = Monitored::Collection("inOk_outOk",roi_inOk_outOk);
      auto val_roi_inOk_outNg = Monitored::Collection("inOk_outNg",roi_inOk_outNg);
      auto val_roi_inNg_outOk = Monitored::Collection("inNg_outOk",roi_inNg_outOk);

      ctpMonVariables.push_back(val_roiMatching_CTPin);
      ctpMonVariables.push_back(val_roiMatching_CTPout);
      ctpMonVariables.push_back(val_ctpMultiplicity);
      ctpMonVariables.push_back(val_rawMultiplicity);
      ctpMonVariables.push_back(val_countDiff);
      ctpMonVariables.push_back(val_roi_Eta);
      ctpMonVariables.push_back(val_roi_Phi);
      ctpMonVariables.push_back(val_roi_dRmin);
      ctpMonVariables.push_back(val_roi_pTdiff);
      ctpMonVariables.push_back(val_roi_ThrNum);
      ctpMonVariables.push_back(val_roi_Charge);
      ctpMonVariables.push_back(val_roi_BW3Coin);
      ctpMonVariables.push_back(val_roi_InnerCoin);
      ctpMonVariables.push_back(val_roi_GoodMF);
      ctpMonVariables.push_back(val_roi_IsMoreCandInRoI);
      ctpMonVariables.push_back(val_roi_PhiOverlap);
      ctpMonVariables.push_back(val_roi_EtaOverlap);
      ctpMonVariables.push_back(val_roi_isVetoed);
      ctpMonVariables.push_back(val_roi_inOk_outOk);
      ctpMonVariables.push_back(val_roi_inOk_outNg);
      ctpMonVariables.push_back(val_roi_inNg_outOk);
      fill(m_packageName + monObj.title.data(), ctpMonVariables);
    }
    ATH_MSG_DEBUG("End filling histograms for MuonRoIs after trigger decision");
  }
  ///////////////// End filling histograms for MuonRoIs after trigger decision /////////////////

  ///////////////// Filling histograms for MuonRoIs in thresholdPattern /////////////////
  std::map<const xAOD::MuonRoI*,std::set<std::string>> roiAndMenu;
  if(m_monitorThresholdPatterns.value() && rois != nullptr ){
    ATH_MSG_DEBUG("Filling histograms for MuonRoIs in thresholdPattern");
    SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_L1MenuKey, ctx);
    SG::ReadDecorHandle<xAOD::MuonRoIContainer,uint64_t> thrPatternAcc = SG::makeHandle<uint64_t>(m_thresholdPatternsKey, ctx);
    if(l1Menu.isValid() && thrPatternAcc.isPresent() && thrPatternAcc.isAvailable()){
      for(const auto& item : m_thrMonList){
	ATH_MSG_DEBUG("Item = " << item);
	bool ok = false;
	for(const auto& m : l1Menu->thresholdNames()){
	  ATH_MSG_DEBUG("item = " << m);
	  if( m == item ){
	    ok = true;
	    break;
	  }
	}
	if(!ok){
	  ATH_MSG_DEBUG("skipping " << item);
	  continue;
	}
	ATH_MSG_DEBUG("continue checking " << item);
	const TrigConf::L1Threshold& thr = l1Menu->threshold(item.data());
	std::vector<const xAOD::MuonRoI*> passed_rois;
	for(const auto roi : *rois){
	  const uint64_t thrPattern = thrPatternAcc(*roi);
	  bool passed = ( thrPattern & (1 << thr.mapping()) );
	  if(passed){
	    passed_rois.push_back(roi);
	    ATH_MSG_DEBUG("This RoI passed "<< item <<", roiWord=" << roi->roiWord() << ", thrNumber=" << roi->getThrNumber() << " eta=" << roi->eta() << " phi=" << roi->phi());
	    if(roiAndMenu.count(roi)==0){
	      std::set<std::string> items;
	      roiAndMenu.insert(std::make_pair(roi,items));
	    }
	    roiAndMenu[roi].insert(item);
	  }
	}

	MonVariables thrMonVariables;
	auto l1item_roi_eta = Monitored::Collection(Form("l1item_roi_eta_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return m->eta();
	  });
	thrMonVariables.push_back(l1item_roi_eta);
	auto l1item_roi_phi = Monitored::Collection(Form("l1item_roi_phi_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return m->phi();
	  });
	thrMonVariables.push_back(l1item_roi_phi);
	auto l1item_roi_thrNumber = Monitored::Collection(Form("l1item_roi_thrNumber_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return m->getThrNumber();
	  });
	thrMonVariables.push_back(l1item_roi_thrNumber);
	auto l1item_roi_ismorecand = Monitored::Collection(Form("l1item_roi_ismorecand_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return (m->getSource()==xAOD::MuonRoI::Barrel)?(m->isMoreCandInRoI()):(-1);
	  });
	thrMonVariables.push_back(l1item_roi_ismorecand);
	auto l1item_roi_bw3coin = Monitored::Collection(Form("l1item_roi_bw3coin_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return (m->getSource()!=xAOD::MuonRoI::Barrel)?(m->getBW3Coincidence()):(-1);
	  });
	thrMonVariables.push_back(l1item_roi_bw3coin);
	auto l1item_roi_innercoin = Monitored::Collection(Form("l1item_roi_innercoin_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return (m->getSource()!=xAOD::MuonRoI::Barrel)?(m->getInnerCoincidence()):(-1);
	  });
	thrMonVariables.push_back(l1item_roi_innercoin);
	auto l1item_roi_goodmf = Monitored::Collection(Form("l1item_roi_goodmf_%s",item.data()), passed_rois, [](const xAOD::MuonRoI *m) {
	    return (m->getSource()!=xAOD::MuonRoI::Barrel)?(m->getGoodMF()):(-1);
	  });
	thrMonVariables.push_back(l1item_roi_goodmf);
	fill(m_packageName + item.data(), thrMonVariables);
      }
    }
    ATH_MSG_DEBUG("End filling histograms for MuonRoIs in thresholdPattern");
  }
  ///////////////// End filling histograms for MuonRoIs in thresholdPattern /////////////////

  ///////////////// Filling offline muon-related histograms /////////////////
  std::vector < MyMuon > mymuons;
  std::map < std::string, std::vector< ExtPos > > extpositions;
  std::vector< ExtPos > extpositions_pivot;
  if (m_anaOfflMuon.value()) {
    SG::ReadHandle < xAOD::MuonContainer > muons(m_MuonContainerKey, ctx);
    if (!muons.isValid()) {
      ATH_MSG_ERROR("Failed to get xAOD::MuonContainer");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Filling offline muon-related histograms");
    for (const auto muon : *muons) {
      // skip if muon is empty
      if (muon == nullptr) continue;

      // standard quality cuts for muons
      if (muon->pt() < 1000.) continue;
      if(!m_muonSelectionTool->accept(*muon)) continue;

      // initialize for muon-isolation check
      bool isolated = true;

      // initialize for tag-and-probe check
      bool probeOK = true;
      if( !nonMuonTriggerFired && m_TagAndProbe.value() ) probeOK = false; // t&p should be performed

      // OK, let's start looking at the second muons
      for(const auto muon2 : *muons){

	// skip if muon is empty
	if (muon2 == nullptr) continue;

	// skip the same muon candidate
	if( muon == muon2 )continue;

	// skip possible mismeasured muons
	if( muon2->pt() < 1000. ) continue;

	// check muon-muon isolation using the loosest-quality muons
	if ( isolated ){
	  double dr = xAOD::P4Helpers::deltaR(muon,muon2,false);
	  if( dr < m_isolationWindow.value() ) isolated = false;
	}

	// no need to check further if probeOK is already True
	// 0) if muon-orthogonal triggers are avaialble/fired
	// 1) if we don't use tag-and-probe
	// 2) if TrigDecTool is not available
	// 3) if the second muon matches the trigger requirement
	if(probeOK)continue;

	//  standard quality cuts for muons
	if(!m_muonSelectionTool->accept(*muon2)) continue;

	// loop over the single muon triggers if at least one of them matches this second muon
	for (const auto &trigName : list_of_single_muon_triggers) {
	  // check if this particular tirgger has fired in this event
	  if (!getTrigDecisionTool()->isPassed(trigName.data(),TrigDefs::Physics)) continue;
	  ATH_MSG_DEBUG("This muon trigger, " << trigName << ", is fired in this event!!");
	  // check if this second muon matches the HLT muon trigger
	  if(getTrigDecisionTool()->getNavigationFormat() == "TriggerElement") { // run 2 access
	    ATH_MSG_DEBUG("Trying Run2-style feature access");
	    auto fc = getTrigDecisionTool()->features(trigName.data(),TrigDefs::Physics);
	    for(const auto& comb : fc.getCombinations()){
	      if(!comb.active())continue;
	      auto MuFeatureContainers = comb.get<xAOD::MuonContainer>("MuonEFInfo",TrigDefs::Physics);
	      for(const auto& mucont : MuFeatureContainers){
		if(mucont.empty())continue;
		if(mucont.te()==nullptr)continue;
		if(!mucont.te()->getActiveState())continue;
		for(const auto hltmu : *mucont.cptr()){
		  if (hltmu == nullptr) continue; // skip if hltmu is empty
		  if (hltmu->pt() < 1000.)continue; // skip if pT is very small
		  double dr = xAOD::P4Helpers::deltaR(muon2,hltmu,false);
		  if( dr < m_trigMatchWindow.value() ){
		    ATH_MSG_DEBUG("Trigger matched: "<<trigName<<" dR=" << dr );
		    probeOK = true;
		  }
		  if(probeOK) break; // no need to check further if probeOK is already True
		}// end loop of mucont.cptr()
		if(probeOK) break; // no need to check further if probeOK is already True
	      }// end loop of MuonFeatureContainers
	      if(probeOK) break; // no need to check further if probeOK is already True
	    }//end loop of Combinations
	  }else{ // run 3 access
	    ATH_MSG_DEBUG("Trying Run3-style feature access");
	    auto features = getTrigDecisionTool()->features < xAOD::MuonContainer > (trigName.data(), TrigDefs::Physics, "HLT_MuonsCB_RoI");
	    for (const auto& aaa : features) {
	      if (!aaa.isValid()) continue;
	      auto hltmu_link = aaa.link;
	      if (!hltmu_link.isValid()) continue;
	      auto hltmu = *hltmu_link;
	      if (hltmu == nullptr) continue; // skip if hltmu is empty
	      if (hltmu->pt() < 1000.)continue; // skip if pT is very small
	      double dr = xAOD::P4Helpers::deltaR(muon2,hltmu,false);
	      if( dr < m_trigMatchWindow.value() ){
		ATH_MSG_DEBUG("Trigger matched: "<<trigName<<" dR=" << dr );
		probeOK = true;
	      }
	      if(probeOK) break; // no need to check further if probeOK is already True
	    } // end loop of features
	  } // end IF Run2 or Run3 feature access
	  if(probeOK) break; // no need to check further if probeOK is already True
	} // end loop of single muon triggers
	// check if the second muon matches the single muon trigger
	if(!probeOK) continue;
	ATH_MSG_DEBUG("Basic Tag-and-Probe is OK");
	// check further if this muon pair satisfies Z->mumu criteria
	if( m_TagAndProbeZmumu.value() && muon->charge() != muon2->charge() ){
	  double m2 = 2. * muon->pt() * muon2->pt() * ( std::cosh(muon->eta() - muon2->eta()) - std::cos(muon->phi() - muon2->phi()) );
	  double m = (m2>0.) ? ( std::sqrt(m2) ) : (0.);
	  double mdiff = std::abs( m - m_zMass.value() );
	  probeOK = mdiff < m_zMassWindow.value();
	  ATH_MSG_DEBUG("Checking Zmumu cut: " << probeOK);
	}
	ATH_MSG_DEBUG("Final condition of probleOK for this muon is: " << probeOK);
	if(probeOK) break; // no need to check further if probeOK is already True
      } // end loop of the second muons
      // check if the isolation requirement is OK
      if(m_requireIsolated.value() && !isolated)continue;
      // check if the tag-and-probe requirement is OK
      if(!probeOK)continue;

      MyMuon mymuon;
      /* fill basic info */
      mymuon.muon = muon;
      /* fill extrapolation info (only to TGC) */
      if ( std::abs(muon->eta()) > 0.5 // only endcap region
	   && muon->pt() > m_pTCutOnExtrapolation ) { // only reasonably-high pT muons
	for (const auto &z : m_extZposition) {
	  if( muon->eta()<0 && z>0 )continue;
	  if( muon->eta()>0 && z<0 )continue;
	  xAOD::Muon::TrackParticleType trkPtclType;
	  if(m_useIDTrackForExtrapolation){ trkPtclType = xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle;
	  }else if(m_useMSTrackForExtrapolation){trkPtclType = xAOD::Muon::TrackParticleType::MuonSpectrometerTrackParticle;
	  }else if(m_useCBTrackForExtrapolation){trkPtclType = xAOD::Muon::TrackParticleType::CombinedTrackParticle;
	  }else if(m_useExtMSTrackForExtrapolation){trkPtclType = xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle;
	  }else if(m_useMSOnlyExtMSTrackForExtrapolation){trkPtclType = xAOD::Muon::TrackParticleType::MSOnlyExtrapolatedMuonSpectrometerTrackParticle;
	  }else{ trkPtclType = xAOD::Muon::TrackParticleType::Primary; } // default is Primary (i.e. same as muonType )
	  auto trackParticle = (m_useDirectPrimaryTrackForExtrapolation) ? muon->primaryTrackParticle() : muon->trackParticle( trkPtclType );
	  if(trackParticle==nullptr)continue;
	  auto matrix = std::make_unique<Amg::Transform3D>();
	  matrix->setIdentity();
	  matrix->translation().z() = z;
	  auto disc = std::make_unique < Trk::DiscSurface > (*matrix,
							     m_endcapPivotPlaneMinimumRadius.value(),
							     m_endcapPivotPlaneMaximumRadius.value());
	  const Trk::BoundaryCheck boundaryCheck = true;
	  const auto extTrkParams = m_extrapolator->extrapolate(ctx,
								*trackParticle,
								*disc,
								Trk::alongMomentum,
								boundaryCheck,
								Trk::muon);
	  if(extTrkParams != nullptr){
	    if( std::abs(extTrkParams->position().z() - z) > 10. )continue; // wrong extrapolation
	    ExtPos ext;
	    ext.extPosZ = z;
	    ext.extPos = extTrkParams->position();
	    ext.extVec = extTrkParams->momentum();
	    Amg::Vector3D extVec(extTrkParams->position().x(),extTrkParams->position().y(),z);
	    ext.passedChambers = m_tgcMonTool->getPassedChambers(extVec);
	    ext.muon = muon;
	    if( std::abs( std::abs(z) - m_M3_Z ) < 10. &&  // trigger pivot plane (i.e. M3)
		std::abs( muon->eta() ) > 1.05 &&
		std::abs( muon->eta() ) < 2.40){ // only endcap
	      extpositions_pivot.push_back(ext);
	    }
	    for(const auto& cham : ext.passedChambers){
	      extpositions[cham].push_back(ext);
	    }
	  }
	}
      }
      /* L1Muon RoI matching */
      mymuon.matchedL1Charge=false;
      mymuon.passBW3Coin=false;
      mymuon.passInnerCoin=false;
      mymuon.passGoodMF=false;
      mymuon.passIsMoreCandInRoI=false;
      double max_dr = 999;
      double pt = mymuon.muon->pt();
      if (pt > pt_15_cut) max_dr = m_l1trigMatchWindow1.value();
      else if (pt > pt_10_cut) max_dr = m_l1trigMatchWindow2.value() + m_l1trigMatchWindow3.value() * pt / Gaudi::Units::GeV;
      else max_dr = m_l1trigMatchWindow4.value() + m_l1trigMatchWindow5.value() * pt / Gaudi::Units::GeV;
      if (rois == nullptr) {
	ATH_MSG_DEBUG("No RoI matching possible as no container has been retrieved");
	mymuons.push_back(mymuon);
	continue;
      }
      for(const auto roi : *rois){
	double dr = xAOD::P4Helpers::deltaR(*muon,roi->eta(),roi->phi(),false);
	if( dr < max_dr ){
	  if(roiAndMenu.count(roi)>0)mymuon.matchedL1Items.insert( roiAndMenu[roi].begin(), roiAndMenu[roi].end() );
	  mymuon.matchedL1ThrExclusive.insert( roi->getThrNumber() );
	  if(muon->charge()<0 && roi->getCharge()==xAOD::MuonRoI::Neg)mymuon.matchedL1Charge|=true;
	  else if(muon->charge()>0 && roi->getCharge()==xAOD::MuonRoI::Pos)mymuon.matchedL1Charge|=true;
	  mymuon.passBW3Coin|=roi->getBW3Coincidence();
	  mymuon.passInnerCoin|=roi->getInnerCoincidence();
	  mymuon.passGoodMF|=roi->getGoodMF();
	  mymuon.passIsMoreCandInRoI|=roi->isMoreCandInRoI();
	}
      }
      const bool L1ThrsEmpt = mymuon.matchedL1ThrExclusive.empty();
      for (int ithr = 0; ithr < 16 && !L1ThrsEmpt; ++ithr) {
	bool pass = false;
	for (const auto &thr : mymuon.matchedL1ThrExclusive) {
	  if (thr >= ithr) {
	    pass = true;
	    break;
	  }
	}
	if (pass) mymuon.matchedL1ThrInclusive.insert(ithr);
      }

      /* store MyMuon */
      mymuons.push_back(mymuon);
    }


    auto oflmuon_num=Monitored::Scalar<int>("oflmuon_num",(*muons).size());
    auto oflmuon_muonType=Monitored::Collection("oflmuon_muonType",*muons,[](const xAOD::Muon*m){return m->muonType();});
    auto oflmuon_author=Monitored::Collection("oflmuon_author",*muons,[](const xAOD::Muon*m){return m->author();});
    auto oflmuon_quality=Monitored::Collection("oflmuon_quality",*muons,[](const xAOD::Muon*m){return m->quality();});
    auto oflmuon_pt=Monitored::Collection("oflmuon_pt",*muons,[](const xAOD::Muon*m){return m->pt() / Gaudi::Units::GeV;});
    auto oflmuon_eta=Monitored::Collection("oflmuon_eta",*muons,[](const xAOD::Muon*m){return m->eta();});
    auto oflmuon_phi=Monitored::Collection("oflmuon_phi",*muons,[](const xAOD::Muon*m){return m->phi();});
    fill(m_packageName+"_Muon",oflmuon_num,oflmuon_muonType,oflmuon_author,oflmuon_quality,oflmuon_pt,oflmuon_eta,oflmuon_phi);


    
    MonVariables  oflmuon_variables;
    oflmuon_variables.push_back(oflmuon_num);
    oflmuon_variables.push_back(oflmuon_muonType);
    oflmuon_variables.push_back(oflmuon_author);
    oflmuon_variables.push_back(oflmuon_quality);
    oflmuon_variables.push_back(oflmuon_pt);
    oflmuon_variables.push_back(oflmuon_eta);
    oflmuon_variables.push_back(oflmuon_phi);


    auto muon_eta4gev = Monitored::Collection("muon_eta4gev",mymuons,[](const MyMuon& m){
    	return (m.muon->pt()>pt_4_cut)?m.muon->eta():-10;
      });
    oflmuon_variables.push_back(muon_eta4gev);
    auto muon_phi4gev = Monitored::Collection("muon_phi4gev",mymuons,[](const MyMuon& m){
    	return (m.muon->pt()>pt_4_cut)?m.muon->phi():-10;
      });
    oflmuon_variables.push_back(muon_phi4gev);
    auto muon_eta = Monitored::Collection("muon_eta", mymuons, [](const MyMuon &m) {
    	return (m.muon->pt() > pt_30_cut) ? m.muon->eta() : -10;
      });
    oflmuon_variables.push_back(muon_eta);
    auto muon_phi = Monitored::Collection("muon_phi", mymuons, [](const MyMuon &m) {
    	return (m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
      });
    oflmuon_variables.push_back(muon_phi);
    auto muon_phi_rpc = Monitored::Collection("muon_phi_rpc", mymuons, [](const MyMuon &m) {
    	return (std::abs(m.muon->eta()) < barrel_end && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
      });
    oflmuon_variables.push_back(muon_phi_rpc);
    auto muon_phi_tgc = Monitored::Collection("muon_phi_tgc", mymuons, [](const MyMuon &m) {
    	return (std::abs(m.muon->eta()) > barrel_end && std::abs(m.muon->eta()) < trigger_end && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
      });
    oflmuon_variables.push_back(muon_phi_tgc);
    auto muon_pt_rpc = Monitored::Collection("muon_pt_rpc", mymuons, [](const MyMuon &m) {
    	return (std::abs(m.muon->eta()) < barrel_end) ? m.muon->pt() / Gaudi::Units::GeV : -10;
      });
    oflmuon_variables.push_back(muon_pt_rpc);
    auto muon_pt_tgc = Monitored::Collection("muon_pt_tgc", mymuons, [](const MyMuon &m) {
    	return (std::abs(m.muon->eta()) > barrel_end && std::abs(m.muon->eta()) < trigger_end) ? m.muon->pt() / Gaudi::Units::GeV : -10;
      });
    oflmuon_variables.push_back(muon_pt_tgc);
    auto muon_l1passThr1 = Monitored::Collection("muon_l1passThr1", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(1) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr1);
    auto muon_l1passThr2 = Monitored::Collection("muon_l1passThr2", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(2) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr2);
    auto muon_l1passThr3 = Monitored::Collection("muon_l1passThr3", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(3) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr3);
    auto muon_l1passThr4 = Monitored::Collection("muon_l1passThr4", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(4) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr4);
    auto muon_l1passThr5 = Monitored::Collection("muon_l1passThr5", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(5) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr5);
    auto muon_l1passThr6 = Monitored::Collection("muon_l1passThr6", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(6) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr6);
    auto muon_l1passThr7 = Monitored::Collection("muon_l1passThr7", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(7) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr7);
    auto muon_l1passThr8 = Monitored::Collection("muon_l1passThr8", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(8) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr8);
    auto muon_l1passThr9 = Monitored::Collection("muon_l1passThr9", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(9) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr9);
    auto muon_l1passThr10 = Monitored::Collection("muon_l1passThr10", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(10) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr10);
    auto muon_l1passThr11 = Monitored::Collection("muon_l1passThr11", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(11) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr11);
    auto muon_l1passThr12 = Monitored::Collection("muon_l1passThr12", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(12) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr12);
    auto muon_l1passThr13 = Monitored::Collection("muon_l1passThr13", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(13) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr13);
    auto muon_l1passThr14 = Monitored::Collection("muon_l1passThr14", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(14) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr14);
    auto muon_l1passThr15 = Monitored::Collection("muon_l1passThr15", mymuons, [](const MyMuon &m) {
    	return m.matchedL1ThrInclusive.find(15) != m.matchedL1ThrInclusive.end();
      });
    oflmuon_variables.push_back(muon_l1passThr15);
    auto muon_l1passCharge = Monitored::Collection("muon_l1passCharge",mymuons,[](const MyMuon& m){
    	return m.matchedL1Charge;
      });
    oflmuon_variables.push_back(muon_l1passCharge);
    auto muon_l1passBW3Coin = Monitored::Collection("muon_l1passBW3Coin",mymuons,[](const MyMuon& m){
    	return m.passBW3Coin;
      });
    oflmuon_variables.push_back(muon_l1passBW3Coin);
    auto muon_l1passBW3CoinVeto = Monitored::Collection("muon_l1passBW3CoinVeto",mymuons,[](const MyMuon& m){
    	return !m.passBW3Coin;
      });
    oflmuon_variables.push_back(muon_l1passBW3CoinVeto);
    auto muon_l1passInnerCoin = Monitored::Collection("muon_l1passInnerCoin",mymuons,[](const MyMuon& m){
    	return m.passInnerCoin;
      });
    oflmuon_variables.push_back(muon_l1passInnerCoin);
    auto muon_l1passInnerCoinVeto = Monitored::Collection("muon_l1passInnerCoinVeto",mymuons,[](const MyMuon& m){
    	return !m.passInnerCoin;
      });
    oflmuon_variables.push_back(muon_l1passInnerCoinVeto);
    auto muon_l1passGoodMF = Monitored::Collection("muon_l1passGoodMF",mymuons,[](const MyMuon& m){
    	return m.passGoodMF;
      });
    oflmuon_variables.push_back(muon_l1passGoodMF);
    auto muon_l1passBadMF = Monitored::Collection("muon_l1passBadMF",mymuons,[](const MyMuon& m){
    	return !m.passGoodMF;
      });
    oflmuon_variables.push_back(muon_l1passBadMF);
    auto muon_l1passIsMoreCandInRoI = Monitored::Collection("muon_l1passIsMoreCandInRoI",mymuons,[](const MyMuon& m){
    	return m.passIsMoreCandInRoI;
      });
    oflmuon_variables.push_back(muon_l1passIsMoreCandInRoI);
    fill(m_packageName, oflmuon_variables);

    for(const auto& item : m_thrMonList){
      std::vector<bool> passed;
      passed.reserve(mymuons.size());
      for(const auto& mymuon : mymuons){
    	passed.push_back( mymuon.matchedL1Items.find(item) != mymuon.matchedL1Items.end() );
      }
      auto muon_passed_l1item = Monitored::Collection(Form("muon_passed_l1item_%s",item.data()),passed);
      fill(m_packageName + item.data(),
    	   muon_passed_l1item,
    	   muon_eta, muon_phi, muon_pt_rpc, muon_pt_tgc );
    }
    ATH_MSG_DEBUG("End filling offline muon-related histograms");
  }
  ///////////////// End filling offline muon-related histograms /////////////////



  ///////////////// Filling TGC PRD histograms /////////////////
  if (m_anaTgcPrd.value()) {
    ATH_MSG_DEBUG("m_anaTgcPrd.value() = True");
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey,ctx};
    const MuonGM::MuonDetectorManager* muonMgr = DetectorManagerHandle.cptr();
    SG::ReadHandle < Muon::TgcPrepDataContainer > tgcPrd(m_TgcPrepDataContainerKey, ctx);
    if(tgcPrd.isValid() && muonMgr!=nullptr){
      ATH_MSG_DEBUG("Filling TGC PRD histograms");
      const TgcIdHelper &tgcIdHelper = m_idHelperSvc->tgcIdHelper();
      std::vector < TGC::TgcHit > tgcHits;
      std::map<std::string, std::vector<TGC::TgcHit>> tgcHitsMap;
      std::map<std::string, std::vector<int>> tgcHitPhiMap;
      std::map<std::string, std::vector<int>> tgcHitEtaMap;
      std::map<std::string, std::vector<int>> tgcHitPhiMapGlobal;
      std::map<std::string, std::vector<int>> tgcHitTiming;
      std::map<std::string, std::vector<int>> tgcHitPhiMapGlobalWithTrack;
      std::map<std::string, std::vector<int>> tgcHitTimingWithTrack;
      std::map<std::string, int> tgcHitBCMaskMap;
      std::vector <int> vec_bw24sectors; // 1..12 BW-A, -1..-12 BW-C
      std::vector <int> vec_bw24sectors_wire;
      std::vector <int> vec_bw24sectors_strip;
      std::vector <int> vec_bwfulleta; // 0(Forward), 1..4(M1), 1..5(M2,M3)
      std::vector <int> vec_bwfulleta_wire;
      std::vector <int> vec_bwfulleta_strip;
      std::vector <int> vec_bwtiming;
      std::vector <int> vec_bwtiming_wire;
      std::vector <int> vec_bwtiming_strip;
      std::vector <int> vec_bw24sectors_wTrack; // 1..12 BW-A, -1..-12 BW-C
      std::vector <int> vec_bw24sectors_wire_wTrack;
      std::vector <int> vec_bw24sectors_strip_wTrack;
      std::vector <int> vec_bwfulleta_wTrack; // 0(Forward), 1..4(M1), 1..5(M2,M3)
      std::vector <int> vec_bwfulleta_wire_wTrack;
      std::vector <int> vec_bwfulleta_strip_wTrack;
      std::vector <int> vec_bwtiming_wTrack;
      std::vector <int> vec_bwtiming_wire_wTrack;
      std::vector <int> vec_bwtiming_strip_wTrack;
      for (const auto tgccnt : *tgcPrd) {
	for (const auto data : *tgccnt) {
	  int bunch = -10;
	  if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_PREVIOUS) == Muon::TgcPrepData::BCBIT_PREVIOUS) bunch = -1;
	  if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_CURRENT) == Muon::TgcPrepData::BCBIT_CURRENT) bunch = 0;
	  if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_NEXT) == Muon::TgcPrepData::BCBIT_NEXT) bunch = +1;
	  const MuonGM::TgcReadoutElement *element = data->detectorElement();
	  const Identifier id = data->identify();
	  const int gasGap = tgcIdHelper.gasGap(id);
	  const int channel = tgcIdHelper.channel(id);
	  const bool isStrip = tgcIdHelper.isStrip(id);
	  const Amg::Vector3D &pos = isStrip ? element->stripPos(gasGap, channel) : element->gangPos(gasGap, channel);
	  auto shortWidth = (isStrip)?(element->stripShortWidth(gasGap, channel)):(element->gangShortWidth(gasGap, channel));
	  auto longWidth = (isStrip)?(element->stripLongWidth(gasGap, channel)):(element->gangLongWidth(gasGap, channel));
	  auto length = (isStrip)?(element->stripLength(gasGap, channel)):(element->gangLength(gasGap, channel));

	  TGC::TgcHit tgcHit(pos[0],pos[1],pos[2],
			     shortWidth,longWidth, length,
			     isStrip,gasGap,channel,tgcIdHelper.stationEta(id),tgcIdHelper.stationPhi(id),tgcIdHelper.stationName(id),
			     bunch);
	  bool hasMatchedTrack = false;
	  if(extpositions.find(tgcHit.cham_name())!=extpositions.end()){
	    for(auto& cham : extpositions[tgcHit.cham_name()]){
	      double newX = cham.extPos.x() + cham.extVec.x() / cham.extVec.z() * ( tgcHit.Z() - cham.extPos.z() );
	      double newY = cham.extPos.y() + cham.extVec.y() / cham.extVec.z() * ( tgcHit.Z() - cham.extPos.z() );
	      auto id2 = muonMgr->tgcIdHelper()->elementID(tgcHit.StationName(), tgcHit.StationEta(), tgcHit.StationPhi());
	      IdentifierHash hash_id;
	      muonMgr->tgcIdHelper()->get_detectorElement_hash(id2,hash_id);
	      auto detEle = muonMgr->getTgcDetectorElement(hash_id);
	      double chamPhi = detEle->center().phi();
	      TVector2 extPos(newX,newY);
	      TVector2 hitPos(tgcHit.X(),tgcHit.Y());
	      TVector2 rot_extPos  = extPos.Rotate(-chamPhi + M_PI/2.);
	      TVector2 rot_hitPos  = hitPos.Rotate(-chamPhi + M_PI/2.);
	      if(tgcHit.isStrip()){ // strip
		tgcHit.setResidual( std::sin( rot_extPos.DeltaPhi( rot_hitPos ) ) * rot_extPos.Mod() );
	      }else{ // wire
		tgcHit.setResidual( rot_hitPos.Y() - rot_extPos.Y() );
	      }
	      if(std::abs(tgcHit.residual()) < m_residualWindow.value()){
		cham.chambersHasHit.insert(tgcHit.gap_name());
		hasMatchedTrack = true;
	      }
	    }
	  }

	  tgcHits.push_back(tgcHit);
	  tgcHitsMap[tgcHit.gap_name()].push_back(tgcHit);
	  std::string chamberNameWithWS = Form("%s%s",tgcHit.cham_name().data(),(tgcHit.isStrip())?("S"):("W"));
	  tgcHitsMap[chamberNameWithWS].push_back(tgcHit);

	  if(hasMatchedTrack){
	    std::string chamberNameWithLandWSandChannel = Form("%sL%02d%sCh%03d",tgcHit.cham_name().data(),gasGap,(tgcHit.isStrip())?("S"):("W"),channel);
	    int BCMask = ( tgcHitBCMaskMap.find(chamberNameWithLandWSandChannel)==tgcHitBCMaskMap.end() ) ? ( 0 ) : ( tgcHitBCMaskMap[chamberNameWithLandWSandChannel]);
	    if( (data->getBcBitMap() & Muon::TgcPrepData::BCBIT_NEXT) != 0 ) BCMask |= 0x1;
	    else if( (data->getBcBitMap() & Muon::TgcPrepData::BCBIT_CURRENT) != 0 )BCMask |= 0x2;
	    else if( (data->getBcBitMap() & Muon::TgcPrepData::BCBIT_PREVIOUS) != 0 )BCMask |= 0x4;
	    else{ BCMask = 0;} // undefined
	    tgcHitBCMaskMap[chamberNameWithLandWSandChannel] = BCMask;
	  }
	  
	  std::string station_name = Form("%sM%02d%s",(tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?("A"):("C"),tgcHit.iM(),(tgcHit.isStrip())?("S"):("W"));
	  
	  int phimap_index = 0;
	  int etamap_index = 0;
	  int phimap_global_index = 0; // no empty bins compare to the above index
	  m_tgcMonTool->getMapIndex(tgcHit,etamap_index,phimap_index,phimap_global_index );
	  tgcHitPhiMap[station_name].push_back(phimap_index);
	  tgcHitEtaMap[station_name].push_back(etamap_index);
	  tgcHitPhiMapGlobal[station_name].push_back(phimap_global_index);
	  tgcHitTiming[station_name].push_back(bunch);
	  if(hasMatchedTrack){
	    tgcHitPhiMapGlobalWithTrack[station_name].push_back(phimap_global_index);
	    tgcHitTimingWithTrack[station_name].push_back(bunch);
	  }

	  if(tgcHit.iM()!=4){
	    vec_bw24sectors.push_back((tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?(tgcHit.iSec()):(-tgcHit.iSec()));
	    vec_bwfulleta.push_back(tgcHit.iEta());
	    vec_bwtiming.push_back(tgcHit.bunch());
	    if(hasMatchedTrack){
	      vec_bw24sectors_wTrack.push_back((tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?(tgcHit.iSec()):(-tgcHit.iSec()));
	      vec_bwfulleta_wTrack.push_back(tgcHit.iEta());
	      vec_bwtiming_wTrack.push_back(tgcHit.bunch());
	    }
	    if(tgcHit.isStrip()){
	      vec_bw24sectors_strip.push_back((tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?(tgcHit.iSec()):(-tgcHit.iSec()));
	      vec_bwfulleta_strip.push_back(tgcHit.iEta());
	      vec_bwtiming_strip.push_back(tgcHit.bunch());
	      if(hasMatchedTrack){
		vec_bw24sectors_strip_wTrack.push_back((tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?(tgcHit.iSec()):(-tgcHit.iSec()));
		vec_bwfulleta_strip_wTrack.push_back(tgcHit.iEta());
		vec_bwtiming_strip_wTrack.push_back(tgcHit.bunch());
	      }
	    }else{
	      vec_bw24sectors_wire.push_back((tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?(tgcHit.iSec()):(-tgcHit.iSec()));
	      vec_bwfulleta_wire.push_back(tgcHit.iEta());
	      vec_bwtiming_wire.push_back(tgcHit.bunch());
	      if(hasMatchedTrack){
		vec_bw24sectors_wire_wTrack.push_back((tgcHit.iSide()==TGC::TGCSIDE::TGCASIDE)?(tgcHit.iSec()):(-tgcHit.iSec()));
		vec_bwfulleta_wire_wTrack.push_back(tgcHit.iEta());
		vec_bwtiming_wire_wTrack.push_back(tgcHit.bunch());
	      }
	    }
	  }

	}
      }
      
      ATH_MSG_DEBUG("filling hit_variables");
      
      MonVariables  hit_variables;
      hit_variables.push_back(mon_bcid);
      hit_variables.push_back(mon_pileup);
      hit_variables.push_back(mon_lb);

      auto hit_n = Monitored::Scalar<int>("hit_n", tgcHits.size());
      hit_variables.push_back(hit_n);

      auto hit_bunch=Monitored::Collection("hit_bunch",tgcHits,[](const TGC::TgcHit&m){return m.bunch();});
      hit_variables.push_back(hit_bunch);

      auto hit_sideA=Monitored::Collection("hit_sideA",tgcHits,[](const TGC::TgcHit&m){return m.Z()>0;});
      hit_variables.push_back(hit_sideA);

      auto hit_sideC=Monitored::Collection("hit_sideC",tgcHits,[](const TGC::TgcHit&m){return m.Z() < 0;});
      hit_variables.push_back(hit_sideC);

      auto hit_bw24sectors=Monitored::Collection("hit_bw24sectors",vec_bw24sectors,[](const int&m){return m;});
      hit_variables.push_back(hit_bw24sectors);
      auto hit_bw24sectors_strip=Monitored::Collection("hit_bw24sectors_strip",vec_bw24sectors_strip,[](const int&m){return m;});
      hit_variables.push_back(hit_bw24sectors_strip);
      auto hit_bw24sectors_wire=Monitored::Collection("hit_bw24sectors_wire",vec_bw24sectors_wire,[](const int&m){return m;});
      hit_variables.push_back(hit_bw24sectors_wire);
      auto hit_bwfulleta=Monitored::Collection("hit_bwfulleta",vec_bwfulleta,[](const int&m){return m;});
      hit_variables.push_back(hit_bwfulleta);
      auto hit_bwfulleta_strip=Monitored::Collection("hit_bwfulleta_strip",vec_bwfulleta_strip,[](const int&m){return m;});
      hit_variables.push_back(hit_bwfulleta_strip);
      auto hit_bwfulleta_wire=Monitored::Collection("hit_bwfulleta_wire",vec_bwfulleta_wire,[](const int&m){return m;});
      hit_variables.push_back(hit_bwfulleta_wire);
      auto hit_bwtiming=Monitored::Collection("hit_bwtiming",vec_bwtiming,[](const int&m){return m;});
      hit_variables.push_back(hit_bwtiming);
      auto hit_bwtiming_strip=Monitored::Collection("hit_bwtiming_strip",vec_bwtiming_strip,[](const int&m){return m;});
      hit_variables.push_back(hit_bwtiming_strip);
      auto hit_bwtiming_wire=Monitored::Collection("hit_bwtiming_wire",vec_bwtiming_wire,[](const int&m){return m;});
      hit_variables.push_back(hit_bwtiming_wire);

      auto hit_bw24sectors_wTrack=Monitored::Collection("hit_bw24sectors_wTrack",vec_bw24sectors_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bw24sectors_wTrack);
      auto hit_bw24sectors_strip_wTrack=Monitored::Collection("hit_bw24sectors_strip_wTrack",vec_bw24sectors_strip_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bw24sectors_strip_wTrack);
      auto hit_bw24sectors_wire_wTrack=Monitored::Collection("hit_bw24sectors_wire_wTrack",vec_bw24sectors_wire_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bw24sectors_wire_wTrack);
      auto hit_bwfulleta_wTrack=Monitored::Collection("hit_bwfulleta_wTrack",vec_bwfulleta_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bwfulleta_wTrack);
      auto hit_bwfulleta_strip_wTrack=Monitored::Collection("hit_bwfulleta_strip_wTrack",vec_bwfulleta_strip_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bwfulleta_strip_wTrack);
      auto hit_bwfulleta_wire_wTrack=Monitored::Collection("hit_bwfulleta_wire_wTrack",vec_bwfulleta_wire_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bwfulleta_wire_wTrack);
      auto hit_bwtiming_wTrack=Monitored::Collection("hit_bwtiming_wTrack",vec_bwtiming_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bwtiming_wTrack);
      auto hit_bwtiming_strip_wTrack=Monitored::Collection("hit_bwtiming_strip_wTrack",vec_bwtiming_strip_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bwtiming_strip_wTrack);
      auto hit_bwtiming_wire_wTrack=Monitored::Collection("hit_bwtiming_wire_wTrack",vec_bwtiming_wire_wTrack,[](const int&m){return m;});
      hit_variables.push_back(hit_bwtiming_wire_wTrack);

      std::vector<Monitored::ObjectsCollection<std::vector<int>, double>> varowner;
      varowner.reserve(tgcHitPhiMap.size() * 2 + tgcHitPhiMapGlobal.size() * 2 + tgcHitPhiMapGlobalWithTrack.size() * 2);
      for (const auto &phimap : tgcHitPhiMap) {
	varowner.push_back(Monitored::Collection(Form("hit_x_%s",phimap.first.data()),tgcHitEtaMap[phimap.first],[](const int&m){return m;}));
      	hit_variables.push_back(varowner.back());
	varowner.push_back(Monitored::Collection(Form("hit_y_%s", phimap.first.data()),phimap.second,[](const int&m){return m;}));
      	hit_variables.push_back(varowner.back());
      }
      for (const auto &phimap : tgcHitPhiMapGlobal) {
	varowner.push_back(Monitored::Collection(Form("hit_glblphi_%s", phimap.first.data()),phimap.second,[](const int&m){return m;}));
	hit_variables.push_back(varowner.back());
	varowner.push_back(Monitored::Collection(Form("hit_bunch_%s", phimap.first.data()),tgcHitTiming[phimap.first],[](const int&m){return m;}));
	hit_variables.push_back(varowner.back());
      }
      for (const auto &phimap : tgcHitPhiMapGlobalWithTrack) {
	varowner.push_back(Monitored::Collection(Form("hit_glblphi_wTrack_%s", phimap.first.data()),phimap.second,[](const int&m){return m;}));
	hit_variables.push_back(varowner.back());
	varowner.push_back(Monitored::Collection(Form("hit_bunch_wTrack_%s", phimap.first.data()),tgcHitTimingWithTrack[phimap.first],[](const int&m){return m;}));
	hit_variables.push_back(varowner.back());
      }
      
      // BCMask plots (for TTCrx gate delay scan)
      std::map<std::string, std::vector<int>> tgcHitBCMaskGlobalIndex;
      std::map<std::string, std::vector<int>> tgcHitBCMask;
      std::map<std::string, std::vector<int>> tgcHitBCMaskBWSectors;
      std::map<std::string, std::vector<int>> tgcHitBCMaskForBWSectors;
      for(const auto& channelNameAndBCMask : tgcHitBCMaskMap){
	std::string chamberNameWithWS = channelNameAndBCMask.first.substr(0,16); // e.g. A01M01f01E01L01W
	int thisChannel = std::atoi( channelNameAndBCMask.first.substr(18,3).data() ); // e.g. 001 of "Ch001"
	std::string prevChannelName = Form("%sCh%03d",chamberNameWithWS.data(),thisChannel-1);
	if(tgcHitBCMaskMap.find(prevChannelName)!=tgcHitBCMaskMap.end())continue; // remove if the neighboring channel also has a hit
	std::string nextChannelName = Form("%sCh%03d",chamberNameWithWS.data(),thisChannel+1);
	if(tgcHitBCMaskMap.find(nextChannelName)!=tgcHitBCMaskMap.end())continue; // remove if the neighboring channel also has a hit
	int BCMask = channelNameAndBCMask.second;
	std::string cham_name = channelNameAndBCMask.first.substr(0,12); // e.g. A01M01f01E01
	int iLay = std::atoi( channelNameAndBCMask.first.substr(13,2).data() );
	TGC::TgcChamber cham; cham.initChamber(cham_name);
	int phimap_index = 0;
	int etamap_index = 0;
	int phimap_global_index = 0;
	if(!m_tgcMonTool->getMapIndex(cham,iLay,etamap_index,phimap_index,phimap_global_index ))continue;
	std::string station_name = Form("%sM%02d%s",(cham.iSide()==TGC::TGCSIDE::TGCASIDE)?("A"):("C"),cham.iM(),channelNameAndBCMask.first.substr(15,1).data());
	tgcHitBCMaskGlobalIndex[station_name].push_back(phimap_global_index);
	tgcHitBCMask[station_name].push_back(BCMask);
	if(cham.iM()!=4){
	  tgcHitBCMaskBWSectors["All"].push_back( (cham.iSide()==TGC::TGCSIDE::TGCASIDE)?( +1 * cham.iSec() ):(-1 * cham.iSec()) );
	  tgcHitBCMaskForBWSectors["All"].push_back(BCMask);
	  if(chamberNameWithWS.find("W")!=std::string::npos){
	    tgcHitBCMaskBWSectors["Wire"].push_back( (cham.iSide()==TGC::TGCSIDE::TGCASIDE)?( +1 * cham.iSec() ):(-1 * cham.iSec()) );
	    tgcHitBCMaskForBWSectors["Wire"].push_back(BCMask);
	  }else{
	    tgcHitBCMaskBWSectors["Strip"].push_back( (cham.iSide()==TGC::TGCSIDE::TGCASIDE)?( +1 * cham.iSec() ):(-1 * cham.iSec()) );
	    tgcHitBCMaskForBWSectors["Strip"].push_back(BCMask);
	  }
	}
      }
      std::vector<Monitored::ObjectsCollection<std::vector<int>, double>> varowner_bcmask;
      varowner_bcmask.reserve(tgcHitBCMask.size() * 2 + tgcHitBCMaskBWSectors.size() * 2);
      for(const auto& chamType : tgcHitBCMaskBWSectors){
	varowner_bcmask.push_back(Monitored::Collection(Form("hit_bcmask_bw24sectors_%s",chamType.first.data()),chamType.second,[](const int&m){return m;}));
	hit_variables.push_back(varowner_bcmask.back());
	varowner_bcmask.push_back(Monitored::Collection(Form("hit_bcmask_for_bw24sectors_%s",chamType.first.data()),tgcHitBCMaskForBWSectors[chamType.first],[](const int&m){return m;}));
	hit_variables.push_back(varowner_bcmask.back());
      }
      for(const auto& stationNameAndBCMask : tgcHitBCMask){
	varowner_bcmask.push_back(Monitored::Collection(Form("hit_bcmask_glblphi_%s",stationNameAndBCMask.first.data()),tgcHitBCMaskGlobalIndex[stationNameAndBCMask.first],[](const int&m){return m;}));
	hit_variables.push_back(varowner_bcmask.back());
	varowner_bcmask.push_back(Monitored::Collection(Form("hit_bcmask_%s",stationNameAndBCMask.first.data()),stationNameAndBCMask.second,[](const int&m){return m;}));
	hit_variables.push_back(varowner_bcmask.back());
      }



      // gap-by-gap efficiency by track extrapolation
      ATH_MSG_DEBUG("preparing for efficiency plots");
      std::map<std::string, std::vector<double>> tgcEffPhiMap_Denominator;
      std::map<std::string, std::vector<double>> tgcEffEtaMap_Denominator;
      std::map<std::string, std::vector<double>> tgcEffPhiMapGlobal_Denominator;
      std::map<std::string, std::vector<double>> tgcEffPhiMap_Numerator;
      std::map<std::string, std::vector<double>> tgcEffEtaMap_Numerator;
      std::map<std::string, std::vector<double>> tgcEffPhiMapGlobal_Numerator;
      std::map<std::string, std::vector<double>> tgcEffMapExtX;
      std::map<std::string, std::vector<double>> tgcEffMapExtY;
      std::map<std::string, std::vector<double>> tgcEffMapHasHit;
      for(const auto& exts : extpositions){
	const std::string& cham_name = exts.first;
	TGC::TgcChamber cham; cham.initChamber(cham_name);
	// local-coordinate x'-y'
	auto id2 = muonMgr->tgcIdHelper()->elementID(cham.StationName(), cham.StationEta(), cham.StationPhi());
	IdentifierHash hash_id;
	muonMgr->tgcIdHelper()->get_detectorElement_hash(id2,hash_id);
	auto detEle = muonMgr->getTgcDetectorElement(hash_id);
	for(const auto& ext : exts.second){ // how often tracks are extrapolated to this chamber surface,e.i. denominator
	  Amg::Vector3D extPosLocal = detEle->transform().inverse() * ext.extPos;
	  Amg::Vector3D extVecLocal = detEle->transform().inverse() * ext.extVec;
	  for(int iLay = 1 ; iLay <= 3 ; iLay++){
	    int phimap_index = 0;
	    int etamap_index = 0;
	    int phimap_global_index = 0;
	    if(!m_tgcMonTool->getMapIndex(cham,iLay,etamap_index,phimap_index,phimap_global_index ))continue;
	    double gapZ = detEle->readoutElement()->localGasGapPos(iLay).z();
	    double newX = extPosLocal.x() + extVecLocal.x() / extVecLocal.z() * ( gapZ - extPosLocal.z() );
	    double newY = extPosLocal.y() + extVecLocal.y() / extVecLocal.z() * ( gapZ - extPosLocal.z() );
	    for(int iSorW = 0 ; iSorW < 2 ; iSorW++){
	      if(cham.iM()==1 && iLay==2 && iSorW==0)continue;
	      std::string gap_name = Form("%sL%02d%s",cham_name.data(),iLay,(iSorW==0)?("S"):("W"));
	      std::string station_name = Form("%sM%02d%s",(cham.iSide()==TGC::TGCSIDE::TGCASIDE)?("A"):("C"),cham.iM(),(iSorW==0)?("S"):("W"));
	      tgcEffPhiMap_Denominator[station_name].push_back(phimap_index);
	      tgcEffEtaMap_Denominator[station_name].push_back(etamap_index);
	      tgcEffPhiMapGlobal_Denominator[station_name].push_back(phimap_global_index);
	      tgcEffMapExtX[gap_name].push_back(newX);
	      tgcEffMapExtY[gap_name].push_back(newY);
	      double hitExist = 0;
	      if( ext.chambersHasHit.find(gap_name) != ext.chambersHasHit.end()) hitExist=1;
	      tgcEffPhiMap_Numerator[station_name].push_back(hitExist);
	      tgcEffEtaMap_Numerator[station_name].push_back(hitExist);
	      tgcEffPhiMapGlobal_Numerator[station_name].push_back(hitExist);
	      tgcEffMapHasHit[gap_name].push_back(hitExist);
	    }
	  }
	}
      }

      std::vector<Monitored::ObjectsCollection<std::vector<double>, double>> varowner_hiteff;
      std::vector<Monitored::ObjectsCollection<std::vector<TGC::TgcHit>, double>> varowner_eachchamber;

      if(m_fillGapByGapHistograms.value()){

	ATH_MSG_DEBUG("hit efficiency plots");
	varowner_hiteff.reserve(tgcEffPhiMap_Denominator.size() * 4 + tgcEffPhiMapGlobal_Denominator.size() * 2 + tgcEffMapHasHit.size() * 3);
	for (const auto &phimap : tgcEffPhiMap_Denominator) {
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_effden_x_%s",phimap.first.data()),tgcEffEtaMap_Denominator[phimap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_effden_y_%s", phimap.first.data()),tgcEffPhiMap_Denominator[phimap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_effnum_x_%s",phimap.first.data()),tgcEffEtaMap_Numerator[phimap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_effnum_y_%s", phimap.first.data()),tgcEffPhiMap_Numerator[phimap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	}
	for (const auto &phimap : tgcEffPhiMapGlobal_Denominator) {
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_glblphi_effden_%s", phimap.first.data()),tgcEffPhiMapGlobal_Denominator[phimap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_glblphi_effnum_%s", phimap.first.data()),tgcEffPhiMapGlobal_Numerator[phimap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	}
	for(const auto& hiteffmap : tgcEffMapHasHit){
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_localX_effden_%s", hiteffmap.first.data()),tgcEffMapExtX[hiteffmap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_localY_effden_%s", hiteffmap.first.data()),tgcEffMapExtY[hiteffmap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	  varowner_hiteff.push_back(Monitored::Collection(Form("hit_effnum_%s", hiteffmap.first.data()),tgcEffMapHasHit[hiteffmap.first],[](const double&m){return m;}));
	  hit_variables.push_back(varowner_hiteff.back());
	}

	ATH_MSG_DEBUG("gap-by-gap occupancy plots and residual plots");
      	varowner_eachchamber.reserve(tgcHitsMap.size()*2);
      	for (const auto &tgcHitMap : tgcHitsMap) {
	  auto chanName = tgcHitMap.first;
	  if(chanName.find("L")!=std::string::npos){ // individual gaps
	    varowner_eachchamber.push_back(Monitored::Collection(Form("hit_on_%s",chanName.data()),tgcHitMap.second,[](const TGC::TgcHit&m){return m.channel();}));
	    hit_variables.push_back(varowner_eachchamber.back());
	  }else{ // only summed over the gaps
	    varowner_eachchamber.push_back(Monitored::Collection(Form("hit_residual_on_%s",chanName.data()),tgcHitMap.second,[](const TGC::TgcHit&m){return m.residual();}));
	    hit_variables.push_back(varowner_eachchamber.back());
	  }
      	}

      }

      ATH_MSG_DEBUG("before fill for hits");
      fill(m_packageName+"_TgcHit", hit_variables);
      ATH_MSG_DEBUG("End filling TGC PRD histograms");
    }else{
      ATH_MSG_WARNING("Couldn't get TGC PRD");
    }
  }
  ///////////////// End filling TGC PRD histograms /////////////////


  ///////////////// Filling TGC CoinData histograms /////////////////
  if(m_anaTgcCoin){
    SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinCurr(m_TgcCoinDataContainerCurrBCKey, ctx);
    SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinNext(m_TgcCoinDataContainerNextBCKey, ctx);
    SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinPrev(m_TgcCoinDataContainerPrevBCKey, ctx);
    if (!tgcCoinCurr.isValid() || !tgcCoinNext.isValid() || !tgcCoinPrev.isValid()) {
      ATH_MSG_WARNING("Couldn't get TGC Coin Data");
    }else{
      ATH_MSG_DEBUG("Filling TGC CoinData histograms");
      std::map<int, SG::ReadHandle<Muon::TgcCoinDataContainer> > tgcCoin;
      tgcCoin[0] = tgcCoinCurr;
      tgcCoin[+1] = tgcCoinNext;
      tgcCoin[-1] = tgcCoinPrev;
      std::vector< TgcTrig > tgcTrigMap_SL;
      std::vector< TgcTrig > tgcTrigMap_SL_Endcap;
      std::vector< TgcTrig > tgcTrigMap_SL_Forward;
      std::vector< TgcTrig > tgcTrigMap_HPT_Wire;
      std::vector< TgcTrig > tgcTrigMap_HPT_Endcap_Wire;
      std::vector< TgcTrig > tgcTrigMap_HPT_Forward_Wire;
      std::vector< TgcTrig > tgcTrigMap_HPT_Strip;
      std::vector< TgcTrig > tgcTrigMap_HPT_Endcap_Strip;
      std::vector< TgcTrig > tgcTrigMap_HPT_Forward_Strip;
      std::vector< TgcTrig > tgcTrigMap_LPT_Wire;
      std::vector< TgcTrig > tgcTrigMap_LPT_Endcap_Wire;
      std::vector< TgcTrig > tgcTrigMap_LPT_Forward_Wire;
      std::vector< TgcTrig > tgcTrigMap_LPT_Strip;
      std::vector< TgcTrig > tgcTrigMap_LPT_Endcap_Strip;
      std::vector< TgcTrig > tgcTrigMap_LPT_Forward_Strip;
      int n_TgcCoin_detElementIsNull = 0;
      int n_TgcCoin_postOutPtrIsNull = 0;
      for (auto thisCoin : tgcCoin) {
	int bunch = thisCoin.first;
	for (const auto tgccnt : *(thisCoin.second)) {
	  for (const auto data : *tgccnt) {
	    if ( data->detectorElementOut() == nullptr ) n_TgcCoin_detElementIsNull++;
	    if ( data->posOutPtr() == nullptr ) n_TgcCoin_postOutPtrIsNull++;
	    if ( data->detectorElementOut() == nullptr ||
		 data->posOutPtr() == nullptr )continue; // to avoid FPE
	    TgcTrig tgcTrig;
	    tgcTrig.lb = GetEventInfo(ctx)->lumiBlock();
	    const int type = data->type();
	    const Amg::Vector3D &posIn = data->globalposIn();
	    tgcTrig.x_In = posIn[0];
	    tgcTrig.y_In = posIn[1];
	    tgcTrig.z_In = posIn[2];
	    const Amg::Vector3D &posOut = data->globalposOut();
	    tgcTrig.x_Out = posOut[0];
	    tgcTrig.y_Out = posOut[1];
	    tgcTrig.z_Out = posOut[2];
	    tgcTrig.eta = posOut.eta();
	    tgcTrig.phi = posOut.phi();
	    tgcTrig.width_In = data->widthIn();
	    tgcTrig.width_Out = data->widthOut();
	    if (type == Muon::TgcCoinData::TYPE_SL) {
	      const Amg::MatrixX &matrix = data->errMat();
	      tgcTrig.width_R = matrix(0, 0);
	      tgcTrig.width_Phi = matrix(1, 1);
	    } else {
	      tgcTrig.width_R = 0.;
	      tgcTrig.width_Phi = 0.;
	    }
	    int etaout = 0;
	    int etain = 0;
	    const Identifier tcdidout = data->channelIdOut();
	    if (tcdidout.is_valid()) {
	      etaout = std::abs(int(m_idHelperSvc->tgcIdHelper().stationEta(tcdidout)));
	    }
	    const Identifier tcdidin  = data->channelIdIn();
	    if (tcdidin.is_valid()) {
	      etain  = std::abs(int(m_idHelperSvc->tgcIdHelper().stationEta(tcdidin)));
	    }
	    tgcTrig.etaout = etaout;
	    tgcTrig.etain = etain;
	    tgcTrig.isAside = data->isAside();
	    tgcTrig.isForward = data->isForward();
	    tgcTrig.isStrip = data->isStrip();
	    tgcTrig.isInner = data->isInner();
	    tgcTrig.isPositiveDeltaR = data->isPositiveDeltaR();
	    tgcTrig.type = type;
	    tgcTrig.trackletId = data->trackletId();
	    tgcTrig.trackletIdStrip = data->trackletIdStrip();
	    tgcTrig.sector = (data->isAside())?(data->phi()):(-data->phi());
	    tgcTrig.roi = data->roi();
	    tgcTrig.pt = data->pt();
	    tgcTrig.delta = data->delta();
	    tgcTrig.sub = data->sub();
	    tgcTrig.veto = data->veto();
	    tgcTrig.bunch = bunch;
	    tgcTrig.inner = data->inner();
	    if (type == Muon::TgcCoinData::TYPE_SL && !tgcTrig.isForward) {
	      tgcTrigMap_SL_Endcap.push_back(tgcTrig);
	      tgcTrigMap_SL.push_back(tgcTrig);
	    }else if (type == Muon::TgcCoinData::TYPE_SL && tgcTrig.isForward) {
	      tgcTrigMap_SL_Forward.push_back(tgcTrig);
	      tgcTrigMap_SL.push_back(tgcTrig);
	    }else if(type == Muon::TgcCoinData::TYPE_HIPT && !tgcTrig.isForward){
	      if(tgcTrig.isStrip){
		tgcTrigMap_HPT_Endcap_Strip.push_back(tgcTrig);
		tgcTrigMap_HPT_Strip.push_back(tgcTrig);
	      }else{
		tgcTrigMap_HPT_Endcap_Wire.push_back(tgcTrig);
		tgcTrigMap_HPT_Wire.push_back(tgcTrig);
	      }
	    }else if(type == Muon::TgcCoinData::TYPE_HIPT && tgcTrig.isForward){
	      if(tgcTrig.isStrip){
		tgcTrigMap_HPT_Forward_Strip.push_back(tgcTrig);
		tgcTrigMap_HPT_Strip.push_back(tgcTrig);
	      }else{
		tgcTrigMap_HPT_Forward_Wire.push_back(tgcTrig);
		tgcTrigMap_HPT_Wire.push_back(tgcTrig);
	      }
	    }else if(type == Muon::TgcCoinData::TYPE_TRACKLET && !tgcTrig.isForward){
	      if(tgcTrig.isStrip){
		tgcTrigMap_LPT_Endcap_Strip.push_back(tgcTrig);
		tgcTrigMap_LPT_Strip.push_back(tgcTrig);
	      }else{
		tgcTrigMap_LPT_Endcap_Wire.push_back(tgcTrig);
		tgcTrigMap_LPT_Wire.push_back(tgcTrig);
	      }
	    }else if(type == Muon::TgcCoinData::TYPE_TRACKLET && tgcTrig.isForward){
	      if(tgcTrig.isStrip){
		tgcTrigMap_LPT_Forward_Strip.push_back(tgcTrig);
		tgcTrigMap_LPT_Strip.push_back(tgcTrig);
	      }else{
		tgcTrigMap_LPT_Forward_Wire.push_back(tgcTrig);
		tgcTrigMap_LPT_Wire.push_back(tgcTrig);
	      }
	    }
	  }
	}
      }
      
      MonVariables  tgcCoin_variables;
      tgcCoin_variables.push_back(mon_bcid);
      tgcCoin_variables.push_back(mon_pileup);
      tgcCoin_variables.push_back(mon_lb);
      
      auto mon_nTgcCoin_detElementIsNull = Monitored::Scalar<int>("nTgcCoinDetElementIsNull", n_TgcCoin_detElementIsNull);
      auto mon_nTgcCoin_postOutPtrIsNull = Monitored::Scalar<int>("nTgcCoinPostOutPtrIsNull", n_TgcCoin_postOutPtrIsNull);
      tgcCoin_variables.push_back(mon_nTgcCoin_detElementIsNull);
      tgcCoin_variables.push_back(mon_nTgcCoin_postOutPtrIsNull);

      std::vector<Monitored::ObjectsCollection<std::vector<TgcTrig>, double>> vo_coin;
      vo_coin.reserve(26 * 15);

      fillTgcCoin("SL",tgcTrigMap_SL,vo_coin,tgcCoin_variables);
      fillTgcCoin("SL_Endcap",tgcTrigMap_SL_Endcap,vo_coin,tgcCoin_variables);
      fillTgcCoin("SL_Forward",tgcTrigMap_SL_Forward,vo_coin,tgcCoin_variables);
      fillTgcCoin("HPT_Wire",tgcTrigMap_HPT_Wire,vo_coin,tgcCoin_variables);
      fillTgcCoin("HPT_Endcap_Wire",tgcTrigMap_HPT_Endcap_Wire,vo_coin,tgcCoin_variables);
      fillTgcCoin("HPT_Forward_Wire",tgcTrigMap_HPT_Forward_Wire,vo_coin,tgcCoin_variables);
      fillTgcCoin("HPT_Strip",tgcTrigMap_HPT_Strip,vo_coin,tgcCoin_variables);
      fillTgcCoin("HPT_Endcap_Strip",tgcTrigMap_HPT_Endcap_Strip,vo_coin,tgcCoin_variables);
      fillTgcCoin("HPT_Forward_Strip",tgcTrigMap_HPT_Forward_Strip,vo_coin,tgcCoin_variables);
      fillTgcCoin("LPT_Wire",tgcTrigMap_LPT_Wire,vo_coin,tgcCoin_variables);
      fillTgcCoin("LPT_Endcap_Wire",tgcTrigMap_LPT_Endcap_Wire,vo_coin,tgcCoin_variables);
      fillTgcCoin("LPT_Forward_Wire",tgcTrigMap_LPT_Forward_Wire,vo_coin,tgcCoin_variables);
      fillTgcCoin("LPT_Strip",tgcTrigMap_LPT_Strip,vo_coin,tgcCoin_variables);
      fillTgcCoin("LPT_Endcap_Strip",tgcTrigMap_LPT_Endcap_Strip,vo_coin,tgcCoin_variables);
      fillTgcCoin("LPT_Forward_Strip",tgcTrigMap_LPT_Forward_Strip,vo_coin,tgcCoin_variables);
      
      std::vector<Monitored::ObjectsCollection<std::vector<double>, double>> vo_double;
      vo_double.reserve(3 * 5);
      std::vector<double> extEta_SL,extPhi_SL,extMatched_SL;
      std::vector<double> extEta_HPT_Wire,extPhi_HPT_Wire,extMatched_HPT_Wire;
      std::vector<double> extEta_HPT_Strip,extPhi_HPT_Strip,extMatched_HPT_Strip;
      std::vector<double> extEta_LPT_Wire,extPhi_LPT_Wire,extMatched_LPT_Wire;
      std::vector<double> extEta_LPT_Strip,extPhi_LPT_Strip,extMatched_LPT_Strip;
      fillTgcCoinEff("SL",tgcTrigMap_SL,extpositions_pivot,extEta_SL,extPhi_SL,extMatched_SL,vo_double,tgcCoin_variables);
      fillTgcCoinEff("HPT_Wire",tgcTrigMap_HPT_Wire,extpositions_pivot,extEta_HPT_Wire,extPhi_HPT_Wire,extMatched_HPT_Wire,vo_double,tgcCoin_variables);
      fillTgcCoinEff("HPT_Strip",tgcTrigMap_HPT_Strip,extpositions_pivot,extEta_HPT_Strip,extPhi_HPT_Strip,extMatched_HPT_Strip,vo_double,tgcCoin_variables);
      fillTgcCoinEff("LPT_Wire",tgcTrigMap_LPT_Wire,extpositions_pivot,extEta_LPT_Wire,extPhi_LPT_Wire,extMatched_LPT_Wire,vo_double,tgcCoin_variables);
      fillTgcCoinEff("LPT_Strip",tgcTrigMap_LPT_Strip,extpositions_pivot,extEta_LPT_Strip,extPhi_LPT_Strip,extMatched_LPT_Strip,vo_double,tgcCoin_variables);
      
      fill(m_packageName+"_TgcCoin", tgcCoin_variables);

      ATH_MSG_DEBUG("End filling TGC CoinData histograms");
    }
  }

  ///////////////// End filling TGC CoinData histograms /////////////////
  ATH_MSG_DEBUG("Done fillHistograms()");
  return StatusCode::SUCCESS;
}

void TgcRawDataMonitorAlgorithm::fillTgcCoin(const std::string & type,
					     const std::vector<TgcTrig>& tgcTrigs, 
					     std::vector<Monitored::ObjectsCollection<std::vector<TgcTrig>, double>>& varowner,
					     MonVariables& variables) const {
  varowner.push_back(Monitored::Collection(type+"_coin_lb",tgcTrigs,[](const TgcTrig&m){return m.lb;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_eta",tgcTrigs,[](const TgcTrig&m){return m.eta;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_phi",tgcTrigs,[](const TgcTrig&m){return m.phi + tgc_coin_phi_small_offset;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_bunch",tgcTrigs,[](const TgcTrig&m){return m.bunch;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_roi",tgcTrigs,[](const TgcTrig&m){return m.roi;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_sector",tgcTrigs,[](const TgcTrig&m){return m.sector;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_pt",tgcTrigs,[](const TgcTrig&m){return m.pt;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_veto",tgcTrigs,[](const TgcTrig&m){return m.veto;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_veto_sector",tgcTrigs,[](const TgcTrig&m){return (m.veto==1)?(m.sector):(-1);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_veto_roi",tgcTrigs,[](const TgcTrig&m){return (m.veto==1)?(m.roi):(-1);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_isPositiveDeltaR",tgcTrigs,[](const TgcTrig&m){return m.isPositiveDeltaR;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt1",tgcTrigs,[](const TgcTrig&m){return (m.pt==1);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt2",tgcTrigs,[](const TgcTrig&m){return (m.pt==2);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt3",tgcTrigs,[](const TgcTrig&m){return (m.pt==3);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt4",tgcTrigs,[](const TgcTrig&m){return (m.pt==4);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt5",tgcTrigs,[](const TgcTrig&m){return (m.pt==5);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt6",tgcTrigs,[](const TgcTrig&m){return (m.pt==6);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt7",tgcTrigs,[](const TgcTrig&m){return (m.pt==7);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt8",tgcTrigs,[](const TgcTrig&m){return (m.pt==8);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt9",tgcTrigs,[](const TgcTrig&m){return (m.pt==9);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt10",tgcTrigs,[](const TgcTrig&m){return (m.pt==10);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt11",tgcTrigs,[](const TgcTrig&m){return (m.pt==11);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt12",tgcTrigs,[](const TgcTrig&m){return (m.pt==12);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt13",tgcTrigs,[](const TgcTrig&m){return (m.pt==13);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt14",tgcTrigs,[](const TgcTrig&m){return (m.pt==14);}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_cutmask_pt15",tgcTrigs,[](const TgcTrig&m){return (m.pt==15);}));
  variables.push_back(varowner.back());
}
void TgcRawDataMonitorAlgorithm::fillTgcCoinEff(const std::string & type,
						const std::vector<TgcTrig>& tgcTrigs, 
						const std::vector<ExtPos>& extpositions_pivot,
						std::vector<double>& extEta,std::vector<double>& extPhi,std::vector<double>& extMatched,
						std::vector<Monitored::ObjectsCollection<std::vector<double>, double>>& varowner,
						MonVariables& variables) const {
  extEta.clear();
  extPhi.clear();
  extMatched.clear();
  for(const auto& ext : extpositions_pivot){
    double matched = 0;
    for(const auto& tgcTrig : tgcTrigs){
      if(tgcTrig.bunch!=0)continue; // only the current bunch
      if(tgcTrig.isAside==1 && ext.extPos.z()<0)continue;
      if(tgcTrig.isAside==0 && ext.extPos.z()>0)continue;
      TVector2 vec(tgcTrig.x_Out,tgcTrig.y_Out);
      double deltaPhi = vec.DeltaPhi( TVector2(ext.extPos.x(), ext.extPos.y()) );
      double deltaR = vec.Mod() - TVector2(ext.extPos.x(), ext.extPos.y()).Mod();
      if( std::abs(deltaPhi) > m_dPhiCutOnM3 || std::abs(deltaR) > m_dRCutOnM3 )continue;
      matched = 1;
      break;
    }
    extEta.push_back(ext.extPos.eta());
    extPhi.push_back(ext.extPos.phi());
    extMatched.push_back(matched);
  }
  varowner.push_back(Monitored::Collection(type+"_coin_ext_matched",extMatched,[](const double&m){return m;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_ext_eta",extEta,[](const double&m){return m;}));
  variables.push_back(varowner.back());
  varowner.push_back(Monitored::Collection(type+"_coin_ext_phi",extPhi,[](const double&m){return m;}));
  variables.push_back(varowner.back());
}
