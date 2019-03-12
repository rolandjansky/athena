/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODSCTRDOContainerCnv.h"

xAODSCTRDOContainerCnv::xAODSCTRDOContainerCnv( ISvcLocator* svcLoc )
  : xAODSCTRDOContainerCnvBase( svcLoc )
{ }

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODSCTRDOContainerCnv::createObj(IOpaqueAddress* pAddr,
                                             DataObject*& pObj )
{
   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::SCTRawHitValidationContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::SCTRawHitValidationContainer*
xAODSCTRDOContainerCnv::createPersistent( xAOD::SCTRawHitValidationContainer* trans )
{
  // Create a view copy of the container:
  return new xAOD::SCTRawHitValidationContainer( trans->begin(), trans->end(), SG::VIEW_ELEMENTS );
}

xAOD::SCTRawHitValidationContainer* xAODSCTRDOContainerCnv::createTransient() {
   // The known ID(s) for this container:
   const pool::Guid v1_guid( "BC8CE35A-5785-4F97-82B6-1E4A21A8C28F" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::SCTRawHitValidationContainer* c = poolReadObject< xAOD::SCTRawHitValidationContainer >();
      c->setStore(DataLink<SG::IConstAuxStore>(m_key + "Aux."));
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of xAOD::SCTRawHitValidationContainer found" );
}
