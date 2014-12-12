/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.cxx 635809 2014-12-12 22:19:22Z ssnyder $

// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"

// Local include(s):
#include "xAODCaloClusterAuxContainerCnv.h"
#include "xAODCaloClusterAuxContainerCnv_v1.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODCaloClusterAuxContainerCnv::xAODCaloClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloClusterAuxContainerCnvBase( svcLoc ) {

}

xAOD::CaloClusterAuxContainer*
xAODCaloClusterAuxContainerCnv::
createPersistent( xAOD::CaloClusterAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::CaloClusterAuxContainer* xAODCaloClusterAuxContainerCnv::createTransient() {

   // The known ID(s) of this container:
   static const pool::Guid v1_guid( "CE498B3B-A32D-43A3-B9B3-C13D136BACFC" );
   static const pool::Guid v2_guid( "451393B0-69CD-11E4-A739-02163E00A64D" );

   // Check which version of the container we're reading:

   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CaloClusterAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

     // The v1 converter:
     static xAODCaloClusterAuxContainerCnv_v1 converter;

     // Read in the v1 version:
     std::unique_ptr< xAOD::CaloClusterAuxContainer_v1 >
       old( poolReadObject< xAOD::CaloClusterAuxContainer_v1 >() );

     // Return the converted object:
     return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CaloClusterAuxContainer found" );
   return 0;
}
