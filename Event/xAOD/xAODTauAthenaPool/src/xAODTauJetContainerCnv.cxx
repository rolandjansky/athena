/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetContainerCnv.cxx 595863 2014-05-07 13:58:22Z janus $

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODTauJetContainerCnv.h"

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

xAODTauJetContainerCnv::xAODTauJetContainerCnv( ISvcLocator* svcLoc )
   : xAODTauJetContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODTauJetContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TauJetContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TauJetContainer*
xAODTauJetContainerCnv::
createPersistent( xAOD::TauJetContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TauJetContainer* result =
      new xAOD::TauJetContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TauJetContainer::iterator itr = result->begin();
   xAOD::TauJetContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TauJetContainer* xAODTauJetContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "93CCE680-47C0-11E3-997C-02163E00A614" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TauJetContainer* c =
         poolReadObject< xAOD::TauJetContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TauJetContainer found" );
   return 0;
}

namespace {
   /// Helper function
   void toPersistent( xAOD::TauJet::TrackParticleLinks_t& links ) {
      xAOD::TauJet::TrackParticleLinks_t::iterator itr =
         links.begin();
      xAOD::TauJet::TrackParticleLinks_t::iterator end =
         links.end();
      for( ; itr != end; ++itr ) {
         itr->toPersistent();
      }
      return;
   }
   /// Helper function
   void toPersistent( xAOD::TauJet::PFOLinks_t& links ) {
      xAOD::TauJet::PFOLinks_t::iterator itr =
         links.begin();
      xAOD::TauJet::PFOLinks_t::iterator end =
         links.end();
      for( ; itr != end; ++itr ) {
         itr->toPersistent();
      }
      return;
   }
} // private namespace

void xAODTauJetContainerCnv::
toPersistent( xAOD::TauJet* tau ) const {

   const_cast< xAOD::TauJet::JetLink_t& >( tau->jetLink() ).toPersistent();
   const_cast< xAOD::TauJet::VertexLink_t& >( tau->vertexLink() ).toPersistent();
   const_cast< xAOD::TauJet::VertexLink_t& >( tau->secondaryVertexLink() ).toPersistent();

   typedef xAOD::TauJet::TrackParticleLinks_t TPLinks_t;
   ::toPersistent( const_cast< TPLinks_t& >( tau->trackLinks() ) );
   ::toPersistent( const_cast< TPLinks_t& >( tau->conversionTrackLinks() ) );
   ::toPersistent( const_cast< TPLinks_t& >( tau->wideTrackLinks() ) );
   ::toPersistent( const_cast< TPLinks_t& >( tau->otherTrackLinks() ) );

   typedef xAOD::TauJet::PFOLinks_t PFOLinks_t;
   ::toPersistent( const_cast< PFOLinks_t& >( tau->charged_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->neutral_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->pi0_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->cellBased_Charged_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->cellBased_Neutral_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->cellBased_Pi0_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->eflowRec_Charged_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->eflowRec_Neutral_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->eflowRec_Pi0_PFOLinks() ) );
   ::toPersistent( const_cast< PFOLinks_t& >( tau->shot_PFOLinks() ) );

   return;
}
