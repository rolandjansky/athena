/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonTree.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "TROOT.h"
#include "TVector3.h"

namespace Muon{

  MuonTree::MuonTree(PlotBase* pParent, const std::string& sDir, bool mcFlag):
  	PlotBase(pParent, sDir),
    	m_tree(nullptr),
     m_isMC(mcFlag),
     m_runNumber(-999),
     m_lumiBlock(-999),
     m_eventNumber(-999),
     m_mcChannelNumber(-999),
     m_mcBeamSpotWeight(1.0)
  {
  }

  MuonTree::MuonTree(PlotBase* pParent, const std::string& sDir):
    PlotBase(pParent, sDir),
    m_tree(nullptr),
    m_isMC(true),
    m_runNumber(-999),
    m_lumiBlock(-999),
    m_eventNumber(-999),
    m_mcChannelNumber(-999),
    m_mcBeamSpotWeight(1.0)
  {
  }
 
  MuonTree::~MuonTree()
  {
  	if(m_tree) delete m_tree;
  }

  void MuonTree::initializePlots()
  {
    m_tree = BookTree("MuonTree");

    //EventInfo
    m_tree->Branch("runNumber", &m_runNumber, "runNumber/i");
    m_tree->Branch("lumiBlock", &m_lumiBlock, "lumiBlock/i");
    m_tree->Branch("eventNumber", &m_eventNumber, "eventNumber/l");
    m_tree->Branch("mcChannelNumber", &m_mcChannelNumber, "mcChannelNumber/i");
    m_tree->Branch("mcBeamSpotWeight", &m_mcBeamSpotWeight, "mcBeamSpotWeight/f");
    
    // 
    //Reco
    //
    //General properties
    m_tree->Branch("pt",  &m_pt);
    m_tree->Branch("eta", &m_eta);
    m_tree->Branch("phi", &m_phi);
    m_tree->Branch("e",   &m_e);
    m_tree->Branch("rapidity", &m_rapidity);
    
    //authors and types
    m_tree->Branch("allAuthors", &m_allAuthors);
    m_tree->Branch("muonType",   &m_muonType);

    //summary values
    m_tree->Branch("innerSmallHits",   &m_innerSmallHits);
    m_tree->Branch("innerLargeHits",   &m_innerLargeHits);
    m_tree->Branch("innerSmallHoles",  &m_innerSmallHoles);
    m_tree->Branch("innerLargeHoles",  &m_innerLargeHoles);

    //parameters
    m_tree->Branch("msInnerMatchChi2", &m_msInnerMatchChi2);
    m_tree->Branch("msInnerMatchDOF",  &m_msInnerMatchDOF);
    m_tree->Branch("msOuterMatchChi2", &m_msOuterMatchChi2);
    m_tree->Branch("msOuterMatchDOF",  &m_msOuterMatchDOF);
    m_tree->Branch("CaloLRLikelihood", &m_CaloLRLikelihood);
    m_tree->Branch("CaloMuonIDTag",    &m_CaloMuonIDTag);
    m_tree->Branch("EnergyLoss",       &m_EnergyLoss);
    
    //quality
    m_tree->Branch("Quality", &m_Quality);
    
    //isolation
    m_tree->Branch("iso_etcone20",    &m_iso_etcone20);
    m_tree->Branch("iso_ptcone20",    &m_iso_ptcone20);
    m_tree->Branch("iso_topoetcon20", &m_iso_topoetcon20);
    m_tree->Branch("iso_ptvarcon20",  &m_iso_ptvarcon20);
    m_tree->Branch("iso_neflowisol20",&m_iso_neflowisol20);

    //associated TrackParticle
    m_tree->Branch("trkp_d0", &m_trkp_d0);
    m_tree->Branch("trkp_z0", &m_trkp_z0);
    m_tree->Branch("trkp_qOverP",     &m_trkp_qOverP);
    m_tree->Branch("trkp_chiSquared", &m_trkp_chiSquared);
    m_tree->Branch("trkp_numberDoF",  &m_trkp_numberDoF);

    //used MuonSegment
    m_tree->Branch("nMuonSegments", &m_nMuonSegments);
    m_tree->Branch("museg_x", &m_museg_x);
    m_tree->Branch("museg_y", &m_museg_y);
    m_tree->Branch("museg_z", &m_museg_z);
    m_tree->Branch("museg_sector",         &m_museg_sector);
    m_tree->Branch("museg_chamberIndex", 	 &m_museg_chamberIndex);
    m_tree->Branch("museg_technology", 	 &m_museg_technology);
    m_tree->Branch("museg_nPrecisionHits", &m_museg_nPrecisionHits);
    m_tree->Branch("museg_nPhiLayers", 	 &m_museg_nPhiLayers);
    m_tree->Branch("museg_nTrigEtaLayers", &m_museg_nTrigEtaLayers);
    
    if(!m_isMC) return;

    //	
    //Truth
    //
    //Trk::Param
    // all TruthTrack
    m_tree->Branch("th_isGoodTruthTrack",  &m_th_isGoodTruthTrack);
    
    m_tree->Branch("th_pt",  &m_th_pt);
    m_tree->Branch("th_eta", &m_th_eta);
    m_tree->Branch("th_phi", &m_th_phi);
    
    //Trk::TruthInfo
    // all TruthTrack
    m_tree->Branch("th_truthType",   &m_th_truthType);
    m_tree->Branch("th_truthOrigin", &m_th_truthOrigin);
    
    //Trk::TruthTrkExtrapolation
    // all TruthTrack
    m_tree->Branch("th_CaloEntryLayer_p",        &m_th_CaloEntry_p);
    m_tree->Branch("th_MuonEntryLayer_p",        &m_th_MuonEntry_p);
    m_tree->Branch("th_MuonExitLayer_p",         &m_th_MuonExit_p);

    //Trk::MSHit
    // all TruthTrack
    m_tree->Branch("th_nprecLayers",    &m_th_nprecLayers);
    m_tree->Branch("th_nphiLayers",     &m_th_nphiLayers);
    m_tree->Branch("th_ntrigEtaLayers", &m_th_ntrigEtaLayers);

  }

  void MuonTree::fillEventBranches(const xAOD::EventInfo* eventInfo, bool isData)	{
	m_runNumber = eventInfo->runNumber();
	if(isData)	{
		m_lumiBlock = eventInfo->lumiBlock();
		m_eventNumber = eventInfo->eventNumber();
		m_mcChannelNumber = 0;
		m_mcBeamSpotWeight = 1.0;
	}
	else	{
		m_lumiBlock = 0;
		m_eventNumber =  eventInfo->mcEventNumber();
		m_mcChannelNumber = eventInfo->mcChannelNumber(); 
		m_mcBeamSpotWeight = eventInfo->beamSpotWeight();
	}	
  }

  void MuonTree::fillRecoMuonBranches(const xAOD::Muon& mu)	{
	//general properties	
	m_pt.push_back(mu.pt());
	m_eta.push_back(mu.eta());
	m_phi.push_back(mu.phi());
	m_e.push_back(mu.e());
	m_rapidity.push_back(mu.rapidity());
	
	//authors and types
	m_allAuthors.push_back(mu.allAuthors());
	m_muonType.push_back(mu.muonType());

	int8_t hitVal = 0;
	if( !mu.summaryValue((uint8_t&)hitVal, xAOD::MuonSummaryType::innerSmallHits) ) hitVal=-1;
	m_innerSmallHits.push_back(hitVal);
	if( !mu.summaryValue((uint8_t&)hitVal, xAOD::MuonSummaryType::innerLargeHits) ) hitVal=-1;
	m_innerLargeHits.push_back(hitVal);
	if( !mu.summaryValue((uint8_t&)hitVal ,xAOD::MuonSummaryType::innerSmallHoles) ) hitVal=-1;
	m_innerSmallHoles.push_back(hitVal);
	if( !mu.summaryValue((uint8_t&)hitVal, xAOD::MuonSummaryType::innerLargeHoles) ) hitVal=-1;
	m_innerLargeHoles.push_back(hitVal);

	//parameters
     float value;
     int ivalue;
	if( !(mu.parameter(value, xAOD::Muon::ParamDef::msInnerMatchChi2)) ) value=-999.;  
	m_msInnerMatchChi2.push_back(value);
	if( !(mu.parameter(ivalue, xAOD::Muon::ParamDef::msInnerMatchDOF)) ) ivalue=-999;  
	m_msInnerMatchDOF.push_back(ivalue);
	if( !(mu.parameter(value, xAOD::Muon::ParamDef::msOuterMatchChi2)) ) value=-999.;  
	m_msOuterMatchChi2.push_back(value);
	if( !(mu.parameter(ivalue, xAOD::Muon::ParamDef::msOuterMatchDOF)) ) ivalue=-999;  
	m_msOuterMatchDOF.push_back(ivalue);
	if( !(mu.parameter(value, xAOD::Muon::ParamDef::CaloLRLikelihood)) ) value=-999.;  
	m_CaloLRLikelihood.push_back(value);
	if( !(mu.parameter(ivalue, xAOD::Muon::ParamDef::CaloMuonIDTag)) ) ivalue=-999;  
	m_CaloMuonIDTag.push_back(ivalue);
	if( !(mu.parameter(value, xAOD::Muon::ParamDef::EnergyLoss)) ) value=-999.;  
	m_EnergyLoss.push_back(value);

	//quality	
	m_Quality.push_back(mu.quality());

	//isolation
     if( !(mu.isolation(value, xAOD::Iso::IsolationType::etcone20)) ) value=-999.;
	m_iso_etcone20.push_back(value);
     if( !(mu.isolation(value, xAOD::Iso::IsolationType::ptcone20)) ) value=-999.;
	m_iso_ptcone20.push_back(value);
     if( !(mu.isolation(value, xAOD::Iso::IsolationType::topoetcone20)) ) value=-999.;
	m_iso_topoetcon20.push_back(value);
     if( !(mu.isolation(value, xAOD::Iso::IsolationType::ptvarcone20)) ) value=-999.;
	m_iso_ptvarcon20.push_back(value);
     if( !(mu.isolation(value, xAOD::Iso::IsolationType::neflowisol20)) ) value=-999.;
	m_iso_neflowisol20.push_back(value);

	//associated TrackParticle
	m_trkp_d0.push_back( mu.primaryTrackParticle()->d0() );
	m_trkp_z0.push_back( mu.primaryTrackParticle()->z0() );
	m_trkp_qOverP.push_back( mu.primaryTrackParticle()->qOverP() );
	m_trkp_chiSquared.push_back( mu.primaryTrackParticle()->chiSquared() );
	m_trkp_numberDoF.push_back( mu.primaryTrackParticle()->numberDoF() );

	//used MuonSegments
	size_t nMuonSegments = mu.nMuonSegments();
	m_nMuonSegments.push_back( nMuonSegments );	
	size_t muonIndx = m_nMuonSegments.size()-1;

	m_museg_x.push_back( std::vector<float >() );
	m_museg_y.push_back( std::vector<float >() );
	m_museg_z.push_back( 			std::vector<float >() );
	m_museg_sector.push_back( 		std::vector<int >() );
	m_museg_chamberIndex.push_back( 	std::vector<unsigned int >() );
	m_museg_technology.push_back( 	std::vector<unsigned int >() );
	m_museg_nPrecisionHits.push_back( 	std::vector<int >() );
	m_museg_nPhiLayers.push_back( 	std::vector<int >() );
	m_museg_nTrigEtaLayers.push_back( 	std::vector<int >() );

	for(int i=0; (size_t)i<nMuonSegments; i++ ) {
		const xAOD::MuonSegment* muonSegment = mu.muonSegment(i);
		m_museg_x[ muonIndx ].push_back(muonSegment->x());
		m_museg_y[ muonIndx ].push_back(muonSegment->y());
		m_museg_z[ muonIndx ].push_back(muonSegment->z());
		m_museg_sector[ 		muonIndx ].push_back(muonSegment->sector());
		m_museg_chamberIndex[ 	muonIndx ].push_back((int)muonSegment->chamberIndex());
		m_museg_technology[ 	muonIndx ].push_back((int)muonSegment->technology());
		m_museg_nPrecisionHits[ 	muonIndx ].push_back(muonSegment->nPrecisionHits());
		m_museg_nPhiLayers[ 	muonIndx ].push_back(muonSegment->nPhiLayers());
		m_museg_nTrigEtaLayers[ 	muonIndx ].push_back(muonSegment->nTrigEtaLayers());
	}
	
   }


  void MuonTree::fillTruthMuonBranches(const xAOD::TruthParticle &truthMu, bool isGoodTruthTrack)	{

	//
	//Trk::Param
	//
	if(isGoodTruthTrack) { m_th_isGoodTruthTrack.push_back(true);  }
	else                 { m_th_isGoodTruthTrack.push_back(false); }

	m_th_pt.push_back(truthMu.pt());
     m_th_eta.push_back(truthMu.eta());
     m_th_phi.push_back(truthMu.phi());
     
	//
	//Trk::TruthInfo
	//
	if (truthMu.isAvailable<int>("truthType")) 	{ m_th_truthType.push_back(truthMu.auxdata< int >("truthType")); } 
	else 								{ m_th_truthType.push_back(-999); }
	
	if (truthMu.isAvailable<int>("truthOrigin")) { m_th_truthOrigin.push_back(truthMu.auxdata< int >("truthOrigin")); } 
	else 								{ m_th_truthOrigin.push_back(-999); }
	
	//
	//Trk::TruthTrkExtrapolation
	//
     if (truthMu.isAvailable<float>("CaloEntryLayer_px") &&
         truthMu.isAvailable<float>("CaloEntryLayer_py") &&
         truthMu.isAvailable<float>("CaloEntryLayer_pz")) 	{
       
		TVector3 v(truthMu.auxdata<float>("CaloEntryLayer_px"), truthMu.auxdata<float>("CaloEntryLayer_py"), truthMu.auxdata<float>("CaloEntryLayer_pz"));
		m_th_CaloEntry_p.push_back(v.Mag()*0.001);
	} else {
		m_th_CaloEntry_p.push_back(-999.);
	}
	
	if (truthMu.isAvailable<float>("MuonEntryLayer_px") &&
         truthMu.isAvailable<float>("MuonEntryLayer_py") &&
         truthMu.isAvailable<float>("MuonEntryLayer_pz")) 	{
       
		TVector3 v(truthMu.auxdata<float>("MuonEntryLayer_px"), truthMu.auxdata<float>("MuonEntryLayer_py"), truthMu.auxdata<float>("MuonEntryLayer_pz"));
		m_th_MuonEntry_p.push_back(v.Mag()*0.001);
	} else {
		m_th_MuonEntry_p.push_back(-999.);
	}
    
	if (truthMu.isAvailable<float>("MuonExitLayer_px") &&
         truthMu.isAvailable<float>("MuonExitLayer_py") &&
         truthMu.isAvailable<float>("MuonExitLayer_pz")) 	{
       
		TVector3 v(truthMu.auxdata<float>("MuonExitLayer_px"), truthMu.auxdata<float>("MuonExitLayer_py"), truthMu.auxdata<float>("MuonExitLayer_pz"));
		m_th_MuonExit_p.push_back(v.Mag()*0.001);
	} else {
		m_th_MuonExit_p.push_back(-999.);
	}
	
	//
	//Trk::MSHit
	//
	if (truthMu.isAvailable<uint8_t>("nprecLayers")) 	{ m_th_nprecLayers.push_back(truthMu.auxdata<uint8_t>("nprecLayers")); } 
	else										{ m_th_nprecLayers.push_back(-99); }
	
	if (truthMu.isAvailable<uint8_t>("nphiLayers")) 	{ m_th_nphiLayers.push_back(truthMu.auxdata<uint8_t>("nphiLayers")); } 
	else 									{ m_th_nphiLayers.push_back(-99); }
	
	if (truthMu.isAvailable<uint8_t>("ntrigEtaLayers")) { m_th_ntrigEtaLayers.push_back(truthMu.auxdata<uint8_t>("ntrigEtaLayers")); } 
	else 									  { m_th_ntrigEtaLayers.push_back(-99); }

  }


  void MuonTree::fillTruthMuonBranches(const xAOD::TruthParticle &truthMu, const xAOD::Muon& mu, const xAOD::TrackParticleContainer* MSTracks, bool isGoodTruthTrack)	{
  // If there is an interest to have an additional observable, which is derived from both reco and truth muon objects, stored in the validation tree, 
  // then this observable can be calculated/retrieved like, e.g., it is done here:
  //
  // line:0175  athena/MuonSpectrometer/MuonValidation/MuonDQA/MuonPhysValMonitoring/src/MuonValidationPlots.cxx
  //                  athena/MuonSpectrometer/MuonValidation/MuonHistogramming/MuonHistUtils/MuonHistUtils/TruthRelatedMuonPlotOrganizer.h
  //      line: 0090     athena/MuonSpectrometer/MuonValidation/MuonHistogramming/MuonHistUtils/Root/TruthRelatedMuonPlotOrganizer.cxx
  // 
  // 
  // Meanwhile, use these useless statements to void compilation warnings
  	if(MSTracks) return;
  	if(isGoodTruthTrack) return;
  	if(mu.m()) return;
  	if(truthMu.status()) return;
  }

  void MuonTree::postFillTreeActions()	{

    // 
    // Event Info	
    //
    m_runNumber=-999;
    m_lumiBlock=-999;
    m_eventNumber=-999;
    m_mcChannelNumber=-999;
    
    //
    // Reco 
    //
    m_pt.clear();
    m_eta.clear();
    m_phi.clear();
    m_e.clear();
    m_rapidity.clear();
    
    //authors and types
    m_allAuthors.clear();
    m_muonType.clear();
   
    //summary values
    m_innerSmallHits.clear();
    m_innerLargeHits.clear();
    m_innerSmallHoles.clear();
    m_innerLargeHoles.clear();

    //parameters
    m_msInnerMatchChi2.clear();
    m_msInnerMatchDOF.clear();
    m_msOuterMatchChi2.clear();
    m_msOuterMatchDOF.clear();
    m_CaloLRLikelihood.clear();
    m_CaloMuonIDTag.clear();
    m_EnergyLoss.clear();
    
    //quality
    m_Quality.clear();
    
    //isolation
    m_iso_etcone20.clear();
    m_iso_ptcone20.clear();
    m_iso_topoetcon20.clear();
    m_iso_ptvarcon20.clear();
    m_iso_neflowisol20.clear();
    
    //associated TrackParticle
    m_trkp_d0.clear();
    m_trkp_z0.clear();
    m_trkp_qOverP.clear();
    m_trkp_chiSquared.clear();
    m_trkp_numberDoF.clear();
    
    //used MuonSegments
    m_nMuonSegments.clear();
    m_museg_x.clear();
    m_museg_y.clear();
    m_museg_z.clear();
    m_museg_sector.clear();
    m_museg_chamberIndex.clear();
    m_museg_technology.clear();
    m_museg_nPrecisionHits.clear();
    m_museg_nPhiLayers.clear();
    m_museg_nTrigEtaLayers.clear(); 
    
    //	
    // Truth
    //
    //Trk::Param 
    m_th_isGoodTruthTrack.clear();
    m_th_pt.clear();
    m_th_eta.clear();
    m_th_phi.clear();
    
    //Trk::TruthInfo
    m_th_truthType.clear();
    m_th_truthOrigin.clear();

    //Trk::TruthTrkExtrapolation
    m_th_CaloEntry_p.clear();
    m_th_MuonEntry_p.clear();
    m_th_MuonExit_p.clear();

    //Trk::MSHit
    m_th_nprecLayers.clear();          
    m_th_nphiLayers.clear();
    m_th_ntrigEtaLayers.clear();
  }

  TTree* MuonTree::getTree() {  return m_tree; }

} // closing namespace Muon


