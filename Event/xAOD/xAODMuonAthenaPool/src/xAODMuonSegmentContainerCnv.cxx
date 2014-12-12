/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMuonSegmentContainerCnv.h"

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

xAODMuonSegmentContainerCnv::xAODMuonSegmentContainerCnv( ISvcLocator* svcLoc )
   : xAODMuonSegmentContainerCnvBase( svcLoc ) {
}

xAOD::MuonSegmentContainer*
xAODMuonSegmentContainerCnv::
createPersistent( xAOD::MuonSegmentContainer* trans ) {

   // Create a view copy of the container:
   xAOD::MuonSegmentContainer* result =
      new xAOD::MuonSegmentContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::MuonSegmentContainer::iterator itr = result->begin();
   xAOD::MuonSegmentContainer::iterator end = result->end();
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
StatusCode xAODMuonSegmentContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::MuonSegmentContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::MuonSegmentContainer* xAODMuonSegmentContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "9516C67E-98A5-11E3-BDFD-6C3BE51AB9F1" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::MuonSegmentContainer* c = poolReadObject< xAOD::MuonSegmentContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MuonSegmentContainer found" );
   return 0;
}

void xAODMuonSegmentContainerCnv::
toPersistent( xAOD::MuonSegment* ms ) const {

#ifndef XAOD_ANALYSIS
   try {

      const_cast< ElementLink< Trk::SegmentCollection >& >( ms->muonSegment() ).toPersistent();

   } catch( const SG::ExcBadAuxVar& ) {
      // Sometimes, very rarely, the dynamically defined ElementLink doesn't
      // exist. The code should not die in that case, but just continue.
      // The I/O system should be fine with the element link missing...
   }
#endif

   return;
}
