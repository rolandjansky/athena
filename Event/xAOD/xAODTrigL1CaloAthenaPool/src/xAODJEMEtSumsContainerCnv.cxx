/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMEtSumsContainerCnv.h"

xAODJEMEtSumsContainerCnv::xAODJEMEtSumsContainerCnv( ISvcLocator* svcLoc )
   : xAODJEMEtSumsContainerCnvBase( svcLoc ) {
}

xAOD::JEMEtSumsContainer*
xAODJEMEtSumsContainerCnv::
createPersistent( xAOD::JEMEtSumsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::JEMEtSumsContainer* result =
      new xAOD::JEMEtSumsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::JEMEtSumsContainer::iterator itr = result->begin();
   xAOD::JEMEtSumsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::JEMEtSumsContainer* xAODJEMEtSumsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "DDB6D476-D1E2-490E-A81B-8D4FC65F1D81" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::JEMEtSumsContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JEMEtSumsContainer found" );
   return 0;
}

void xAODJEMEtSumsContainerCnv::
toPersistent( xAOD::JEMEtSums* /*cluster*/ ) const {

   return;
}
