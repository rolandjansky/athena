/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetMonitorAlgorithm.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"   //EN

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "Particle/TrackParticleContainer.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrigParticle/TrigEFBjet.h"
#include "TrigParticle/TrigEFBjetContainer.h"

#include "JetEvent/JetCollection.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

// Calculates the track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"



TrigBjetMonitorAlgorithm::TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  ,m_doRandom(true)
  ,m_allChains{}
  ,m_muonContainerKey("Muons")
  ,m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty ("AllChains", m_allChains);
  declareProperty("MuonContainerName",m_muonContainerKey);
}


TrigBjetMonitorAlgorithm::~TrigBjetMonitorAlgorithm() {}


StatusCode TrigBjetMonitorAlgorithm::initialize() {
  ATH_CHECK( m_muonContainerKey.initialize() );

  ATH_CHECK( m_offlineVertexContainerKey.initialize() );
  ATH_CHECK( m_onlineVertexContainerKey.initialize() );
  ATH_CHECK( m_onlineTrackContainerKey.initialize() );
  ATH_CHECK( m_onlineBTaggingContainerKey.initialize() );

  return AthMonitorAlgorithm::initialize();
}

bool LLR(double pu, double pc, double pb, double &w)  {
  w = 0.;
  bool ll = false;
  double denom;
  float cfrac(0.08);
  if (pb > 0.) {
    denom = pu*(1.-cfrac)+pc*cfrac;
    if (denom > 0.) {
      w = log(pb/denom);
      ll = true;
    }
  }
  return ll; 
}

StatusCode TrigBjetMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;


  // Read off-line PV's  and fill histograms 

  auto OffNVtx = Monitored::Scalar<int>("Off_NVtx",0);
  auto OffxVtx = Monitored::Scalar<float>("Off_xVtx",0.0);
  auto OffyVtx = Monitored::Scalar<float>("Off_yVtx",0.0);
  auto OffzVtx = Monitored::Scalar<float>("Off_zVtx",0.0);

  SG::ReadHandle<xAOD::VertexContainer> offlinepv = SG::makeHandle( m_offlineVertexContainerKey, ctx );
  if (! offlinepv.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain VertexContainer Collection with name "<< m_offlineVertexContainerKey);
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG(" Size of the Off-line PV container: " << offlinepv->size() );
  OffNVtx = offlinepv->size() ;
  for (unsigned int j = 0; j<offlinepv->size(); j++){
    OffxVtx = (*(offlinepv))[j]->x();
    OffyVtx = (*(offlinepv))[j]->y();
    OffzVtx = (*(offlinepv))[j]->z();
    fill("TrigBjetMonitor",OffxVtx,OffyVtx,OffzVtx);
  }
  fill("TrigBjetMonitor",OffNVtx);
  

  // print the trigger chain names 

  std::string chainName;
  
  int size_AllChains = m_allChains.size();
  ATH_MSG_DEBUG(" Size of the AllChains trigger container: " << size_AllChains );
  for (int i =0; i<size_AllChains; i++){
    chainName = m_allChains.at(i);
    ATH_MSG_DEBUG("  Chain number: " << i << " AllChains Chain Name: " << chainName );
  }
  
  // Verifiy if the trigger chain was fired and if yes, fill the corresponding histogram
  
  bool mujetChain = false;
  bool bjetChain = true;
  
  

  for ( auto& trigName : m_allChains ) {

    
    if ( m_trigDecTool->isPassed(trigName) ) {
      ATH_MSG_DEBUG(" Trigger chain from AllChains list: " << trigName << " has fired !!! " );

      // bjet vs mujet
      std::size_t found = trigName.find("HLT_mu");
      if (found!=std::string::npos) {
	mujetChain = true;
	bjetChain = false;
      }// found
      
      
      ATH_MSG_DEBUG("  ===> Run 3 access to Trigger Item: " << trigName);
      
      // online track container 
      SG::ReadHandle<xAOD::TrackParticleContainer> theTracks(m_onlineTrackContainerKey, ctx);
      // verify the content 
      for ( const xAOD::TrackParticle* track : *theTracks ) {
	ATH_MSG_DEBUG( " Pt of track in TrackParticleContainer: " << track->pt() );
      }

      // bjet chains
      if (bjetChain) {
	// online PV 
	SG::ReadHandle<xAOD::VertexContainer> vtxContainer = SG::makeHandle( m_onlineVertexContainerKey, ctx );
	int nPV = 0;
	for (const xAOD::Vertex* vtx : *vtxContainer) {
	  if (vtx->vertexType() == xAOD::VxType::PriVtx) {
	    nPV++;
	    std::string NameH = "PVz_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto PVz_tr = Monitored::Scalar<float>(NameH,0.0);
	    PVz_tr = vtx->z();
	    ATH_MSG_DEBUG("        PVz_tr: " << PVz_tr);
	    fill("TrigBjetMonitor",PVz_tr);
	    NameH = "PVx_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto PVx_tr = Monitored::Scalar<float>(NameH,0.0);
	    PVx_tr = vtx->x();
	    ATH_MSG_DEBUG("        PVx_tr: " << PVx_tr);
	    fill("TrigBjetMonitor",PVx_tr);
	    NameH = "PVy_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto PVy_tr = Monitored::Scalar<float>(NameH,0.0);
	    PVy_tr = vtx->y();
	    ATH_MSG_DEBUG("        PVy_tr: " << PVy_tr);
	    fill("TrigBjetMonitor",PVy_tr);
	  } // if vtx type
	} // loop on vtxContainer
	std::string NpvH = "nPV_tr_"+trigName;
	ATH_MSG_DEBUG( " NpvH: " << NpvH  );
	auto nPV_tr = Monitored::Scalar<int>(NpvH,0.0);
	nPV_tr = nPV;
	fill("TrigBjetMonitor",nPV_tr);
      } // if bjetChain
      
	//bjet or mujet chains 
      if (bjetChain || mujetChain) {
	
	// Jets and PV and tracks through jet link

	std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > onlinejets = m_trigDec->features<xAOD::JetContainer>(trigName, TrigDefs::Physics, m_onlineBjetContainerKey);
	
	int ijet = 0;
	int itrack = 0;
	std::string nJetH = "nJet_"+trigName;
	auto nJet = Monitored::Scalar<int>(nJetH,0.0);
	nJet = onlinejets.size();
	fill("TrigBjetMonitor",nJet);
	for(const auto& jetLinkInfo : onlinejets) {
	  // jetPt
	  const xAOD::Jet* jet = *(jetLinkInfo.link);
	  std::string NameH = "jetPt_"+trigName;
	  ATH_MSG_DEBUG( " NameH: " << NameH  );
	  auto jetPt = Monitored::Scalar<float>(NameH,0.0);
	  jetPt = (jet->pt())*1.e-3;
	  ATH_MSG_DEBUG("        jetPt: " << jetPt);
	  fill("TrigBjetMonitor",jetPt);
	  NameH = "jetEta_"+trigName;
	  ATH_MSG_DEBUG( " NameH: " << NameH  );
	  auto jetEta = Monitored::Scalar<float>(NameH,0.0);
	  jetEta = jet->eta();
	  NameH = "jetPhi_"+trigName;
	  ATH_MSG_DEBUG( " NameH: " << NameH  );
	  auto jetPhi = Monitored::Scalar<float>(NameH,0.0);
	  jetPhi = jet->phi();
	  ATH_MSG_DEBUG("        jetEta: " << jetEta << " jetPhi : " << jetPhi);
	  fill("TrigBjetMonitor",jetEta,jetPhi);
	  // zPV associated to the jets in the same event: they are the same for every jet in the same event so only the first zPV should be plotted
	  if (ijet == 0) {
	    
	    // Fetch and plot PV
	    
	    std::string vtxname = m_onlineVertexContainerKey.key();
	    if ( vtxname.find("HLT_")==0 ) vtxname.erase(0,4);
	    auto vertexLinkInfo = TrigCompositeUtils::findLink<xAOD::VertexContainer>(jetLinkInfo.source, vtxname ); // CV 200120 & MS 290620
	    ATH_CHECK( vertexLinkInfo.isValid() ) ; // TM 200120
	    const xAOD::Vertex* vtx = *(vertexLinkInfo.link);
	    NameH = "PVz_jet_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto PVz_jet = Monitored::Scalar<float>(NameH,0.0);
	    PVz_jet = vtx->z();
	    ATH_MSG_DEBUG("        PVz_jet: " << PVz_jet);
	    fill("TrigBjetMonitor",PVz_jet);
	    NameH = "PVx_jet_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto PVx_jet = Monitored::Scalar<float>(NameH,0.0);
	    PVx_jet = vtx->x();
	    ATH_MSG_DEBUG("        PVx_jet: " << PVx_jet);
	    fill("TrigBjetMonitor",PVx_jet);
	    NameH = "PVy_jet_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto PVy_jet = Monitored::Scalar<float>(NameH,0.0);
	    PVy_jet = vtx->y();
	    ATH_MSG_DEBUG("        PVy_jet: " << PVy_jet);
	    fill("TrigBjetMonitor",PVy_jet);
	    
	    // Fetch and plot BTagging information
	    
	    std::string btagname = m_onlineBTaggingContainerKey.key();
	    if ( btagname.find("HLT_")==0 ) btagname.erase(0,4);
	    auto btaggingLinkInfo = TrigCompositeUtils::findLink<xAOD::BTaggingContainer>(jetLinkInfo.source, btagname );
	    ATH_CHECK( btaggingLinkInfo.isValid() ) ;
	    const xAOD::BTagging* btag = *(btaggingLinkInfo.link);
	    
	    // IP3D variables
	    NameH = "IP3D_pu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto IP3D_pu = Monitored::Scalar<float>(NameH,0.0);
	    IP3D_pu = btag->IP3D_pu();
	    ATH_MSG_DEBUG("        IP3D_pu: " << IP3D_pu);
	    fill("TrigBjetMonitor",IP3D_pu);
	    
	    NameH = "IP3D_pb_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto IP3D_pb = Monitored::Scalar<float>(NameH,0.0);
	    IP3D_pb = btag->IP3D_pb();
	    ATH_MSG_DEBUG("        IP3D_pb: " << IP3D_pb);
	    fill("TrigBjetMonitor",IP3D_pb);
	    
	    NameH = "IP3D_pc_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto IP3D_pc = Monitored::Scalar<float>(NameH,0.0);
	    IP3D_pc = btag->IP3D_pc();
	    ATH_MSG_DEBUG("        IP3D_pc: " << IP3D_pc);
	    fill("TrigBjetMonitor",IP3D_pc);
	    
	    // LogLH variables
	    NameH = "wIP3D_Rbu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto wIP3D = Monitored::Scalar<double>(NameH,0.0);
	    btag->loglikelihoodratio("IP3D", wIP3D);
	    ATH_MSG_DEBUG("        wIP3D: " << wIP3D);
	    fill("TrigBjetMonitor",wIP3D);
	    
	    NameH = "wSV1_Rbu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto wSV1 = Monitored::Scalar<double>(NameH,0.0);
	    btag->loglikelihoodratio("SV1", wSV1);
	    ATH_MSG_DEBUG("        wSV1: " << wSV1);
	    fill("TrigBjetMonitor",wSV1);
	    
	    NameH = "wCOMB_Rbu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto wCOMB = Monitored::Scalar<double>(NameH,0.0);
	    wCOMB = wIP3D+wSV1;
	    ATH_MSG_DEBUG("        wCOMB: " << wCOMB);
	    fill("TrigBjetMonitor",wCOMB);
	    
	    // Discriminants
	    NameH = "wMV2c10_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto wMV2c10 = Monitored::Scalar<double>(NameH,0.0);
	    wMV2c10 = btag->auxdata<double>("MV2c10_discriminant");
	    ATH_MSG_DEBUG("        wMV2c10: " << wMV2c10);
	    fill("TrigBjetMonitor",wMV2c10);
	    
	    // SV1 variables (credit LZ)
	    NameH = "xNVtx_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto svp_n2t = Monitored::Scalar<int>(NameH,0.0);
	    btag->variable<int>("SV1", "N2Tpair", svp_n2t);
	    ATH_MSG_DEBUG("        svp_n2t: " << svp_n2t);
	    fill("TrigBjetMonitor",svp_n2t);
	    
	    NameH = "xMVtx_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto svp_mass = Monitored::Scalar<float>(NameH,0.0);
	    btag->variable<float>("SV1", "masssvx", svp_mass);
	    svp_mass *= 1.e-3;
	    ATH_MSG_DEBUG("        svp_mass in GeV: " << svp_mass );
	    fill("TrigBjetMonitor",svp_mass);
	    
	    NameH = "xEVtx_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto svp_efrc = Monitored::Scalar<float>(NameH,0.0);
	    btag->variable<float>("SV1", "efracsvx", svp_efrc);
	    ATH_MSG_DEBUG("        svp_efrc: " << svp_efrc);
	    fill("TrigBjetMonitor",svp_efrc);
	    
	    // Run-3 discriminators
	    
	    NameH = "RNNIP_pu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto RNNIP_pu = Monitored::Scalar<double>(NameH,0.0);
	    btag->pu("rnnip",RNNIP_pu);
	    ATH_MSG_DEBUG("        RNNIP_pu: " << RNNIP_pu);
	    fill("TrigBjetMonitor",RNNIP_pu);
	    
	    NameH = "RNNIP_pc_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto RNNIP_pc = Monitored::Scalar<double>(NameH,0.0);
	    btag->pc("rnnip",RNNIP_pc);
	    ATH_MSG_DEBUG("        RNNIP_pc: " << RNNIP_pc);
	    fill("TrigBjetMonitor",RNNIP_pc);
	    
	    NameH = "RNNIP_pb_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto RNNIP_pb = Monitored::Scalar<double>(NameH,0.0);
	    btag->pb("rnnip",RNNIP_pb);
	    ATH_MSG_DEBUG("        RNNIP_pb: " << RNNIP_pb);
	    fill("TrigBjetMonitor",RNNIP_pb);
	    

	    NameH = "DL1_pu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1_pu = Monitored::Scalar<double>(NameH,0.0);
	    btag->pu("DL1",DL1_pu);
	    ATH_MSG_DEBUG("        DL1_pu: " << DL1_pu);
	    fill("TrigBjetMonitor",DL1_pu);

	    NameH = "DL1_pc_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1_pc = Monitored::Scalar<double>(NameH,0.0);
	    btag->pc("DL1",DL1_pc);
	    ATH_MSG_DEBUG("        DL1_pc: " << DL1_pc);
	    fill("TrigBjetMonitor",DL1_pc);

	    NameH = "DL1_pb_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1_pb = Monitored::Scalar<double>(NameH,0.0);
	    btag->pb("DL1",DL1_pb);
	    ATH_MSG_DEBUG("        DL1_pb: " << DL1_pb);
	    fill("TrigBjetMonitor",DL1_pb);

	    NameH = "DL1_mv_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1_mv = Monitored::Scalar<double>(NameH,0.0);
	    if ( LLR (DL1_pu, DL1_pc, DL1_pb, DL1_mv) ) fill("TrigBjetMonitor",DL1_mv);
	    ATH_MSG_DEBUG("        DL1_mv: " << DL1_mv << " LLR: " << LLR); 


	    NameH = "DL1r_pu_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1r_pu = Monitored::Scalar<double>(NameH,0.0);
	    btag->pu("DL1r",DL1r_pu);
	    ATH_MSG_DEBUG("        DL1r_pu: " << DL1r_pu);
	    fill("TrigBjetMonitor",DL1r_pu);

	    NameH = "DL1r_pc_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1r_pc = Monitored::Scalar<double>(NameH,0.0);
	    btag->pc("DL1r",DL1r_pc);
	    ATH_MSG_DEBUG("        DL1r_pc: " << DL1r_pc);
	    fill("TrigBjetMonitor",DL1r_pc);

	    NameH = "DL1r_pb_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1r_pb = Monitored::Scalar<double>(NameH,0.0);
	    btag->pb("DL1r",DL1r_pb);
	    ATH_MSG_DEBUG("        DL1r_pb: " << DL1r_pb);
	    fill("TrigBjetMonitor",DL1r_pb);

	    NameH = "DL1r_mv_tr_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto DL1r_mv = Monitored::Scalar<double>(NameH,0.0);
	    if ( LLR (DL1r_pu, DL1r_pc, DL1r_pb, DL1r_mv) ) fill("TrigBjetMonitor",DL1r_mv);
	    ATH_MSG_DEBUG("        DL1r_mv: " << DL1r_mv << " LLR: " << LLR); 

	  } // if (ijet == 0)
	  
	  ijet++;
	  
	  // Tracks associated to triggered jets ( featurs = onlinejets ) courtesy of Tim Martin on 12/05/2020 
	  const auto track_it_pair = m_trigDec->associateToEventView(theTracks, jetLinkInfo.source, "roi");
	  const xAOD::TrackParticleContainer::const_iterator start_it = track_it_pair.first;
	  const xAOD::TrackParticleContainer::const_iterator end_it = track_it_pair.second;
	  
	  int count = 0;
	  for ( xAOD::TrackParticleContainer::const_iterator it = start_it; it != end_it; ++it) {
	    count++;
	    ATH_MSG_DEBUG( " Track " << count << " with pT " << (*it)->pt() <<" from BJet with pT " << (*jetLinkInfo.link)->pt() );
	    ATH_MSG_DEBUG( " Track " << count << " with pT/eta/phi " << (*it)->pt() << "/" << (*it)->eta() << "/" << (*it)->phi() );
	    ATH_MSG_DEBUG( " Track " << count << " with d0/sigd0 " << (*it)->d0() << "/" << Amg::error((*it)->definingParametersCovMatrix(), 0) );
	    ATH_MSG_DEBUG( " Track " << count << " with z0/sigz0 " << (*it)->z0() << "/" << Amg::error((*it)->definingParametersCovMatrix(), 1) );
	    std::string NameH = "trkPt_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto trkPt = Monitored::Scalar<float>(NameH,0.0);
	    trkPt = ((*it)->pt())*1.e-3;
	    ATH_MSG_DEBUG("        trkPt: " << trkPt);
	    fill("TrigBjetMonitor",trkPt);
	    NameH = "trkEta_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto trkEta = Monitored::Scalar<float>(NameH,0.0);
	    trkEta = (*it)->eta();
	    NameH = "trkPhi_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto trkPhi = Monitored::Scalar<float>(NameH,0.0);
	    trkPhi = (*it)->phi();
	    ATH_MSG_DEBUG("        trkEta: " << trkEta << " trkPhi : " << trkPhi);
	    fill("TrigBjetMonitor",trkEta,trkPhi);
	    NameH = "d0_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto d0 = Monitored::Scalar<float>(NameH,0.0);
	    d0 = (*it)->d0();
	    ATH_MSG_DEBUG("        d0: " << d0);
	    fill("TrigBjetMonitor",d0);
	    NameH = "z0_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto z0 = Monitored::Scalar<float>(NameH,0.0);
	    z0 = (*it)->z0();
	    ATH_MSG_DEBUG("        z0: " << z0);
	    fill("TrigBjetMonitor",z0);
	    NameH = "ed0_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto ed0 = Monitored::Scalar<float>(NameH,0.0);
	    ed0 = Amg::error((*it)->definingParametersCovMatrix(), 0);
	    ATH_MSG_DEBUG("        ed0: " << ed0);
	    fill("TrigBjetMonitor",ed0);
	    NameH = "ez0_"+trigName;
	    ATH_MSG_DEBUG( " NameH: " << NameH  );
	    auto ez0 = Monitored::Scalar<float>(NameH,0.0);
	    ez0 = Amg::error((*it)->definingParametersCovMatrix(), 1);
	    ATH_MSG_DEBUG("        ez0: " << ez0);
	    fill("TrigBjetMonitor",ez0);
	  } // it on tracks
	  ATH_MSG_DEBUG( "  Number of tracks: " << count );
	  itrack += count;
	  
	} // jetLinkInfo from onlinejets
	
	ATH_MSG_DEBUG("  Total number of triggered b-jets: " << ijet << " nJet : " << nJet);
	ATH_MSG_DEBUG(" Total number of triggered tracks associated to the b-jets: " << itrack);
	std::string nTrackH = "nTrack_"+trigName;
	auto nTrack = Monitored::Scalar<int>(nTrackH,0.0);
	nTrack = itrack;
	fill("TrigBjetMonitor",nTrack);
	
      } //bjet or mujet
       
    } else {
      ATH_MSG_DEBUG( " Trigger chain from AllChains list: " << trigName << " has not fired "  );
    } // trigger not fired
    
    
  } // for AllChains

  return StatusCode::SUCCESS;
}
