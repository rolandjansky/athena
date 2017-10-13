/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
//#include "AthenaBaseComps/AthMsgStreamMacros.h"

// EDM include(s):
#include "xAODForward/versions/AFPTrackContainer_v1.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/versions/AFPTrack_v1.h"
#include "xAODForward/AFPTrack.h"

// Local include(s):
#include "xAODAFPTrackAuxContainerCnv_v1.h"

xAODAFPTrackAuxContainerCnv_v1::xAODAFPTrackAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::AFPTrackAuxContainer,
                           xAOD::AFPTrackAuxContainer_v1 >() {

}

void xAODAFPTrackAuxContainerCnv_v1::
persToTrans( const xAOD::AFPTrackAuxContainer_v1* oldObj,
             xAOD::AFPTrackAuxContainer* newObj,
             MsgStream& /* log */ ) {

   // Clear the transient object:
   newObj->resize( 0 );

   // Set up interface containers on top of them:
   xAOD::AFPTrackContainer_v1 oldInterface;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInterface.push_back( new xAOD::AFPTrack_v1() );
   }
   oldInterface.setStore( oldObj );
   xAOD::AFPTrackContainer newInterface;
   newInterface.setStore( newObj );

   // Loop over the interface objects, and do the conversion with their help:
   for( const xAOD::AFPTrack_v1* oldTrack : oldInterface ) {

      // Add an object to the output container:
      xAOD::AFPTrack* newTrack = new xAOD::AFPTrack;
      newInterface.push_back( newTrack );

      // copy content except for links to tracks
      newTrack->setStationID( oldTrack->stationID() );
      newTrack->setXLocal( oldTrack->xLocal() );
      newTrack->setYLocal( oldTrack->yLocal() );
      newTrack->setZLocal( oldTrack->zLocal() );
      newTrack->setXSlope( oldTrack->xSlope() );
      newTrack->setYSlope( oldTrack->ySlope() );
      newTrack->setNHoles( oldTrack->nHoles() );
      newTrack->setNClusters( oldTrack->nHits() );
      newTrack->setChi2( oldTrack->chi2() );
      newTrack->setAlgID( oldTrack->algID() );
   }
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
void xAODAFPTrackAuxContainerCnv_v1::transToPers( const xAOD::AFPTrackAuxContainer*,
                                                xAOD::AFPTrackAuxContainer_v1*,
                                                MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODAFPTrackAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODAFPTrackAuxContainerCnv_v1::"
                             "transToPers" );
}
