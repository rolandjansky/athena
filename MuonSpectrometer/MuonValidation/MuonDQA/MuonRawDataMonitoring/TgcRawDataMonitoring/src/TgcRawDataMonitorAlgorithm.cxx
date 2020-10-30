/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRawDataMonitorAlgorithm.h"

#include "TObjArray.h"

TgcRawDataMonitorAlgorithm::TgcRawDataMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode TgcRawDataMonitorAlgorithm::initialize() {
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_extrapolator.retrieve()); 
  ATH_CHECK(m_MuonContainerKey.initialize());
  ATH_CHECK(m_MuonRoIContainerKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_TgcPrepDataContainerKey.initialize());
  ATH_CHECK(m_TgcCoinDataContainerCurrBCKey.initialize());
  ATH_CHECK(m_TgcCoinDataContainerNextBCKey.initialize());
  ATH_CHECK(m_TgcCoinDataContainerPrevBCKey.initialize());

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
  
  TObjArray* tagList = TString(m_trigTagList.value()).Tokenize(",");
  std::set<TString> alllist;
  for(int i = 0 ; i < tagList->GetEntries(); i++){
    TString tagTrig = tagList->At(i)->GetName();
    if(alllist.find(tagTrig)!=alllist.end())continue;
    alllist.insert(tagTrig);
    TObjArray* arr = tagTrig.Tokenize(";");
    if(arr->GetEntries()==0)continue;
    TagDef def;
    def.eventTrig = TString(arr->At(0)->GetName());
    def.tagTrig = def.eventTrig;
    if(arr->GetEntries()==2)def.tagTrig = TString(arr->At(1)->GetName());
    m_trigTagDefs.push_back(def);
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TgcRawDataMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;
  
  auto bcid = Monitored::Scalar<int>("bcid",GetEventInfo(ctx)->bcid());
  auto lumiPerBCID = Monitored::Scalar<int>("lumiPerBCID",lbAverageInteractionsPerCrossing(ctx));
  auto lb = Monitored::Scalar<int>("lb",GetEventInfo(ctx)->lumiBlock());
  auto run = Monitored::Scalar<int>("run",GetEventInfo(ctx)->runNumber());

  variables.push_back(bcid);
  variables.push_back(lumiPerBCID);
  variables.push_back(lb);
  variables.push_back(run);


  if( !m_anaMuonRoI.value()){
    fill(m_packageName,variables);
    variables.clear();
    return StatusCode::SUCCESS;
  }

  xAOD::MuonRoIContainer rois;
  if (!m_MuonRoIContainerKey.empty()) {
    /* raw LVL1MuonRoIs distributions */
    auto roisptr = SG::get<xAOD::MuonRoIContainer>(m_MuonRoIContainerKey, ctx);
    if(! roisptr){
      ATH_MSG_ERROR("evtStore() does not contain muon RoI Collection with name "<< m_MuonRoIContainerKey);
      return StatusCode::FAILURE;
    }
    rois = *roisptr;
  }
  
  auto roi_eta = Monitored::Collection("roi_eta",rois, []( const xAOD::MuonRoI* m ){return m->eta();});  variables.push_back( roi_eta );
  auto roi_phi = Monitored::Collection("roi_phi",rois, []( const xAOD::MuonRoI* m ){return m->phi();});  variables.push_back( roi_phi );
  auto roi_phi_rpc = Monitored::Collection("roi_phi_rpc",rois, []( const xAOD::MuonRoI* m ){return (m->getSource()==xAOD::MuonRoI::Barrel)?m->phi():-10;});  variables.push_back( roi_phi_rpc );
  auto roi_phi_tgc = Monitored::Collection("roi_phi_tgc",rois, []( const xAOD::MuonRoI* m ){return (m->getSource()!=xAOD::MuonRoI::Barrel)?m->phi():-10;});  variables.push_back( roi_phi_tgc );
  auto roi_thr = Monitored::Collection("roi_thr",rois, []( const xAOD::MuonRoI* m ){return m->getThrNumber();});  variables.push_back( roi_thr );
  auto roi_rpc = Monitored::Collection("roi_rpc",rois, []( const xAOD::MuonRoI* m ){return m->getSource()==xAOD::MuonRoI::Barrel;});  variables.push_back( roi_rpc );
  auto roi_tgc = Monitored::Collection("roi_tgc",rois, []( const xAOD::MuonRoI* m ){return m->getSource()!=xAOD::MuonRoI::Barrel;});  variables.push_back( roi_tgc );

  auto roi_barrel = Monitored::Collection("roi_barrel",rois, []( const xAOD::MuonRoI* m ){return m->getSource()==xAOD::MuonRoI::Barrel;});  variables.push_back( roi_barrel );
  auto roi_endcap = Monitored::Collection("roi_endcap",rois, []( const xAOD::MuonRoI* m ){return m->getSource()==xAOD::MuonRoI::Endcap;});  variables.push_back( roi_endcap );
  auto roi_forward = Monitored::Collection("roi_forward",rois, []( const xAOD::MuonRoI* m ){return m->getSource()==xAOD::MuonRoI::Forward;});  variables.push_back( roi_forward );

  auto roi_phi_barrel = Monitored::Collection("roi_phi_barrel",rois, []( const xAOD::MuonRoI* m ){return (m->getSource()==xAOD::MuonRoI::Barrel)?m->phi():-10;});  variables.push_back( roi_phi_barrel );
  auto roi_phi_endcap = Monitored::Collection("roi_phi_endcap",rois, []( const xAOD::MuonRoI* m ){return (m->getSource()==xAOD::MuonRoI::Endcap)?m->phi():-10;});  variables.push_back( roi_phi_endcap );
  auto roi_phi_forward = Monitored::Collection("roi_phi_forward",rois, []( const xAOD::MuonRoI* m ){return (m->getSource()==xAOD::MuonRoI::Forward)?m->phi():-10;});  variables.push_back( roi_phi_forward );

  auto roi_sideA = Monitored::Collection("roi_sideA",rois, []( const xAOD::MuonRoI* m ){return m->getHemisphere()==xAOD::MuonRoI::Positive;});  variables.push_back( roi_sideA );
  auto roi_sideC = Monitored::Collection("roi_sideC",rois, []( const xAOD::MuonRoI* m ){return m->getHemisphere()==xAOD::MuonRoI::Negative;});  variables.push_back( roi_sideC );

  auto thrmask1 = Monitored::Collection("thrmask1",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==1;});variables.push_back(thrmask1);
  auto thrmask2 = Monitored::Collection("thrmask2",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==2;});variables.push_back(thrmask2);
  auto thrmask3 = Monitored::Collection("thrmask3",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==3;});variables.push_back(thrmask3);
  auto thrmask4 = Monitored::Collection("thrmask4",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==4;});variables.push_back(thrmask4);
  auto thrmask5 = Monitored::Collection("thrmask5",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==5;});variables.push_back(thrmask5);
  auto thrmask6 = Monitored::Collection("thrmask6",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==6;});variables.push_back(thrmask6);
  auto thrmask7 = Monitored::Collection("thrmask7",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==7;});variables.push_back(thrmask7);
  auto thrmask8 = Monitored::Collection("thrmask8",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==8;});variables.push_back(thrmask8);
  auto thrmask9 = Monitored::Collection("thrmask9",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==9;});variables.push_back(thrmask9);
  auto thrmask10 = Monitored::Collection("thrmask10",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==10;});variables.push_back(thrmask10);
  auto thrmask11 = Monitored::Collection("thrmask11",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==11;});variables.push_back(thrmask11);
  auto thrmask12 = Monitored::Collection("thrmask12",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==12;});variables.push_back(thrmask12);
  auto thrmask13 = Monitored::Collection("thrmask13",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==13;});variables.push_back(thrmask13);
  auto thrmask14 = Monitored::Collection("thrmask14",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==14;});variables.push_back(thrmask14);
  auto thrmask15 = Monitored::Collection("thrmask15",rois,[](const xAOD::MuonRoI* m){return m->getThrNumber()==15;});variables.push_back(thrmask15);

  if( !m_anaOfflMuon.value() ){
    fill(m_packageName,variables);
    variables.clear();
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if(!muons.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE; 
  }

  std::vector<MyMuon> mymuons;
  for(const auto& muon : *muons){
    if(muon->muonType()!=xAOD::Muon::Combined)continue;
    if(muon->author()!=xAOD::Muon::MuidCo && muon->author()!=xAOD::Muon::STACO)continue;
    if(muon->quality()!=xAOD::Muon::Tight && muon->quality()!=xAOD::Muon::Medium)continue;

    MyMuon mymuon;
    /* fill basic info */
    mymuon.muon = muon;
    mymuon.fourvec.SetPtEtaPhiM(muon->pt(),muon->eta(),muon->phi(),m_muonMass.value());
    /* fill tag of tag-and-probe info */
    mymuon.tagged = triggerMatching(muon,m_trigTagDefs)==StatusCode::SUCCESS ? true : false;
    /* fill info of isolation among muons */
    mymuon.isolated = true;
    mymuon.probeOK = false;
    if(!m_TagAndProbe.value()) mymuon.probeOK = true;
    for(const auto& muon2 : *muons){
      if( muon == muon2 )continue;

      TLorentzVector muonvec2;
      muonvec2.SetPtEtaPhiM(muon2->pt(),muon2->eta(),muon2->phi(),m_muonMass.value());
      float dr = muonvec2.DeltaR( mymuon.fourvec );
      if( dr < m_isolationWindow.value() ){
  	mymuon.isolated = false;
      }

      if( muon2->muonType()!=xAOD::Muon::Combined )continue;
      if( muon2->author()!=xAOD::Muon::MuidCo && muon2->author()!=xAOD::Muon::STACO )continue;
      if( muon2->quality()!=xAOD::Muon::Tight && muon2->quality()!=xAOD::Muon::Medium )continue;
      if( triggerMatching(muon2,m_trigTagDefs)!=StatusCode::SUCCESS )continue;
      if(!m_TagAndProbeZmumu.value()){
	mymuon.probeOK=true;
      }else{
	if( muon->charge() == muon2->charge() )continue;
	double dimuon_mass = (muonvec2 + mymuon.fourvec).M();
	if(std::abs( dimuon_mass - m_zMass.value()) > m_zMassWindow.value() )continue;
	mymuon.probeOK=true;
      }

    }
    /* fill extrapolation info (only to TGC) */
    extrapolate( muon, mymuon );
    /* L1Muon RoI matching */
    float max_dr = 999;
    float pt = mymuon.muon->pt();
    if( pt/1000.>15.)max_dr = m_l1trigMatchWindow1.value();
    else if(pt/1000.>10.)max_dr = m_l1trigMatchWindow2.value() + m_l1trigMatchWindow3.value() * pt/1000.;
    else max_dr = m_l1trigMatchWindow4.value() + m_l1trigMatchWindow5.value() * pt/1000.;
    for(const auto& roi : rois){
      TLorentzVector roivec;
      roivec.SetPtEtaPhiM(1,roi->eta(),roi->phi(),1);
      double dr = roivec.DeltaR( mymuon.fourvec );
      if( dr < max_dr ){
	mymuon.matchedL1ThrExclusive.insert( roi->getThrNumber() );
      }
    }
    for(int ithr = 0 ; ithr < 16 ; ithr++){
      bool pass = false;
      for(const auto thr : mymuon.matchedL1ThrExclusive){
	if(thr >= ithr){
	  pass = true;
	  break;
	}
      }
      if(pass) mymuon.matchedL1ThrInclusive.insert(ithr);
    }

    /* store MyMuon */
    mymuons.push_back( mymuon );
  }

  
  auto muon_eta = Monitored::Collection("muon_eta",mymuons,[](const MyMuon& m){return (m.probeOK&&m.muon->pt()/1000>30)?m.muon->eta():-10;});variables.push_back(muon_eta);
  auto muon_phi = Monitored::Collection("muon_phi",mymuons,[](const MyMuon& m){return (m.probeOK&&m.muon->pt()/1000>30)?m.muon->phi():-10;});variables.push_back(muon_phi);
  auto muon_phi_rpc = Monitored::Collection("muon_phi_rpc",mymuons,[](const MyMuon& m){return (m.probeOK&&std::abs(m.muon->eta())<1.05&&m.muon->pt()/1000>30)?m.muon->phi():-10;});variables.push_back(muon_phi_rpc);
  auto muon_phi_tgc = Monitored::Collection("muon_phi_tgc",mymuons,[](const MyMuon& m){return (m.probeOK&&std::abs(m.muon->eta())>1.05&&std::abs(m.muon->eta())<2.4&&m.muon->pt()/1000>30)?m.muon->phi():-10;});variables.push_back(muon_phi_tgc);
  auto muon_pt_rpc = Monitored::Collection("muon_pt_rpc",mymuons,[](const MyMuon& m){return (m.probeOK&&std::abs(m.muon->eta())<1.05)?m.muon->pt()/1000:-10;});variables.push_back(muon_pt_rpc);
  auto muon_pt_tgc = Monitored::Collection("muon_pt_tgc",mymuons,[](const MyMuon& m){return (m.probeOK&&std::abs(m.muon->eta())>1.05&&std::abs(m.muon->eta())<2.4)?m.muon->pt()/1000:-10;});variables.push_back(muon_pt_tgc);
  auto muon_l1passThr1 = Monitored::Collection("muon_l1passThr1",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(1)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr1);
  auto muon_l1passThr2 = Monitored::Collection("muon_l1passThr2",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(2)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr2);
  auto muon_l1passThr3 = Monitored::Collection("muon_l1passThr3",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(3)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr3);
  auto muon_l1passThr4 = Monitored::Collection("muon_l1passThr4",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(4)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr4);
  auto muon_l1passThr5 = Monitored::Collection("muon_l1passThr5",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(5)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr5);
  auto muon_l1passThr6 = Monitored::Collection("muon_l1passThr6",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(6)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr6);
  auto muon_l1passThr7 = Monitored::Collection("muon_l1passThr7",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(7)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr7);
  auto muon_l1passThr8 = Monitored::Collection("muon_l1passThr8",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(8)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr8);
  auto muon_l1passThr9 = Monitored::Collection("muon_l1passThr9",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(9)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr9);
  auto muon_l1passThr10 = Monitored::Collection("muon_l1passThr10",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(10)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr10);
  auto muon_l1passThr11 = Monitored::Collection("muon_l1passThr11",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(11)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr11);
  auto muon_l1passThr12 = Monitored::Collection("muon_l1passThr12",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(12)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr12);
  auto muon_l1passThr13 = Monitored::Collection("muon_l1passThr13",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(13)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr13);
  auto muon_l1passThr14 = Monitored::Collection("muon_l1passThr14",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(14)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr14);
  auto muon_l1passThr15 = Monitored::Collection("muon_l1passThr15",mymuons,[](const MyMuon& m){return m.matchedL1ThrInclusive.find(15)!=m.matchedL1ThrInclusive.end();});variables.push_back(muon_l1passThr15);

  
  if(!m_anaTgcPrd.value()){
    fill(m_packageName,variables);
    variables.clear();
    return StatusCode::SUCCESS;
  }

  
  SG::ReadHandle<Muon::TgcPrepDataContainer> tgcPrd(m_TgcPrepDataContainerKey, ctx);
  if(!tgcPrd.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain TgcPrepDataContainer with name "<< m_TgcPrepDataContainerKey);
    return StatusCode::FAILURE;
  }
  const TgcIdHelper& tgcIdHelper = m_idHelperSvc->tgcIdHelper();
  std::vector<TgcHit> tgcHits;
  std::set<TString> chamber_list;
  std::map<TString,std::vector<TgcHit>> tgcHitsMap;
  std::map<TString,std::vector<int>> tgcHitPhiMap;
  std::map<TString,std::vector<int>> tgcHitEtaMap;
  std::map<TString,std::vector<int>> tgcHitPhiMapGlobal;
  std::map<TString,std::vector<int>> tgcHitTiming;
  for(auto tgccnt : *tgcPrd){
    for(auto data : *tgccnt){
      TgcHit tgcHit;
      int bunch = -10;
      if ((data->getBcBitMap()&Muon::TgcPrepData::BCBIT_PREVIOUS)==Muon::TgcPrepData::BCBIT_PREVIOUS) bunch = -1;
      if ((data->getBcBitMap()&Muon::TgcPrepData::BCBIT_CURRENT)==Muon::TgcPrepData::BCBIT_CURRENT) bunch = 0;
      if ((data->getBcBitMap()&Muon::TgcPrepData::BCBIT_NEXT)==Muon::TgcPrepData::BCBIT_NEXT) bunch = +1;
      const MuonGM::TgcReadoutElement* element = data->detectorElement();
      const Identifier id = data->identify();
      const int gasGap = tgcIdHelper.gasGap(id);
      const int channel = tgcIdHelper.channel(id);
      const bool isStrip = tgcIdHelper.isStrip(id);
      const Amg::Vector3D& pos = isStrip ? element->stripPos(gasGap, channel) : element->gangPos(gasGap, channel);
      tgcHit.x = pos[0];
      tgcHit.y = pos[1];
      tgcHit.z = pos[2];
      if (isStrip) {
	tgcHit.shortWidth = element->stripShortWidth(gasGap, channel);
	tgcHit.longWidth = element->stripLongWidth(gasGap, channel);
	tgcHit.length = element->stripLength(gasGap, channel);
      } else {
	tgcHit.shortWidth = element->gangShortWidth(gasGap, channel);
	tgcHit.longWidth = element->gangLongWidth(gasGap, channel);
	tgcHit.length = element->gangLength(gasGap, channel);
      }
      tgcHit.isStrip = tgcIdHelper.isStrip(id);
      tgcHit.gasGap = tgcIdHelper.gasGap(id);
      tgcHit.channel = tgcIdHelper.channel(id);
      tgcHit.eta = tgcIdHelper.stationEta(id);
      tgcHit.phi = tgcIdHelper.stationPhi(id);
      tgcHit.station = tgcIdHelper.stationName(id);
      tgcHit.bunch = bunch;

      tgcHit.igasGap = tgcHit.gasGap;
      tgcHit.ieta = tgcHit.eta;
      tgcHit.iphi = tgcHit.phi;
      tgcHit.side = ( tgcHit.ieta > 0 )?("A"):("C");
      tgcHit.iside = ( tgcHit.ieta > 0 )?(0):(1);
      tgcHit.M = 0;
      tgcHit.istation = tgcHit.station;
      if( tgcHit.istation == 41 || tgcHit.istation == 42 ) tgcHit.M = 1;
      else if( tgcHit.istation == 43 || tgcHit.istation == 44 ) tgcHit.M = 2;
      else if( tgcHit.istation == 45 || tgcHit.istation == 46 ) tgcHit.M = 3;
      else if( tgcHit.istation == 47 || tgcHit.istation == 48 ) tgcHit.M = 4; // EIFI
      if(tgcHit.M == 0){
	ATH_MSG_ERROR("unknown station: " << tgcHit.istation);
      }

      if( tgcHit.M != 4 ){ // Big Wheel, (M1,M2,M3)
	if( tgcHit.istation % 2 == 0 ){ // Endcap
	  int iphi2 = tgcHit.iphi + 1; // 2,3,4,..,49
	  if(iphi2>=48)iphi2-=48; // 0,1,2,3 ..., 47
	  tgcHit.sector = int(iphi2 / 4) + 1; // 1,2,3,,,12
	  tgcHit.f = iphi2 - (tgcHit.sector-1) * 4; // 0,1,2,3
	  tgcHit.E = (tgcHit.M==1) ? ( 5 - TMath::Abs( tgcHit.ieta ) ) : ( 6 - TMath::Abs( tgcHit.ieta ) );
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form( "%s%02iM%02if%02iE%02iL%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.M, tgcHit.f, tgcHit.E, tgcHit.L,(tgcHit.isStrip>0)?("S"):("W"));
	}else{ // Forward
	  int iphi2 = tgcHit.iphi; // 1,2,3,4,..,25
	  if(iphi2>=24)iphi2-=24; // 0,1,2,3 ...,23
	  tgcHit.sector = int(iphi2 / 2) + 1; // 1,2,3,,,12
	  tgcHit.f = iphi2 - (tgcHit.sector-1) * 2; // 0,1
	  if(tgcHit.f==1)tgcHit.f = 2;//0,2
	  tgcHit.E = 0; // F
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form( "%s%02iM%02if%02iF00L%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.M, tgcHit.f, tgcHit.L,(tgcHit.isStrip>0)?("S"):("W"));
	}
      }else{ // Small Wheel (M4)
	if( tgcHit.istation == 47 ){// FI
	  tgcHit.sector = 0;
	  tgcHit.f = tgcHit.iphi; // 1,2,3..24
	  tgcHit.E = 0;
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form("%s00M04f%02iF00L%02i%s",tgcHit.side.Data(),tgcHit.f,tgcHit.L,(tgcHit.isStrip>0)?("S"):("W"));
	}else if( tgcHit.istation == 48 ){// EI
	  int iphi2 = (tgcHit.iphi>=21)?(tgcHit.iphi-21):(tgcHit.iphi); // 0,1,2,..,20
	  if(iphi2>=0&&iphi2<=2){ tgcHit.sector = 1; tgcHit.f = iphi2;}
	  else if(iphi2>=3&&iphi2<=5){ tgcHit.sector = 3; tgcHit.f = iphi2-3;}
	  else if(iphi2>=6&&iphi2<=8){ tgcHit.sector = 5; tgcHit.f = iphi2-6;}
	  else if(iphi2>=9&&iphi2<=10){ tgcHit.sector = 7; tgcHit.f = iphi2-9 +1;}
	  else if(iphi2>=11&&iphi2<=13){ tgcHit.sector = 9; tgcHit.f = iphi2-11;}
	  else if(iphi2>=14&&iphi2<=15){ tgcHit.sector = 11; tgcHit.f = iphi2-13;}
	  else if(iphi2>=16&&iphi2<=18){ tgcHit.sector = 13; tgcHit.f = iphi2-16;}
	  else if(iphi2>=19&&iphi2<=20){ tgcHit.sector = 15; tgcHit.f = iphi2-19 +1;}
	  tgcHit.E = 1;
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form("%s%02iM04f%02iE01L%02i%s",tgcHit.side.Data(),tgcHit.sector,tgcHit.f,tgcHit.L,(tgcHit.isStrip>0)?("S"):("W"));
	}else{
	  ATH_MSG_ERROR("Unknown detector");
	}
      }
      tgcHits.push_back(tgcHit);
      tgcHitsMap[tgcHit.name].push_back(tgcHit);
      chamber_list.insert(tgcHit.name);

      TString station_name = Form("%sM%02i%s",tgcHit.side.Data(),tgcHit.M,(tgcHit.isStrip>0)?("S"):("W"));
      int phimap_index = 0;
      int etamap_index = 0;
      int phimap_global_index = 0; // no empty bins compare to the above index
      if( tgcHit.M==1 ){
	phimap_index = (tgcHit.sector - 1) * 4 + tgcHit.f + 1;
	int tmpeta = (tgcHit.E==0)?(5):(tgcHit.E);
	etamap_index = (tmpeta - 1) * 3 + tgcHit.L;
	int tmpphi = tgcHit.f;
	if( tgcHit.E==0 && tgcHit.f==2 ) tmpphi = 1;
	if( tgcHit.E>0 ) phimap_global_index = (tmpeta - 1) * 144 + (tgcHit.sector - 1) * 12 + tmpphi * 3 + tgcHit.L;
	else             phimap_global_index = 576 + (tgcHit.sector - 1) * 6 + tmpphi * 3 + tgcHit.L;
      }else if( tgcHit.M==2 || tgcHit.M==3 ){
	phimap_index = (tgcHit.sector - 1) * 4 + tgcHit.f + 1;
	int tmpeta = (tgcHit.E==0)?(6):(tgcHit.E);
	etamap_index = (tmpeta - 1) * 2 + tgcHit.L;
	int tmpphi = tgcHit.f;
	if( tgcHit.E==0 && tgcHit.f==2 ) tmpphi = 1;
	if( tgcHit.E>0 ) phimap_global_index = (tmpeta - 1) * 96 + (tgcHit.sector - 1) * 8 + tmpphi * 2 + tgcHit.L;
	else             phimap_global_index = 480 + (tgcHit.sector - 1) * 4 + tmpphi * 2 + tgcHit.L;
      }else if( tgcHit.M==4 ){
	phimap_index = tgcHit.iphi;
	int tmpeta = (tgcHit.E==0)?(2):(tgcHit.E);
	etamap_index = (tmpeta - 1) * 2 + tgcHit.L;
	if( tgcHit.E>0 ) phimap_global_index = (tgcHit.iphi-1) * 2 + tgcHit.L;
	else             phimap_global_index = 42 + (tgcHit.iphi-1) * 2 + tgcHit.L;
      }
      tgcHitPhiMap[station_name].push_back(phimap_index);
      tgcHitEtaMap[station_name].push_back(etamap_index);
      tgcHitPhiMapGlobal[station_name].push_back(phimap_global_index);
      tgcHitTiming[station_name].push_back(bunch);

    }
  }

  auto hit_n = Monitored::Scalar<int>("hit_n",tgcHits.size());variables.push_back(hit_n);
  auto hit_bunch = Monitored::Collection("hit_bunch",tgcHits,[](const TgcHit& m){return m.bunch;});variables.push_back(hit_bunch);
  auto hit_sideA = Monitored::Collection("hit_sideA",tgcHits,[](const TgcHit& m){return m.z>0;});variables.push_back(hit_sideA);
  auto hit_sideC = Monitored::Collection("hit_sideC",tgcHits,[](const TgcHit& m){return m.z<0;});variables.push_back(hit_sideC);

  for(auto chamber_name : chamber_list){
    auto hits_on_a_chamber = Monitored::Collection(Form("hits_on_%s",chamber_name.Data()),tgcHitsMap[chamber_name],[](const TgcHit& m){return m.channel;});
    fill(m_packageName,hits_on_a_chamber);
  }

  for(auto phimap : tgcHitPhiMap){
    auto x = Monitored::Collection(Form("x_%s",phimap.first.Data()),tgcHitEtaMap[phimap.first],[](const int& m){return m;});
    auto y = Monitored::Collection(Form("y_%s",phimap.first.Data()),phimap.second,[](const int& m){return m;});
    fill(m_packageName,x,y);
  }
  for(auto phimap : tgcHitPhiMapGlobal){
    auto x = Monitored::Scalar<int>(Form("lb_for_%s",phimap.first.Data()),GetEventInfo(ctx)->lumiBlock());
    auto y = Monitored::Collection(Form("%s",phimap.first.Data()),phimap.second,[](const int& m){return m;});
    auto z = Monitored::Collection(Form("timing_for_%s",phimap.first.Data()),tgcHitTiming[phimap.first],[](const int& m){return m;});
    fill(m_packageName,x,y,z);
  }
  
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgcCoinCurr(m_TgcCoinDataContainerCurrBCKey, ctx);
  if(!tgcCoinCurr.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name "<< m_TgcCoinDataContainerCurrBCKey);
    return StatusCode::FAILURE;
  }
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgcCoinNext(m_TgcCoinDataContainerNextBCKey, ctx);
  if(!tgcCoinNext.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name "<< m_TgcCoinDataContainerNextBCKey);
    return StatusCode::FAILURE;
  }
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgcCoinPrev(m_TgcCoinDataContainerPrevBCKey, ctx);
  if(!tgcCoinPrev.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name "<< m_TgcCoinDataContainerPrevBCKey);
    return StatusCode::FAILURE;
  }
  std::map<int, SG::ReadHandle<Muon::TgcCoinDataContainer> > tgcCoin;
  tgcCoin[0] =  tgcCoinCurr;
  tgcCoin[+1] = tgcCoinNext;
  tgcCoin[-1] = tgcCoinPrev;
  std::vector<TgcTrig> tgcTrigs;
  for(auto thisCoin : tgcCoin){
    int bunch = thisCoin.first;
    for(auto tgccnt : *(thisCoin.second)){
      for(auto data : *tgccnt){
	TgcTrig tgcTrig;
	const int type = data->type();
	const Amg::Vector3D& posIn = data->globalposIn();
	tgcTrig.x_In = posIn[0];
	tgcTrig.y_In = posIn[1];
	tgcTrig.z_In = posIn[2];
	const Amg::Vector3D& posOut = data->globalposOut();
	tgcTrig.x_Out = posOut[0];
	tgcTrig.y_Out = posOut[1];
	tgcTrig.z_Out = posOut[2];
	tgcTrig.width_In = data->widthIn();
	tgcTrig.width_Out = data->widthOut();
	if (type == Muon::TgcCoinData::TYPE_SL) {
	  const Amg::MatrixX& matrix = data->errMat();
	  tgcTrig.width_R = matrix(0,0);
	  tgcTrig.width_Phi = matrix(1,1);
	} else {
	  tgcTrig.width_R = 0.;
	  tgcTrig.width_Phi = 0.;
	}
	tgcTrig.isAside = data->isAside();
	tgcTrig.isForward = data->isForward();
	tgcTrig.isStrip = data->isStrip();
	tgcTrig.isInner = data->isInner();
	tgcTrig.isPositiveDeltaR = data->isPositiveDeltaR();
	tgcTrig.type = type;
	tgcTrig.trackletId = data->trackletId();
	tgcTrig.trackletIdStrip = data->trackletIdStrip();
	tgcTrig.phi = data->phi();
	tgcTrig.roi = data->roi();
	tgcTrig.pt = data->pt();
	tgcTrig.delta = data->delta();
	tgcTrig.sub = data->sub();
	tgcTrig.veto = data->veto();
	tgcTrig.bunch = bunch;
	tgcTrig.inner = data->inner();
	tgcTrigs.push_back( tgcTrig );
      }
    }
  }

  auto coin_n = Monitored::Scalar<int>("coin_n",tgcTrigs.size());variables.push_back(coin_n);
  auto coin_bunch = Monitored::Collection("coin_bunch",tgcTrigs,[](const TgcTrig& m){return m.bunch;});variables.push_back(coin_bunch);
  auto coin_sideA = Monitored::Collection("coin_sideA",tgcTrigs,[](const TgcTrig& m){return m.isAside==1;});variables.push_back(coin_sideA);
  auto coin_sideC = Monitored::Collection("coin_sideC",tgcTrigs,[](const TgcTrig& m){return m.isAside!=1;});variables.push_back(coin_sideC);

  
  fill(m_packageName,variables);
  variables.clear();
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////
StatusCode TgcRawDataMonitorAlgorithm::triggerMatching(const xAOD::Muon* offline_muon, const std::vector<TagDef>& list_of_triggers ) const
{
  if( !m_TagAndProbe.value() )return StatusCode::SUCCESS;
  if( getTrigDecisionTool().empty() ) return StatusCode::SUCCESS;
  TVector3 muonvec; muonvec.SetPtEtaPhi(offline_muon->pt(),offline_muon->eta(),offline_muon->phi());
  for(auto tagTrig : list_of_triggers ){
    if( !getTrigDecisionTool()->isPassed( tagTrig.eventTrig.Data() ) ) return StatusCode::FAILURE;
    auto features = getTrigDecisionTool()->features<xAOD::MuonContainer>( tagTrig.tagTrig.Data() ,TrigDefs::Physics,m_MuonEFContainerName.value());
    for(auto aaa : features){
      ATH_CHECK( aaa.isValid() );
      auto trigmuon_link = aaa.link;
      auto trigmuon = *trigmuon_link;
      TVector3 trigvec; trigvec.SetPtEtaPhi(trigmuon->pt(),trigmuon->eta(),trigmuon->phi());
      if( trigvec.DeltaR( muonvec ) < m_trigMatchWindow.value() ) return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
///////////////////////////////////////////////////////////////
void TgcRawDataMonitorAlgorithm::extrapolate(const xAOD::Muon* muon, MyMuon& mymuon) const
{
  const ElementLink< xAOD::TrackParticleContainer >& el = muon->combinedTrackParticleLink();
  const xAOD::TrackParticle* track = *el;
  for(const auto z : m_extZposition){
    Amg::Vector2D etaDeta(0,0);
    Amg::Vector2D phiDphi(0,0);
    Amg::Vector3D mom(0,0,0);
    Amg::Vector3D extrapolateTo(0, 0, z);
    if( extrapolate(track,extrapolateTo,TGC, etaDeta, phiDphi, mom) ){
      double pt = extrapolateTo.z() / std::sinh(etaDeta[0]);
      TVector3 vec,pos;
      pos.SetPtEtaPhi(pt,etaDeta[0],phiDphi[0]);
      vec.SetXYZ(mom.x(),mom.y(),mom.z());
      mymuon.extPos.push_back(pos);
      mymuon.extVec.push_back(vec);
    }
  }
}
///////////////////////////////////////////////////////////////
/* track extrapolator tool */
bool
TgcRawDataMonitorAlgorithm::extrapolate(const xAOD::TrackParticle* trackParticle,
				const Amg::Vector3D& pos,
				const int detector,
				Amg::Vector2D& eta,
				Amg::Vector2D& phi,
				Amg::Vector3D& mom) const
{
  if (!trackParticle) {
    return false;
  }
  const Trk::Track* track = trackParticle->track();
  if (!track) {
    return false;
  }
  const DataVector<const Trk::TrackStateOnSurface>* trackOnSurf = track->trackStateOnSurfaces();
  if(!trackOnSurf){
    return false;
  }
  std::vector<double> vExtEta, vExtPhi, vExtPx, vExtPy, vExtPz, vExtDeltaEta, vExtDeltaPhi, vExtDistance;
  int counter = 0;
  for ( auto it : *trackOnSurf ) {
    if ( ! it->type(Trk::TrackStateOnSurface::Measurement) ){
      continue;
    }
    const Trk::TrackParameters* point = NULL;
    Amg::Vector2D distance(-1.e30, -1.e30);
    if (detector == TgcRawDataMonitorAlgorithm::TGC) {
      point = extrapolateToTGC(it, pos, distance);
    } else if (detector == TgcRawDataMonitorAlgorithm::RPC) {
      point = extrapolateToRPC(it, pos, distance);
    } else {
      continue;
    }
    if(!point){
      continue;
    }
    vExtEta.push_back(point->position().eta());
    vExtPhi.push_back(point->position().phi());
    vExtPx.push_back(point->momentum().x());
    vExtPy.push_back(point->momentum().y());
    vExtPz.push_back(point->momentum().z());
    vExtDistance.push_back(distance.y());
    counter++;
    delete point;
  }
  if (counter==0){
    return false;
  }
  double minimum = 1.e30;
  int number = -1;
  for (int ii = 0; ii < counter; ii++) {
    if (minimum>vExtDistance.at(ii)) {
      minimum = vExtDistance.at(ii);
      number = ii;
    }
  }
  if (number==-1){
    return false;
  }
  eta[0] = vExtEta.at(number);
  phi[0] = vExtPhi.at(number);
  eta[1] = getError(vExtEta);
  phi[1] = getError(vExtPhi);
  mom[0] = vExtPx.at(number);
  mom[1] = vExtPy.at(number);
  mom[2] = vExtPz.at(number);
  return true;
}
///////////////////////////////////////////////////////////////
const Trk::TrackParameters*
TgcRawDataMonitorAlgorithm::extrapolateToTGC(const Trk::TrackStateOnSurface* tsos,
                                             const Amg::Vector3D& pos,
                                             Amg::Vector2D& distance) const
{
  const Trk::TrackParameters* track = tsos->trackParameters();
  if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
    return 0;
  }
  double targetZ = pos.z();
  double trackZ = track->position().z();
  if (std::abs(trackZ)<std::abs(targetZ)-2000. || std::abs(trackZ)>std::abs(targetZ)+2000.){
    return 0;
  }
  Amg::Transform3D* matrix = new Amg::Transform3D;
  matrix->setIdentity();
  matrix->translation().z() = targetZ;
  Trk::DiscSurface* disc = new Trk::DiscSurface(matrix,
                                                m_endcapPivotPlaneMinimumRadius.value(),
                                                m_endcapPivotPlaneMaximumRadius.value());
  if(!disc){
    delete matrix;
    matrix = 0;
    return 0;
  }
  distance[0] = trackZ;
  distance[1] = std::abs(trackZ - targetZ);
  const bool boundaryCheck = true;
  const Trk::Surface* surface = disc;
  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*track,
                                                                  *surface,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);
  delete disc;
  disc = 0;
  surface = 0;
  matrix = 0;
  if (!param) {
    return 0;
  }
  const Trk::AtaDisc* ataDisc =
    dynamic_cast<const Trk::AtaDisc*>(param);
  return ataDisc;
}
///////////////////////////////////////////////////////////////
const Trk::TrackParameters*
TgcRawDataMonitorAlgorithm::extrapolateToRPC(const Trk::TrackStateOnSurface* tsos,
                                             const Amg::Vector3D& pos,
                                             Amg::Vector2D& distance) const
{
  const Trk::TrackParameters* track = tsos->trackParameters();
  if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
    return 0;
  }
  double radius = pos.perp();
  double trackRadius = track->position().perp();
  if(trackRadius<radius-2000. || trackRadius>radius+2000.) return 0;
  Amg::Transform3D* matrix = new Amg::Transform3D;
  matrix->setIdentity();
  Trk::CylinderSurface* cylinder = 
    new Trk::CylinderSurface(matrix,
                             radius,
                             m_barrelPivotPlaneHalfLength.value());
  if (!cylinder) {
    delete matrix;
    matrix = 0;
    return 0;
  }
  distance[0] = trackRadius;
  distance[1] = trackRadius - radius;
  const bool boundaryCheck = true;
  const Trk::Surface* surface = cylinder;
  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*track,
                                                                  *surface,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);
  delete cylinder;
  cylinder = 0;
  surface = 0;
  matrix = 0;
  if (!param) {
    return 0;
  }
  const Trk::AtaCylinder* ataCylinder =
    dynamic_cast<const Trk::AtaCylinder*>(param);
  return ataCylinder;
}
///////////////////////////////////////////////////////////////
double
TgcRawDataMonitorAlgorithm::getError(const std::vector<double>& inputVec) const
{
  const int nSize = inputVec.size();
  double sum = 0;
  double sum2 = 0;
  for (int ii = 0; ii < nSize; ii++) {
    sum += inputVec.at(ii);
  }
  const double mean = sum/nSize;
  for(int jj = 0; jj < nSize; jj++){
    sum2 = sum2 + std::pow((inputVec.at(jj)-mean),2);
  }
  const double stdDev = std::sqrt(sum2/nSize);
  return stdDev;
}



