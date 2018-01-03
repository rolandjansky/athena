/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcLv1RawDataValAlg
// Authors:  A.Ishikawa(Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/DataHandle.h"

#include "xAODEventInfo/EventInfo.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// GeoModel
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 

#include "Identifier/Identifier.h"

#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

//offline 
#include "xAODMuon/MuonContainer.h"

#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetEtRoI.h"
#include "xAODTrigger/EnergySumRoI.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// clearVectorsArrays
///////////////////////////////////////////////////////////////////////////
// Clears Vectors and Arrays ready for them to be refilled by readTgcCoinContainer
void
TgcLv1RawDataValAlg::clearVectorsArrays(){
  m_earliestTrigger=-1;

  for(int pcnt=0;pcnt<4;pcnt++){// PCNT
    m_nLptWire[pcnt]=0;
    m_nLptStrip[pcnt]=0;
    m_nHptWire[pcnt]=0;
    m_nHptStrip[pcnt]=0;
    m_nSL[pcnt]=0;

    if(pcnt!=TOTA){
      // reset timing flag arrays
      for(int ac=0;ac<2;ac++)
	for(int ipt=0;ipt<6;ipt++)
	  for(int iphi=0;iphi<48;iphi++){
	    for(int ws=0;ws<2;ws++)
	      m_hasLpTtrigger[ws][ac][ipt][iphi][pcnt]=false;
	    m_maxSLtrigger[ac][ipt][iphi][pcnt]=0;
	  }

      // clear variable vectors
      m_sl_pt[pcnt].clear();
      m_sl_eta[pcnt].clear();
      m_sl_phi[pcnt].clear();
      m_sl_tidw[pcnt].clear();
      m_sl_tids[pcnt].clear();
      m_sl_roi[pcnt].clear();
      m_sl_etaout[pcnt].clear();
      m_sl_phi48[pcnt].clear();
      m_sl_isAside[pcnt].clear();
      m_sl_isForward[pcnt].clear();
      m_sl_etaroi[pcnt].clear();
      m_sl_phiroi[pcnt].clear();

      for(int ws=0;ws<2;ws++){// wirestrip
	m_lpt_delta[ws][pcnt].clear();
	m_lpt_tid[ws][pcnt].clear();
	m_lpt_sub[ws][pcnt].clear();
	m_lpt_eta[ws][pcnt].clear();
	m_lpt_phi[ws][pcnt].clear();
	m_lpt_etain[ws][pcnt].clear();
	m_lpt_etaout[ws][pcnt].clear();
	m_lpt_phi48[ws][pcnt].clear();
	m_lpt_isAside[ws][pcnt].clear();
	m_lpt_isForward[ws][pcnt].clear();

	m_hpt_delta[ws][pcnt].clear();
	m_hpt_tid[ws][pcnt].clear();
	m_hpt_sub[ws][pcnt].clear();
	m_hpt_eta[ws][pcnt].clear();
	m_hpt_phi[ws][pcnt].clear();
	m_hpt_etain[ws][pcnt].clear();
	m_hpt_etaout[ws][pcnt].clear();
	m_hpt_phi48[ws][pcnt].clear();
	m_hpt_isAside[ws][pcnt].clear();
	m_hpt_isForward[ws][pcnt].clear();
	m_hpt_isInner[ws][pcnt].clear(); 
	m_hpt_inner[ws][pcnt].clear();

      }// wirestrip
    }// PCN
  }// PCNT
}


///////////////////////////////////////////////////////////////////////////
// readTgcCoinContainer
///////////////////////////////////////////////////////////////////////////
// Reads Tgc Coincidence Data from container into vectors for use in filling histograms
void 
TgcLv1RawDataValAlg::readTgcCoinDataContainer(const Muon::TgcCoinDataContainer* tgc_coin_container, int pcn){
  std::string WS[2]={"wire","strip"};
  ATH_MSG_DEBUG( "reading TgcCoinDataContainer into vectors"  );

  if(pcn!=TOTA){
    // PREV/CURR/NEXT timing
    // Loop over TGCCoinDataContainer
    Muon::TgcCoinDataContainer::const_iterator it_end=tgc_coin_container->end();
    for(Muon::TgcCoinDataContainer::const_iterator it=tgc_coin_container->begin();
	it!=it_end;
	++it){
      if(it == it_end || (*it)->size()==0)continue;  //check if there are counts 
      ATH_MSG_DEBUG( "size of tgc collection is " << (*it) -> size()  );

      // Loop over Collection
      Muon::TgcCoinDataCollection::const_iterator itc_end=(*it)->end();
      for(Muon::TgcCoinDataCollection::const_iterator itc=(*it)->begin();
	  itc!= itc_end;
	  ++itc){
	const Muon::TgcCoinData* tcd=*itc;

	// Get Variables from TgcCoinData object
	int ac = (tcd->isAside()==false);  //isNotAside   a:0, c:1
	int ef = (tcd->isForward()==false);//isNotForward f:0, e:1
	int ws = (tcd->isStrip());         //isStrip      w:0, s:1 (invalid in case of SL)
	int phi48 = tcd->phi() -1;//[0:47]MidEnd[0:23]Forward/EIFI
	if(ef==0) phi48 = phi48*2;//[0:2:46]
	//int sector = phi2sector(phi48,ef);
	int pt = tcd->pt();
	Amg::Vector3D gposout = tcd->globalposOut(); 
	double eta, phi;
	if(gposout[0]==0) { eta = 0; phi = 0;} 
	else { eta = gposout.eta();  phi = gposout.phi(); } 

	const Identifier tcdidout = tcd->channelIdOut();
	int etaout = abs(int(m_tgcIdHelper->stationEta(tcdidout)));
	if(ef==0) etaout = 0;

	const Identifier tcdidin  = tcd->channelIdIn();
	int etain  = abs(int(m_tgcIdHelper->stationEta(tcdidin)));
	if(ef==0) etain  = 0;

	// Fill vectors for different Coincidence Types
	if( tcd->type() == Muon::TgcCoinData::TYPE_TRACKLET ){
	  // LpT triggers
	  ATH_MSG_DEBUG( "Low_Pt" << WS[ws]  );

	  // Do flags and counters
	  m_hasLpTtrigger[ws][ac][etaout][phi48][pcn]=true;
	  if(ws==1)m_nLptStrip[pcn]++;
	  else     m_nLptWire[pcn]++;

	  //for timing
	  m_lpt_delta[ws][pcn].push_back( tcd->delta() );
	  m_lpt_tid[ws][pcn].push_back( tcd->trackletId() );
	  m_lpt_sub[ws][pcn].push_back( tcd->sub() );
	  m_lpt_eta[ws][pcn].push_back( eta );
	  m_lpt_phi[ws][pcn].push_back( phi );
	  m_lpt_etain[ws][pcn].push_back( etain );
	  m_lpt_etaout[ws][pcn].push_back( etaout );
	  m_lpt_phi48[ws][pcn].push_back( tcd->phi() -1 );
	  m_lpt_isAside[ws][pcn].push_back( tcd->isAside() );
	  m_lpt_isForward[ws][pcn].push_back( tcd->isForward() );

	}
	else if( tcd->type() == Muon::TgcCoinData::TYPE_HIPT ){
	  // HpT triggers
	  ATH_MSG_DEBUG( "HighPt" << WS[ws]  );

	  // Do flags and counters
	  //m_hasHpTtrigger[ws][ac][etaout][phi48][pcn]=true;
	  if(ws==1)m_nHptStrip[pcn]++;
	  else     m_nHptWire[pcn]++;

	  //for coincidence window
	  m_hpt_delta[ws][pcn].push_back( tcd->delta() );
	  m_hpt_tid[ws][pcn].push_back( tcd->trackletId() );
	  m_hpt_sub[ws][pcn].push_back( tcd->sub() );
	  m_hpt_eta[ws][pcn].push_back( eta );
	  m_hpt_phi[ws][pcn].push_back( phi );
	  m_hpt_etain[ws][pcn].push_back( etain );
	  m_hpt_etaout[ws][pcn].push_back( etaout );
	  m_hpt_phi48[ws][pcn].push_back( tcd->phi() -1 );
	  m_hpt_isAside[ws][pcn].push_back( tcd->isAside() );
	  m_hpt_isForward[ws][pcn].push_back( tcd->isForward() );
	  m_hpt_isInner[ws][pcn].push_back( tcd->isInner() ); 
	  m_hpt_inner[ws][pcn].push_back( tcd->inner() );

	}
	else if( tcd->type() == Muon::TgcCoinData::TYPE_SL ){
	  // SL triggers
	  ATH_MSG_DEBUG( "SL"  );

	  // Do flags and counters
	  if(m_earliestTrigger<0)m_earliestTrigger=pcn;
	  if(m_maxSLtrigger[ac][etaout][phi48][pcn]<pt){
	    m_maxSLtrigger[ac][etaout][phi48][pcn]=pt;
	  }
	  m_nSL[pcn]++;

	  // Get eta and phi RoI indexes
	  int etaroi,phiroi,roi;
	  roi=tcd->roi();
	  roi2etaphi(*tcd,etaroi,phiroi);
	  int pt=tcd->pt();

	  //for turn on curves and timing
	  m_sl_pt[pcn].push_back(pt);
	  m_sl_eta[pcn].push_back( eta );
	  m_sl_phi[pcn].push_back( phi );
	  m_sl_tidw[pcn].push_back( tcd->trackletId() );
	  m_sl_tids[pcn].push_back( tcd->trackletIdStrip() );
	  m_sl_roi[pcn].push_back( roi );
	  m_sl_etaout[pcn].push_back( etaout );
	  m_sl_phi48[pcn].push_back( tcd->phi() -1 );
	  m_sl_isAside[pcn].push_back( tcd->isAside() );
	  m_sl_isForward[pcn].push_back( tcd->isForward() );
	  m_sl_etaroi[pcn].push_back( etaroi );
	  m_sl_phiroi[pcn].push_back( phiroi );
	}
      }// loop TgcCoinDataContainer
    }// loop TGCCoinDataCollection
  }// if pcn !TOTA
  else{
    // TOTA timing
    // Add nTriggers to total
    m_nLptWire[3]  += m_nLptWire[pcn];
    m_nLptStrip[3] += m_nLptStrip[pcn];
    m_nHptWire[3]  += m_nHptWire[pcn];
    m_nHptStrip[3] += m_nHptStrip[pcn];
    m_nSL[3]       += m_nSL[pcn];
  }// if pcn TOTA

  return;
}// EOF


///////////////////////////////////////////////////////////////////////////
// readOfflineMuonContainer
///////////////////////////////////////////////////////////////////////////
// Reads Tgc Coincidence Data from container into vectors for use in filling histograms
StatusCode
TgcLv1RawDataValAlg::readOfflineMuonContainer(std::string key,
    vector<float>* mu_pt, vector<float>* mu_eta,
    vector<float>* mu_phi,vector<float>* mu_q){
  mu_pt->clear();
  mu_eta->clear();
  mu_phi->clear();
  mu_q->clear();

  const float ptcut = 1.0;
  const float etamin = 1.05;
  const float etamax = 2.4;

  const xAOD::MuonContainer* muonCont;

  StatusCode sc = (*m_activeStore)->retrieve(muonCont, key);
  if(sc.isFailure()){
    ATH_MSG_WARNING("Container of muon particle with key " << key << " not found in ActiveStore");
    return StatusCode::SUCCESS;
  }

  xAOD::MuonContainer::const_iterator it  = muonCont->begin();
  xAOD::MuonContainer::const_iterator ite = muonCont->end();

  for( ; it != ite; it++ ){
    float pt = (*it)->pt();
    float eta = (*it)->eta();
    float phi = (*it)->phi();
    if( fabs(pt) < ptcut || 
	fabs(eta) < etamin ||
	fabs(eta) > etamax ) continue;

    bool getvalue = true;

    uint8_t pixHits = 0; 
    getvalue = (*it)->summaryValue( pixHits,  xAOD::SummaryType::numberOfPixelHits);
    if(!getvalue) pixHits = 0;

    uint8_t sctHits = 0;
    getvalue = (*it)->summaryValue( sctHits,  xAOD::SummaryType::numberOfSCTHits);
    if(!getvalue) sctHits = 0;

    uint8_t trtHits = 0;
    getvalue = (*it)->summaryValue( trtHits, xAOD::SummaryType::numberOfTRTHits);
    if(!getvalue) trtHits = 0;

    uint8_t pixHoles = 0;
    getvalue = (*it)->summaryValue( pixHoles, xAOD::SummaryType::numberOfPixelHoles);
    if(!getvalue) pixHoles = 0;

    uint8_t sctHoles = 0;
    getvalue = (*it)->summaryValue( sctHoles, xAOD::SummaryType::numberOfSCTHoles);
    if(!getvalue) sctHoles = 0;

    uint8_t trtOL = 0;
    getvalue = (*it)->summaryValue( trtOL, xAOD::SummaryType::numberOfTRTOutliers);
    if(!getvalue) trtOL = 0;

    int trtOLfrac = 0;
    if( trtHits + trtOL > 0 )
      trtOLfrac = trtOL/(trtHits + trtOL);

    bool trt=false;
    if( fabs(eta) < 1.9 ){
      trt= ( ( trtHits > 5 ) && ( trtOLfrac < 0.9 ) );
    }
    else{
      if( trtHits > 5 ){
	trt =  ( trtOLfrac < 0.9 ) ;
      }
      else{
	trt=true;
      }
    }
    //int siliconHits = (*it)->numberOfPixelHits() + (*it)->numberOfSCTHits();
    //int phiHits = (*it)->numberOfRCPPhiHits() + (*it)->numberOfTGCPhiHits();
    //float matchChi2 = (*it)->matchChi2();

    //Muid MCP except phi hits
    if( key == "Muons" &&
	!( (*it)->combinedTrackParticleLink() &&
	  sctHits >= 6 &&
	  pixHits >= 2 &&
	  pixHoles+sctHoles <=1 &&
	  trt 
	 ) ) continue;


    /*
       if( !(*it)->combinedMuonTrackParticle() ||
    //( (*it)->numberOfMDTHits() < 5 &&
    //( (*it)->numberOfCSCEtaHits() < 3 || (*it)->numberOfCSCPhiHits() < 3 ) ) ||
    //(*it)->numberOfTGCPhiHits() < 1  ||
    (*it)->numberOfPixelHits() < 1 ||
    (*it)->numberOfSCTHits() < 6 ||
    (*it)->matchChi2() > 100 ) continue;
    */


    bool overlapped = false;

    for(unsigned int itr=0; itr<mu_eta->size(); itr++){
      float deta = fabs(mu_eta->at(itr) - eta);
      float dphi = fabs(mu_phi->at(itr) - phi);
      if(dphi > M_PI) dphi = 2*M_PI - dphi;
      if(sqrt(deta*deta + dphi*dphi) < 0.1){
	if(pt > mu_pt->at(itr)){
	  std::vector<float>::iterator ipt;
	  ipt = mu_pt->begin();
	  for(unsigned int j=0; j < itr; j++) ++ipt;
	  mu_pt->erase(ipt);

	  std::vector<float>::iterator ieta;
	  ieta = mu_eta->begin();
	  for(unsigned int j=0; j < itr; j++) ++ieta;
	  mu_eta->erase(ieta);

	  std::vector<float>::iterator iphi;
	  iphi = mu_phi->begin();
	  for(unsigned int j=0; j < itr; j++) ++iphi;
	  mu_phi->erase(iphi);

	  std::vector<float>::iterator iq;
	  iq = mu_q->begin();
	  for(unsigned int j=0; j < itr; j++) ++iq;
	  mu_q->erase(iq);

	} else overlapped = true;
      }
    }

    if(overlapped) continue;

    mu_pt->push_back ( pt );
    mu_eta->push_back( eta );
    mu_phi->push_back( phi );
    mu_q->push_back  ( (*it)->charge() );
  }

  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////
// readL1TriggerType
///////////////////////////////////////////////////////////////////////////
// Reads L1RoI data into TriggerType, L1RPC, L1Calo vectors from store
StatusCode
TgcLv1RawDataValAlg::readL1TriggerType(){

  m_L1TriggerType[0] = 0;//TGC
  m_L1TriggerType[1] = 0;//RPC low 
  m_L1TriggerType[2] = 0;//RPC high 
  m_L1TriggerType[3] = 0;//L1Calo

  m_L1RPCetas.clear();
  m_L1RPCphis.clear();
  m_L1Caloetas.clear();
  m_L1Calophis.clear();

  ///////////////////////////////////////////////////
  //fill number of RPC or TGC RoI and position of RoI
  const xAOD::MuonRoIContainer* muonRoIs; 
  StatusCode sc = (*m_activeStore)->retrieve(muonRoIs, m_L1muonRoIName);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_WARNING( " Cannot get LVL1 muon ROI "  );
    return sc ;
  }
  xAOD::MuonRoIContainer::const_iterator mu_it = muonRoIs->begin(); 
  xAOD::MuonRoIContainer::const_iterator mu_ite= muonRoIs->end(); 
  for( ;
      mu_it != mu_ite;
      mu_it++){
    //RPC
    if( (*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Barrel ){

      if( (*mu_it)->getThrNumber() <= 3 ){
	m_L1TriggerType[1]++;//low PT
      }
      else{
	m_L1TriggerType[2]++;//high PT
      }
      m_L1RPCetas.push_back((*mu_it)->eta());
      m_L1RPCphis.push_back((*mu_it)->phi());

      //TGC
    }
    else if ( (*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Endcap ||
	(*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Forward ){
      m_L1TriggerType[0]++;
      m_L1TGCetas.push_back((*mu_it)->eta());
      m_L1TGCphis.push_back((*mu_it)->phi());
    }
  }

  ///////////////////////////////////////////////////
  //fill number of L1Calo RoI
  const xAOD::EmTauRoIContainer* emtauRoIs; 
  sc = (*m_activeStore)->retrieve(emtauRoIs, m_L1emtauRoIName);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_WARNING( " Cannot get LVL1 emtau ROI "  );
    return sc ;
  }
  xAOD::EmTauRoIContainer::const_iterator em_it = emtauRoIs->begin(); 
  xAOD::EmTauRoIContainer::const_iterator em_ite= emtauRoIs->end(); 
  for( ;
      em_it != em_ite;
      em_it++){

    m_L1TriggerType[3]++;
    m_L1Caloetas.push_back((*em_it)->eta());
    m_L1Calophis.push_back((*em_it)->phi());
  }


  const xAOD::JetRoIContainer* jetRoIs; 
  sc = (*m_activeStore)->retrieve(jetRoIs, m_L1jetRoIName);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_WARNING( " Cannot get LVL1 jet ROI "  );
    return sc ;
  }
  xAOD::JetRoIContainer::const_iterator j_it = jetRoIs->begin(); 
  xAOD::JetRoIContainer::const_iterator j_ite= jetRoIs->end(); 
  for( ;
      j_it != j_ite;
      j_it++){

    m_L1TriggerType[3]++;
    m_L1Caloetas.push_back((*j_it)->eta());
    m_L1Calophis.push_back((*j_it)->phi());
  }


  //*  const xAOD::JetEtRoI* jetetRoIs; 
  //*  sc = (*m_activeStore)->retrieve(jetetRoIs);
  //*  if (sc != StatusCode::SUCCESS ) {
  //*    m_log << MSG::WARNING << " Cannot get LVL1 jetet ROI " << endmsg;
  //*    return sc ;
  //*  }
  //*  m_L1TriggerType[3]++;
  //*  m_L1Caloetas.push_back(50.);//no position of RoI for JetEt trigger
  //*  m_L1Calophis.push_back(50.);//no position of RoI for JetEt trigger

  const xAOD::EnergySumRoI* esumRoIs; 
  sc = (*m_activeStore)->retrieve(esumRoIs);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_WARNING( " Cannot get LVL1 jetet ROI "  );
    return sc ;
  }
  m_L1TriggerType[3]++;
  m_L1Caloetas.push_back(100.);//no position of RoI for Esum trigger
  m_L1Calophis.push_back(100.);//no position of RoI for Esum trigger

  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////
// readEventInfo
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::readEventInfo(){
  static int errcnt = 0;
  // Reset event index variables
  m_lumiblock = -1;
  m_event     = -1;
  m_BCID      = -1;

  // Get Event Info DataHandle
  const DataHandle<xAOD::EventInfo> event_handle;
  ATH_CHECK( (*m_activeStore)->retrieve(event_handle) );

  // Get event index variables
  m_lumiblock = event_handle->lumiBlock() ;
  m_event     = event_handle->eventNumber() ;
  m_BCID      = event_handle->bcid() ;

  // **********************************************
  // Get Stream Tags
  const std::vector<xAOD::EventInfo::StreamTag> &stream = event_handle->streamTags();

  // Search Stream Tags for express stream
  m_found_express_stream = false;
  for(auto stag : stream) {
    if(stag.type()=="express" && stag.name()=="express") {
      m_found_express_stream = true;
      break;
    }
  }

  if(m_found_express_stream){
    // Search Store for non-muon express chain
    m_found_nonmuon_express_chain = false;
    bool continuesearch=true;

    // Check that store contains HLT express
    const std::string key = "HLT_EXPRESS_OPI_HLT";
    if( !(*m_activeStore)->contains<TrigOperationalInfoCollection>(key) ){
      continuesearch=false;
      if (errcnt < 1) {
	ATH_MSG_INFO(" Missing TrigOperationalInfoCollection: key= " << key  );
	errcnt++;
      }
    }

    // Get TrigOperationalInfoCollection from store
    const TrigOperationalInfoCollection* opi=0;
    if(continuesearch){
      if( !(*m_activeStore)->retrieve<TrigOperationalInfoCollection>(opi,key).isSuccess() ){
	continuesearch=false;
	if (errcnt < 1) {
	  ATH_MSG_INFO(" Failed to retrieve TrigOperationalInfoCollection: key= " << key  );
	  errcnt++;
	}
      }
    }

    // Search Collection's express chains for at least one non-muon express chain
    if(continuesearch){
      for(TrigOperationalInfoCollection::const_iterator it=opi->begin();it!=opi->end();++it){// trigopinfo
	const TrigOperationalInfo *ptr = *it;
	if(!ptr)continue;

	m_expressChains = ptr->infos();
	for(unsigned i=0;i<m_expressChains.first.size();++i){// express chains
	  // Check that at least one express trigger chain is not muon related
	  TString s = m_expressChains.first.at(i);
	  if( !s.Contains("mu") ){
	    m_found_nonmuon_express_chain = true;
	    ATH_MSG_INFO( " non-muon express chain found " << s  );
            continuesearch=false;
	    break;
	  }
	}// express chains
	if(!continuesearch) break;
      }// trigopinfo
    }

  }// if found express stream

  return StatusCode::SUCCESS;
}
