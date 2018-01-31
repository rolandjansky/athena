/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
//#include "AthenaBaseComps/AthMsgStreamMacros.h"

// EDM include(s):
#include "xAODForward/versions/AFPSiHitContainer_v1.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/versions/AFPSiHit_v1.h"
#include "xAODForward/AFPSiHit.h"

// Local include(s):
#include "xAODAFPSiHitAuxContainerCnv_v1.h"

xAODAFPSiHitAuxContainerCnv_v1::xAODAFPSiHitAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::AFPSiHitAuxContainer,
                           xAOD::AFPSiHitAuxContainer_v1 >() {

}

void xAODAFPSiHitAuxContainerCnv_v1::
persToTrans( const xAOD::AFPSiHitAuxContainer_v1* oldObj,
             xAOD::AFPSiHitAuxContainer* newObj,
             MsgStream& /* log */ ) {

   // Clear the transient object:
   newObj->resize( 0 );

   // Set up interface containers on top of them:
   xAOD::AFPSiHitContainer_v1 oldInterface;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInterface.push_back( new xAOD::AFPSiHit_v1() );
   }
   oldInterface.setStore( oldObj );
   xAOD::AFPSiHitContainer newInterface;
   newInterface.setStore( newObj );

   // Loop over the interface objects, and do the conversion with their help:
   for( const xAOD::AFPSiHit_v1* oldHit : oldInterface ) {

      // Add an object to the output container:
      xAOD::AFPSiHit* newHit = new xAOD::AFPSiHit;
      newInterface.push_back( newHit );

      // copy content except for links to tracks
      newHit->setDepositedCharge( oldHit->depositedCharge() );
      newHit->setTimeOverThreshold( oldHit->timeOverThreshold() );
      newHit->setStationID( oldHit->stationID() );
      newHit->setPixelLayerID( oldHit->pixelLayerID() );
      newHit->setPixelRowIDChip( oldHit->pixelRowIDChip() );
      newHit->setPixelColIDChip( oldHit->pixelColIDChip() );
      newHit->setPixelColIDChip( oldHit->pixelColIDChip() );
   }
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
void xAODAFPSiHitAuxContainerCnv_v1::transToPers( const xAOD::AFPSiHitAuxContainer*,
                                                xAOD::AFPSiHitAuxContainer_v1*,
                                                MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODAFPSiHitAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODAFPSiHitAuxContainerCnv_v1::"
                             "transToPers" );
}
