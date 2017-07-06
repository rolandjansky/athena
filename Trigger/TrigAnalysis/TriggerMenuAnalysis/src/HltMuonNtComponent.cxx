/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  HltMuonNtComponent.cxx
*/
#include "TriggerMenuAnalysis/HltMuonNtComponent.h"
#include "TriggerMenuAnalysis/TrigMenuNtupleAlg.h"
#include "TriggerMenuAnalysis/ConvertData.h"
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "TriggerMenuNtuple/RoIData_Muon.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "Particle/TrackParticle.h"
#include <boost/functional.hpp>

//TGC HPT
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

//offline Muon
#include "muonEvent/MuonContainer.h"
#include "TileEvent/TileContainer.h"

using namespace std;
using namespace NtComponent;

HltMuonNtComponent::HltMuonNtComponent(NtupleAlgorithm* mainAlg, 
				       NtComponentParameters parameters) : 
  NtupleComponent(mainAlg, parameters), 
  m_trigEventInfo(0), 
  m_tgcHptWire(0), 
  m_tgcHptStrip(0), 
  m_moore(0), 
  m_muid(0), 
  m_tileL2Muon(0), 
  m_tileMuon(0), m_muonChains() {
  //  declareProperty("EventInfoName", m_eventInfoName = "McEventInfo", "EventInfo key");
  
  m_muonDataVec = 0;
  m_tileMuDataVec = 0;

  m_trigEventInfo = 0;
  m_tgcHptWire = 0;
  m_tgcHptStrip = 0;
  m_moore = 0;
  m_muid = 0;
  m_tileL2Muon = 0;
  m_tileMuon = 0;

  m_activeStore = 0;
  m_trigAccessTool = 0;
  m_event = 0;

  TrigMenuNtupleAlg* mymainAlg = dynamic_cast<TrigMenuNtupleAlg*>(mainAlg);

  if (mymainAlg) {
    m_activeStore = mymainAlg->activeStoreSvc();
    m_trigAccessTool = mymainAlg->trigAccessTool();
  }

  // Configure properties from the NtComponentParameters
  const std::string container_key = "EventInfo";
  const std::map<std::string, std::string>& mm = parameters.containerKeys;
  std::map<std::string, std::string>::const_iterator p_mm = 
    mm.find(container_key);
  if (p_mm != mm.end()) {
    m_eventInfoName = p_mm->second;
  }
  unsigned int i=0;
  for (i=0; i<parameters.options.size(); ++i) {
    const std::string& chain_name = parameters.options[i];
    m_muonChains.push_back(chain_name);
  }

  createChainGroups();
}

HltMuonNtComponent::~HltMuonNtComponent() {

  if( m_trigEventInfo ) delete m_trigEventInfo;

  if( m_muonDataVec ) delete m_muonDataVec;
  if( m_tileMuDataVec ) delete m_tileMuDataVec;


  if( m_tgcHptWire ) delete m_tgcHptWire;
  if( m_tgcHptStrip ) delete m_tgcHptStrip;

  if( m_moore ) delete m_moore;
  if( m_muid ) delete m_muid;
  if( m_tileL2Muon ) delete m_tileL2Muon;
  if( m_tileMuon ) delete m_tileMuon;
  

}

StatusCode HltMuonNtComponent::book() {
  m_trigEventInfo = new TrigEventInfo();

  m_muonDataVec = new std::vector<RoIData_Muon>();
  m_tileMuDataVec = new std::vector<RoIData_TileMu>();

  //for Muon HLT
  m_tgcHptWire = new std::vector<TgcHpt>();
  m_tgcHptStrip = new std::vector<TgcHpt>();
  m_moore = new std::vector<MSMuon>();
  m_muid = new std::vector<CBMuon>();
  m_tileL2Muon = new std::vector<TileL2Muon>();
  m_tileMuon = new std::vector<TileMuon>();

  m_tree->Branch("EventInfo", m_trigEventInfo);

  m_tree->Branch("Muon", m_muonDataVec);
  m_tree->Branch("TileMu",m_tileMuDataVec);

  //for Muon HLT
  m_tree->Branch("TgcHptWire", m_tgcHptWire);
  m_tree->Branch("TgcHptStrip", m_tgcHptStrip);
  m_tree->Branch("Moore", m_moore);
  m_tree->Branch("Muid", m_muid);
  m_tree->Branch("TileL2Muon", m_tileL2Muon);
  m_tree->Branch("TileMuon", m_tileMuon);

  m_event = TrigMenuEvent::getInstance();

  return StatusCode::SUCCESS;
}

StatusCode HltMuonNtComponent::fill() {

  //clear variables
  m_trigEventInfo->clear();

  m_muonDataVec->clear();
  m_tileMuDataVec->clear();

  m_tgcHptWire->clear();
  m_tgcHptStrip->clear();

  m_moore->clear();
  m_muid->clear();
  m_tileL2Muon->clear();
  m_tileMuon->clear();


  fillHltData();

  //  fillEventInfo();
  fillTgcHpt();
  fillOfflineMuon();
  
  return StatusCode::SUCCESS;
}

MsgStream& HltMuonNtComponent::log() const {
  //  m_msg->setLevel(MSG::DEBUG);
  return *m_msg;
}

void HltMuonNtComponent::fillHltData() {
  std::vector<HLTObjectsInRoI*> muonRoIs;

  std::vector<std::string>::const_iterator p_chain;
  for (p_chain=m_muonChains.begin(); p_chain!=m_muonChains.end(); ++p_chain) {
    m_trigAccessTool->getChainObjects<MuonObjectsInRoI>(*p_chain, muonRoIs);
  }
  std::vector<HLTObjectsInRoI*>::const_iterator p_roi;
  for (p_roi=muonRoIs.begin(); p_roi!=muonRoIs.end(); ++p_roi) {
    RoIData_Muon x;
    MuonObjectsInRoI* x1 = dynamic_cast<MuonObjectsInRoI*>(*p_roi);
    if (x1) {
      convertMuonData(x, *x1);
      m_muonDataVec->push_back(x);
    }
  }
}

void HltMuonNtComponent::fillEventInfo() {
  //  MsgStream log(msgSvc(), name());

  log() << MSG::DEBUG << " fillEventInfo start " << endmsg;

  //get Event Info
  EventInfo* eventInfo;
  StatusCode sc = (*m_activeStore) -> retrieve( eventInfo, m_eventInfoName );
  //StatusCode sc = (*m_activeStore) -> retrieve( m_eventInfo, "McEventInfo" );
  if ( sc.isFailure() || 0 == eventInfo ) {
    log() << MSG::ERROR << " Cannot retrieve EventInfo " << endmsg;
    return;
  }

  const EventID* evtid = eventInfo->event_ID();

  int run_number = -1;
  int lumi_block = -1;
  int event_number = -1;
  int time = -1;
  int BCID = -1;
  if( evtid != 0 ){
    run_number   = evtid->run_number();
    lumi_block   = evtid->lumi_block();
    event_number = evtid->event_number();
    time         = evtid->time_stamp();
    BCID         = evtid->bunch_crossing_id();
  }
  m_trigEventInfo -> setTrigEventInfo( run_number, lumi_block,  event_number, time, BCID);

  log() << MSG::DEBUG << " fillEventInfo end " << endmsg;
}

void HltMuonNtComponent::fillTgcHpt() {
  //  MsgStream log(msgSvc(), name());

  log() << MSG::DEBUG << " fillTgcHpt start " << endmsg;

  //TGC trigger PRD
  const Muon::TgcCoinDataContainer* tgc_container(0);

  StatusCode sc = (*m_activeStore) -> retrieve( tgc_container, "TrigT1CoinDataCollection" );
  if ( sc.isFailure() || 0 == tgc_container ) {
    log() << MSG::ERROR << " Cannot retrieve TgcCoinDataContainer " << endmsg;
    return;
  }

  // PRD loop
  Muon::TgcCoinDataContainer::const_iterator it_end  = tgc_container -> end();
  for( Muon::TgcCoinDataContainer::const_iterator it = tgc_container -> begin();
       it != it_end;
       ++ it ){ 
       
    if ( it == tgc_container->end() || (*it)->size()==0) continue;  //check if there are counts      
    log() << MSG::DEBUG << "size of tgc collection is " << (*it) -> size() << endmsg;


    //loop over collection
    Muon::TgcCoinDataCollection::const_iterator itc_end  = (*it) -> end();
    for( Muon::TgcCoinDataCollection::const_iterator itc = (*it) -> begin();
         itc != itc_end;
         ++ itc ){

      const Muon::TgcCoinData* tcd = *itc;

      //if not HPT continue
      log() << MSG::ERROR << "type " << Muon::TgcCoinData::TYPE_HIPT<<" "<< tcd -> type() << endmsg;
      if( tcd -> type() != Muon::TgcCoinData::TYPE_HIPT ) continue;

      TgcHpt hpt;
      hpt.isAside    = ( tcd -> isAside() );
      hpt.isForward  = ( tcd -> isForward() );
      hpt.phi        = ( tcd -> phi() );
      hpt.trackletId = ( tcd -> trackletId() );
      hpt.sub   =     ( tcd -> sub() );
      hpt.sub        = ( 0 );//no "sub" before 15.1.0
      hpt.delta      = ( tcd -> delta() );

      if( tcd -> isStrip() == true ){//strip HPT

        log() << MSG::ERROR << "push back hpt strip" <<endmsg;
        m_tgcHptStrip -> push_back(hpt);

      }else{//wire HPT

        log() << MSG::ERROR << "push back hpt wire" <<endmsg;
        m_tgcHptWire -> push_back(hpt);

      }

    }//loop over collection end

  }//loop over container end

  log() << MSG::DEBUG << " fillTGC end " << endmsg;
}

void HltMuonNtComponent::fillOfflineMuon() {
  //  MsgStream log(msgSvc(), name());

  //Moore and Muid
  const Analysis::MuonContainer* muonContainer;
  StatusCode sc = (*m_activeStore)->retrieve(muonContainer, "MuidMuonCollection");

  if( sc.isFailure() ) {
    //return StatusCode::SUCCESS;
    return;
  } else {
    Analysis::MuonContainer::const_iterator muonItr  = muonContainer->begin();
    Analysis::MuonContainer::const_iterator muonItrE = muonContainer->end();
    //int nTracks_RecMS = muonContainer->size();
    for ( ; muonItr != muonItrE; ++muonItr) {
      const Analysis::Muon* thisMuon = *muonItr;
      const Rec::TrackParticle* mooreMuon = thisMuon->muonSpectrometerTrackParticle();
      if(mooreMuon){
        MSMuon m;
        setMSMuon(m, mooreMuon);
        m_moore->push_back(m);
      }
      CBMuon m;
      const Rec::TrackParticle* muidsaMuon = thisMuon->muonExtrapolatedTrackParticle();
      if(muidsaMuon){
        setSAMuon(m, muidsaMuon);
     }
      const Rec::TrackParticle* muidcbMuon = thisMuon->combinedMuonTrackParticle();
      if(muidcbMuon){
        setCBMuon(m, muidcbMuon);
      }
      if(muidsaMuon || muidcbMuon){
        m_muid->push_back(m);
      }
    }
  }

  //TileL2Muon
  const TileL2Container* mutags_ROD;
  sc=(*m_activeStore)->retrieve(mutags_ROD, "TileL2Cnt" );
  log() << MSG::DEBUG << " filling TileL2" << endmsg;
  if(sc == StatusCode::FAILURE) {
    log() << MSG::WARNING
        << "cannot retrieve TileL2Container in TDS"
        << endmsg;
    return;
  }

  TileL2Container::const_iterator it= mutags_ROD->begin();
  TileL2Container::const_iterator end=mutags_ROD->end();

  int ntag=0;

  //loop over ROD
  for(; it != end; ++it) {

    std::vector<float> eta;
    std::vector<float> phi;
    std::vector<float> energy0;
    std::vector<float> energy1;
    std::vector<float> energy2;
    std::vector<float> quality;

    //loop over energy clasters
    for(unsigned int a=0; a<((*it)->NMuons()); ++a) {
      eta.push_back( (*it)->eta(a) );
      phi.push_back( (*it)->phi(a) );
      energy0.push_back( (*it)->enemu0(a) );
      energy1.push_back( (*it)->enemu1(a) );
      energy2.push_back( (*it)->enemu2(a) );
      quality.push_back( (*it)->qual(a) );
      ntag++;
    }

    TileL2Muon m;

    m.setTileL2Muon(eta, phi, energy0, energy1, energy1, quality);

    m_tileL2Muon->push_back(m);

    //if (ntag >= max_ntag) break;

  }


  //TileMuon
  // step1: read  from TDS
  const TileMuContainer*  mutags_cont;
  sc=(*m_activeStore)->retrieve(mutags_cont, "TileMuObj");
  log() << MSG::DEBUG << " filling TileMu" << endmsg;

  if(sc == StatusCode::FAILURE) {
    log()<<MSG::WARNING<<"cannot retrieve TileMuContainer in TDS"<<endmsg;
    return;
  }

  TileMuContainer::const_iterator it2= mutags_cont->begin();
  TileMuContainer::const_iterator end2=mutags_cont->end();

  ntag=0;

  for(; it2 != end2; ++it2) {
    TileMuon m;

    m.eta = (*it2)->eta() ;
    m.phi = (*it2)->phi() ;
    m.energy.push_back( (*it2)->enedep() [0] );
    m.energy.push_back( (*it2)->enedep() [1] );
    m.energy.push_back( (*it2)->enedep() [2] );
    m.energy.push_back( (*it2)->enedep() [3] );
    m.quality = (*it2)->quality() ;

    m_tileMuon->push_back(m);

    ntag++;

    //if (ntag >= max_ntag) break;
  }

  return;
}

void HltMuonNtComponent::readHLTData() {
}

void HltMuonNtComponent::printChainNames(const std::vector<std::string>& /*v*/, 
					 const std::string& /*type*/) {
}

void HltMuonNtComponent::createChainGroups() {
  createChainGroups(m_muonChains);
}

void HltMuonNtComponent::createChainGroups(const std::vector<std::string>& v) {

  std::vector<std::string>::const_iterator p;
  for (p=v.begin(); p!=v.end(); ++p) {
    log() << MSG::DEBUG << "Create chaingroup for chain: " << (*p) << endmsg;
    m_trigAccessTool->createChainGroup(*p);
  }
}

