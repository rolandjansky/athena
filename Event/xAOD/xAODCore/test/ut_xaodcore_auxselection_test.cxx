/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodcore_auxselection_test.cxx 653257 2015-03-11 11:26:15Z krasznaa $
//
// Unit test for the xAOD::AuxSelection class.
//

// System include(s):
#include <iostream>

// Core include(s):
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODCore/AuxSelection.h"

/// Helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                        \
   do {                                                             \
      const bool result = EXP;                                      \
      if( ! result ) {                                              \
         std::cerr << "Expression \"" << #EXP                       \
                   << "\" failed the evaluation" << std::endl;      \
         return 1;                                                  \
      }                                                             \
   } while( 0 )

/// Helper operator that is apparently missing from the base code
bool operator== ( const SG::auxid_set_t& id1, const SG::auxid_set_t& id2 ) {

   // Check that they are of the same size:
   if( id1.size() != id2.size() ) {
      return false;
   }
   // Check that all elements of the first set appear in the second set:
   for( SG::auxid_t id : id1 ) {
      if( ! id2.count( id ) ) {
         return false;
      }
   }
   // If all of these succeeded, then let's consider them equal:
   return true;
}

int main() {

   // Access the type registry:
   SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

   // Construct some example variable lists:
   SG::auxid_set_t idset1;
   idset1.insert( reg.getAuxID< int >( "IntVar1" ) );
   idset1.insert( reg.getAuxID< int >( "IntVar2" ) );
   SG::auxid_set_t idset2;
   idset2.insert( reg.getAuxID< float >( "FloatVar1" ) );
   idset2.insert( reg.getAuxID< float >( "FloatVar2" ) );
   SG::auxid_set_t idset3;
   idset3.insert( idset1.begin(), idset1.end() );
   idset3.insert( idset2.begin(), idset2.end() );

   // Construct the objects to be tested:
   xAOD::AuxSelection sel1;
   sel1.selectAux( std::set< std::string >{ "IntVar1", "FloatVar2" } );
   xAOD::AuxSelection sel2;
   sel2.selectAux( std::set< std::string >{ "IntVar2", "FloatVar1" } );
   xAOD::AuxSelection sel3;
   sel3.selectAux( std::set< std::string >{} );
   xAOD::AuxSelection sel4;
   sel4.selectAux( std::set< std::string >{ "-" } );
   xAOD::AuxSelection sel5;
   sel5.selectAux( std::set< std::string >{ "-IntVar1", "-FloatVar2" } );
   xAOD::AuxSelection sel6;
   sel6.selectAux( std::set< std::string >{ "-IntVar2", "-FloatVar1" } );
   xAOD::AuxSelection sel7;
   sel7.selectAux( std::set< std::string >{ "IntVar1", "-FloatVar1" } );
   xAOD::AuxSelection sel8;
   sel8.selectAux( std::set< std::string >{ "NonExistent", "IntVar1" } );
   xAOD::AuxSelection sel9;
   sel9.selectAux( std::set< std::string >{ "NonExistent", "FloatVar2" } );

   // Construct the variables that should come out of the processing:
   SG::auxid_set_t idref1;
   idref1.insert( reg.getAuxID< int >( "IntVar1" ) );
   idref1.insert( reg.getAuxID< float >( "FloatVar2" ) );
   SG::auxid_set_t idref2;
   idref2.insert( reg.getAuxID< int >( "IntVar2" ) );
   idref2.insert( reg.getAuxID< float >( "FloatVar1" ) );
   SG::auxid_set_t idref3;
   idref3.insert( reg.getAuxID< int >( "IntVar1" ) );
   SG::auxid_set_t idref4;
   idref4.insert( reg.getAuxID< int >( "IntVar2" ) );
   SG::auxid_set_t idref5;
   idref5.insert( reg.getAuxID< float >( "FloatVar1" ) );
   SG::auxid_set_t idref6;
   idref6.insert( reg.getAuxID< float >( "FloatVar2" ) );
   SG::auxid_set_t idref7;

   // Now check that the selections behave as we expected:
   SIMPLE_ASSERT( sel1.getSelectedAuxIDs( idset1 ) == idref3 );
   SIMPLE_ASSERT( sel1.getSelectedAuxIDs( idset2 ) == idref6 );
   SIMPLE_ASSERT( sel1.getSelectedAuxIDs( idset3 ) == idref1 );

   SIMPLE_ASSERT( sel2.getSelectedAuxIDs( idset1 ) == idref4 );
   SIMPLE_ASSERT( sel2.getSelectedAuxIDs( idset2 ) == idref5 );
   SIMPLE_ASSERT( sel2.getSelectedAuxIDs( idset3 ) == idref2 );

   SIMPLE_ASSERT( sel3.getSelectedAuxIDs( idset1 ) == idset1 );
   SIMPLE_ASSERT( sel3.getSelectedAuxIDs( idset2 ) == idset2 );
   SIMPLE_ASSERT( sel3.getSelectedAuxIDs( idset3 ) == idset3 );

   SIMPLE_ASSERT( sel4.getSelectedAuxIDs( idset1 ) == idref7 );
   SIMPLE_ASSERT( sel4.getSelectedAuxIDs( idset2 ) == idref7 );
   SIMPLE_ASSERT( sel4.getSelectedAuxIDs( idset3 ) == idref7 );

   SIMPLE_ASSERT( sel5.getSelectedAuxIDs( idset1 ) == idref4 );
   SIMPLE_ASSERT( sel5.getSelectedAuxIDs( idset2 ) == idref5 );
   SIMPLE_ASSERT( sel5.getSelectedAuxIDs( idset3 ) == idref2 );

   SIMPLE_ASSERT( sel6.getSelectedAuxIDs( idset1 ) == idref3 );
   SIMPLE_ASSERT( sel6.getSelectedAuxIDs( idset2 ) == idref6 );
   SIMPLE_ASSERT( sel6.getSelectedAuxIDs( idset3 ) == idref1 );

   SIMPLE_ASSERT( sel7.getSelectedAuxIDs( idset1 ) == idset1 );
   SIMPLE_ASSERT( sel7.getSelectedAuxIDs( idset2 ) == idset2 );
   SIMPLE_ASSERT( sel7.getSelectedAuxIDs( idset3 ) == idset3 );

   SIMPLE_ASSERT( sel8.getSelectedAuxIDs( idset1 ) == idref3 );
   SIMPLE_ASSERT( sel8.getSelectedAuxIDs( idset2 ) == idref7 );
   SIMPLE_ASSERT( sel8.getSelectedAuxIDs( idset3 ) == idref3 );

   SIMPLE_ASSERT( sel9.getSelectedAuxIDs( idset1 ) == idref7 );
   SIMPLE_ASSERT( sel9.getSelectedAuxIDs( idset2 ) == idref6 );
   SIMPLE_ASSERT( sel9.getSelectedAuxIDs( idset3 ) == idref6 );

   // Return gracefully:
   return 0;
}
