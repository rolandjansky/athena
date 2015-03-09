/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2MbtsBitsAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigT2MbtsBitsAuxContainerCnv.h"

xAODTrigT2MbtsBitsAuxContainerCnv::xAODTrigT2MbtsBitsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigT2MbtsBitsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigT2MbtsBitsAuxContainer*
xAODTrigT2MbtsBitsAuxContainerCnv::
createPersistent( xAOD::TrigT2MbtsBitsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigT2MbtsBitsAuxContainer* result =
      new xAOD::TrigT2MbtsBitsAuxContainer( *trans );

   return result;
}

xAOD::TrigT2MbtsBitsAuxContainer* xAODTrigT2MbtsBitsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "0055818E-1207-11E4-9C7B-02163E00A892" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigT2MbtsBitsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigT2MbtsBitsAuxContainer found" );
   return 0;
}
