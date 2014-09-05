/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMeasurementStateOnSurfaceContainerCnv.h"

namespace {

   /// Helper function setting up the container's link to its auxiliary store
   void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {

      // The link to set up:
      DataLink< SG::IConstAuxStore > link( key + "Aux." );

      // Give it to the container:
      cont->setStore( link );

      return;
   }

} // private namespace

xAODMeasurementStateOnSurfaceContainerCnv::xAODMeasurementStateOnSurfaceContainerCnv( ISvcLocator* svcLoc )
   : xAODMeasurementStateOnSurfaceContainerCnvBase( svcLoc ) {
}

xAOD::MeasurementStateOnSurfaceContainer*
xAODMeasurementStateOnSurfaceContainerCnv::
createPersistent( xAOD::MeasurementStateOnSurfaceContainer* trans ) {

   // Create a view copy of the container:
   xAOD::MeasurementStateOnSurfaceContainer* result =
      new xAOD::MeasurementStateOnSurfaceContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::MeasurementStateOnSurfaceContainer::iterator itr = result->begin();
   xAOD::MeasurementStateOnSurfaceContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODMeasurementStateOnSurfaceContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::MeasurementStateOnSurfaceContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::MeasurementStateOnSurfaceContainer* xAODMeasurementStateOnSurfaceContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "DBAAD4D2-334F-11E4-8DF6-02163E00A738" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::MeasurementStateOnSurfaceContainer* c = poolReadObject< xAOD::MeasurementStateOnSurfaceContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MeasurementStateOnSurfaceContainer found" );
   return 0;
}

void xAODMeasurementStateOnSurfaceContainerCnv::
toPersistent( xAOD::MeasurementStateOnSurface* /*np*/ ) const {
   return;
}
