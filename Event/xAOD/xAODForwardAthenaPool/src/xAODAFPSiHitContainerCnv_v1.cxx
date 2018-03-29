/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "xAODForward/AFPSiHit.h"

// Local include(s):
#include "xAODAFPSiHitContainerCnv_v1.h"

xAODAFPSiHitContainerCnv_v1::xAODAFPSiHitContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::AFPSiHitContainer,
                           xAOD::AFPSiHitContainer_v1 >() {

}

void xAODAFPSiHitContainerCnv_v1::
persToTrans( const xAOD::AFPSiHitContainer_v1* oldObj,
             xAOD::AFPSiHitContainer* newObj,
             MsgStream& /* log */ ) {

   // Clear the transient object:
   newObj->clear();

   // Simply fill the transient object with as many new objects as many we have
   // in the persistent object:
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      newObj->push_back( new xAOD::AFPSiHit() );
   }
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
void xAODAFPSiHitContainerCnv_v1::transToPers( const xAOD::AFPSiHitContainer*,
                                             xAOD::AFPSiHitContainer_v1*,
                                             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODAFPSiHitContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODAFPSiHitContainerCnv_v1::"
                             "transToPers" );
}
