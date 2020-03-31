// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_tstore_test.cxx 663791 2015-04-29 13:08:06Z krasznaa $

// System include(s):
#include <memory>

// ROOT include(s):
#include <TError.h>

// EDM that the package uses anyway:
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/Utils.h"

/// Helper macro
#define RETURN_CHECK( CONTEXT, EXP )                                 \
   do {                                                              \
      const xAOD::TReturnCode result = EXP;                          \
      if( ! result.isSuccess() ) {                                   \
         ::Error( CONTEXT, XAOD_MESSAGE( "Failed to execute: %s" ),  \
                  #EXP );                                            \
         return 1;                                                   \
      }                                                              \
   } while( false )

/// Helper macro for evaluating logical tests
#define SIMPLE_ASSERT( CONTEXT, EXP )                                        \
   do {                                                                      \
      const bool result = EXP;                                               \
      if( ! result ) {                                                       \
         ::Error( CONTEXT,                                                   \
                  XAOD_MESSAGE( "Expression \"%s\" failed the evaluation" ), \
                  #EXP );                                                    \
         return 1;                                                           \
      }                                                                      \
   } while( 0 )

/// Type used in the store test
class ClassA {

public:
   int m_var1;
   float m_var2;

}; // class ClassA

/// Type used in the store test
class ClassB : public ClassA {

public:
   int m_var3;
   float m_var4;

}; // class ClassB

/// Class making some of TStore's internal functions public
class TStoreTester : public xAOD::TStore {

public:
   /// Inherit the TStore constructor(s)
   using xAOD::TStore::TStore;

   /// Make all implementations of the "contains" function public
   using xAOD::TStore::contains;
   /// Make all implementations of the "getName" function public
   using xAOD::TStore::getName;

}; // class TStoreTester


int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_tstore_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Create the TStore object that we'll be testing:
   TStoreTester store;

   // Record an EventFormat object:
   std::unique_ptr< xAOD::AuxContainerBase > c1( new xAOD::AuxContainerBase() );
   RETURN_CHECK( APP_NAME, store.record( std::move( c1 ), "Container1" ) );

   // Check that we can retrieve it both as its concrete type, and also
   // as its base class:
   const xAOD::AuxContainerBase* dummy1 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( dummy1, "Container1" ) );
   const SG::IConstAuxStore* dummy2 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( dummy2, "Container1" ) );

   // Let's record objects that have no dictionary:
   std::unique_ptr< ClassA > objA( new ClassA() );
   void* objAPtr = objA.get();
   RETURN_CHECK( APP_NAME, store.record( std::move( objA ), "MyObjA" ) );
   std::unique_ptr< ClassB > objB( new ClassB() );
   RETURN_CHECK( APP_NAME, store.record( std::move( objB ), "MyObjB" ) );

   // Try to read them back:
   const ClassA* dummy3 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( dummy3, "MyObjA" ) );
   const ClassB* dummy4 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( dummy4, "MyObjB" ) );

   // This is not supposed to work:
   if( store.retrieve( dummy3, "MyObjB" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Something strange happened" ) );
      return 1;
   }

   // Test some of the internals of the class:
   if( ! store.contains( objAPtr ) ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Didn't find pointer %p in the store!" ),
               objAPtr );
      return 1;
   }
   if( store.contains( ( void* ) 0x12345678 ) ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "The code found an imaginary pointer in "
                                       "the store?!?" ) );
      return 1;
   }
   if( store.contains( 0x12345678 ) ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "The code found an imaginary hash in "
                                       "the store?!?" ) );
      return 1;
   }
   if( store.getName( objAPtr ) != "MyObjA" ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "getName(objA) = %s (!=\"MyObjA\")" ),
               store.getName( objAPtr ).c_str() );
      return 1;
   }
   if( store.getName( xAOD::Utils::hash( "Container1" ) ) != "Container1" ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Name lookup for \"Container1\" "
                                       "unsuccessful" ) );
      return 1;
   }
   if( store.getName( 0x12345678 ) != "" ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "We really found a name for an "
                                       "imaginary hash?!?" ) );
      return 1;
   }

   // Clear the container:
   store.clear();

   // Now this is not supposed to work either:
   if( store.retrieve( dummy4, "MyObjB" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Something strange happened" ) );
      return 1;
   }

   // Create a DataVector and a ConstDataVector using the custom type:
   std::unique_ptr< DataVector< ClassA > > dv( new DataVector< ClassA >() );
   dv->push_back( new ClassA() );
   dv->push_back( new ClassA() );
   std::unique_ptr< ConstDataVector< DataVector< ClassA > > >
      cdv( new ConstDataVector< DataVector< ClassA > >( SG::VIEW_ELEMENTS ) );
   cdv->push_back( dv->at( 0 ) );
   cdv->push_back( dv->at( 1 ) );

   // Now record them:
   RETURN_CHECK( APP_NAME, store.record( std::move( dv ), "DataVector" ) );
   RETURN_CHECK( APP_NAME, store.record( std::move( cdv ),
                                         "ConstDataVector" ) );
   // The smart pointers should by now be invalid:
   SIMPLE_ASSERT( APP_NAME, dv.get() == 0 );
   SIMPLE_ASSERT( APP_NAME, cdv.get() == 0 );

   // Now try to retrieve them in all possible manner:
   const DataVector< ClassA >* dv1 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( dv1, "DataVector" ) );
   DataVector< ClassA >* dv2 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( dv2, "DataVector" ) );
   const ConstDataVector< DataVector< ClassA > >* cdv1 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( cdv1, "ConstDataVector" ) );
   ConstDataVector< DataVector< ClassA > >* cdv2 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( cdv2, "ConstDataVector" ) );
   const DataVector< ClassA >* cdv3 = 0;
   RETURN_CHECK( APP_NAME, store.retrieve( cdv3, "ConstDataVector" ) );

   // But this is supposed to fail:
   DataVector< ClassA >* cdv4 = 0;
   if( store.retrieve( cdv4, "ConstDataVector" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "This was not supposed to work..." ) );
      return 1;
   }

   // Test the isConst function:
   typedef ConstDataVector< DataVector< ClassA > > CDV_t;
   SIMPLE_ASSERT( APP_NAME,
                  store.isConst< CDV_t >( "ConstDataVector" ) == kFALSE );
   typedef DataVector< ClassA > DV_t;
   SIMPLE_ASSERT( APP_NAME,
                  store.isConst< DV_t >( "ConstDataVector" ) == kTRUE );

   // Return gracefully:
   return 0;
}
