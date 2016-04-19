/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronAuxContainerCnv.cxx 635115 2014-12-10 17:10:47Z ssnyder $

// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

// Local include(s):
#include "xAODElectronAuxContainerCnv.h"
#include "xAODElectronAuxContainerCnv_v1.h"
#include "xAODElectronAuxContainerCnv_v2.h"

xAODElectronAuxContainerCnv::xAODElectronAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODElectronAuxContainerCnvBase( svcLoc ) {

}

xAOD::ElectronAuxContainer*
xAODElectronAuxContainerCnv::
createPersistent( xAOD::ElectronAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::ElectronAuxContainer* xAODElectronAuxContainerCnv::createTransient() {

   // The known ID(s) of this container:
   static const pool::Guid v1_guid( "85A46300-3F57-454C-8B7E-94B653AA70CF" );
   static const pool::Guid v2_guid( "7160FC1C-937D-474C-909B-2C0FCE1DD755" );
   static const pool::Guid v3_guid( "5435686E-4B00-11E4-AEAA-02163E00A5BB" );

   // Check which version of the container we're reading:

   if( compareClassGuid( v3_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::ElectronAuxContainer >();

   } else if( compareClassGuid( v2_guid ) ) {

     // The v2 converter:
     static xAODElectronAuxContainerCnv_v2 converter;
     
     // Read in the v2 version:
     std::unique_ptr< xAOD::ElectronAuxContainer_v2 >
       old( poolReadObject< xAOD::ElectronAuxContainer_v2 >() );
     
     // Return the converted object:
     return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v1_guid ) ) {
     
     // The v1 converter:
     static xAODElectronAuxContainerCnv_v1 converter;
     
     // Read in the v1 version:
     std::unique_ptr< xAOD::ElectronAuxContainer_v1 >
       old( poolReadObject< xAOD::ElectronAuxContainer_v1 >() );
     
     // Return the converted object:
     return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::ElectronAuxContainer found" );
   return 0;
}
