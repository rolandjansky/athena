/////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

PhysValTau::PhysValTau(const std::string& type, 
		       const std::string& name, 
                       const IInterface* parent) : 
   ManagedMonitorToolBase(type, name, parent),
   m_truthTool("TauAnalysisTools::TauTruthMatchingTool/"
	       "TauTruthMatchingTool", this),
   m_primTauSel("TauAnalysisTools::TauSelectionTool/"
		"PrimitiveTauSelectionTool", this),
   m_nomiTauSel("TauAnalysisTools::TauSelectionTool/"
		"NominalTauSelectionTool", this)
{
   declareProperty("TauContainerName", m_TauJetContainerName = "TauRecContainer");
   declareProperty("TruthParticleContainerName", m_TruthParticleContainerName = "TruthParticle");
   declareProperty("TauDetailsContainerName", m_TauDetailsContainerName = "TauRecDetailsContainer");
   declareProperty("isMC", m_isMC = false);
   declareProperty("TauTruthMatchingTool", m_truthTool);
   declareProperty("PrimitiveTauSelectionTool", m_primTauSel);
   declareProperty("NominalTauSelectionTool", m_nomiTauSel);
}

// Athena algtool's Hooks
////////////////////////////
StatusCode PhysValTau::initialize()
{
   ATH_MSG_INFO ("Initializing " << name() << "...");    
   ATH_CHECK(ManagedMonitorToolBase::initialize());

   if ( m_isMC ) {
      ATH_CHECK(m_truthTool.retrieve());
   }
   // selections are configured in PhysicsValidation job options
   ATH_CHECK(m_primTauSel.retrieve());
   ATH_CHECK(m_nomiTauSel.retrieve());
   
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

   // Retrieve tau container:
   const xAOD::TauJetContainer* taus = evtStore()
      ->retrieve<const xAOD::TauJetContainer>(m_TauJetContainerName); 
   if ( !taus ) { 
      ATH_MSG_ERROR("Couldn't retrieve tau container with key: "
		    << m_TauJetContainerName); 
      return StatusCode::FAILURE; 
   }         
   ATH_MSG_DEBUG("Number of taus: " << taus->size());

   // Retrieve truth container:     
   const xAOD::TruthParticleContainer* truthParticles = 0;
   if ( m_isMC ) {
      truthParticles = evtStore()->retrieve<const xAOD::TruthParticleContainer>(m_TruthParticleContainerName);
      if ( !truthParticles ) {
	 ATH_MSG_ERROR ("Couldn't retrieve tau container with key: "
			<< m_TruthParticleContainerName);
	 return StatusCode::FAILURE;
      }   
      //  ATH_MSG_DEBUG( "Number of truth: " << truth->size() );
   }

   // Loop through reco tau jet container
   for (auto tau : *taus) {
      if ( m_detailLevel < 10 ) continue;
      if ( !static_cast<bool>(m_primTauSel->accept(*tau)) ) continue;
      bool nominal = static_cast<bool>(m_nomiTauSel->accept(*tau));
      
      // fill histograms for reconstructed taus
      m_oTauValidationPlots->m_oRecoTauAllProngsPlots.fill(*tau);
      m_oTauValidationPlots->m_oNewCorePlots.fill(*tau);
      m_oTauValidationPlots->m_oRecTauEffPlots.fill(*tau);
      m_oTauValidationPlots->m_oRecoGeneralTauAllProngsPlots.fill(*tau);
      if ( nominal ) {
	 m_oTauValidationPlots->m_oRecoGeneralNom.fill(*tau);
	 m_oTauValidationPlots->m_oRecTauEffPlotsNom.fill(*tau);
	 m_oTauValidationPlots->m_oRecTauRecoTauPlotsNom.fill(*tau);
	 m_oTauValidationPlots->m_oNewCoreRecTauPlotsNom.fill(*tau);
      }
      int recProng = tau->nTracks();
      if ( recProng == 1 ) {
	 m_oTauValidationPlots->m_oRecoHad1ProngPlots.fill(*tau);
	 m_oTauValidationPlots->m_oRecTauEff1PPlots.fill(*tau);
	 if ( nominal ) {
	    m_oTauValidationPlots->m_oRecoHad1ProngNom.fill(*tau);
	    m_oTauValidationPlots->m_oRecTauEff1PPlotsNom.fill(*tau);
	 }
      }
      else if ( recProng == 3 ) { 
	 m_oTauValidationPlots->m_oRecoHad3ProngPlots.fill(*tau);
	 m_oTauValidationPlots->m_oRecTauEff3PPlots.fill(*tau);
	 if ( nominal ) {
	    m_oTauValidationPlots->m_oRecoHad3ProngNom.fill(*tau);
	    m_oTauValidationPlots->m_oRecTauEff3PPlotsNom.fill(*tau);
	 }
      }
      
      // Don't fill truth and fake histograms if we are running on data.
      if ( !m_isMC ) continue;
      
      ATH_MSG_DEBUG("Trying to truth-match tau");
      auto trueTau = m_truthTool->getTruth(*tau);

      // Fill truth and fake histograms
      if ( (bool)tau->auxdata<char>("IsTruthMatched") ) {
      	 ATH_MSG_DEBUG("Tau is truth-matched");
	 if ( trueTau->isTau() ) {
	    if ( (bool)trueTau->auxdata<char>("IsHadronicTau") ) {
	       ATH_MSG_DEBUG("Tau is hadronic tau");
	       m_oTauValidationPlots->m_oGeneralTauAllProngsPlots.fill(*tau);
	       m_oTauValidationPlots->m_oNewCoreMatchedPlots.fill(*tau);
	       // Substructure/PFO histograms 
	       m_oTauValidationPlots->m_oMatchedTauAllProngsPlots.fill(*tau);
	       m_oTauValidationPlots->m_oMatchedTauEffPlots.fill(*tau);
	       if ( nominal ) {
		  m_oTauValidationPlots->m_oMatchedGeneralNom.fill(*tau);
		  m_oTauValidationPlots->m_oMatchedTauEffPlotsNom.fill(*tau);
		  m_oTauValidationPlots->m_oMatchedTauRecoTauPlotsNom.fill(*tau);
		  m_oTauValidationPlots->m_oNewCoreMatchedPlotsNom.fill(*tau);
	       }
	       if ( recProng == 1 ) {
		  m_oTauValidationPlots->m_oHad1ProngPlots.fill(*tau);
		  m_oTauValidationPlots->m_oMatchedTauEff1PPlots.fill(*tau);  
		  if ( nominal ) {
		     m_oTauValidationPlots->m_oMatchedHad1ProngNom.fill(*tau);
		     m_oTauValidationPlots->m_oMatchedTauEff1PPlotsNom.fill(*tau);
		  }
	       }
	       else if ( recProng == 3 ) {
		  m_oTauValidationPlots->m_oHad3ProngPlots.fill(*tau);
		  m_oTauValidationPlots->m_oMatchedTauEff3PPlots.fill(*tau);  
		  if ( nominal ) {
		     m_oTauValidationPlots->m_oMatchedHad3ProngNom.fill(*tau);
		     m_oTauValidationPlots->m_oMatchedTauEff3PPlotsNom.fill(*tau);
		  }
	       }

	       xAOD::TauJetParameters::DecayMode trueMode
		  = m_truthTool->getDecayMode(*trueTau);
	       m_oTauValidationPlots->m_oMigrationPlots.fill(*tau, trueMode);
	       if ( nominal ) {
		  m_oTauValidationPlots->m_oMigrationPlotsNom.fill(*tau, trueMode);
	       }
	    }
	 }
      }
      else {
      	 ATH_MSG_DEBUG("Tau is fake");
	 // try to match to truth electrons
	 bool isElectron = false;
	 for ( auto truth : *truthParticles ) {
	    if ( abs(truth->pdgId()) != 11 ) continue;
	    if ( truth->status() != 1 ) continue;
	    if ( truth->pt() < 10000.0 ) continue;
	    if ( tau->p4().DeltaR(truth->p4()) > 0.2 ) continue;
	    // OK, now it probably is an electron
	    isElectron = true;
	    break;
	 }
	 if ( isElectron ) {
	    m_oTauValidationPlots->m_oElMatchedParamPlots.fill(*tau);
	    m_oTauValidationPlots->m_oElMatchedEVetoPlots.fill(*tau);
	 }
	 else { // other fakes
	    m_oTauValidationPlots->m_oFakeGeneralTauAllProngsPlots.fill(*tau);
	    // Substructure/PFO histograms
	    m_oTauValidationPlots->m_oFakeTauAllProngsPlots.fill(*tau);
	    m_oTauValidationPlots->m_oNewCoreFakePlots.fill(*tau);
	    m_oTauValidationPlots->m_oFakeTauEffPlots.fill(*tau);
	    if ( nominal ) {
	       m_oTauValidationPlots->m_oFakeGeneralNom.fill(*tau);
	       m_oTauValidationPlots->m_oFakeTauEffPlotsNom.fill(*tau);
	       m_oTauValidationPlots->m_oFakeTauRecoTauPlotsNom.fill(*tau);
	       m_oTauValidationPlots->m_oNewCoreFakePlotsNom.fill(*tau);
	    }
	    if ( recProng == 1 ) {
	       m_oTauValidationPlots->m_oFakeHad1ProngPlots.fill(*tau);
	       m_oTauValidationPlots->m_oFakeTauEff1PPlots.fill(*tau);
	       if ( nominal ) {
		  m_oTauValidationPlots->m_oFakeHad1ProngNom.fill(*tau);
		  m_oTauValidationPlots->m_oFakeTauEff1PPlotsNom.fill(*tau);
	       }
	    }
	    if ( recProng == 3 ) {
	       m_oTauValidationPlots->m_oFakeTauEff3PPlots.fill(*tau);
	       m_oTauValidationPlots->m_oFakeHad3ProngPlots.fill(*tau);    
	       if ( nominal ) {
		  m_oTauValidationPlots->m_oFakeHad3ProngNom.fill(*tau);
		  m_oTauValidationPlots->m_oFakeTauEff3PPlotsNom.fill(*tau);
	       }
	    }
	 }
      }
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
