/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMeasurementStateOnSurfaceAuxContainerCnv.cxx $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMeasurementStateOnSurfaceAuxContainerCnv.h"

xAODMeasurementStateOnSurfaceAuxContainerCnv::
xAODMeasurementStateOnSurfaceAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODMeasurementStateOnSurfaceAuxContainerCnvBase( svcLoc ) {

}

xAOD::MeasurementStateOnSurfaceAuxContainer*
xAODMeasurementStateOnSurfaceAuxContainerCnv::
createPersistent( xAOD::MeasurementStateOnSurfaceAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::MeasurementStateOnSurfaceAuxContainer* result =
      new xAOD::MeasurementStateOnSurfaceAuxContainer( *trans );

   return result;
}

xAOD::MeasurementStateOnSurfaceAuxContainer*
xAODMeasurementStateOnSurfaceAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "3D48E694-334F-11E4-AA51-02163E00A738" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::MeasurementStateOnSurfaceAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MeasurementStateOnSurfaceAuxContainer found" );
   return 0;
}
