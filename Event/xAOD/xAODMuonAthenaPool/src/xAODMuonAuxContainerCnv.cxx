/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODMuonAuxContainerCnv_v1.h"
#include "xAODMuonAuxContainerCnv_v2.h"
#include "xAODMuonAuxContainerCnv_v3.h"
#include "xAODMuonAuxContainerCnv_v4.h"

xAODMuonAuxContainerCnv::
xAODMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::MuonAuxContainer*
xAODMuonAuxContainerCnv::
createPersistent( xAOD::MuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::MuonAuxContainer*
xAODMuonAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "EC9B677A-B3BA-4C75-87D3-373FC478291E" );
   static const pool::Guid v2_guid( "AB53C81C-B10C-11E4-AC59-6C3BE51AB9F1" );
   static const pool::Guid v3_guid( "ECC65005-302B-4662-ACBA-D2CE5A0218B8" );
   static const pool::Guid v4_guid( "8251F481-EA4C-4852-AE72-BED87E6FD2FB" );
   static const pool::Guid v5_guid( "9245FA19-1552-4BBB-8CC3-57224E57A466" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v5_guid ) ) {
     // It's the latest version, read it directly:
     return poolReadObject< xAOD::MuonAuxContainer >();
   }else if( compareClassGuid( v4_guid ) ) {
     // Handle dynamic to aux conversion.
     // The v4 converter:
     static xAODMuonAuxContainerCnv_v4 converter;
     std::unique_ptr< xAOD::MuonAuxContainer_v4 >
       old( poolReadObject< xAOD::MuonAuxContainer_v4 >() );

     return converter.createTransient( old.get(), msg() );
   }else if( compareClassGuid( v3_guid ) ) {
     // Handle dynamic to aux conversion.
     // The v3 converter:
     static xAODMuonAuxContainerCnv_v3 converter;
     std::unique_ptr< xAOD::MuonAuxContainer_v3 >
       old( poolReadObject< xAOD::MuonAuxContainer_v3 >() );

     return converter.createTransient( old.get(), msg() );
   } else if( compareClassGuid( v2_guid ) ) {
     // Handle dynamic to aux conversion.
     // The v2 converter:
     static xAODMuonAuxContainerCnv_v2 converter;
     std::unique_ptr< xAOD::MuonAuxContainer_v2 >
             old( poolReadObject< xAOD::MuonAuxContainer_v2 >() );
     
     return converter.createTransient( old.get(), msg() );
   } else if( compareClassGuid( v1_guid ) ) {
     // Fix for 'charge' bug ATLASRECTS-1577
  	 static xAODMuonAuxContainerCnv_v1 converter;
  	 std::unique_ptr< xAOD::MuonAuxContainer_v1 >
  	         old( poolReadObject< xAOD::MuonAuxContainer_v1 >() );
	 
  	 return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MuonAuxContainer found" );
   return 0;
}


