/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "xAODForward/AFPTrack.h"


// Local include(s):
#include "xAODAFPTrackContainerCnv_v1.h"

xAODAFPTrackContainerCnv_v1::xAODAFPTrackContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::AFPTrackContainer,
                           xAOD::AFPTrackContainer_v1 >() {

}

void xAODAFPTrackContainerCnv_v1::
persToTrans( const xAOD::AFPTrackContainer_v1* oldObj,
             xAOD::AFPTrackContainer* newObj,
             MsgStream& /* log */ ) {

   // Clear the transient object:
   newObj->clear();

   // Simply fill the transient object with as many new objects as many we have
   // in the persistent object:
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      newObj->push_back( new xAOD::AFPTrack() );
   }
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
void xAODAFPTrackContainerCnv_v1::transToPers( const xAOD::AFPTrackContainer*,
                                             xAOD::AFPTrackContainer_v1*,
                                             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODAFPTrackContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODAFPTrackContainerCnv_v1::"
                             "transToPers" );
}
