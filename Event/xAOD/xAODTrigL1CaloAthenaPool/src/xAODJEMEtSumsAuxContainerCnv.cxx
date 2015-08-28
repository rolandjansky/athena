/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMEtSumsAuxContainerCnv.h"

xAODJEMEtSumsAuxContainerCnv::
xAODJEMEtSumsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJEMEtSumsAuxContainerCnvBase( svcLoc ) {

}

xAOD::JEMEtSumsAuxContainer*
xAODJEMEtSumsAuxContainerCnv::
createPersistent( xAOD::JEMEtSumsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::JEMEtSumsAuxContainer* result =
      new xAOD::JEMEtSumsAuxContainer( *trans );

   return result;
}

xAOD::JEMEtSumsAuxContainer*
xAODJEMEtSumsAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "4D3C658D-597A-414E-AAA3-BE61A1D664C3" );
   static const pool::Guid v2_guid( "015D34AC-2FD5-4357-850E-04FD5EF6F945" );
   
   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JEMEtSumsAuxContainer >();
   }
   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::JEMEtSumsAuxContainer found - bail out for now" );
    return 0;    
   }  

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JEMEtSumsAuxContainer found" );
   return 0;
}
