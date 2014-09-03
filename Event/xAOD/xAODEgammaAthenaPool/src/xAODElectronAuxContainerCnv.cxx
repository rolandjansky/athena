/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronAuxContainerCnv.cxx 614855 2014-09-03 11:42:57Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// EDM include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"

// Local include(s):
#include "xAODElectronAuxContainerCnv.h"

xAODElectronAuxContainerCnv::xAODElectronAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODElectronAuxContainerCnvBase( svcLoc ) {

}

xAOD::ElectronAuxContainer*
xAODElectronAuxContainerCnv::
createPersistent( xAOD::ElectronAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::ElectronAuxContainer* result =
      new xAOD::ElectronAuxContainer( *trans );

   return result;
}

xAOD::ElectronAuxContainer* xAODElectronAuxContainerCnv::createTransient() {

   // The known ID(s) of this container:
   static const pool::Guid v1_guid( "85A46300-3F57-454C-8B7E-94B653AA70CF" );
   static const pool::Guid v2_guid( "7160FC1C-937D-474C-909B-2C0FCE1DD755" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // Read in the v1 version:
      std::unique_ptr< xAOD::ElectronAuxContainer_v1 >
         old( poolReadObject< xAOD::ElectronAuxContainer_v1 >() );

      // Create a v2 object:
      xAOD::ElectronAuxContainer* result = new xAOD::ElectronAuxContainer();

      // Copy the payload of the v1 object into the v2 one by misusing
      // the thinning code a bit...
      SG::copyAuxStoreThinned( *old, *result, 0 );

      // Return the new object:
      return result;

   } else if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::ElectronAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::ElectronAuxContainer found" );
   return 0;
}
