/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetContainerCnv_v1.cxx 632169 2014-11-29 15:13:34Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "xAODTauJetContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTauJetContainerCnv_v1::xAODTauJetContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::TauJetContainer,
                           xAOD::TauJetContainer_v1 >() {

}

void xAODTauJetContainerCnv_v1::
persToTrans( const xAOD::TauJetContainer_v1* oldObj,
             xAOD::TauJetContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::TauJetContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->clear();

   // Simply fill the transient object with as many new objects as many we have
   // in the persistent object:
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      newObj->push_back( new xAOD::TauJet() );
   }

   // Print what happened:
   ATH_MSG( "Converting xAOD::TauJetContainer_v1 to current version [OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTauJetContainerCnv_v1::transToPers( const xAOD::TauJetContainer*,
                                             xAOD::TauJetContainer_v1*,
                                             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODTauJetContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODTauJetContainerCnv_v1::"
                             "transToPers" );

   return;
}
