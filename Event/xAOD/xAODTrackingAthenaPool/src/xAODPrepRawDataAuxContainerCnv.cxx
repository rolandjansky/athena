/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPrepRawDataAuxContainerCnv.cxx 581533 2014-02-05 09:28:36Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODPrepRawDataAuxContainerCnv.h"

xAODPrepRawDataAuxContainerCnv::
xAODPrepRawDataAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODPrepRawDataAuxContainerCnvBase( svcLoc ) {

}

xAOD::PrepRawDataAuxContainer*
xAODPrepRawDataAuxContainerCnv::
createPersistent( xAOD::PrepRawDataAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::PrepRawDataAuxContainer* result =
      new xAOD::PrepRawDataAuxContainer( *trans );

   return result;
}

xAOD::PrepRawDataAuxContainer*
xAODPrepRawDataAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "11B4BF93-0074-4BF5-AD64-216762A58E5C" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::PrepRawDataAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::PrepRawDataAuxContainer found" );
   return 0;
}
