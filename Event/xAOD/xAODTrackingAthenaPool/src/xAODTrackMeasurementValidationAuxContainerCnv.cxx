/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackMeasurementValidationAuxContainerCnv.cxx 581533 2014-02-05 09:28:36Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrackMeasurementValidationAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTrackMeasurementValidationAuxContainerCnv::
xAODTrackMeasurementValidationAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackMeasurementValidationAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrackMeasurementValidationAuxContainer*
xAODTrackMeasurementValidationAuxContainerCnv::
createPersistent( xAOD::TrackMeasurementValidationAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TrackMeasurementValidationAuxContainer*
xAODTrackMeasurementValidationAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "61B62A1A-4C51-43A2-9623-1B9E910A81E8" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrackMeasurementValidationAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrackMeasurementValidationAuxContainer found" );
   return 0;
}
