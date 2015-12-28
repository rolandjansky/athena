/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/ZeeValidationMonitoringTool.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h" 
#include "xAODPrimitives/IsolationType.h"

#include "xAODEgamma/EgammaDefs.h"

#include "AthenaBaseComps/AthCheckMacros.h"
using CLHEP::GeV;

namespace ZeeValidation {
  
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  
  // Constructors
  ////////////////
  
  ZeeValidationMonitoringTool::ZeeValidationMonitoringTool( const std::string& type,
				  const std::string& name,
				  const IInterface* parent ) :
    ManagedMonitorToolBase( type, name, parent ),
    m_isData(false),
    m_ReconElectronsPlots(0, "Zee/ReconElectrons/", "Electrons"),
    m_TrueElectronsPlots(0, "Zee/TrueElectrons/", "True Electrons"),
    m_TrueFwdElectronsPlots(0, "Zee/TrueFwdElectrons/", "True FWD Electrons"),
    m_ZeePlots(0, "Zee/Zee/", "Zee"),
    m_FWDZeePlots(0, "Zee/FWDZee/", "FWD Zee"),
    drmin_elreco_truth(0), matchedE(false),
    matched_electron(NULL),
    drmin_phreco_truth(0), matchedP(false)
  {
    declareProperty( "IsData", m_isData = false);
    declareProperty( "EventInfoContainerName", m_eventInfoName = "EventInfo");
    declareProperty( "ElectronContainerName", m_elecName = "ElectronCollection" );
    declareProperty( "ElectronContainerFrwdName", m_elecFwdName = "FwdElectrons" ); 
    declareProperty( "PhotonContainerName", m_photonName = "PhotonCollection" ); 
    declareProperty( "VertexContainerName", m_vertexName = "PrimaryVertices" );
    declareProperty( "TrackParticleContainerName", m_trackName = "InDetTrackParticles" );
    declareProperty( "TruthParticleContainerName", m_truthName = "TruthParticle"); 

    declareProperty( "PtMinCent", m_PtCentCut = 20.0 );
    declareProperty( "PtMinFwd", m_PtFwdCut = 15.0 );
    declareProperty( "MeeLow", m_MeeLowCut = 66.0 );
    declareProperty( "MeeHigh", m_MeeHighCut = 116.0 );
    declareProperty( "EtaCent", m_EtaCentCut = 2.47 );
    declareProperty( "EtaLowFwd", m_EtaLowFwdCut = 2.5 );
    declareProperty( "EtaHighFwd", m_EtaHighFwdCut = 4.8 );
    declareProperty( "EtaCrackLow", m_EtaCrackLowCut = 1.37 );
    declareProperty( "EtaCrackHigh", m_EtaCrackHighCut = 1.52 );
    declareProperty( "dRminRecoTrue", m_dRminRecoTrue = 0.1);  
  }
  
  // Destructor
  ///////////////
  ZeeValidationMonitoringTool::~ZeeValidationMonitoringTool()
  {}
  
  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode ZeeValidationMonitoringTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");   
    ATH_CHECK(ManagedMonitorToolBase::initialize());
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode ZeeValidationMonitoringTool::book(PlotBase& plots)
  {
    plots.initialize();
    std::vector<HistData> hists = plots.retrieveBookedHistograms();
    
    for (auto& hist : hists){
      ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
      ATH_CHECK(regHist(hist.first, hist.second,all));
    }
    return StatusCode::SUCCESS;     
  }
  
  StatusCode ZeeValidationMonitoringTool::bookHistograms()
  {
    ATH_MSG_INFO ("Booking hists " << name() << "...");     
    
    if (m_detailLevel >= 10) {
      ATH_CHECK(book(m_ReconElectronsPlots));
      if( !m_isData )
	{
	  ATH_CHECK(book(m_TrueElectronsPlots));
	  ATH_CHECK(book(m_TrueFwdElectronsPlots));
	}
      ATH_CHECK(book(m_ZeePlots));
      ATH_CHECK(book(m_FWDZeePlots));
    }
    
    return StatusCode::SUCCESS;     
  }
  
  StatusCode ZeeValidationMonitoringTool::fillHistograms()
  {
    ATH_MSG_INFO ("Filling hists " << name() << "..."); 
    if (m_detailLevel < 10) return StatusCode::SUCCESS;

    const xAOD::EventInfo* eventInfo(0);
    ATH_CHECK(evtStore() -> retrieve(eventInfo, m_eventInfoName));

    const xAOD::VertexContainer* vertices(0);
    ATH_CHECK(evtStore() -> retrieve(vertices, m_vertexName));

    const xAOD::ElectronContainer* electrons(0);
    ATH_CHECK(evtStore() -> retrieve(electrons, m_elecName));

    const xAOD::ElectronContainer* fwd_electrons(0);
    ATH_CHECK(evtStore() -> retrieve(fwd_electrons, m_elecFwdName));

    const xAOD::PhotonContainer* photons(0);
    ATH_CHECK(evtStore() -> retrieve(photons, m_photonName));

    ///////////////////////////////////////////////////////////////////
    // Reconstructed electrons
    ///////////////////////////////////////////////////////////////////

    int nrecel = 0;
    for (auto electron : *electrons){

      m_ReconElectronsPlots.h_electron_author -> Fill(electron -> author());
      
      //select electrons which passed author and pT cut
      if (!( electron -> author(xAOD::EgammaParameters::AuthorElectron) )) continue; 
      if ( electron -> pt()*(1./GeV) < m_PtCentCut ) continue;
      nrecel++;

      bool inAcceptance = (TMath::Abs(electron -> eta()) > m_EtaCrackHighCut || TMath::Abs(electron -> eta()) < m_EtaCrackLowCut) && TMath::Abs(electron -> eta()) < m_EtaCentCut;

      if (inAcceptance)
	m_ReconElectronsPlots.fill(eventInfo, vertices);
      
      bool loose = false, medium = false, tight = false, lhloose = false, lhmedium = false, lhtight = false,  oq = false;
      electron -> passSelection(loose, "Loose"); 
      electron -> passSelection(medium, "Medium");
      electron -> passSelection(tight, "Tight");
      electron -> passSelection(lhloose, "LHLoose"); 
      electron -> passSelection(lhmedium, "LHMedium");
      electron -> passSelection(lhtight, "LHTight");
      if ( electron -> isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON) ) oq = true;

      //fill if passed author & pT cuts
      m_ReconElectronsPlots.fill(electron, 0);
      if (inAcceptance)
	m_ReconElectronsPlots.fillinAcc(electron, 0);

      if (oq){ //fill if passed OQ cuts
	m_ReconElectronsPlots.fill(electron, 1);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 1);
      }

      if (oq && loose){ //fill if passed OQ and Loose++ cuts
	m_ReconElectronsPlots.fill(electron, 2);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 2);
      }

      if (oq && medium){ //fill if passed OQ and Medium++ cuts
	m_ReconElectronsPlots.fill(electron, 3);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 3);
      }

      if (oq && tight){ //fill if passed OQ and Tight++ cuts
	m_ReconElectronsPlots.fill(electron, 4);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 4);
      }


      if (oq && lhloose){ //fill if passed OQ and Loose++ cuts
	m_ReconElectronsPlots.fill(electron, 5);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 5);
      }

      if (oq && lhmedium){ //fill if passed OQ and Medium++ cuts
	m_ReconElectronsPlots.fill(electron, 6);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 6);
      }

      if (oq && lhtight){ //fill if passed OQ and Tight++ cuts
	m_ReconElectronsPlots.fill(electron, 7);
	if (inAcceptance)
	  m_ReconElectronsPlots.fillinAcc(electron, 7);
      }
      //shower shape variables

      m_ReconElectronsPlots.fillShowerShape( electron );
      m_ReconElectronsPlots.fillIsolation( electron );
      //track variables
      const xAOD::TrackParticle* track  = electron -> trackParticle();
      if (track){
	m_ReconElectronsPlots.fillHitInfo( electron );
	m_ReconElectronsPlots.fillTrackCaloMatch( electron, track );
      }

    }
    m_ReconElectronsPlots.h_electron_n -> Fill(nrecel);
    m_ReconElectronsPlots.h_photon_n -> Fill(photons -> size());

    ///////////////////////////////////////////////////////////////////
    // Truth electrons
    ///////////////////////////////////////////////////////////////////
    if( !m_isData )
      {
	const xAOD::TruthParticleContainer* truth_particles(0);
	ATH_CHECK(evtStore() -> retrieve(truth_particles, m_truthName));
    
	for(auto truth_part : *truth_particles)
	  {
	    // Select truth particle which passed pT, eta cuts and have pid=11
	    if( TMath::Abs(truth_part -> pdgId()) != 11 || truth_part -> status() != 1  || truth_part -> barcode() > 100000 ) continue;
	    if( truth_part -> pt()*(1./GeV) < m_PtCentCut ) continue;
	    if(TMath::Abs(truth_part -> eta()) > m_EtaCentCut ) continue;

	    bool inAcceptance = TMath::Abs(truth_part -> eta()) > m_EtaCrackHighCut || TMath::Abs(truth_part -> eta()) < m_EtaCrackLowCut;
	    // Fill plots with all truth particles passed cuts
	    m_TrueElectronsPlots.fill(truth_part, 0);
	    if( inAcceptance )
	      m_TrueElectronsPlots.fillinAcc(truth_part, 0);
	    
	    MatchElec(truth_part, electrons); // Matching to reconstructed electrons
	    MatchPhot(truth_part, photons); // Matching to reconstructed photons
	    
	    if( matchedE && drmin_elreco_truth < m_dRminRecoTrue) { //if matched to reconstructed electron
	      m_TrueElectronsPlots.fill(truth_part, 1); //fill "associated" plots: if matched to electron or photon
	      m_TrueElectronsPlots.fill(truth_part, 2); //fill "matched" plots: if matched to electron
	      
	      if( inAcceptance )
		{
		  m_TrueElectronsPlots.fillinAcc(truth_part, 1);
		  m_TrueElectronsPlots.fillinAcc(truth_part, 2);
		}
	      
	      bool loose = false, medium = false, tight = false, lhloose = false, lhmedium = false, lhtight = false, oq = false;
	      matched_electron -> passSelection(loose, "Loose"); 
	      matched_electron -> passSelection(medium, "Medium");
	      matched_electron -> passSelection(tight, "Tight");
	      matched_electron -> passSelection(lhloose, "LHLoose"); 
	      matched_electron -> passSelection(lhmedium, "LHMedium");
	      matched_electron -> passSelection(lhtight, "LHTight");
	      if ( matched_electron -> isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON) ) oq = true;
	      
	      if (oq) //fill if passed OQ cuts
		m_TrueElectronsPlots.fill(truth_part, 3);
	      if (oq && loose) //fill if passed OQ and Loose++ cuts
		m_TrueElectronsPlots.fill(truth_part, 4);
	      if (oq && medium ) //fill if passed OQ and Medium++ cuts
		m_TrueElectronsPlots.fill(truth_part, 5);
	      if (oq && tight ) //fill if passed OQ and Tight++ cuts
		m_TrueElectronsPlots.fill(truth_part, 6);
	      if (oq && lhloose) //fill if passed OQ and LHLoose++ cuts
		m_TrueElectronsPlots.fill(truth_part, 7);
	      if (oq && lhmedium ) //fill if passed OQ and LHMedium++ cuts
		m_TrueElectronsPlots.fill(truth_part, 8);
	      if (oq && lhtight ) //fill if passed OQ and LHTight++ cuts
		m_TrueElectronsPlots.fill(truth_part, 9);
	      
	      if (inAcceptance){ 
		if (oq)
		  m_TrueElectronsPlots.fillinAcc(truth_part, 3);
		if (oq && loose)
		  m_TrueElectronsPlots.fillinAcc(truth_part, 4);
		if (oq && medium )
		  m_TrueElectronsPlots.fillinAcc(truth_part, 5);
		if (oq && tight )
		  m_TrueElectronsPlots.fillinAcc(truth_part, 6);
		if (oq && lhloose)
		  m_TrueElectronsPlots.fillinAcc(truth_part, 7);
		if (oq && lhmedium )
		  m_TrueElectronsPlots.fillinAcc(truth_part, 8);
		if (oq && lhtight )
		  m_TrueElectronsPlots.fillinAcc(truth_part, 9);
	      }
	      
	      //fill plots for electron energy response
	      m_TrueElectronsPlots.fillResponse( truth_part, matched_electron);
	      const xAOD::CaloCluster* cluster = matched_electron -> caloCluster();
	      if (cluster)
		m_TrueElectronsPlots.fillResponseCluster( truth_part, cluster );
	    }

	    if (!(matchedE && drmin_elreco_truth < m_dRminRecoTrue) && drmin_phreco_truth < m_dRminRecoTrue){  //if matched to reconstructed photon
	      m_TrueElectronsPlots.h_dr_photon -> Fill( drmin_phreco_truth );
	      
	      m_TrueElectronsPlots.fill(truth_part, 1); //fill "associated" plots: if matched to electron or photon
	      if (inAcceptance)
		m_TrueElectronsPlots.fillinAcc(truth_part, 1);
	    }
	    
	  }
	
	///////////////////////////////////////////////////////////////////
	// Truth FWD electrons
	///////////////////////////////////////////////////////////////////

	for (auto truth_part : *truth_particles){
	  
	  //select truth particle which passed fwd pT, eta cuts and have pid=11
	  if ( TMath::Abs(truth_part -> pdgId()) != 11 || truth_part -> status() != 1  || truth_part -> barcode() > 100000 ) continue;
	  if ( truth_part -> pt()*(1./GeV) < m_PtFwdCut ) continue;
	  if (TMath::Abs( truth_part -> eta() ) < m_EtaLowFwdCut || TMath::Abs(  truth_part -> eta() ) > m_EtaHighFwdCut ) continue;
	  
	  //fill plots with all truth particles passed fwd cuts
	  m_TrueFwdElectronsPlots.fill(truth_part, 0);
	  
	  MatchElec(truth_part, fwd_electrons);
	  if (matchedE && drmin_elreco_truth < m_dRminRecoTrue){ //if matched to reconstructed electron
	    m_TrueFwdElectronsPlots.fill(truth_part, 1);
	    
	    bool loose = false, tight = false;
	    matched_electron -> passSelection(loose, "Loose"); 
	    matched_electron -> passSelection(tight, "Tight");
	    
	    if (loose) //fill if passed Loose++ cuts
	      m_TrueFwdElectronsPlots.fill(truth_part, 2);
	    if (tight) //fill if passed  Tight++ cuts
	      m_TrueFwdElectronsPlots.fill(truth_part, 3);
	    
	    //fill plots for electron energy response
	    m_TrueFwdElectronsPlots.fillResponse( truth_part, matched_electron);
	    
	  }
	}
      
      } // if( !m_isData )
	
	///////////////////////////////////////////////////////////////////
    // Z->ee
    ///////////////////////////////////////////////////////////////////

    const xAOD::Electron *z_el[2] = {0, 0};
  
    int nel = 0;
    // first electron loop: collect "good" electrons passing pT, OQ and Medium++ selection
    for (auto electron : *electrons){

      if (!( electron -> author(xAOD::EgammaParameters::AuthorElectron) )) continue; 
      if (!( electron -> isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON) )) continue;
      if ( electron -> pt()*(1./GeV) < m_PtCentCut ) continue;

      bool medium = false; 
      electron -> passSelection(medium, "Medium");
      if (medium == false) continue;

      if (nel == 0) {
	z_el[0] = electron;
      } else if (nel >= 1 && electron -> pt() > z_el[0] -> pt()) {
	z_el[1] = z_el[0];
	z_el[0] = electron;
      } else if (nel == 1 || (nel >= 2 && electron -> pt() > z_el[1] -> pt())) {
	z_el[1] = electron;
      }
      nel++;
    }

    // if number of electrons after the first loop < 2, second electron loop:
    // collect electrons passing pT and at least one of OQ or Medium++ cuts
    if ( nel < 2 ){
      
      for (auto electron : *electrons){

	if (!( electron -> author(xAOD::EgammaParameters::AuthorElectron) )) continue; 
	if ( electron -> pt()*(1./GeV) < m_PtCentCut ) continue;

	bool medium = false; 
	electron -> passSelection(medium, "Medium");
	//to avoid double counting
	if (( electron -> isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) ) && (medium == true))  continue;

	if (nel == 0) {
	  z_el[0] = electron;
	} else if (nel >= 1 && electron -> pt() > z_el[0] -> pt()) {
	  z_el[1] = z_el[0];
	  z_el[0] = electron;
	} else if (nel == 1 || (nel >= 2 && electron -> pt() > z_el[1] -> pt())) {
	  z_el[1] = electron;
	}
	nel++;
      }
     
    }
   
    // if number of electrons after the second loop < 2, third electron loop:
    // collect electrons passing pT cut
    if ( nel < 2 ){
      
      for (auto electron : *electrons){

	if (!( electron -> author(xAOD::EgammaParameters::AuthorElectron) )) continue; 
	//to avoid double counting
	if ( electron -> pt()*(1./GeV) >= m_PtCentCut ) continue;

	if (nel == 0) {
	  z_el[0] = electron;
	} else if (nel >= 1 && electron -> pt() > z_el[0] -> pt()) {
	  z_el[1] = z_el[0];
	  z_el[0] = electron;
	} else if (nel == 1 || (nel >= 2 && electron -> pt() > z_el[1] -> pt())) {
	  z_el[1] = electron;
	}
	nel++;
      }
     
    }
 
    //if selected at least two electrons 
    if( nel >= 2 ){

      TLorentzVector el4v[2], elclus4v[2], eltrack4v[2];
      bool inAcceptance[2];
      for (int i = 0; i < 2; i++) {
	el4v[i] = z_el[i] -> p4();
	
	const xAOD::CaloCluster* cluster = z_el[i] -> caloCluster();
	if (cluster)
	  elclus4v[i] = z_el[i]->caloCluster()-> p4();
	
	const xAOD::TrackParticle* trackp  = z_el[i] -> trackParticle();

	if (trackp){

	  eltrack4v[i] = trackp -> p4();
	  uint8_t iPixHits, iSCTHits;
	  bool hits = false;
	  hits = trackp -> summaryValue(iPixHits, xAOD::numberOfPixelHits);
	  hits &= trackp -> summaryValue(iSCTHits, xAOD::numberOfSCTHits);

	  if (hits&& (iPixHits + iSCTHits > 3)){
	
	    double e = z_el[i] -> caloCluster() -> e();

	    double eta = trackp -> eta();
	    double phi = trackp -> phi();
	    // Create tlv
	    double m = 0;
	    double p  = std::sqrt(e*e - m*m);
	    double pt = p * 1/std::cosh(eta);
	    double px = pt * std::cos(phi);
	    double py = pt * std::sin(phi);
	    double pz = p * std::tanh(eta);
	       
	    el4v[i] = TLorentzVector(px, py, pz, e);	    
	  }
	}
	inAcceptance[i] = (TMath::Abs(elclus4v[i].Eta()) > m_EtaCrackHighCut || TMath::Abs(elclus4v[i].Eta()) < m_EtaCrackLowCut) && TMath::Abs(elclus4v[i].Eta()) < m_EtaCentCut;
      }
   	             
      TLorentzVector z = el4v[0] + el4v[1];

      bool z_passed_cuts = true;
      bool OS = true;

      bool bothLoose = false, bothMedium = false, bothTight = false, bothOQ = false;
      bool loose1 = false, loose2 = false, medium1 = false, medium2 = false, tight1 = false, tight2 = false;
      
      z_el[0] -> passSelection(loose1, "Loose");
      z_el[1] -> passSelection(loose2, "Loose");
      z_el[0] -> passSelection(medium1, "Medium");
      z_el[1] -> passSelection(medium2, "Medium");
      z_el[0] -> passSelection(tight1, "Tight");
      z_el[1] -> passSelection(tight2, "Tight");
      
      if ( z_el[0] -> isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) && z_el[1] -> isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) ) bothOQ = true;
      if ( loose1 && loose2 ) bothLoose = true;
      if ( medium1 && medium2 ) bothMedium = true;
      if ( tight1 && tight2 ) bothTight = true;
   
      if (elclus4v[0].Perp()*(1./GeV) < m_PtCentCut || !inAcceptance[0]) z_passed_cuts = false;	
      if (elclus4v[1].Perp()*(1./GeV) < m_PtCentCut || !inAcceptance[1]) z_passed_cuts = false;
     
      if (z_passed_cuts){
       	m_ZeePlots.fillGenPlots(nel);
      }

      if (z.M()*(1./GeV) < m_MeeLowCut || z.M()*(1./GeV) > m_MeeHighCut) z_passed_cuts = false;
      if (z_el[0] -> charge() + z_el[1] -> charge() != 0) OS = false;
 
      //10 plot levels: Reco OS, OQ OS, Loose OS, Medium OS, Tight OS, Reco SS, OQ SS, Loose SS, Medium SS, Tight SS
      if(z_passed_cuts && OS){ //fill OS plots if Z passed selection: mass cut, each electron cluster passed pT cut and is out of crack region
	m_ZeePlots.fillZPlots(z, 0);
	m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 0);

	if (bothOQ){ //if both electrons passed OQ cuts
	  m_ZeePlots.fillZPlots(z, 1);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 1);
	}
	if (bothLoose){  //if both electrons passed Loose++ cuts
	  m_ZeePlots.fillZPlots(z, 2);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 2);
	}
	if (bothMedium){ //if both electrons passed Medium++ cuts
	  m_ZeePlots.fillZPlots(z, 3);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 3);
	}
	if (bothTight){  //if both electrons passed Tight++ cuts
	  m_ZeePlots.fillZPlots(z, 4);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 4);
	} 
      } //end if Z passed selection and OS

      if(z_passed_cuts && !OS){ //fill SS plots if Z passed selection: mass cut, each electron cluster passed pT cut and is out of crack region
	m_ZeePlots.fillZPlots(z, 5);
	m_ZeePlots.fillElectronPlots(el4v[0], el4v[1],  elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 5);

	if (bothOQ){//if both electrons passed OQ cuts
	  m_ZeePlots.fillZPlots(z, 6);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 6);
	}
	if (bothLoose){ //if both electrons passed Loose++ cuts
	  m_ZeePlots.fillZPlots(z, 7);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 7);
	}
	if (bothMedium){//if both electrons passed Medium++ cuts
	  m_ZeePlots.fillZPlots(z, 8);
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 8);
	}
	if (bothTight){
	  m_ZeePlots.fillZPlots(z, 9); //if both electrons passed Tight++ cuts
	  m_ZeePlots.fillElectronPlots(el4v[0], el4v[1], elclus4v[0], elclus4v[1], eltrack4v[0], eltrack4v[1], z_el[0] -> charge(), z_el[1] -> charge(), 9);
	} 
      } //end if Z passed cut and SS

    }
 
    ///////////////////////////////////////////////////////////////////
    // Fwd Z->ee
    ///////////////////////////////////////////////////////////////////
  
    const xAOD::Electron *zfwd_el[2] = {0, 0};
    int nelcent = 0;
    int nelfwd = 0;
   
    // first electron loop: collect "good" electrons passing pT, OQ and Medium++ selection
    for (auto electron : *electrons){ 
      //
      if (!( electron -> author(xAOD::EgammaParameters::AuthorElectron) )) continue; 
      if (!( electron -> isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) )) continue;
      if ( electron -> pt()*(1./GeV) < m_PtCentCut ) continue;

      bool medium = false; 
      electron -> passSelection(medium, "Medium");
      if (medium == false) continue;

      if (nelcent == 0) {
	zfwd_el[0] = electron;
      } else if (nelcent >= 1 && electron -> pt() > zfwd_el[0] -> pt()) {
	zfwd_el[0] = electron;
      }
      nelcent++;
    }
    //if exactly one "good" central electron selected then loop over forward electrons
    if (nelcent == 1){ 
      for (auto fwd_electron : *fwd_electrons){ //first loop: collect fwd Tight++ electrons passed pT cut

	if (!( fwd_electron -> author(xAOD::EgammaParameters::AuthorFwdElectron) )) continue; 
	if ( fwd_electron -> pt()*(1./GeV) < m_PtFwdCut ) continue;

	bool tight = false; 
	fwd_electron -> passSelection(tight, "Tight");
	if (tight == false) continue;

	if (nelfwd == 0) {
	  zfwd_el[1] = fwd_electron;
	} else if (nelfwd >= 1 && fwd_electron -> pt() > zfwd_el[1] -> pt()) {
	  zfwd_el[1] = fwd_electron;
	}
	nelfwd++;
      }
       
      if (nelfwd==0) //if no Tight++ electrons selected collect fwd Loose++ electrons passed pT cut
	for (auto fwd_electron : *fwd_electrons){

	  if (!( fwd_electron -> author(xAOD::EgammaParameters::AuthorFwdElectron) )) continue; 
	  if ( fwd_electron -> pt()*(1./GeV) < m_PtFwdCut ) continue;

	  bool loose = false; 
	  fwd_electron -> passSelection(loose, "Loose");
	  if (loose == false) continue;

	  if (nelfwd == 0) {
	    zfwd_el[1] = fwd_electron;
	  } else if (nelfwd >= 1 && fwd_electron -> pt() > zfwd_el[1] -> pt()) {
	    zfwd_el[1] = fwd_electron;
	  }
	  nelfwd++;
	}
   
      if (nelfwd==0) //if no Tight++ or Loose++ electrons selected collect remaining electrons 
	for (auto fwd_electron : *fwd_electrons){

	  if (!( fwd_electron -> author(xAOD::EgammaParameters::AuthorFwdElectron) )) continue; 

	  if (nelfwd == 0) {
	    zfwd_el[1] = fwd_electron;
	  } else if (nelfwd >= 1 && fwd_electron -> pt() > zfwd_el[1] -> pt()) {
	    zfwd_el[1] = fwd_electron;
	  }
	  nelfwd++;
	} 
    
      //if fwd electron was found
      if (nelfwd > 0){

	TLorentzVector elfwd4v[2];
	bool inAcceptance[2];
	
	// rebuild central electron 4v
	double e =  zfwd_el[0] -> caloCluster() -> e();
	double etaclus = zfwd_el[0] -> caloCluster() -> eta();
	double eta = zfwd_el[0] -> trackParticle() -> eta();
	double phi = zfwd_el[0] -> trackParticle() -> phi();
	// Create tlv
	float pt = e * 1/std::cosh(eta);
	float px = pt * std::cos(phi);
	float py = pt * std::sin(phi);
	float pz = e * std::tanh(eta);
	
	elfwd4v[0] = TLorentzVector(px, py, pz, e);	
	inAcceptance[0] = (TMath::Abs(etaclus) > m_EtaCrackHighCut || TMath::Abs(etaclus) < m_EtaCrackLowCut) && TMath::Abs(etaclus) < m_EtaCentCut;

	elfwd4v[1] = zfwd_el[1] -> p4();
	inAcceptance[1] = TMath::Abs(elfwd4v[1].Eta()) > m_EtaLowFwdCut && TMath::Abs(elfwd4v[1].Eta()) < m_EtaHighFwdCut;
	
	bool tight_cent = false, loose_fwd = false, tight_fwd = false;
	bool oq_fwd = false;
	float ptcone40_value = -100;
	zfwd_el[0] -> passSelection(tight_cent, "Tight");
 	zfwd_el[1] -> passSelection(loose_fwd, "Loose"); 
   	zfwd_el[1] -> passSelection(tight_fwd, "Tight"); 	
	zfwd_el[0] -> isolationValue(ptcone40_value, xAOD::Iso::ptcone40);
	
	if (zfwd_el[1] -> isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON) ) oq_fwd = true;

	TLorentzVector zfwd = elfwd4v[0] + elfwd4v[1];

	bool zfwd_passed_cuts = true;
	if ( !(tight_cent && ptcone40_value < 2000.)) zfwd_passed_cuts = false;
	if (zfwd.M()*(1./GeV) < m_MeeLowCut|| zfwd.M()*(1./GeV) >= m_MeeHighCut) zfwd_passed_cuts = false;
	if (elfwd4v[0].Perp()*(1./GeV) < m_PtCentCut || !inAcceptance[0]) zfwd_passed_cuts = false;
	if (elfwd4v[1].Perp()*(1./GeV) < m_PtFwdCut || !inAcceptance[1]) zfwd_passed_cuts = false;

	if(zfwd_passed_cuts){ //fill plots if Z passed selection: mass cut, central electron Tight++, each electron passed corresponding pT cut and is out of crack regions
	  m_FWDZeePlots.fillZPlots(zfwd, 0);
	  m_FWDZeePlots.fillElPlots(elfwd4v[0], elfwd4v[1], 0);

	  if (oq_fwd){ // fwd electron passed OQ cut
	    m_FWDZeePlots.fillZPlots(zfwd, 1);
	    m_FWDZeePlots.fillElPlots(elfwd4v[0], elfwd4v[1], 1);
	  }
	  if (loose_fwd){ // fwd electron passed Loose++ cut
	    m_FWDZeePlots.fillZPlots(zfwd, 2);
	    m_FWDZeePlots.fillElPlots(elfwd4v[0], elfwd4v[1], 2);
	  }
	  if (tight_fwd){ // fwd electron passed Tight++ cut
	    m_FWDZeePlots.fillZPlots(zfwd, 3);
	    m_FWDZeePlots.fillElPlots(elfwd4v[0], elfwd4v[1], 3);
	  }

	}
	  
      }

    } //end if one central electron
    ///////////////////////////////////////////////////////////////////
    return StatusCode::SUCCESS;
  }
  //////////////////////////////////////////////////////////////////	  
  StatusCode ZeeValidationMonitoringTool::procHistograms()
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    //fill efficiencies 
    m_ReconElectronsPlots.finalize();
    if( !m_isData )
      {
	m_TrueElectronsPlots.finalize();
	m_TrueFwdElectronsPlots.finalize();
      }
    m_ZeePlots.finalize();
    m_FWDZeePlots.finalize();

    return StatusCode::SUCCESS;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////Tool for matching between truth particles and reconstructed electrons/////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  void ZeeValidationMonitoringTool::MatchElec(const xAOD::TruthParticle* truth_electron, const xAOD::ElectronContainer* electrons){
    drmin_elreco_truth = 9999.;
    matched_electron = nullptr;
    matchedE = false;

    for (auto electron : *electrons){
      double dr_reco_truth = truth_electron -> p4().DeltaR(electron -> p4());

      if (dr_reco_truth < drmin_elreco_truth){ 
	drmin_elreco_truth = dr_reco_truth;
	matched_electron = electron;
	matchedE = true;
      }
    }

    return;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////Tool for matching between truth particles and reconstructed photons///////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  void ZeeValidationMonitoringTool::MatchPhot(const xAOD::TruthParticle* truth_electron, const xAOD::PhotonContainer* photons){
    drmin_phreco_truth = 9999.;
    matchedP = false;

    for (auto photon : *photons){
      double dr_reco_truth = truth_electron -> p4().DeltaR(photon -> p4());

      if (dr_reco_truth < drmin_phreco_truth){ 
	drmin_phreco_truth = dr_reco_truth;
	matchedP = true;
      }
    }
   
    return;
  }

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////
	
  ///////////////////////////////////////////////////////////////////
  // Protected methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////
		
}
