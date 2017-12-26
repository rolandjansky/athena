/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_tauxstore_test.cxx 697574 2015-09-30 11:58:22Z krasznaa $

// System include(s):
#include <memory>

// ROOT include(s):
#include <TTree.h>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TAuxStore.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "CxxUtils/StrFormat.h"

/// Helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                            \
   do {                                                                 \
      const bool result = EXP;                                          \
      if( ! result ) {                                                  \
         ::Error( APP_NAME, "Expression \"%s\" failed the evaluation",  \
                  #EXP );                                               \
         return 1;                                                      \
      }                                                                 \
   } while( 0 )

int main() {

   // The name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_tauxstore_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Reference to the auxiliary type registry:
   SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

   // Create a TTree in memory to help with the reading tests:
   std::unique_ptr< ::TTree > itree( new ::TTree( "InputTree", "Input Tree" ) );
   itree->SetDirectory( 0 );

   // Fill this transient tree with some information:
   std::vector< float > var1{  1,  2,  3,  4,  5 };
   std::vector< float > var2{ 11, 12, 13, 14, 15 };
   std::vector< float >* var1Ptr = &var1;
   std::vector< float >* var2Ptr = &var2;
   ::TBranch* br1 = itree->Branch( "PrefixAuxDyn.var1", &var1Ptr );
   ::TBranch* br2 = itree->Branch( "PrefixAuxDyn.var2", &var2Ptr );
   if( ( ! br1 ) || ( ! br2 ) ) {
      ::Error( APP_NAME, "Couldn't create branches in transient input tree" );
      return 1;
   }
   if( itree->Fill() < 0 ) {
      ::Error( APP_NAME, "Failed to fill the transient data into the input "
               "tree" );
      return 1;
   }
   itree->Print();
   ::Info( APP_NAME, "Created transient input TTree for the test" );

   // Create the object that we want to test:
   xAOD::TAuxStore store( "PrefixAux." );
   store.lock();

   // Connect it to this transient input tree:
   RETURN_CHECK( APP_NAME, store.readFrom( itree.get() ) );

   // Check that it found the two variables that it needed to:
   ::Info( APP_NAME, "Auxiliary variables found on the input:" );
   std::vector<std::string> vars;
   for( SG::auxid_t auxid : store.getAuxIDs() ) {
     vars.push_back (CxxUtils::strformat ("  - name: %s, type: %s",
                                          reg.getName( auxid ).c_str(),
                                          reg.getTypeName( auxid ).c_str() ));
   }
   std::sort (vars.begin(), vars.end());
   for (const std::string& s : vars) {
      ::Info( APP_NAME, "%s", s.c_str());
   }
   SIMPLE_ASSERT( store.getAuxIDs().size() == 2 );

   // Create a transient decoration in the object:
   const auto decId = reg.getAuxID< int >( "decoration" );
   SIMPLE_ASSERT( store.getDecoration( decId, 5, 5 ) != 0 );

   // Make sure that the store now knows about this variable:
   SIMPLE_ASSERT( store.getAuxIDs().size() == 3 );

   // Check that it can be cleared out:
   SIMPLE_ASSERT (store.clearDecorations() == true);
   SIMPLE_ASSERT( store.getAuxIDs().size() == 2 );
   SIMPLE_ASSERT (store.clearDecorations() == false);
   SIMPLE_ASSERT( store.getAuxIDs().size() == 2 );

   // Try to overwrite an existing variable with a decoration, to check that
   // it can't be done:
   const SG::auxid_t var1Id = reg.findAuxID( "var1" );
   SIMPLE_ASSERT( var1Id != SG::null_auxid );
   bool exceptionThrown = false;
   try {
      store.getDecoration( var1Id, 2, 2 );
   } catch( const SG::ExcStoreLocked& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );

   // Set up a selection rule for the output writing:
   store.selectAux( { "var1", "decoration" } );

   // Create another transient tree to test the tree writing with:
   std::unique_ptr< ::TTree > otree( new ::TTree( "OutputTree",
                                                  "Output Tree" ) );
   otree->SetDirectory( 0 );

   // Connect the store object to the tree:
   RETURN_CHECK( APP_NAME, store.writeTo( otree.get() ) );

   // Create the decoration again:
   SIMPLE_ASSERT( store.getDecoration( decId, 5, 5 ) != 0 );
   SIMPLE_ASSERT( store.getAuxIDs().size() == 3 );

   // Try to overwrite an existing variable with a decoration, to check that
   // it can't be done:
   const SG::auxid_t var2Id = reg.findAuxID( "var2" );
   SIMPLE_ASSERT( var2Id != SG::null_auxid );
   exceptionThrown = false;
   try {
      store.getDecoration( var1Id, 2, 2 );
   } catch( const SG::ExcStoreLocked& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );
   exceptionThrown = false;
   try {
      store.getDecoration( var2Id, 2, 2 );
   } catch( const SG::ExcStoreLocked& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );

   // Check that the output tree looks as it should:
   if( otree->Fill() < 0 ) {
      ::Error( APP_NAME, "Failed to fill the transient data into the output "
               "tree" );
      return 1;
   }
   otree->Print();
   SIMPLE_ASSERT( otree->GetNbranches() == 2 );

   // Make sure that when we clear the decorations, the auxiliary ID is not
   // removed. Since this is a "persistent decoration" now:
   SIMPLE_ASSERT (store.clearDecorations() == false);
   SIMPLE_ASSERT( store.getAuxIDs().size() == 3 );

   return 0;
}
