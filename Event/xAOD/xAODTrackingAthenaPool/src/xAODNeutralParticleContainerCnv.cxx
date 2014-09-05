/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODNeutralParticleContainerCnv.h"

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

xAODNeutralParticleContainerCnv::xAODNeutralParticleContainerCnv( ISvcLocator* svcLoc )
   : xAODNeutralParticleContainerCnvBase( svcLoc ) {
}

xAOD::NeutralParticleContainer*
xAODNeutralParticleContainerCnv::
createPersistent( xAOD::NeutralParticleContainer* trans ) {

   // Create a view copy of the container:
   xAOD::NeutralParticleContainer* result =
      new xAOD::NeutralParticleContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::NeutralParticleContainer::iterator itr = result->begin();
   xAOD::NeutralParticleContainer::iterator end = result->end();
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
StatusCode xAODNeutralParticleContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::NeutralParticleContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::NeutralParticleContainer* xAODNeutralParticleContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "12869D6A-5CEB-11E3-B64A-02163E00A92F" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::NeutralParticleContainer* c = poolReadObject< xAOD::NeutralParticleContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::NeutralParticleContainer found" );
   return 0;
}

void xAODNeutralParticleContainerCnv::
toPersistent( xAOD::NeutralParticle* /*np*/ ) const {
   return;
}
