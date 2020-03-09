/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRawDataMonitorAlgorithm.h"
#include "TObjArray.h"


TgcRawDataMonitorAlgorithm::TgcRawDataMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_packageName("TgcRawDataMonitor"),
    m_MuonContainerKey("Muons"),
    m_MuonEFContainerKey("HLT_MuonsCB_RoI"),
    m_MuonRoIContainerKey("LVL1MuonRoIs"),
    m_trigTagList("HLT_mu26_ivarmedium_L1MU20"),
    m_TagAndProbe(false)
{
  declareProperty("PackageName",m_packageName);  
  declareProperty("MuonContainerName",m_MuonContainerKey);
  declareProperty("MuonEFContainerName",m_MuonEFContainerKey);
  declareProperty("MuonRoIContainerName",m_MuonRoIContainerKey);
  declareProperty("TagTrigList",m_trigTagList);
  declareProperty("TagAndProbe",m_TagAndProbe);
}


TgcRawDataMonitorAlgorithm::~TgcRawDataMonitorAlgorithm() {}


StatusCode TgcRawDataMonitorAlgorithm::initialize() {
  
  ATH_CHECK(m_MuonContainerKey.initialize());
  ATH_CHECK(m_MuonEFContainerKey.initialize());
  ATH_CHECK(m_MuonRoIContainerKey.initialize());
  
  TObjArray* tagList = TString(m_trigTagList).Tokenize(",");
  std::set<TString> alllist;
  for(int i = 0 ; i < tagList->GetEntries(); i++){
    TString tagTrig = tagList->At(i)->GetName();
    if(alllist.find(tagTrig)!=alllist.end())continue;
    alllist.insert(tagTrig);
    TObjArray* arr = tagTrig.Tokenize(";");
    if(arr->GetEntries()==0)continue;
    TagDef def;
    def.eventTrig = TString(arr->At(0)->GetName());
    TString matchTrig = def.eventTrig;
    if(arr->GetEntries()==2)matchTrig = TString(arr->At(1)->GetName());
    def.isolated = matchTrig.Contains("ivarmedium");
    matchTrig.ReplaceAll("HLT_mu","");
    if(matchTrig.Contains("_")) matchTrig.Remove(matchTrig.Index("_"),matchTrig.Sizeof());
    def.threshold = matchTrig.Atof() * 1000.;
    m_trigTagDefs.push_back(def);
  }
  
  m_Groups_trig2bcid = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiEta2Phi = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiEta = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiPhi_barrel_A = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiPhi_endcap_A = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiPhi_forward_A = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiPhi_barrel_C = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiPhi_endcap_C = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_roiPhi_forward_C = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muEta2Phi = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muEta = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPhi_endcap_A = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPhi_forward_A = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPhi_barrel_C = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPhi_endcap_C = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPhi_forward_C = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPt_barrel = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPt_endcap = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPt_forward = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  m_Groups_muPhi_barrel_A = Monitored::buildToolMap<int>(m_tools,m_packageName,16);
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode TgcRawDataMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  bool ok = true;
  SG::ReadHandle<xAOD::MuonRoIContainer> rois(m_MuonRoIContainerKey, ctx);
  if(!rois.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain muon RoI Collection with name "<< m_MuonRoIContainerKey);
    ok = false;
  }
  if(!ok){
    return StatusCode::FAILURE;
  }

  auto thrNumber_barrel = Monitored::Scalar<float>("thrNumber_barrel",0.0);
  auto thrNumber_endcap = Monitored::Scalar<float>("thrNumber_endcap",0.0);
  auto thrNumber_forward = Monitored::Scalar<float>("thrNumber_forward",0.0);

  auto roiEta = Monitored::Scalar<float>("roiEta",0.0);
  auto roiEta2 = Monitored::Scalar<float>("roiEta2",0.0);
  auto roiPhi2 = Monitored::Scalar<float>("roiPhi2",0.0);
  auto roiPhi_barrel_A = Monitored::Scalar<float>("roiPhi_barrel_A",0.0);
  auto roiPhi_endcap_A = Monitored::Scalar<float>("roiPhi_endcap_A",0.0);
  auto roiPhi_forward_A = Monitored::Scalar<float>("roiPhi_forward_A",0.0);
  auto roiPhi_barrel_C = Monitored::Scalar<float>("roiPhi_barrel_C",0.0);
  auto roiPhi_endcap_C = Monitored::Scalar<float>("roiPhi_endcap_C",0.0);
  auto roiPhi_forward_C = Monitored::Scalar<float>("roiPhi_forward_C",0.0);

  auto bcID_barrel = Monitored::Scalar<float>("bcID_barrel",0.0);
  auto bcID_endcap = Monitored::Scalar<float>("bcID_endcap",0.0);
  auto bcID_forward = Monitored::Scalar<float>("bcID_forward",0.0);

  auto bcid = GetEventInfo(ctx)->bcid();
  
  for(const auto& roi : *rois){
    roiEta = roi->eta();
    roiEta2 = roi->eta();
    roiPhi2 = roi->phi();
    fill(m_tools[m_Groups_roiEta2Phi[roi->getThrNumber()]],roiEta2,roiPhi2);
    fill(m_tools[m_Groups_roiEta[roi->getThrNumber()]],roiEta);

    if(roi->getSource()==xAOD::MuonRoI::Barrel){
      thrNumber_barrel = roi->getThrNumber();
      fill(m_packageName,thrNumber_barrel);
      bcID_barrel = bcid;
      fill(m_tools[m_Groups_trig2bcid[roi->getThrNumber()]],bcID_barrel);
      if(roi->getHemisphere()==xAOD::MuonRoI::Positive){
	roiPhi_barrel_A = roi->phi();
	fill(m_tools[m_Groups_roiPhi_barrel_A[roi->getThrNumber()]],roiPhi_barrel_A);
      }else{
	roiPhi_barrel_C = roi->phi();
	fill(m_tools[m_Groups_roiPhi_barrel_C[roi->getThrNumber()]],roiPhi_barrel_C);
      }
    }
    if(roi->getSource()==xAOD::MuonRoI::Endcap){
      thrNumber_endcap = roi->getThrNumber();
      fill(m_packageName,thrNumber_endcap);
      bcID_endcap = bcid;
      fill(m_tools[m_Groups_trig2bcid[roi->getThrNumber()]],bcID_endcap);
      if(roi->getHemisphere()==xAOD::MuonRoI::Positive){
	roiPhi_endcap_A = roi->phi();
	fill(m_tools[m_Groups_roiPhi_endcap_A[roi->getThrNumber()]],roiPhi_endcap_A);
      }else{
	roiPhi_endcap_C = roi->phi();
	fill(m_tools[m_Groups_roiPhi_endcap_C[roi->getThrNumber()]],roiPhi_endcap_C);
      }
    }
    if(roi->getSource()==xAOD::MuonRoI::Forward){
      thrNumber_forward = roi->getThrNumber();
      fill(m_packageName,thrNumber_forward);
      bcID_forward = bcid;
      fill(m_tools[m_Groups_trig2bcid[roi->getThrNumber()]],bcID_forward);
      if(roi->getHemisphere()==xAOD::MuonRoI::Positive){
	roiPhi_forward_A = roi->phi();
	fill(m_tools[m_Groups_roiPhi_forward_A[roi->getThrNumber()]],roiPhi_forward_A);
      }else{
	roiPhi_forward_C = roi->phi();
	fill(m_tools[m_Groups_roiPhi_forward_C[roi->getThrNumber()]],roiPhi_forward_C);
      }
    }

  }

  
  if( !m_TagAndProbe ) return StatusCode::SUCCESS;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if(!muons.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    ok = false;
  }
  SG::ReadHandle<xAOD::MuonContainer> efmuons(m_MuonEFContainerKey, ctx);
  if(!efmuons.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonEFContainerKey);
    ok = false;
  }
  if(!ok){
    return StatusCode::FAILURE;
  }
  
  std::vector<Muons> mymuons;
  for(const auto& muon : *muons){
    if(muon->author()!=xAOD::Muon::MuidCo && muon->author()!=xAOD::Muon::STACO)continue;
    if(muon->muonType()!=xAOD::Muon::Combined)continue;
    if(muon->quality()!=xAOD::Muon::Tight && muon->quality()!=xAOD::Muon::Medium)continue;
    Muons mymuon;
    mymuon.muon = muon;
    mymuon.tagged = false;
    mymuon.isolated = true;
    TVector3 muonvec;
    muonvec.SetPtEtaPhi(muon->pt(),muon->eta(),muon->phi());
    for(auto tagdef : m_trigTagDefs){
      if(!m_trigDecTool->isPassed(tagdef.eventTrig.Data()))continue;
      for(const auto& efmuon : *efmuons){
	if(efmuon->pt()<tagdef.threshold)continue;
	if(tagdef.isolated){
	  float ptcone30 = -1;
	  muon->isolation(ptcone30, xAOD::Iso::IsolationType::ptvarcone30);
	  float isolation = ptcone30 / muon->pt();
	  if( isolation>0.07 )continue;
	}
	TVector3 efmuonvec;
	efmuonvec.SetPtEtaPhi(efmuon->pt(),efmuon->eta(),efmuon->phi());
	if( efmuonvec.DeltaR(muonvec) < 0.05 ){
	  mymuon.tagged = true;
	  break;
	}
      }
      if(mymuon.tagged)break;
    }
    for(const auto& muon2 : *muons){
      if( muon == muon2 )continue;
      TVector3 muonvec2;
      muonvec2.SetPtEtaPhi(muon2->pt(),muon2->eta(),muon2->phi());
      float dr = muonvec2.DeltaR( muonvec );
      if( dr < 0.5 ){
	mymuon.isolated = false;
	break;
      }
    }
    mymuons.push_back(mymuon);
  }

  auto muEta = Monitored::Scalar<float>("muEta",0.0);
  auto muEta2 = Monitored::Scalar<float>("muEta2",0.0);
  auto muPhi2 = Monitored::Scalar<float>("muPhi2",0.0);
  auto muPhi_barrel_A = Monitored::Scalar<float>("muPhi_barrel_A",0.0);
  auto muPhi_endcap_A = Monitored::Scalar<float>("muPhi_endcap_A",0.0);
  auto muPhi_forward_A = Monitored::Scalar<float>("muPhi_forward_A",0.0);
  auto muPhi_barrel_C = Monitored::Scalar<float>("muPhi_barrel_C",0.0);
  auto muPhi_endcap_C = Monitored::Scalar<float>("muPhi_endcap_C",0.0);
  auto muPhi_forward_C = Monitored::Scalar<float>("muPhi_forward_C",0.0);
  auto muPt_barrel = Monitored::Scalar<float>("muPt_barrel",0.0);
  auto muPt_endcap = Monitored::Scalar<float>("muPt_endcap",0.0);
  auto muPt_forward = Monitored::Scalar<float>("muPt_forward",0.0);
  
  for(auto mymuon : mymuons){

    bool canbeprobe = false;
    for(auto mymuon2 : mymuons){
      if(mymuon.muon == mymuon2.muon)continue;
      if(mymuon2.tagged){
	canbeprobe = true;
	break;
      }
    }
    if(!canbeprobe)continue;
    if(!mymuon.isolated)continue;
    
    // Added by M.Aoki (8/10/2019)
    // pT-dependent matching window is introduced.
    // This is an optimal selection for the matching
    // between L1MU RoI and offline muons.
    // Note: This will be re-visited once the muon
    //       track extrapolator is implemented,
    //       where no pT-dependent window is needed
    float max_dr = 999;
    float pt = mymuon.muon->pt();
    if( pt/1000.>15.)max_dr = 0.15;
    else if(pt/1000.>10.)max_dr = 0.3 - 0.01 * pt/1000.;
    else max_dr = 0.36 - 0.0016 * pt/1000.;

    float muPt = mymuon.muon->pt()/1000.;

    TVector3 muonvec;
    muonvec.SetPtEtaPhi(mymuon.muon->pt(),mymuon.muon->eta(),mymuon.muon->phi());

    std::set<int> matchedThr;
    for(const auto& roi : *rois){
      TVector3 roivec;
      roivec.SetPtEtaPhi(1,roi->eta(),roi->phi());
      float dr = roivec.DeltaR( muonvec );
      if(dr < max_dr){
	matchedThr.insert( roi->getThrNumber() );
      }
    }

    for(int i = 0 ; i <= 15 ; i++){
      bool matched = false;
      if(i==0){
	matched = true;
      }else{
	for(int j = i ; j <= 15 ; j++){
	  if( matchedThr.find(j)!=matchedThr.end() ){
	    matched = true;
	    break;
	  }
	}
      }
      if(!matched)continue;
      if(muPt>25.){
	muEta = mymuon.muon->eta();
	muEta2 = mymuon.muon->eta();
	muPhi2 = mymuon.muon->phi();
	fill(m_tools[m_Groups_muEta2Phi[i]],muEta2,muPhi2);
	fill(m_tools[m_Groups_muEta[i]],muEta);
      }
      if(TMath::Abs(mymuon.muon->eta())<1.05){
	muPt_barrel = muPt;
	fill(m_tools[m_Groups_muPt_barrel[i]],muPt_barrel);
	if(mymuon.muon->eta()>0){
	  muPhi_barrel_A = mymuon.muon->phi();
	  fill(m_tools[m_Groups_muPhi_barrel_A[i]],muPhi_barrel_A);
	}else{
	  muPhi_barrel_C = mymuon.muon->phi();
	  fill(m_tools[m_Groups_muPhi_barrel_C[i]],muPhi_barrel_C);
	}
      }else if(TMath::Abs(mymuon.muon->eta())<2.0){
	muPt_endcap = muPt;
	fill(m_tools[m_Groups_muPt_endcap[i]],muPt_endcap);
	if(mymuon.muon->eta()>0){
	  muPhi_endcap_A = mymuon.muon->phi();
	  fill(m_tools[m_Groups_muPhi_endcap_A[i]],muPhi_endcap_A);
	}else{
	  muPhi_endcap_C = mymuon.muon->phi();
	  fill(m_tools[m_Groups_muPhi_endcap_C[i]],muPhi_endcap_C);
	}
      }else if(TMath::Abs(mymuon.muon->eta())<2.4){
	muPt_forward = muPt;
	fill(m_tools[m_Groups_muPt_forward[i]],muPt_forward);
	if(mymuon.muon->eta()>0){
	  muPhi_forward_A = mymuon.muon->phi();
	  fill(m_tools[m_Groups_muPhi_forward_A[i]],muPhi_forward_A);
	}else{
	  muPhi_forward_C = mymuon.muon->phi();
	  fill(m_tools[m_Groups_muPhi_forward_C[i]],muPhi_forward_C);
	}
      }
    }

  }
  
  return StatusCode::SUCCESS;
}
