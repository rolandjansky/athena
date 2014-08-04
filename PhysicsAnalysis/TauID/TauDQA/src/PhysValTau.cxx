/////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValTau.cxx 
// Implementation file for class PhysValTau
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "PhysValTau.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODTau/TauJetContainer.h" 
#include "xAODTruth/TruthParticleAuxContainer.h" 
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"  
#include "AthenaBaseComps/AthCheckMacros.h"
using CLHEP::GeV;
//namespace PhysVal {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

PhysValTau::PhysValTau( const std::string& type, 
			const std::string& name, 
			const IInterface* parent ) : 
    ManagedMonitorToolBase( type, name, parent )
{
    declareProperty( "TauContainerName", m_TauJetContainerName = "TauRecContainer" ); 
    declareProperty( "TruthParticleContainerName", m_TruthParticleContainerName = "TruthParticle" ); 
    declareProperty( "TauDetailsContainerName", m_TauDetailsContainerName = "TauRecDetailsContainer" ); 
    // std::cout << "Match tool Core Var Truth Cuts Et " << matchtool.getvisETCut() << " eta " << matchtool.getvisEtaCut() << std::endl;
}

// Destructor
///////////////
PhysValTau::~PhysValTau()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode PhysValTau::initialize()
{
    ATH_MSG_INFO ("Initializing " << name() << "...");    
    ATH_CHECK(ManagedMonitorToolBase::initialize());
    
    // geometrical matching between Reco Tau and MC tau
    m_DeltaRMatchCut          = 0.2;
    m_LeadingChPiPt_Threshold = 0.0;

    //Truth Matching Cuts
    m_visETcut                = 10.;
    m_visEtacut               = 2.5;

    
    //Initialize Counters
    
    m_truth_all = 0;
    m_truth_1p = 0;
    m_truth_3p = 0;
    
    m_truth_all_matched = 0;
    m_truth_1p_matched = 0;
    m_truth_3p_matched = 0;
    
    return StatusCode::SUCCESS;
}

StatusCode PhysValTau::bookHistograms()
{
    ATH_MSG_INFO ("Booking hists " << name() << "...");
    
    // Physics validation plots are level 10
    m_oTauValidationPlots.reset(new TauValidationPlots(0,"Tau/" + m_TauJetContainerName, m_TauJetContainerName));
    m_oTauValidationPlots->setDetailLevel(100);
    m_oTauValidationPlots->initialize();
    std::vector<HistData> hists = m_oTauValidationPlots->retrieveBookedHistograms();
    ATH_MSG_INFO ("Filling n of hists " << hists.size() << " ");
    for (auto hist : hists) {
	ATH_CHECK(regHist(hist.first,hist.second,all));
    }
    
    return StatusCode::SUCCESS;      
}

StatusCode PhysValTau::fillHistograms()
{

    ATH_MSG_INFO ("Filling hists " << name() << "...");

    // Retrieve tau container: 
    const xAOD::TauJetContainer* taus = evtStore()->retrieve< const xAOD::TauJetContainer >( m_TauJetContainerName ); 
    if (!taus) { 
	ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_TauJetContainerName); 
	return StatusCode::FAILURE; 
    }         
    ATH_MSG_DEBUG( "Number of taus: " << taus->size() ); 
    
    int i(0); 

    // Retrieve truth container:     
    const xAOD::TruthParticleContainer* truth = evtStore()->retrieve< const xAOD::TruthParticleContainer >( m_TruthParticleContainerName );
    if (!truth) { 
	ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_TruthParticleContainerName); 
	return StatusCode::FAILURE; 
    }   
    
    ATH_MSG_DEBUG( "Number of truth: " << truth->size() );      
       
    // Retrieve true hadronic tau container from truth container
    const xAOD::TruthParticleContainer* hadTauContainer = 0;
    hadTauContainer = truthHandler.getHadronicTruth(truth);
    ATH_MSG_DEBUG( "Number of true had taus: " << hadTauContainer->size() );      



    // Loop through reco tau jet container
    for (auto tau : *taus) { 
	if (m_detailLevel < 10) continue;
	if (tau->pt()/GeV < 20) continue;
	m_oTauValidationPlots->m_oRecoTauAllProngsPlots.fill(*tau); // Reconstruction histograms filled      
	m_oTauValidationPlots->m_oBDTinputPlots.fill(*tau); // BDT Histograms      
	m_oTauValidationPlots->m_oNewCorePlots.fill(*tau);          // NewCore histograms filled      

	m_oTauValidationPlots->m_oRecoGeneralTauAllProngsPlots.fill(*tau);//Reco Tau General Plots
	if(tau->nTracks()==1) m_oTauValidationPlots->m_oRecoHad1ProngPlots.fill(*tau); //Reco Tau 1P plots
	if(tau->nTracks()==3) m_oTauValidationPlots->m_oRecoHad3ProngPlots.fill(*tau);	   //Reco Tau 3P plots


	
	// Matching bools
	bool matchedHadTau1P=false;
	bool matchedHadTau3P=false;
	bool matchedElectron=false;
	bool matchedMuon=false;
	bool matchedHadTau=false;
	
	Double_t smallestR_tau = 2.0;
	Double_t smallestR_el  = 2.0;
	Double_t smallestR_mu  = 2.0;
	
	TLorentzVector closestHadTau;                              // Saves TLV of closest true had tau (Used for checks, not for anything else)
	// Loop of hadronic tau container
	if (hadTauContainer != 0) 
	    for (auto hadTau : *hadTauContainer) 
		{
		    TLorentzVector visSum = truthHandler.getTauVisibleSumTruth(hadTau);                    // Get visible TLV for true had tau		
		    if ((visSum.Et()/GeV < m_visETcut) || (fabs(visSum.Eta()) > m_visEtacut)) continue;    // Detector limitations
		    
		    //Number of Prongs currently gives errors (Will be used to match reco prongs to true prongs)
		    //int nPiCh = truthHandler.nProngTruthTau((hadTau));
		    //if ( int(tau->nTracks()) != nPiCh) continue;
		    
		    // find any taus which match to good truth taus using deltaR cut
		    // match reco tau with visible tau vector
		    float matchDR = visSum.DeltaR( tau->p4() );
		    if(matchDR <= smallestR_tau) 
			{//dRcut
			    smallestR_tau = matchDR;
			    matchedHadTau = true;         // used for now until better ele and mu matching
			    closestHadTau = visSum;
			}//dRcut
		}
	

	    


	// Matching to electrons (Improvements needed)
	for (auto mc_candidate : *truth) {
	    
	    if(abs(mc_candidate->pdgId())==11 && mc_candidate->pt()>10000.)
		{
		    float dr = sqrt( (mc_candidate->eta()-tau->eta())*(mc_candidate->eta()-tau->eta()) + (mc_candidate->phi()-tau->phi())*(mc_candidate->phi()-tau->phi()));
		    if( dr < smallestR_el ) {
			smallestR_el = dr; 
		    }
		}
	}
	

	// Matching to muons     (Improvements needed)
	for (auto mc_candidate : *truth) {
	    
	    if(abs(mc_candidate->pdgId())==13 && mc_candidate->pt()>10000.)
		{
		    float dr = sqrt( (mc_candidate->eta()-tau->eta())*(mc_candidate->eta()-tau->eta()) + (mc_candidate->phi()-tau->phi())*(mc_candidate->phi()-tau->phi()));
		    if( dr < smallestR_mu ) {
			smallestR_mu = dr; 
		    }
		}
	}
	
	// Hirachy of matched particles 
	if(smallestR_mu <0.4) matchedMuon=true;
	if(smallestR_el <0.4 && !matchedMuon) matchedElectron=true;
	if((smallestR_tau <0.4) && !matchedElectron && !matchedMuon) matchedHadTau = true;   //currently not doing anything
	
	//      ATH_MSG_DEBUG( "matched tau el mu "<< matchedHadTau<< " "<<matchedElectron<<" "<<matchedMuon);
	
	if(matchedHadTau && tau->nTracks() == 1) matchedHadTau1P = true;
	if(matchedHadTau && tau->nTracks() == 3) matchedHadTau3P = true;
	
	
	if(matchedHadTau) {
	    m_oTauValidationPlots->m_oGeneralTauAllProngsPlots.fill(*tau);
	    if(matchedHadTau1P) {
		m_oTauValidationPlots->m_oHad1ProngPlots.fill(*tau);
		m_oTauValidationPlots->m_oHad1ProngEVetoPlots.fill(*tau);
	    }
	    if(matchedHadTau3P) {
		m_oTauValidationPlots->m_oHad3ProngPlots.fill(*tau);
	    }
	}
	
	if(matchedElectron) {
	    m_oTauValidationPlots->m_oElMatchedParamPlots.fill(*tau);
	    m_oTauValidationPlots->m_oElMatchedEVetoPlots.fill(*tau);
	}
	
	
	if(!matchedHadTau && !matchedElectron && !matchedMuon) {
	    m_oTauValidationPlots->m_oFakeGeneralTauAllProngsPlots.fill(*tau);
	    if(tau->nTracks()==1) m_oTauValidationPlots->m_oFakeHad1ProngPlots.fill(*tau);
	    if(tau->nTracks()==3) m_oTauValidationPlots->m_oFakeHad3ProngPlots.fill(*tau);	   
	}
	
	ATH_MSG_VERBOSE( "Investigating tau #" << i );
	ATH_MSG_VERBOSE( "  taus: pt = " << tau->pt()
			 << ", eta = " << tau->eta()
			 << ", phi = " << tau->phi()
			 << ", e = " << tau->e()
			 << "minR = " << smallestR_tau
			 ); 
	/*	ATH_MSG_VERBOSE( 
			"  best had tau: pt = " << closestHadTau.Pt()
			<< ", eta = " << closestHadTau.Eta()
			<< ", phi = " << closestHadTau.Phi()
			<< ", e = " << closestHadTau.E()
			<< ", minR =" << smallestR_tau
			 ); 
	*/

	//	       << ", mass trk sys "<<tau->massTrkSys() );
	
	
	
	
	++i;
    }
    
    
    return StatusCode::SUCCESS;
}

StatusCode PhysValTau::procHistograms()
{
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
//_____________________________________________________________________________
Double_t DiffPhi(float dPhi) {                                    //If seperated by reflex angle, take <Pi angle.
    if (fabs(dPhi) > M_PI) return fabs(2.*M_PI-fabs(dPhi));
    return fabs(dPhi);
}
//_____________________________________________________________________________
Bool_t Cone(  Double_t eta_t,                                     //Method for Cone Check
	      Double_t phi_t,
	      Double_t eta_mc,
	      Double_t phi_mc,
	      Double_t Distance) {
    Double_t DR = sqrt( ((eta_t - eta_mc)*(eta_t - eta_mc)) 
			+(DiffPhi(phi_t - phi_mc))*(DiffPhi(phi_t - phi_mc)));
    if(DR<=Distance)
	return true;  
    else
	return false;
}

Double_t Cone(  Double_t eta_t,                                     //Method for Cone Check
		Double_t phi_t,
		Double_t eta_mc,
		Double_t phi_mc) {
    Double_t DR = sqrt( ((eta_t - eta_mc)*(eta_t - eta_mc)) 
			+(DiffPhi(phi_t - phi_mc))*(DiffPhi(phi_t - phi_mc)));
    return DR;
}
//____________________________________________________________________________

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


// }

//  LocalWords:  str 
