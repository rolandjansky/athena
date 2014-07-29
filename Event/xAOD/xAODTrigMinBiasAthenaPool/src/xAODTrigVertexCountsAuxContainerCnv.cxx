/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigVertexCountsAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigVertexCountsAuxContainerCnv.h"

xAODTrigVertexCountsAuxContainerCnv::xAODTrigVertexCountsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigVertexCountsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigVertexCountsAuxContainer*
xAODTrigVertexCountsAuxContainerCnv::
createPersistent( xAOD::TrigVertexCountsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigVertexCountsAuxContainer* result =
      new xAOD::TrigVertexCountsAuxContainer( *trans );

   return result;
}

xAOD::TrigVertexCountsAuxContainer* xAODTrigVertexCountsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "70E7BDFE-1207-11E4-ACEF-02163E00A892" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigVertexCountsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigVertexCountsAuxContainer found" );
   return 0;
}
