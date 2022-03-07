/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

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
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 4 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().empty() );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().empty() );
   copyAux.setShallowIO( false );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 4 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 4 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 4 );

   int index = 0;
   for( const SG::AuxElement* el : copyVec ) {
      SIMPLE_ASSERT( el->auxdataConst< int >( "IntVar" ) == index );
      SIMPLE_ASSERT( el->auxdataConst< int >( "Int2Var" ) == index );
      SIMPLE_ASSERT( el->auxdataConst< int >( "Int3Var" ) == index );
      SIMPLE_ASSERT( std::abs( el->auxdataConst< float >( "FloatVar" ) -
                               static_cast< float >( index + 1 ) ) < 0.0001 );
      ++index;
   }

   // Create some modifications
   for( size_t i = 0; i < copyVec.size(); ++i ) {
      copyVec[ i ]->auxdata< int >( "IntVar" ) = i + 2;
      copyVec[ i ]->auxdata< double >( "DoubleVar" ) = 3.14;
   }
   copyVec.front()->auxdata< int >( "Int2Var" ) = 5;
   copyVec.front()->auxdecor< int >( "Int3Var" ) = 6;

   // Check what happened:
   copyAux.setShallowIO( true );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 5 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 4 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 4 );
   copyAux.setShallowIO( false );
   SIMPLE_ASSERT( copyAux.getAuxIDs().size() == 5 );
   SIMPLE_ASSERT( copyAux.getDynamicAuxIDs().size() == 5 );
   SIMPLE_ASSERT( copyAux.getSelectedAuxIDs().size() == 5 );

   index = 0;
   for( const SG::AuxElement* el : copyVec ) {
      SIMPLE_ASSERT( el->auxdataConst< int >( "IntVar" ) == index + 2 );
      SIMPLE_ASSERT( std::abs( el->auxdataConst< float >( "FloatVar" ) -
                               static_cast< float >( index + 1 ) ) < 0.0001 );
      SIMPLE_ASSERT( std::abs( el->auxdataConst< double >( "DoubleVar" ) -
                               3.14 ) < 0.0001 );
      if( index > 0 ) {
         SIMPLE_ASSERT( el->auxdataConst< int >( "Int2Var" ) == index );
         SIMPLE_ASSERT( el->auxdataConst< int >( "Int3Var" ) == index );
      }
      ++index;
   }
   SIMPLE_ASSERT( copyVec.front()->auxdataConst< int >( "Int2Var" ) == 5 );
   SIMPLE_ASSERT( copyVec.front()->auxdataConst< int >( "Int3Var" ) == 6 );

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
      e->auxdata< int >( "Int2Var" ) = i;
      e->auxdata< int >( "Int3Var" ) = i;
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
     SIMPLE_ASSERT( copyAux2.getAuxIDs().size() == 5 );
   }

   // Tell the user that everything went okay:
   std::cout << "All tests with xAOD::ShallowAuxContainer succeeded"
             << std::endl;

   // Return gracefully:
   return 0;
}
