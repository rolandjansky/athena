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
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

// Local include(s):
#include "CPAnalysisExamples/errorcheck.h"
#include "CPAnalysisExamples/JetCalibrationToolExample.h"

// Other includes
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "boost/unordered_map.hpp"

bool ptsorter( const xAOD::IParticle* j1, const xAOD::IParticle* j2 ) {
  return ( j1->pt() > j2->pt() );
}


int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   bool m_debug = false;

   // Check if we received a file name:
   if( argc < 2 ) {
      Error( APP_NAME, "No file name received!" );
      Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
      return 1;
   }

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[ 1 ];
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

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   // Create the tool(s) to test:
   // The tool will take care of registering the systematics
   CP::JetCalibrationToolExample jetTool( "JetCalibration" );



   // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   // IMPORTANT: tools need to be initialized before querying systematics
   // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   jetTool.msg().setLevel( MSG::INFO );
   CHECK( jetTool.initialize() );



   // In this example, we'll just take all of the recommended
   // systematics and loop over them one at a time.  A more
   // sophisticated example would call another tool to place the
   // systematics we evaluate more carefully.  this may involve
   // writing out the list of systematics to a file, ending this
   // program running another tool, and then restarting this program
   // with the generated list of systematics.

   // This example only uses the set of recommended systematics.  That
   // may or may not be the right thing, depending on the tool and
   // analysis in question.

   const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
   const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
   std::vector<CP::SystematicSet> sysList =
      CP::make_systematics_vector(recommendedSystematics);


   // Initialize the output

   // In the output file, we will have one tree per nuisance parameter point.
   TFile* outputFile = TFile::Open("output.root", "recreate");
   // SystematicSet caches its hash value, so it's perfect for unordered_map!
   boost::unordered_map<CP::SystematicSet, TTree*> sysTreeMap;

   // One output branch for this example
   float dijet_mass = 0;

   for (const auto& sys : sysList)
   {
      std::string treeName = "l_tree_" + sys.name();
      TTree* sysTree = new TTree(treeName.c_str(), "An example tree");
      sysTree->Branch("mass", &dijet_mass, "mass/F");
      sysTreeMap[sys] = sysTree;
   }

   // User selection decoration
   SG::AuxElement::Accessor<unsigned int> passSelection("passSelection");

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry )
   {
      if (m_debug) Info( APP_NAME, "Running on event %llu", entry );

      // Tell the object which entry to look at:
      event.getEntry( entry );

      // Print some event information for fun:
      const xAOD::EventInfo* ei = 0;
      CHECK( event.retrieve( ei, "EventInfo" ) );
      if (m_debug) {
         Info( APP_NAME,
               "===>>>  start processing event #%i, "
               "run #%i %i events processed so far  <<<===",
               static_cast< int >( ei->eventNumber() ),
               static_cast< int >( ei->runNumber() ),
               static_cast< int >( entry ) );
      }

      // Get the Jets from the event:
      const xAOD::JetContainer* jets = 0;
      CHECK( event.retrieve( jets, "AntiKt4LCTopoJets" ) );
      if (m_debug) {
         Info( APP_NAME, "Number of jets: %i",
               static_cast< int >( jets->size() ) );
      }

      // Debugging: print 4-vector of jets
      if (m_debug) Info( APP_NAME, "Printing jet 4-vector");

      // This can be used in the example of correctedCopy below
      xAOD::JetContainer::iterator jetItr;

      dijet_mass = 0;

      // Loop over systematic uncertainties
      for (const auto& sys : sysList)
      {

         // Tell the jet tool which variation to apply
         if (jetTool.applySystematicVariation(sys) != CP::SystematicCode::Ok) {
            Error(APP_NAME, "Cannot configure jet tool for systematics");
         }


         /******************************************************
         While looping over the detector-related systematic uncertainties, you
         probably want to create a copy of the final state objects with the
         systematic variation applied. There are two supported ways to do so.
         One is shown in the commented code section: in this case, the relies on
         the correctedCopy method that the CP tools provide. That method creates
         a deep copy of the object, that is, the full auxiliary store of the
         object is physically copied in memory. This is an expensive operation.

         The second approach - the one that is actually in use - delegates to
         the user to make a shallow copy of the object, and operates on the
         copied object with the applyCorrection method. The shallow copy of an
         object will share the auxiliary store with the original object: usually
         a much cheaper operation.  Think about a jet: it comes with ~ a hundred
         moment (a hundred floats) in teh auxiliary store. While applying the
         systematic uncertainties, it is likely that you just want to change the
         4-vector, though
         ******************************************************/

         /************** DEEP COPY EXAMPLE - RELY ON correctedCopy ************

         // Create a new empy jet container and corresponding auxiliary store

         xAOD::JetContainer l_jetContainer;
         xAOD::JetAuxContainer l_jetAuxContainer;

         // Connect the auxiliary store container to the jet container

         l_jetContainer.setStore(&l_jetAuxContainer) ;

         for( jetItr = jets->begin(); jetItr != jets->end(); ++jetItr ) {

            xAOD::Jet * l_jet = 0;

            // Calibrate jets using correctedCopy -- This is a deep copy of the object

            if (!jetTool.correctedCopy(*(*jetItr), l_jet)){
               Error(APP_NAME, "Cannot really apply calibration");
               continue;
            }

            // Apply some selection and store the jet in the container

            if (l_jet->pt() > 20000 && std::abs(l_jet->eta()) < 2.5){
               l_jetContainer.push_back(l_jet);
            } else {
               delete l_jet; // Prevent from a memory leak
            }
            if (m_debug) Info(APP_NAME, "Stored");
         } // End of loop over jets

         if (l_jetContainer.size() >= 2){
            dijet_mass =
               (l_jetContainer.at(0)->p4() + l_jetContainer.at(1)->p4()).M();
         }

         ********************** End of example using correctedCopy ************/

         /**************** Shallow copy examples *************/

         // Create a new jet container to be filled with jets that will pass the
         // selection NOTE: this has to be created in view mode, as the
         // ownership of the objects will have to remain to the shallow copied
         // containers

         xAOD::JetContainer l_jetContainer(SG::VIEW_ELEMENTS);

         // Make a shallow copy of the jet container
         auto jet_shallowCopy = xAOD::shallowCopyContainer( *jets );

         if (m_debug) Info(APP_NAME, "Beginning loop on jets");

         // The any container can be recorder on the TStore object declared above
         // Not really necessary in this case, but the store will take care of cleaning up
         // the containers when we will be done with that
         if( !store.record(jet_shallowCopy.first, "myShallowCopiedJets") ||
             !store.record(jet_shallowCopy.second, "myShallowCopiedJetsAux.") ){
            Error("execute()", "Failed to record shallow copy containers. Exiting.");
            return -1;
         }

         // Objects in the store can be retrieved the same way you do for TEvent

         // For example
         // const xAOD::JetContainer* myownjets = 0;
         // CHECK(m_store->retrieve(myownjets,"myShallowCopiedJets"));

         // Note that this time the loop is on the shallow copy container
         for( auto jet : *jet_shallowCopy.first ) {
            if (m_debug) {
               Info(APP_NAME, "A jet");
               Info(APP_NAME, "Original Jet pt %f, eta %f, phi %f, e %f",
                    jet->pt(), jet->eta(), jet->phi(), jet->e());
               Info(APP_NAME, "Access em fraction %f",
                    jet->auxdata< float > ("EMFrac"));
            }

            // Calibrate jets
            if (!jetTool.applyCorrection(*jet)){
               Error(APP_NAME, "Cannot really apply calibration");
               continue;
            }
            if (m_debug) {
               Info(APP_NAME, "Jet calibrated, about to store it in the container");
               Info(APP_NAME, "calibrated jet pt %f eta %f phi %f e %f",
                    jet->pt(), jet->eta(), jet->phi(), jet->e());
               Info(APP_NAME,"Access em fraction %f",
                    jet->auxdata< float > ("EMFrac"));
            }

            // Apply some selection
            if (jet->pt() > 20000 && std::abs(jet->eta()) < 2.5){
               // decorate the object
               passSelection(*jet) = 1;
               l_jetContainer.push_back(jet);
            }

            if (m_debug) Info(APP_NAME, "Stored");
         }

         // sort the jet container
         l_jetContainer.sort( ptsorter );

         // For debugging purposes: loop again on the container and print some information
         for( auto jet : l_jetContainer ) {
            if (m_debug) std::cout << jet->pt() << std::endl;
            if (m_debug) std::cout << passSelection(*jet) << std::endl;
         }

         // Compute the dijet mass
         if (l_jetContainer.size() >= 2){
            dijet_mass =
               (l_jetContainer.at(0)->p4() + l_jetContainer.at(1)->p4()).M();
         }

         // Write event to good tree
         sysTreeMap[sys]->Fill();


         if (m_debug) Info(APP_NAME, "About to cleanup the store");

         // Clean up
         l_jetContainer.clear();

         // The containers created by the shallow copy are owned by you.
         // Remember to delete them (not needed if they are recorded to the TStore
         //delete jet_shallowCopy.first;
         //delete jet_shallowCopy.second;

         store.clear();

      }

      // Close with a message:
      if ((entry + 1) % 100 == 0) {
         Info( APP_NAME,
               "===>>>  done processing event #%i, "
               "run #%i %i events processed so far  <<<===",
               static_cast< int >( ei->eventNumber() ),
               static_cast< int >( ei->runNumber() ),
               static_cast< int >( entry + 1 ) );
      }
   }

   // Write the output trees
   for (const auto& sys : sysList) {
      sysTreeMap[sys]->Write();
   }

   // Close output file
   outputFile->Close();

   // Return gracefully:
   return 0;
}
