/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetMonitorAlgorithm.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

#include "JetEvent/JetCollection.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

TrigBjetMonitorAlgorithm::TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  ,m_doRandom(true)
  ,m_allChains{}
  ,m_muonContainerKey("Muons")
  ,m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
  ,m_doRun2(false)
{
  declareProperty ("AllChains", m_allChains);
  declareProperty("MuonContainerName",m_muonContainerKey);
  declareProperty("doRun2",m_doRun2);
}


TrigBjetMonitorAlgorithm::~TrigBjetMonitorAlgorithm() {}


StatusCode TrigBjetMonitorAlgorithm::initialize() {
  ATH_CHECK( m_muonContainerKey.initialize() );

  ATH_CHECK( m_offlineVertexContainerKey.initialize() );
  ATH_CHECK( m_onlineVertexContainerKey.initialize() );

  return AthMonitorAlgorithm::initialize();
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

  // Define keys of retrival

  std::string trackKey  = "";
  std::string jetKey  = "";
  std::string priVtxKey  = "EFHistoPrmVtx";
  bool FTKChain = false;
  bool mujetChain = false;
  bool bjetChain = true;
  bool splitChain = false;
  


  for ( auto& trigName : m_allChains ) {

    
    // Access to TrigFeature
    //    bool Run2_Access = true;
    //    bool Run2_Access = false;

    
    if ( m_trigDecTool->isPassed(trigName) ) {
      ATH_MSG_DEBUG(" Trigger chain from AllChains list: " << trigName << " has fired !!! " );


      // Trigger type

      // split vs unsplit
      std::size_t found = trigName.find("split");
      if (found!=std::string::npos) {
	splitChain = true;
	jetKey = "SplitJet";
	priVtxKey = "xPrimVx";
	trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
      }// found

      // FTK vs non FTK
      std::size_t found1 = trigName.find("FTK");
      if (found1!=std::string::npos) {
	priVtxKey = "PrimVertexFTK";
	trackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig";
	std::size_t found2 = trigName.find("FTKRefit");
	if (found2!=std::string::npos) {
	  trackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig";
	}// found2
	std::size_t found3 = trigName.find("FTKVtx");
	if (found3!=std::string::npos) {
	  trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
	}// found3
      }// found1

      // gsc vs non-gsc chain
      std::size_t found4 = trigName.find("gsc");
      if (found4!=std::string::npos) {
	jetKey = "GSCJet";
      }// found4

      // bjet vs mujet
      found = trigName.find("HLT_mu");
      if (found!=std::string::npos) {
	mujetChain = true;
	bjetChain = false;
      }// found

      ATH_MSG_DEBUG( " Trigger type: bjetChain " << bjetChain << " mujetChain " << mujetChain << " FTKChain " << FTKChain << " splitChain " << splitChain );
      ATH_MSG_DEBUG( " Keys -- priVtxKey:  " << priVtxKey << " jetKey: " << jetKey << " trackKey: " << trackKey );

      // Read the TrigFeature contener

      if (m_doRun2) { // Run 2 trigger

	ATH_MSG_DEBUG("  ===> Run 2 access to Trigger feature: " );
	Trig::FeatureContainer fc = m_trigDec->features(trigName);
	const std::vector< Trig::Combination >& triggerCombs = fc.getCombinations();
	ATH_MSG_DEBUG("RETRIEVED " << triggerCombs.size() << " COMBINATIONS FOR "  << trigName);
	ATH_MSG_DEBUG( " Size of triggerCombs : " << triggerCombs.size() );
	// Take all combinations for this b-jet trigger
	std::vector< Trig::Combination >::const_iterator triggerComb;
	for( triggerComb = triggerCombs.begin(); triggerComb != triggerCombs.end(); ++triggerComb ) {
	  const Trig::Combination& comb = *triggerComb;

	  // bjet chains
	  if (bjetChain) {

	    // online PV 
	    const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs = comb.get<xAOD::VertexContainer>(priVtxKey);
	    ATH_MSG_DEBUG("RETRIEVED PV  -   size: " << onlinepvs.size());
	    if ( not onlinepvs.empty() ) {
	      const xAOD::VertexContainer* onlinepv = onlinepvs[0].cptr();
	      ATH_MSG_DEBUG("   for VertexContainer: " << priVtxKey << " nVert: " << onlinepv->size());
	      if( not onlinepv->empty()) {
		// PVz_tr 
		std::string NameH = "PVz_tr_"+trigName;
		ATH_MSG_DEBUG( " NameH: " << NameH );
		auto PVz_tr = Monitored::Scalar<float>(NameH,0.0);
		PVz_tr = (*(onlinepv))[0]->z();
		ATH_MSG_DEBUG("        PVz_tr: " << PVz_tr);
		fill("TrigBjetMonitor",PVz_tr);
	      }// onlinepv
	    }// onlinepvs

	    // Tracks 
	    const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>(trackKey);
	    ATH_MSG_DEBUG("RETRIEVED TRACKS -   size: " << onlinetracks.size());
	    if ( not onlinetracks.empty() ) { 
	      const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
	      ATH_MSG_DEBUG("                 -   nTrack: " << onlinetrack->size());
	      for(const auto* trk : *onlinetrack) {
		// d0 
		std::string NameH = "d0_"+trigName;
		ATH_MSG_DEBUG( " NameH: " << NameH  );
		auto d0 = Monitored::Scalar<float>(NameH,0.0);
		d0 = trk->d0();
		ATH_MSG_DEBUG("        d0: " << d0);
		fill("TrigBjetMonitor",d0);
	      }// onlinetrack
	    }// onlinetracks
	    
	    // b-tagged jets 
	    const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
	    ATH_MSG_DEBUG("RETRIEVED BJETS  from xAOD BTaggingContainer -   size: " << onlinebjets.size());
	    if(not onlinebjets.empty()) { 
	      const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
	      ATH_MSG_DEBUG("                 -   nBjet: " << onlinebjet->size());
	      for(const auto* bjet : *onlinebjet) {
		// wMV2c20 
		std::string NameH = "wMV2c20_"+trigName;
		ATH_MSG_DEBUG( " NameH: " << NameH  );
		auto wMV2c20 = Monitored::Scalar<double>(NameH,0.0);
		wMV2c20 = bjet->auxdata<double>("MV2c20_discriminant");
		ATH_MSG_DEBUG("        wMV2c20: " << wMV2c20);
		fill("TrigBjetMonitor",wMV2c20);
	      }// onlinebjet
	    }// onlinebjets

	  }//bjetChain

	  //bjet or mujet chains
	  if (bjetChain || mujetChain) {

	    // Jets 
	    const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>(jetKey);
	    ATH_MSG_DEBUG("RETRIEVED JETS   -   size: " << onlinejets.size());
	    if( not onlinejets.empty()) { 
	      const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
	      ATH_MSG_DEBUG("                 -   nJet: " << onlinejet->size());
	      for(const auto* jet : *onlinejet) {
		// jetPt 
		std::string NameH = "jetPt_"+trigName;
		ATH_MSG_DEBUG( " NameH: " << NameH  );
		auto jetPt = Monitored::Scalar<float>(NameH,0.0);
		jetPt = (jet->pt())*1.e-3;
		ATH_MSG_DEBUG("        jetPt: " << jetPt);
		fill("TrigBjetMonitor",jetPt);
	      }// onlinejet
	    }// onlinejets
	    
	  }//bjet or mujet

	  // mujet chains

	  if (mujetChain) {

	    // muons  and jets  
	    const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>(jetKey);
	    ATH_MSG_DEBUG("RETRIEVED JETS   -   size: " << onlinejets.size());
	    const std::vector< Trig::Feature<xAOD::MuonContainer> > onlinemuons = comb.get<xAOD::MuonContainer>();
	    ATH_MSG_DEBUG("RETRIEVED MUONS   -   size: " << onlinemuons.size());
	    if( not onlinemuons.empty()) { 
	      const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	      ATH_MSG_DEBUG("                 -   nMuon: " << onlinemuon->size());
	      for(const auto* muon : *onlinemuon) {
		// plot muon attributes: pT, eta, phi, etc .... independently of the muon type
		const xAOD::Muon::MuonType muontype = muon->muonType();
		if( muontype != xAOD::Muon::MuonType::Combined ) continue;
		// if muon type is "Combined" and jet container is not empty loop on jets and plot muon-jet combined quantitites
		//   see Run2 code: https://gitlab.cern.ch/atlas/athena/blob/21.3/Trigger/TrigMonitoring/TrigBjetMonitoring/src/HLTBjetMonTool.cxx 
		if( not onlinejets.empty()) { 
		  const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
		  for(const auto* jet : *onlinejet) {
		    // jetPt 
		    std::string NameH = "jetPt_"+trigName;
		    ATH_MSG_DEBUG( " NameH: " << NameH  );
		    auto jetPt = Monitored::Scalar<float>(NameH,0.0);
		    jetPt = (jet->pt())*1.e-3;
		    ATH_MSG_DEBUG("        jetPt: " << jetPt);
		    fill("TrigBjetMonitor",jetPt);
		  }// onlinejet
		}// onlinejets
	      }// onlinemuon
	    }// onlinemuons
	    
	  }//mujetChain
	  
	}// trigger combinations
	
      } else { // Run 3 trigger
	ATH_MSG_DEBUG("  ===> Run 3 access to Trigger feature: " );

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

	  // Jets and PV through jet link
	  // std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > onlinejets = m_trigDec->features<xAOD::JetContainer>(trigName, TrigDefs::Physics, jetKey);
	  std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > onlinejets = m_trigDec->features<xAOD::JetContainer>(trigName, TrigDefs::Physics, m_onlineBjetContainerKey);
	  // std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > onlinejets = m_trigDec->features<xAOD::JetContainer>(trigName);
	  int ijet = 0;
	  std::string nJetH = "nJet_"+trigName;
	  auto nJet = Monitored::Scalar<int>(nJetH,0.0);
	  nJet = onlinejets.size();
	  fill("TrigBjetMonitor",nJet);
	  for(const auto jetLinkInfo : onlinejets) {
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
	      auto vertexLinkInfo = TrigCompositeUtils::findLink<xAOD::VertexContainer>(jetLinkInfo.source, "EFHistoPrmVtx"); // CV 200120
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
	    }
	    ijet++;
	  } // onlinejets
	  ATH_MSG_DEBUG("  ijet : " << ijet << " nJet : " << nJet);
	} //bjet or mujet
      } // else Run3  

    } else {
      ATH_MSG_DEBUG( " Trigger chain from AllChains list: " << trigName << " has not fired "  );
    } // trigger not fired
    

  } // for AllChains

    return StatusCode::SUCCESS;
}
