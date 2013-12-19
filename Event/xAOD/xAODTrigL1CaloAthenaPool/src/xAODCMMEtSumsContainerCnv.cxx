/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMEtSumsContainerCnv.h"

xAODCMMEtSumsContainerCnv::xAODCMMEtSumsContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMEtSumsContainerCnvBase( svcLoc ) {
}

xAOD::CMMEtSumsContainer*
xAODCMMEtSumsContainerCnv::
createPersistent( xAOD::CMMEtSumsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CMMEtSumsContainer* result =
      new xAOD::CMMEtSumsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CMMEtSumsContainer::iterator itr = result->begin();
   xAOD::CMMEtSumsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CMMEtSumsContainer* xAODCMMEtSumsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "186A613D-0C91-4880-9B09-B41C28B17B53" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::CMMEtSumsContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMEtSumsContainer found" );
   return 0;
}

void xAODCMMEtSumsContainerCnv::
toPersistent( xAOD::CMMEtSums* /*cluster*/ ) const {

   return;
}
