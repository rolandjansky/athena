/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronTrigAuxContainerCnv.cxx 705790 2015-11-04 13:09:23Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyThinned.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v2.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"

// Local include(s):
#include "xAODElectronTrigAuxContainerCnv.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v1.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v2.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v3.h"

xAODElectronTrigAuxContainerCnv::
xAODElectronTrigAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODElectronTrigAuxContainerCnvBase( svcLoc ) {

}

xAOD::ElectronTrigAuxContainer*
xAODElectronTrigAuxContainerCnv::
createPersistent( xAOD::ElectronTrigAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::ElectronTrigAuxContainer*
xAODElectronTrigAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "359610D5-3AEA-4DB4-837E-4C93F81014C2" );
   static const pool::Guid
      v1_offline_guid( "85A46300-3F57-454C-8B7E-94B653AA70CF" );
   static const pool::Guid
      v2_offline_guid( "7160FC1C-937D-474C-909B-2C0FCE1DD755" );
   static const pool::Guid
      v3_offline_guid( "5435686E-4B00-11E4-AEAA-02163E00A5BB" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::ElectronTrigAuxContainer >();

   } else if( compareClassGuid( v3_offline_guid ) ) {

      // The converter:
      static xAODElectronTrigAuxContainerOfflineCnv_v3 converter;

      // Read in the offline v3 version:
      std::unique_ptr< xAOD::ElectronAuxContainer_v3 >
         old( poolReadObject< xAOD::ElectronAuxContainer_v3 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v2_offline_guid ) ) {

      // The converter:
      static xAODElectronTrigAuxContainerOfflineCnv_v2 converter;

      // Read in the offline v2 version:
      std::unique_ptr< xAOD::ElectronAuxContainer_v2 >
         old( poolReadObject< xAOD::ElectronAuxContainer_v2 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v1_offline_guid ) ) {

      // The converter:
      static xAODElectronTrigAuxContainerOfflineCnv_v1 converter;

      // Read in the offline v1 version:
      std::unique_ptr< xAOD::ElectronAuxContainer_v1 >
         old( poolReadObject< xAOD::ElectronAuxContainer_v1 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::ElectronTrigAuxContainer found" );
   return 0;
}
