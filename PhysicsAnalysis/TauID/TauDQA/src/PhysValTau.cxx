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

PhysValTau::PhysValTau(const std::string& type, 
		       const std::string& name, 
                       const IInterface* parent) : 
    ManagedMonitorToolBase(type, name, parent)
{
   declareProperty("TauContainerName", m_TauJetContainerName = "TauRecContainer");
   declareProperty("TruthParticleContainerName", m_TruthParticleContainerName = "TruthParticle");
   declareProperty("TauDetailsContainerName", m_TauDetailsContainerName = "TauRecDetailsContainer");
   declareProperty("isMC", m_isMC= false);
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
   if ( !taus ) { 
      ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_TauJetContainerName); 
      return StatusCode::FAILURE; 
   }         
   ATH_MSG_DEBUG("Number of taus: " << taus->size()); 

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Retrieve truth container:     
   const xAOD::TruthParticleContainer* truth = 0;
   if ( m_isMC ) {
      truth = evtStore()->retrieve<const xAOD::TruthParticleContainer>(m_TruthParticleContainerName);
      if ( !truth ) { 
	 ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_TruthParticleContainerName); 
	 return StatusCode::FAILURE;
      }   
      //  ATH_MSG_DEBUG( "Number of truth: " << truth->size() );      
   }
   //////////////////////////////////////////////////////////////////////////////////////////////////
   // Retrieve true hadronic tau container from truth container
   // getHaderonicTruth finds true taus (ID == 15) with correct status and no leptonic decay.
   const xAOD::TruthParticleContainer* trueTauContainer = 0;
   if ( m_isMC ) {
      trueTauContainer = truthHandler.getHadronicTruth(truth);
      ATH_MSG_DEBUG("Number of true had taus: " << trueTauContainer->size());
   }
    
   //////////////////////////////////////////
   // Plotting TauRec Variables
   
   // Loop through reco tau jet container
   for (auto tau : *taus) { 
      if (m_detailLevel < 10) continue;
      if (tau->pt()/GeV < 20) continue;
      // fill Substructure/PFO histograms
      m_oTauValidationPlots->m_oRecoTauAllProngsPlots.fill(*tau);
      m_oTauValidationPlots->m_oNewCorePlots.fill(*tau);
      m_oTauValidationPlots->m_oRecTauEffPlots.fill(*tau);
      //Reco Tau General Plots
      m_oTauValidationPlots->m_oRecoGeneralTauAllProngsPlots.fill(*tau);
      if ( tau->nTracks()==1 ) {
	 m_oTauValidationPlots->m_oRecoHad1ProngPlots.fill(*tau);
	 m_oTauValidationPlots->m_oRecTauEff1PPlots.fill(*tau);
         
      }
      if ( tau->nTracks()==3 ) { 
	 m_oTauValidationPlots->m_oRecoHad3ProngPlots.fill(*tau);
	 m_oTauValidationPlots->m_oRecTauEff3PPlots.fill(*tau);
      }
   }
   m_matched_itr.clear();
   //////////////////////////////////////////
   // Plotting TruthMatched Variables
   // Loop through reco tau jet container
   
   // 
   
   if ( (trueTauContainer != 0) && (m_isMC == true) ) {
      xAOD::TruthParticleContainer::const_iterator truth_itr = trueTauContainer->begin();
      xAOD::TruthParticleContainer::const_iterator truth_end = trueTauContainer->end();
      for ( ; truth_itr != truth_end; truth_itr++ ) {
	 ATH_MSG_DEBUG( "Trying to match tau :");
	 xAOD::TruthParticle* trueTau = const_cast<xAOD::TruthParticle*>(*truth_itr);
	 const xAOD::TauJet *matchedRecTau = 0;
	 ATH_MSG_DEBUG("Matching Tau");
	 bool matched = matchTrueAndRecoTau(taus,trueTau,matchedRecTau);
	 if ( matched ) {
	    m_oTauValidationPlots->m_oGeneralTauAllProngsPlots.fill(*matchedRecTau);
	    m_oTauValidationPlots->m_oNewCoreMatchedPlots.fill(*matchedRecTau);
	    // Substructure/PFO histograms 
	    m_oTauValidationPlots->m_oMatchedTauAllProngsPlots.fill(*matchedRecTau);
	    m_oTauValidationPlots->m_oMatchedTauEffPlots.fill(*matchedRecTau);
	    // 1-prong
	    if ( matchedRecTau->nTracks() == 1 ) {
	       m_oTauValidationPlots->m_oHad1ProngPlots.fill(*matchedRecTau);
	       m_oTauValidationPlots->m_oMatchedTauEff1PPlots.fill(*matchedRecTau);  
	    }
	    // 3-prong
	    else if( matchedRecTau->nTracks() == 3 ) {
	       m_oTauValidationPlots->m_oHad3ProngPlots.fill(*matchedRecTau);
	       m_oTauValidationPlots->m_oMatchedTauEff3PPlots.fill(*matchedRecTau);  
	    }
	 }
      }
      if ( taus != 0 ) {
	 xAOD::TauJetContainer::const_iterator tau_itr = taus->begin();
	 xAOD::TauJetContainer::const_iterator tau_end = taus->end();
	 for ( size_t iRecTau = 0 ; iRecTau < taus->size(); iRecTau++ ) {
	    bool alreadymatched = false;
	    if ( m_matched_itr.size() > 0 ) {
	       for ( size_t i = 0; i < m_matched_itr.size(); i++ ) {
		  if(m_matched_itr.at(i) == iRecTau) alreadymatched = true;
	       }
	    }
	    if ( alreadymatched ) continue;
	    const xAOD::TauJet *recTau = taus->at(iRecTau);
	    if ( m_detailLevel < 10 ) continue;
	    if ( recTau->pt()/GeV < 20 ) continue;
	    bool eleMatch = false;
	    // Matching to electrons (Improvements needed)
	    for ( auto mc_candidate : *truth ) {
	       if ( abs(mc_candidate->pdgId())==11 && mc_candidate->pt()>10000. ) {
		  float dr = sqrt( (mc_candidate->eta()-recTau->eta())*(mc_candidate->eta()-recTau->eta())
				   + (mc_candidate->phi()-recTau->phi())*(mc_candidate->phi()-recTau->phi()));
		  if( dr < 0.2 ) {
		     eleMatch = true;
		     continue;
		  }
	       }
	    }
	    // electron matched taus
	    if ( eleMatch ) {
	       m_oTauValidationPlots->m_oElMatchedParamPlots.fill(*recTau);
	       m_oTauValidationPlots->m_oElMatchedEVetoPlots.fill(*recTau);
	    }
	    else {
	       m_oTauValidationPlots->m_oFakeGeneralTauAllProngsPlots.fill(*recTau);
	       // Substructure/PFO histograms
	       m_oTauValidationPlots->m_oFakeTauAllProngsPlots.fill(*recTau);
	       m_oTauValidationPlots->m_oNewCoreFakePlots.fill(*recTau);
	       m_oTauValidationPlots->m_oFakeTauEffPlots.fill(*recTau);
	       if ( recTau->nTracks()==1 ) {
		  m_oTauValidationPlots->m_oFakeHad1ProngPlots.fill(*recTau);
		  m_oTauValidationPlots->m_oFakeTauEff1PPlots.fill(*recTau);
	       }
	       if ( recTau->nTracks()==3 ) {
		  m_oTauValidationPlots->m_oFakeTauEff3PPlots.fill(*recTau);
		  m_oTauValidationPlots->m_oFakeHad3ProngPlots.fill(*recTau);    
	       }
	    }
	 }
      }
   }   /////////////////////////////////////////////////////////////////////////////////////////////////
   
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
