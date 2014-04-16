/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxContainerCnv.cxx 586838 2014-03-08 15:10:34Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMissingETAuxContainerCnv.h"

xAODMissingETAuxContainerCnv::xAODMissingETAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODMissingETAuxContainerCnvBase( svcLoc ) {

}

xAOD::MissingETAuxContainer*
xAODMissingETAuxContainerCnv::
createPersistent( xAOD::MissingETAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::MissingETAuxContainer* result =
      new xAOD::MissingETAuxContainer( *trans );

   return result;
}

xAOD::MissingETAuxContainer* xAODMissingETAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "2F92EC94-8CD1-49F3-BCA4-3D78599D4D60" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::MissingETAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MissingETAuxContainer found" );
   return 0;
}
