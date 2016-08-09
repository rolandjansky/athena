/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodcore_safedeepcopy_test.cxx 682039 2015-07-10 09:15:16Z krasznaa $

// System include(s):
#include <vector>
#include <iostream>

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/tools/SafeDeepCopy.h"
#include "xAODCore/tools/PrintHelpers.h"

/// Auxiliary container used during the tests
class BreakableAuxContainer : public xAOD::AuxContainerBase {

public:
   /// Default constructor
   BreakableAuxContainer() {

      AUX_VARIABLE( testVar1 );
      AUX_VARIABLE( testVar2 );
      AUX_VARIABLE( testVar3 );
   }

   /// @name Functions "breaking" various variables in the container
   /// @{
   void breakTestVar1() {
      testVar1.clear();
   }
   void breakTestVar2() {
      testVar2.clear();
   }
   void breakTestVar3() {
      testVar3.clear();
   }
   /// @}

private:
   /// @name Test variables
   /// @{
   std::vector< int > testVar1;
   std::vector< float > testVar2;
   std::vector< short > testVar3;
   /// @}

}; // class BreakableAuxContainer

int main() {

   // Create a healthy and a "broken" container:
   BreakableAuxContainer healthyAux, brokenAux;
   DataVector< SG::AuxElement > healthyCont, brokenCont;
   healthyCont.setStore( &healthyAux );
   brokenCont.setStore( &brokenAux );

   // Fill them with the same data:
   float j = 0.0;
   short k = 0;
   for( int i = 0; i < 3; ++i, j += 1.5, k += 2 ) {
      healthyCont.push_back( new SG::AuxElement() );
      healthyCont.back()->auxdata< int >( "testVar1" ) = i;
      healthyCont.back()->auxdata< float >( "testVar2" ) = j;
      healthyCont.back()->auxdata< short >( "testVar3" ) = k;
      brokenCont.push_back( new SG::AuxElement() );
      brokenCont.back()->auxdata< int >( "testVar1" ) = i;
      brokenCont.back()->auxdata< float >( "testVar2" ) = j;
      brokenCont.back()->auxdata< short >( "testVar3" ) = k;
   }

   // Print their contents:
   std::cout << "Content of healthyCont:" << std::endl;
   for( const SG::AuxElement* el : healthyCont ) {
      std::cout << "  " << *el << std::endl;
   }
   std::cout << "Content of brokenCont (before breaking):" << std::endl;
   for( const SG::AuxElement* el : brokenCont ) {
      std::cout << "  " << *el << std::endl;
   }

   // And now break one variable in the container:
   brokenAux.breakTestVar2();
   brokenCont.setStore( &brokenAux ); // Needed to clear internal caches...

   // Create a container that will hold the deep copies:
   xAOD::AuxContainerBase copyAux;
   DataVector< SG::AuxElement > copyCont;
   copyCont.setStore( &copyAux );

   // Try to make a deep copy of the healthy container. This should work with
   // the usual method.
   for( const SG::AuxElement* orig : healthyCont ) {
      SG::AuxElement* copy = new SG::AuxElement();
      copyCont.push_back( copy );
      *copy = *orig;
   }

   // Print the content of the copy:
   std::cout << "Content of copyCont after healthy deep copy:" << std::endl;
   for( const SG::AuxElement* el : copyCont ) {
      std::cout << "  " << *el << std::endl;
   }

   // Use the safe copy function to do the same copy:
   copyCont.clear();
   for( const SG::AuxElement* orig : healthyCont ) {
      SG::AuxElement* copy = new SG::AuxElement();
      copyCont.push_back( copy );
      xAOD::safeDeepCopy( *orig, *copy );
   }

   // Print the content of the copy:
   std::cout << "Content of copyCont after healthy safe deep copy:"
             << std::endl;
   for( const SG::AuxElement* el : copyCont ) {
      std::cout << "  " << *el << std::endl;
   }

   // Make a safe deep copy of the broken container:
   copyCont.clear();
   for( const SG::AuxElement* orig : brokenCont ) {
      SG::AuxElement* copy = new SG::AuxElement();
      copyCont.push_back( copy );
      xAOD::safeDeepCopy( *orig, *copy );
   }

   // Print the content of the copy:
   std::cout << "Content of copyCont after broken safe deep copy:"
             << std::endl;
   for( const SG::AuxElement* el : copyCont ) {
      std::cout << "  " << *el << std::endl;
   }

   // Finally, just as a double-check, make sure that normal deep copying
   // would've broken for the doctored container:
   bool brokenAsExpected = false;
   try {
      SG::AuxElement* copy = new SG::AuxElement();
      copyCont.push_back( copy );
      *copy = *( brokenCont[ 0 ] );
   } catch( const SG::ExcBadAuxVar& ) {
      brokenAsExpected = true;
   }
   if( ! brokenAsExpected ) {
      std::cout << "The assignment operator is no longer broken"
                << std::endl;
   }

   // Tell the user what happened:
   std::cout << "All tests succeeded!" << std::endl;

   // Return gracefully:
   return 0;
}
