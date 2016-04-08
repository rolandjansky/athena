/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonContainerCnv_v1.cxx 707606 2015-11-12 19:59:49Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "xAODL2StandAloneMuonContainerCnv_v1.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"
#include "CxxUtils/make_unique.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODL2StandAloneMuonContainerCnv_v1::xAODL2StandAloneMuonContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::L2StandAloneMuonContainer,
                           xAOD::L2StandAloneMuonContainer_v1 >() {

}

void xAODL2StandAloneMuonContainerCnv_v1::
persToTrans( const xAOD::L2StandAloneMuonContainer_v1* oldObj,
             xAOD::L2StandAloneMuonContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::L2StandAloneMuonContainer_v1 to current version..." );

   newObj->clear();
   for (size_t i = 0; i < oldObj->size(); i++)
     newObj->push_back (CxxUtils::make_unique<xAOD::L2StandAloneMuon>());
   
   // Print what happened:
   ATH_MSG( "Converting xAOD::L2StandAloneMuonContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODL2StandAloneMuonContainerCnv_v1::
transToPers( const xAOD::L2StandAloneMuonContainer*,
             xAOD::L2StandAloneMuonContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODL2StandAloneMuonContainer_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODL2StandAloneMuonContainer_v1::"
                             "transToPers" );

   return;
}
