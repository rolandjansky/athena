/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEnergySumRoIAuxInfoCnv.h"
#include "xAODEnergySumRoIAuxInfoCnv_v1.h"

xAODEnergySumRoIAuxInfoCnv::
xAODEnergySumRoIAuxInfoCnv( ISvcLocator* svcLoc )
   : xAODEnergySumRoIAuxInfoCnvBase( svcLoc ) {

}

xAOD::EnergySumRoIAuxInfo*
xAODEnergySumRoIAuxInfoCnv::
createPersistent( xAOD::EnergySumRoIAuxInfo* trans ) {
   // Create a copy of the container:
   xAOD::EnergySumRoIAuxInfo* result =
      new xAOD::EnergySumRoIAuxInfo( *trans );

   return result;
}

xAOD::EnergySumRoIAuxInfo*
xAODEnergySumRoIAuxInfoCnv::createTransient() {
   // The known ID(s) for this object:
   static const pool::Guid v1_guid( "7BD71A00-5EB8-11E3-95AF-02163E00A743" );
   static const pool::Guid v2_guid( "7A0DB6D7-05F6-4D3A-9E8B-0230350E3E25" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::EnergySumRoIAuxInfo >();
   }
   if( compareClassGuid( v1_guid ) ) {
      // The v1 converter:
      static xAODEnergySumRoIAuxInfoCnv_v1 converter;

      // Read in the v1 object:
      xAOD::EnergySumRoIAuxInfo_v1* old = poolReadObject< xAOD::EnergySumRoIAuxInfo_v1 >();

      // Return the converted object:
      return converter.createTransient( old, msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::EnergySumRoIAuxInfo found" );
   return 0;
}
