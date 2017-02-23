/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxAssociationMapCnv.cxx 795534 2017-02-04 16:24:29Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMissingETAuxAssociationMapCnv.h"
#include "xAODMissingETAuxAssociationMapCnv_v1.h"

// Correct ElementLink treatment
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyAuxStoreThinned.h"

xAODMissingETAuxAssociationMapCnv::xAODMissingETAuxAssociationMapCnv( ISvcLocator* svcLoc )
   : xAODMissingETAuxAssociationMapCnvBase( svcLoc ) {

}

xAOD::MissingETAuxAssociationMap*
xAODMissingETAuxAssociationMapCnv::
createPersistent( xAOD::MissingETAuxAssociationMap* trans ) {

   // Create a copy of the container:
  xAOD::MissingETAuxAssociationMap* result = new xAOD::MissingETAuxAssociationMap;
  SG::copyAuxStoreThinned( *trans, *result, IThinningSvc::instance() );

   return result;
}

xAOD::MissingETAuxAssociationMap* xAODMissingETAuxAssociationMapCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "48EEF1CD-F937-445C-A09C-B978D152868E" );
   static const pool::Guid v2_guid( "05184C74-A3CE-4C89-959E-D7E9F0A98D13" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::MissingETAuxAssociationMap >();
   } else if( compareClassGuid( v1_guid ) ) {

     // The v1 converter:
     static xAODMissingETAuxAssociationMapCnv_v1 converter;

     // Read in the v1 version:
     std::unique_ptr< xAOD::MissingETAuxAssociationMap_v1 >
       old( poolReadObject< xAOD::MissingETAuxAssociationMap_v1 >() );

     // Return the converted object:
     return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MissingETAuxAssociationMap found" );
   return 0;
}

