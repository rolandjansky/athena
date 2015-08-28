/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMEtSumsAuxContainerCnv.h"

xAODCMMEtSumsAuxContainerCnv::
xAODCMMEtSumsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMEtSumsAuxContainerCnvBase( svcLoc ) {

}

xAOD::CMMEtSumsAuxContainer*
xAODCMMEtSumsAuxContainerCnv::
createPersistent( xAOD::CMMEtSumsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CMMEtSumsAuxContainer* result =
      new xAOD::CMMEtSumsAuxContainer( *trans );

   return result;
}

xAOD::CMMEtSumsAuxContainer*
xAODCMMEtSumsAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "A072EDD3-880D-46D0-A120-48C116FDB14E" );
     
   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CMMEtSumsAuxContainer >();
   }

   
   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMEtSumsAuxContainer found" );
   return 0;
}
