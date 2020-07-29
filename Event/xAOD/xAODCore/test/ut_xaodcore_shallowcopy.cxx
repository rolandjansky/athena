/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodcore_shallowcopy.cxx 673658 2015-06-09 12:45:34Z krasznaa $
//
// This is a unit test for xAOD::ShallowAuxContainer that can only run in
// standalone mode. As it needs to access some internals of DataLink
// unfortunately.

// System include(s):
#include <iostream>
#include <cmath>

// EDM include(s):
#ifdef XAOD_STANDALONE
#define private public
#define protected public
#   include "AthLinks/DataLink.h"
#undef protected
#undef private
#else
# include "AthLinks/DataLink.h"
#endif
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/ShallowAuxContainer.h"

/// Helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                                 \
   do {                                                                      \
      const bool result = EXP;                                               \
      if( ! result ) {                                                       \
         std::cerr << "Expression \"" << #EXP << "\" failed the evaluation"  \
                   << std::endl;                                             \
         return 1;                                                           \
      }                                                                      \
   } while( 0 )


int testCopy (const DataVector<SG::AuxElement>& origVec,
               xAOD::ShallowAuxContainer& copyAux)
{
   DataVector< SG::AuxElement > copyVec;
   for( size_t i = 0; i < origVec.size(); ++i ) {
      copyVec.push_back( new SG::AuxElement() );
   }
   copyVec.setStore( &copyAux );

   // Some starting tests:
   copyAux.setShallowIO( true );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 2 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 0 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 0 );
   copyAux.setShallowIO( false );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 2 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 2 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 2 );

   int index = 0;
   for( const SG::AuxElement* el : copyVec ) {
      SIMPLE_ASSERT( el->auxdata< int >( "IntVar" ) == index );
      SIMPLE_ASSERT( std::abs( el->auxdata< float >( "FloatVar" ) -
                               static_cast< float >( index + 1 ) ) < 0.0001 );
      ++index;
   }

   // Create some modifications
   for( size_t i = 0; i < copyVec.size(); ++i ) {
      copyVec[ i ]->auxdata< int >( "IntVar" ) = i + 2;
      copyVec[ i ]->auxdata< double >( "DoubleVar" ) = 3.14;
   }

   // Check what happened:
   copyAux.setShallowIO( true );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 3 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 2 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 2 );
   copyAux.setShallowIO( false );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 3 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 3 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 3 );

   index = 0;
   for( const SG::AuxElement* el : copyVec ) {
      SIMPLE_ASSERT( el->auxdata< int >( "IntVar" ) == index + 2 );
      SIMPLE_ASSERT( std::abs( el->auxdata< float >( "FloatVar" ) -
                               static_cast< float >( index + 1 ) ) < 0.0001 );
      SIMPLE_ASSERT( std::abs( el->auxdata< double >( "DoubleVar" ) -
                               3.14 ) < 0.0001 );
      ++index;
   }

   // Finally, test variable filtering:
   xAOD::AuxSelection sel;
   sel.selectAux (std::set< std::string >( { "FloatVar", "DoubleVar" } ) );
   copyAux.setShallowIO( true );
   SIMPLE_ASSERT( sel.getSelectedAuxIDs (copyAux.getSelectedAuxIDs()).size() == 1 );
   copyAux.setShallowIO( false );
   SIMPLE_ASSERT( sel.getSelectedAuxIDs (copyAux.getSelectedAuxIDs()).size() == 2 );

   return 0;
}


int main() {

   // Create a test container that we'll make a copy of later on:
   xAOD::AuxContainerBase origAux;
   DataVector< SG::AuxElement > origVec;
   origVec.setStore( &origAux );
   for( int i = 0; i < 10; ++i ) {
      SG::AuxElement* e = new SG::AuxElement();
      origVec.push_back( e );
      e->auxdata< int >( "IntVar" ) = i;
      e->auxdata< float >( "FloatVar" ) = i + 1;
   }

#ifdef XAOD_STANDALONE
   DataLink< SG::IConstAuxStore > link;
   link.m_object = &origAux;
#else
   DataLink< SG::IConstAuxStore > link (&origAux);
#endif

   // Make a shallow copy of it:
   {
     xAOD::ShallowAuxContainer copyAux;
     copyAux.setParent( link );
     if (testCopy (origVec, copyAux))
       return 1;
   }

   {
     xAOD::ShallowAuxContainer copyAux (link);
     if (testCopy (origVec, copyAux))
       return 1;
     xAOD::ShallowAuxContainer copyAux2 (copyAux);
     SIMPLE_ASSERT( copyAux2.getAuxIDs().size() == 3 );
   }

   // Tell the user that everything went okay:
   std::cout << "All tests with xAOD::ShallowAuxContainer succeeded"
             << std::endl;

   // Return gracefully:
   return 0;
}
