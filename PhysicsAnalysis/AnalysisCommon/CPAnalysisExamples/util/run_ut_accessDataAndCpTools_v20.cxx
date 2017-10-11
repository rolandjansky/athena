/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPSystematicExample.cxx 300492 2014-04-30 14:39:05Z krasznaa $

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TTree.h>


// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

// Local include(s):

#include "CPAnalysisExamples/errorcheck.h"
#include "CPAnalysisExamples/MyToolProvider.h"

int main( int /*argc*/, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   bool m_debug = false;

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[1];

   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   TFile * ifile( TFile::Open( fileName, "READ" ) );
   if (!ifile) Error(APP_NAME, "%s", ("Cannot find file " + fileName).Data());

   // Create a TEvent object:
   xAOD::TEvent event( ifile, xAOD::TEvent::kClassAccess );
   CHECK( event.readFrom( ifile ) );
   Info( APP_NAME, "Number of events in the file: %i",
         static_cast< int >( event.getEntries() ) );

   // Create a transient object store. Needed for the tools.                                                      
   xAOD::TStore store; 

   // Create a tool provider and initialise tools 

   MyToolProvider toolProvider("toolProvider");
   CHECK(toolProvider.initialize());

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   const Long64_t e = 100;
   if( e < entries ) {
     entries = e;
   }

   for( Long64_t entry = 0; entry < entries; ++entry )
   {
     if (m_debug) Info( APP_NAME, "Running on event %i", ((int)entry));
      // Tell the object which entry to look at:
      event.getEntry( entry );
     
      // Print some event information for fun:
      const xAOD::EventInfo* ei = 0;
      CHECK( event.retrieve( ei, "EventInfo" ) );
      if (m_debug) Info( APP_NAME,
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry ) );
      
      // Basic test for ability of reading containers: is the pt of any of the objects larger than something?

      bool test_pt = false;

      /***************************************/ 
      // test jets

      // Get the Jets from the event:
      const xAOD::JetContainer* jets = 0;
      CHECK( event.retrieve( jets, "AntiKt4LCTopoJets" ) );

      // Check that the container can be read

      if (m_debug) Info( APP_NAME, "Number of jets: %i",
            static_cast< int >( jets->size() ) );
      
      // Create shallow copy container

      std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jet_shallowCopy = xAOD::shallowCopyContainer( *jets );

      xAOD::JetContainer::iterator jetItr;

      for( jetItr = (jet_shallowCopy.first)->begin(); jetItr != (jet_shallowCopy.first)->end(); ++jetItr ) {
	if (m_debug) Info(APP_NAME, "Original Jet pt %f, eta %f, phi %f, e %f", (*jetItr)->pt(), (*jetItr)->eta(), (*jetItr)->phi(), (*jetItr)->e());
	
	// Calibrate jets

	CHECK(toolProvider.jetCalibTool->applyCorrection(**jetItr));

	// Test JER

	// Get the MC resolution
	double mcRes = toolProvider.jerTool->getRelResolutionMC(*jetItr);
	// Get the resolution uncertaintu
	double uncert = toolProvider.jerTool->getUncertainty(*jetItr);

	if (m_debug) Info( APP_NAME, "mc resolution %f uncertainty %f",mcRes,uncert);

	CHECK(toolProvider.jerSmearingTool->applyCorrection(**jetItr));

	test_pt = (*jetItr)->pt() > 5000; 
	if (test_pt) break;
      }

      if ((jet_shallowCopy.first)->size() > 0 && !test_pt) {
	Error(APP_NAME, "All jets have pt < 5000 MeV. This is weird");
	return -1;
      }

      delete jet_shallowCopy.first;
      delete jet_shallowCopy.second;

      /******************************************/ 
      // test electrons

      test_pt = false;

      const xAOD::ElectronContainer* electrons = 0;
      CHECK( event.retrieve(electrons, "Electrons") );

      // Check that the container can be read

      if (m_debug) Info( APP_NAME, "Number of electrons: %i",
            static_cast< int >( electrons->size() ) );
      
      // Create shallow copy container

      std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > ele_shallowCopy = xAOD::shallowCopyContainer( *electrons );
      
      xAOD::ElectronContainer::iterator eleItr;
      
      for( eleItr = (ele_shallowCopy.first)->begin(); eleItr != (ele_shallowCopy.first)->end(); ++eleItr ) {
	if (m_debug) Info(APP_NAME, "Original ele pt %f, eta %f, phi %f, e %f", (*eleItr)->pt(), (*eleItr)->eta(), (*eleItr)->phi(), (*eleItr)->e());

	// Calibrate electron
	
	if ((*eleItr)->pt() > 10000){
	  CHECK(toolProvider.egammaCalibTool->applyCorrection(**eleItr));
	}

	if ((*eleItr)->pt() > 10000){
          double eleSF = 1.;
          CHECK( toolProvider.elecEfficiencySFTool->getEfficiencyScaleFactor(**eleItr, eleSF) );
          if (m_debug) Info(APP_NAME, "ElectronsSF %g", eleSF);
	  //const Root::TResult &result = toolProvider.elecEfficiencySFTool->calculate(*eleItr);
	  //if (m_debug) Info(APP_NAME,"ElectronsSF %f",result.getScaleFactor());
	}

	// If any electron has pt > 200 GeV declare test successful 

	test_pt = (*eleItr)->pt() > 200; 
	if (test_pt) break;

      }

      if ((ele_shallowCopy.first)->size() > 0 && !test_pt) {
	Error(APP_NAME, "All ele have pt < 200 MeV. This is weird");
	return -1;
      }

      delete ele_shallowCopy.first;
      delete ele_shallowCopy.second;

      /********************************/ 
      // test muons 

      test_pt = false;

      const xAOD::MuonContainer* muons = 0;
      CHECK( event.retrieve(muons, "Muons") );

      // Check that the container can be read

      if (m_debug) Info( APP_NAME, "Number of muons: %i",
            static_cast< int >( muons->size() ) );
      
      // Create shallow copy container

      std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > muo_shallowCopy = xAOD::shallowCopyContainer( *muons );
      
      xAOD::MuonContainer::iterator muoItr;
      
      for( muoItr = (muo_shallowCopy.first)->begin(); muoItr != (muo_shallowCopy.first)->end(); ++muoItr ) {
	if (m_debug) Info(APP_NAME, "Original muon pt %f, eta %f, phi %f, e %f", (*muoItr)->pt(), (*muoItr)->eta(), (*muoItr)->phi(), (*muoItr)->e());

	CHECK(toolProvider.muonCalibrationAndSmearingTool->applyCorrection(**muoItr));
	
	float sf = 0;

	CHECK(toolProvider.muonEfficiencySFTool->getEfficiencyScaleFactor((**muoItr),sf));

	if (m_debug) Info(APP_NAME, "Muon scale factor %f", sf);

	if (!toolProvider.muonSelectionTool->accept(**muoItr))
	  if (m_debug) Info(APP_NAME,"Muon not accepted by selector");

	test_pt = (*muoItr)->pt() > 200; 
	if (test_pt) break;
      }

      if ((muo_shallowCopy.first)->size() > 0 && !test_pt) {
	Error(APP_NAME, "All muons have pt < 200 MeV. This is weird");
	return -1;
      }

      delete muo_shallowCopy.first;
      delete muo_shallowCopy.second;

      /***********************************/
      // test taus 

      test_pt = false;

      const xAOD::TauJetContainer* taus = 0;
      CHECK( event.retrieve(taus, "TauJets") );

      // Check that the container can be read

      if (m_debug) Info( APP_NAME, "Number of taus: %i",
            static_cast< int >( taus->size() ) );
      
      // Create shallo copy container

      std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* > tau_shallowCopy = xAOD::shallowCopyContainer( *taus );
      
      xAOD::TauJetContainer::iterator tauItr;
      
      for( tauItr = (tau_shallowCopy.first)->begin(); tauItr != (tau_shallowCopy.first)->end(); ++tauItr ) {
	if (m_debug) Info(APP_NAME, "Original tau pt %f, eta %f, phi %f, e %f", (*tauItr)->pt(), (*tauItr)->eta(), (*tauItr)->phi(), (*tauItr)->e());


	if (!toolProvider.tauSelTool->accept(**tauItr)){
	  if (m_debug) Info(APP_NAME,"This tau with pt %f is rejected",(*tauItr)->pt());
	  //	  double tau_sf = 0;
	  //	  CHECK(toolProvider.tauEffTool->getEfficiencyScaleFactor(**tauItr, tau_sf));
	}

	
	test_pt = (*tauItr)->pt() > 200; 
	if (test_pt) break;
      }

      if ((tau_shallowCopy.first)->size() > 0 && !test_pt) {
	Error(APP_NAME, "All taus have pt < 200 MeV. This is weird");
	return -1;
      }

      delete tau_shallowCopy.first;
      delete tau_shallowCopy.second;

      /*****************************/
      // test photons
      
      test_pt = false;

      const xAOD::PhotonContainer* photons = 0;
      CHECK( event.retrieve(photons, "Photons") );

      // Check that the container can be read

      if (m_debug) Info( APP_NAME, "Number of photons: %i",
            static_cast< int >( photons->size() ) );
      
      // Create shallow copy container

      std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > pho_shallowCopy = xAOD::shallowCopyContainer( *photons );
      
      xAOD::PhotonContainer::iterator phoItr;
      
      for( phoItr = (pho_shallowCopy.first)->begin(); phoItr != (pho_shallowCopy.first)->end(); ++phoItr ) {
	if (m_debug) Info(APP_NAME, "Original photon pt %f, eta %f, phi %f, e %f", (*phoItr)->pt(), (*phoItr)->eta(), (*phoItr)->phi(), (*phoItr)->e());
	if ((*phoItr)->pt() > 10000){
	  CHECK(toolProvider.egammaCalibTool->applyCorrection(**phoItr));
	}
	test_pt = (*phoItr)->pt() > 200; 
	if (test_pt) break;
      }

      if ((pho_shallowCopy.first)->size() > 0 && !test_pt) {
	Error(APP_NAME, "All photons have pt < 200 MeV. This is weird");
	return -1;
      }

      delete pho_shallowCopy.first;
      delete pho_shallowCopy.second;


     
      // Close with a message:
      if ((entry + 1) % 100 == 0) Info( APP_NAME,
            "===>>>  done processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry + 1 ) );

   }



   // Return gracefully:
   return 0;
}
