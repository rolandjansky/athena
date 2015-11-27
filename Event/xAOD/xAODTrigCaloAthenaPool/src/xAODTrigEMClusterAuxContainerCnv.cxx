/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv.cxx 711098 2015-11-27 09:37:40Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyThinned.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"

// Local include(s):
#include "xAODTrigEMClusterAuxContainerCnv.h"
#include "xAODTrigEMClusterAuxContainerCnv_v1.h"

xAODTrigEMClusterAuxContainerCnv::
xAODTrigEMClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigEMClusterAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigEMClusterAuxContainer*
xAODTrigEMClusterAuxContainerCnv::
createPersistent( xAOD::TrigEMClusterAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::TrigEMClusterAuxContainer*
xAODTrigEMClusterAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "CE08F151-A2BA-44ED-B854-7D8B9C8679DE" );
   static const pool::Guid v2_guid( "2AC227DF-EE58-4469-AE60-F066F75DC0DD" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigEMClusterAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

      // The converter:
      static xAODTrigEMClusterAuxContainerCnv_v1 converter;

      // Read in the old object:
      std::unique_ptr< xAOD::TrigEMClusterAuxContainer_v1 >
         old( poolReadObject< xAOD::TrigEMClusterAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigEMClusterAuxContainer found" );
   return 0;
}
