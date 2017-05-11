/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_remap_test.cxx 646132 2015-02-11 15:17:44Z krasznaa $

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TUUID.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>

// EDM include(s):
#include "AthContainers/AuxVectorBase.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

int main() {

   // The application's name:
   static const char* APP_NAME = "ut_xaodrootaccess_remap_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Create the object(s) used for the testing:
   xAOD::TEvent event;

   // Declare some name remappings:
   RETURN_CHECK( APP_NAME, event.addNameRemap( "ElectronCollection",
                                               "Electrons" ) );
   RETURN_CHECK( APP_NAME, event.addNameRemap( "TauJetCollection", "Taus" ) );

   // Print the definitions:
   event.printNameRemap();

   // Connect the TEvent object to an input file:
   static const char* INFILE = "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/"
      "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD."
      "e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
      "AOD.01597980._000098.pool.root.1";
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( INFILE, "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file %s" ), INFILE );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

   // Load the first event:
   if( event.getEntry( 0 ) < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't load event 0 from file %s" ),
               INFILE );
      return 1;
   }

   // Retrieve a DataVector using an alias, and the actual name:
   const SG::AuxVectorBase* vec = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( vec, "Electrons" ) );
   RETURN_CHECK( APP_NAME, event.retrieve( vec, "ElectronCollection" ) );

   // Create a dummy, temporary file to test the object copying:
   TUUID uuid;
   const ::TString tempFName( ::TString::Format( "%s-%s.root", APP_NAME,
                                                 uuid.AsString() ) );
   std::unique_ptr< ::TFile > ofile( ::TFile::Open( tempFName, "CREATE" ) );
   if( ! ofile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open temporary file with name "
                                       "\"%s\"" ),
               tempFName.Data() );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event.writeTo( ofile.get() ) );

   // Copy the electrons to the output:
   RETURN_CHECK( APP_NAME, event.copy( "Electrons" ) );

   // Write the event:
   if( event.fill() < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "There was an error writing the event "
                                       "to the in-memory file") );
      return 1;
   }

   // Finish writing to the file:
   RETURN_CHECK( APP_NAME, event.finishWritingTo( ofile.get() ) );

   ::TTree* otree = dynamic_cast< ::TTree* >( ofile->Get( "CollectionTree" ) );
   if( ! otree ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "No CollectionTree found in test output "
                                       "file" ) );
      return 1;
   }
   otree->Print();

   // Now start reading from this temporary file to test its health:
   RETURN_CHECK( APP_NAME, event.readFrom( ofile.get() ) );

   // Load the first, and only event:
   if( event.getEntry( 0 ) < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't load event 0 from file %s" ),
               tempFName.Data() );
      return 1;
   }

   // Retrieve a DataVector using the actual name this time:
   RETURN_CHECK( APP_NAME, event.retrieve( vec, "Electrons" ) );

   // And this should fail:
   if( event.retrieve( vec, "ElectronCollection" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Failure is retrieval logic" ) );
      return 1;
   }

   // Clean up:
   ofile.reset();
   gSystem->Unlink( tempFName );

   // Return gracefully:
   return 0;
}
