/////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValTau.cxx 
// Implementation file for class PhysValTau
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "../share/PhysValTau.h"

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
    ManagedMonitorToolBase( type, name, parent ),
    m_tTauTruthMatchingTool("TauAnalysisTools/TauTruthMatchingTool")
    //    
{

    declareProperty( "TauContainerName", m_TauJetContainerName = "TauRecContainer" ); 
    declareProperty( "TruthParticleContainerName", m_TruthParticleContainerName = "TruthParticle" ); 
    declareProperty( "TauDetailsContainerName", m_TauDetailsContainerName = "TauRecDetailsContainer" ); 
    declareProperty("isMC",                         m_isMC= false);
    declareProperty("TauTruthMatchingTool", m_tTauTruthMatchingTool, "the tau truth matching tool");
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
    CHECK( m_tTauTruthMatchingTool.retrieve() );
    
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
    m_oTauValidationPlots.reset(new TauValidationPlots(0,"Tau/" /* + m_TauJetContainerName*/, m_TauJetContainerName));
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
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Retrieve tau container: 
    const xAOD::TauJetContainer* taus = evtStore()->retrieve< const xAOD::TauJetContainer >( m_TauJetContainerName ); 

    if (!taus) { 
	ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_TauJetContainerName); 
	return StatusCode::FAILURE; 
    }         

    ATH_MSG_DEBUG( "Number of taus: " << taus->size() ); 
    CHECK(m_tTauTruthMatchingTool->initializeEvent());
    
    // Loop through reco tau jet container
    for (auto tau : *taus) { 
	if (m_detailLevel < 10) continue;
	if (tau->pt()/GeV < 20) continue; // Should change to tauSelector tool;
	ATH_MSG_DEBUG( "passed selection: "); 	
	m_oTauValidationPlots->m_oRecoTauAllProngsPlots.fill(*tau); // Substructure/PFO histograms filled      
	m_oTauValidationPlots->m_oNewCorePlots.fill(*tau);          // NewCore histograms filled      
	m_oTauValidationPlots->m_oRecTauEffPlots.fill(*tau);        //Eff Reco Tau plots
	m_oTauValidationPlots->m_oRecoGeneralTauAllProngsPlots.fill(*tau);             //Reco Tau General Plots

	if(tau->nTracks()==1){
	    m_oTauValidationPlots->m_oRecoHad1ProngPlots.fill(*tau); //Reco Tau 1P plots
	    m_oTauValidationPlots->m_oRecTauEff1PPlots.fill(*tau); //Eff Reco Tau 1P plots
	    
	}
	if(tau->nTracks()==3){ 
	    m_oTauValidationPlots->m_oRecoHad3ProngPlots.fill(*tau); //Reco Tau 3P plots
	    m_oTauValidationPlots->m_oRecTauEff3PPlots.fill(*tau); //Eff Reco Tau 3P plots
	}
	if (m_isMC){
	    const xAOD::TruthParticle* matched_truth = m_tTauTruthMatchingTool->applyTruthMatch(*tau);
	    if(matched_truth && matched_truth->isTau()) {		
		ATH_MSG_DEBUG("Tau matching found");
		m_oTauValidationPlots->m_oGeneralTauAllProngsPlots.fill(*tau);
		m_oTauValidationPlots->m_oNewCoreMatchedPlots.fill(*tau);          // NewCore histograms filled
		m_oTauValidationPlots->m_oMatchedTauAllProngsPlots.fill(*tau); // Substructure/PFO histograms filled
		m_oTauValidationPlots->m_oMatchedTauEffPlots.fill(*tau); 
		
		if(tau->nTracks() == 1){
		    m_oTauValidationPlots->m_oHad1ProngPlots.fill(*tau);
		    m_oTauValidationPlots->m_oMatchedTauEff1PPlots.fill(*tau);  
		}
		if(tau->nTracks() == 3){
		    m_oTauValidationPlots->m_oHad3ProngPlots.fill(*tau);
		    m_oTauValidationPlots->m_oMatchedTauEff3PPlots.fill(*tau);  
		}
	    }		
	    else if(matched_truth && matched_truth->isElectron()) {
		ATH_MSG_DEBUG("Electron match found");
		m_oTauValidationPlots->m_oElMatchedParamPlots.fill(*tau);
	    }
	    else if(!matched_truth){
		ATH_MSG_DEBUG("No tau match");
		m_oTauValidationPlots->m_oFakeGeneralTauAllProngsPlots.fill(*tau);
		m_oTauValidationPlots->m_oFakeTauAllProngsPlots.fill(*tau); // Substructure/PFO histograms filled
		m_oTauValidationPlots->m_oNewCoreFakePlots.fill(*tau);          // NewCore histograms filled
		m_oTauValidationPlots->m_oFakeTauEffPlots.fill(*tau);		    
		if(tau->nTracks()==1)
		    {
			m_oTauValidationPlots->m_oFakeHad1ProngPlots.fill(*tau);
			m_oTauValidationPlots->m_oFakeTauEff1PPlots.fill(*tau);
		    }
		if(tau->nTracks()==3) 
		    {
			m_oTauValidationPlots->m_oFakeTauEff3PPlots.fill(*tau);
			m_oTauValidationPlots->m_oFakeHad3ProngPlots.fill(*tau);	   
		    }
		
		
	    }
	    
	    
	}

	/*
	if(trueTauContainer != 0 && m_isMC==true){// This is where new truth matching will go
	    const xAOD::TruthParticle* matched_tau = T2MT->getTruth(*tau); // truth matching is done for a single xAOD tau object
	    ATH_MSG_DEBUG( "Trying to match tau :");
	  
	    }*/
    }
    ATH_MSG_DEBUG( "End Event: " ); 
    return StatusCode::SUCCESS;
}

StatusCode PhysValTau::procHistograms()
{
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    return StatusCode::SUCCESS;
}

bool PhysValTau::matchTrueAndRecoTau(const xAOD::TauJetContainer *&taus, const xAOD::TruthParticle *trueTau, const xAOD::TauJet *&matchedRecTau)
{
    std::vector< const xAOD::TruthParticle * > decayParticles;
    int nCharged = 0;
    int nNeutral = 0;
    //    RecoTypes::DecayMode trueDecayMode = getDecayMode(trueTau, decayParticles, nCharged, nNeutral);
    
    nCharged = truthHandler.nProngTruthTau(trueTau,false);
    nNeutral = truthHandler.numNeutPion(trueTau);
    RecoTypes::DecayMode trueDecayMode = RecoTypes::GetDecayMode(nCharged, nNeutral);
    ATH_MSG_DEBUG( "True decay mode is " << nCharged << "p" << nNeutral << "n" ); 
    
    if (!RecoTypes::IsValidMode(trueDecayMode)) {
	ATH_MSG_DEBUG(" Decay mode is not valid, skipping..." ); 
        return false;
    }
    
    TLorentzVector visSum = truthHandler.getTauVisibleSumTruth(trueTau);                    // Get visible TLV for true had tau		
    if ((visSum.Et()/GeV < m_visETcut) || (fabs(visSum.Eta()) > m_visEtacut)) return false;    // Detector limitations
    
    xAOD::TauJetContainer::const_iterator tau_itr = taus->begin();
    xAOD::TauJetContainer::const_iterator tau_end = taus->end();
    double minDeltaR = m_DeltaRMatchCut;
    bool matched = false;
    size_t iMatchedRecTau = 0;
    TLorentzVector hlv_matchedRecTau;
    
    for (size_t iRecTau = 0 ; iRecTau < taus->size(); iRecTau++) {
        const xAOD::TauJet *recTau = taus->at(iRecTau);
        double recPt  = recTau->ptIntermediateAxis();
        double recEta = recTau->etaIntermediateAxis();
        double recPhi = recTau->phiIntermediateAxis();
        double recM   = recTau->mIntermediateAxis();
        TLorentzVector hlv_recTau;
        hlv_recTau.SetPtEtaPhiM(recPt, recEta, recPhi, recM);
	
        double curDeltaR = visSum.DeltaR(hlv_recTau);
        if (curDeltaR < minDeltaR) {
            minDeltaR = curDeltaR;
            matchedRecTau = recTau;
            hlv_matchedRecTau = hlv_recTau;
            matched = true;
            iMatchedRecTau = iRecTau;
        }
    }
    
    if(matched){
	const xAOD::TauJet *recTau = taus->at(iMatchedRecTau);
	m_oTauValidationPlots->m_oMigrationPlots.fill(*recTau,nCharged,nNeutral); //Migration      
	m_matched_itr.push_back(iMatchedRecTau);
	//    std::cout<<"Number of matched between: "<<m_matched_itr.size()<<std::endl;
    }
    
    return matched;
}


/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
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
