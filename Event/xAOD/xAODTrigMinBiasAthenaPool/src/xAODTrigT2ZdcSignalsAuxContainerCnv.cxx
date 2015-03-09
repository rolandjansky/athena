/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2ZdcSignalsAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigT2ZdcSignalsAuxContainerCnv.h"

xAODTrigT2ZdcSignalsAuxContainerCnv::xAODTrigT2ZdcSignalsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigT2ZdcSignalsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigT2ZdcSignalsAuxContainer*
xAODTrigT2ZdcSignalsAuxContainerCnv::
createPersistent( xAOD::TrigT2ZdcSignalsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigT2ZdcSignalsAuxContainer* result =
      new xAOD::TrigT2ZdcSignalsAuxContainer( *trans );

   return result;
}

xAOD::TrigT2ZdcSignalsAuxContainer* xAODTrigT2ZdcSignalsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "83B9F174-5E27-11E4-98C2-02163E00A82" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigT2ZdcSignalsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigT2ZdcSignalsAuxContainer found" );
   return 0;
}
