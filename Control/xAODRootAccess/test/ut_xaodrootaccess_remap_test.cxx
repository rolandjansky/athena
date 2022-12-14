/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

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

#include "AsgMessaging/MessageCheck.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // The application's name:
   static const char* const APP_NAME = "ut_xaodrootaccess_remap_test";

   // Initialise the environment:
   ANA_CHECK( xAOD::Init( APP_NAME ) );

   // Create the object(s) used for the testing:
   xAOD::TEvent event;

   // Declare some name remappings:
   ANA_CHECK( event.addNameRemap( "Muons",
                                               "MyMuons" ) );
   ANA_CHECK( event.addNameRemap( "Taus", "MyTaus" ) );

   // Print the definitions:
   event.printNameRemap();

   // Connect the TEvent object to an input file:
   static const char* const FNAME = "${ASG_TEST_FILE_DATA}";
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file %s" ), FNAME );
      return 1;
   }
   ANA_CHECK( event.readFrom( ifile.get() ) );

   // Load the first event:
   if( event.getEntry( 0 ) < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't load event 0 from file %s" ),
               FNAME );
      return 1;
   }

   // Retrieve a DataVector using an alias, and the actual name:
   const SG::AuxVectorBase* vec = 0;
   ANA_CHECK( event.retrieve( vec, "Muons" ) );
   ANA_CHECK( event.retrieve( vec, "MyMuons" ) );

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
   ANA_CHECK( event.writeTo( ofile.get() ) );

   // Copy the electrons to the output:
   ANA_CHECK( event.copy( "MyMuons" ) );

   // Write the event:
   if( event.fill() < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "There was an error writing the event "
                                       "to the in-memory file") );
      return 1;
   }

   // Finish writing to the file:
   ANA_CHECK( event.finishWritingTo( ofile.get() ) );

   ::TTree* otree = dynamic_cast< ::TTree* >( ofile->Get( "CollectionTree" ) );
   if( ! otree ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "No CollectionTree found in test output "
                                       "file" ) );
      return 1;
   }
   otree->Print();

   // Now start reading from this temporary file to test its health:
   ANA_CHECK( event.readFrom( ofile.get() ) );

   // Load the first, and only event:
   if( event.getEntry( 0 ) < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't load event 0 from file %s" ),
               tempFName.Data() );
      return 1;
   }

   // Retrieve a DataVector using the actual name this time:
   ANA_CHECK( event.retrieve( vec, "MyMuons" ) );

   // And this should fail:
   if( event.retrieve( vec, "Muons" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Failure is retrieval logic" ) );
      return 1;
   }

   // Clean up:
   ofile.reset();
   gSystem->Unlink( tempFName );

   // Return gracefully:
   return 0;
}
