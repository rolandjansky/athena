/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "xAODEnergySumRoICnv_v1.h"
/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODEnergySumRoICnv_v1::xAODEnergySumRoICnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::EnergySumRoI,
                           xAOD::EnergySumRoI_v1 >() {

}

void xAODEnergySumRoICnv_v1::
persToTrans( const xAOD::EnergySumRoI_v1* /*oldObj*/,
             xAOD::EnergySumRoI* /*newObj*/,
             MsgStream& /*log*/ ) {

   // Greet the user:
   //ATH_MSG( "Converting xAOD::EnergySumRoI_v1 to current version..." );

   // Initialising is all done in the AuxStore copy, so nothing to do here really

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODEnergySumRoICnv_v1::transToPers( const xAOD::EnergySumRoI*,
                                               xAOD::EnergySumRoI_v1*,
                                               MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODEnergySumRoICnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODEnergySumRoICnv_v1::"
                             "transToPers" );

   return;
}
