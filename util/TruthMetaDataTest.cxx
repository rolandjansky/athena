// $Id$

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODEventInfo/EventInfo.h"

// ASG include(s):
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"

int main( int argc, char* argv[] ) {

   // Get the application's name:
   const char* APP_NAME = argv[ 0 ];

   // Check that we received at least one file name:
   if( argc < 2 ) {
      ::Error( APP_NAME, "Usage: %s <xAOD file1> [xAOD file2]...", APP_NAME );
      return 1;
   }

   // Initialise the application:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );

   // Create the truth weight tool:
   xAOD::TruthWeightTool weightTool( "TruthWeightTool" );
   weightTool.setProperty( "OutputLevel", MSG::INFO ).ignore();

   /*
   // Try creating ToolHandles for this tool:
   ToolHandle< xAOD::ITruthWeightTool > handle( "TruthWeightTool" );
   ToolHandleArray< xAOD::ITruthWeightTool > handleArray{ "TruthWeightTool" };

   // Check if they can be retrieved:
   if( handle.retrieve().isFailure() || handleArray.retrieve().isFailure() ) {
      ::Error( APP_NAME,
               "Couldn't retrieve the tested tool through tool handles" );
      return 1;
   }
   */

   // Loop over the files:
   for( int i = 1; i < argc; ++i ) {

      // Open the input file:
      std::unique_ptr< ::TFile > ifile( ::TFile::Open( argv[ i ], "READ" ) );
      if( ! ifile.get() ) {
         ::Error( APP_NAME, "Couldn't open file: %s", argv[ i ] );
         return 1;
      }
      ::Info( APP_NAME, "Opened file: %s", argv[ i ] );

      // Connect the TEvent object to it:
      RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

      // Loop over a few events:
      const ::Long64_t entries = event.getEntries();

      for ( ::Long64_t entry = 0; entry < entries; ++entry ) {

	// Get the current entry:
	event.getEntry( entry );

	if ( entry == 0) {
	  auto weightNames = weightTool.getWeightNames();
	  auto weights = weightTool.getWeights();
	  for (size_t i=0;i<weightNames.size();++i) {
	    std::string wname = weightNames[i];
	     ::Info( APP_NAME,"Weight %3lu has value %.3f = %.3f (index %lu) and name \"%s\"",
		     i,weights[i],weightTool.getWeight(wname),weightTool.getWeightIndex(wname),wname.c_str());
	  }
	}
	
	// Give some feedback of where we are:
	if ( (entry+1) % 1000 == 0 ) 
	  ::Info( APP_NAME, "Processed %5llu / %5llu events",entry+1,entries);
      }
   }
   // Return gracefully:
   return 0;
}

