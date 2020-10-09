/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#include "xAODAAuxContainerCnv_v1.h"

// System include(s):
#include <memory>
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODEventFormatCnv/test/A.h"
#include "xAODEventFormatCnv/test/versions/A_v1.h"
#include "xAODEventFormatCnv/test/AVec.h"
#include "xAODEventFormatCnv/test/versions/AVec_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODAAuxContainerCnv_v1::xAODAAuxContainerCnv_v1()
{
}

void xAODAAuxContainerCnv_v1::
persToTrans( const xAODMakerTest::AAuxContainer_v1* oldObj,
             xAODMakerTest::AAuxContainer* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAODMaker::AAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

   // Set up interface containers on top of them:

   //The old uses v_1
   xAODMakerTest::AVec_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) { 
     oldInt.push_back( std::make_unique< xAODMakerTest::A_v1 >() ); 
   }
   oldInt.setStore( oldObj );
   
   xAODMakerTest::AVec newInt;
   for( size_t i = 0; i < newObj->size(); ++i ) { 
     newInt.push_back( std::make_unique< xAODMakerTest::A >() );
   }
   newInt.setStore( newObj );

   // Print what happened:
   ATH_MSG( "Converting xAODMakerTest::AAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
void xAODAAuxContainerCnv_v1::
transToPers( const xAODMakerTest::AAuxContainer*,
             xAODMakerTest::AAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODAAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODAAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}
