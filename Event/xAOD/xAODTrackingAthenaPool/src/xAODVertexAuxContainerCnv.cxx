/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODVertexAuxContainerCnv.cxx 611354 2014-08-12 07:20:27Z arnaez $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODVertexAuxContainerCnv.h"

xAODVertexAuxContainerCnv::
xAODVertexAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODVertexAuxContainerCnvBase( svcLoc ) {

}

xAOD::VertexAuxContainer*
xAODVertexAuxContainerCnv::
createPersistent( xAOD::VertexAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::VertexAuxContainer* result =
      new xAOD::VertexAuxContainer( *trans );

   std::vector< xAOD::VertexAuxContainer::TrackLink_t >::iterator itr =
      result->trackParticleLinks.begin();
   std::vector< xAOD::VertexAuxContainer::TrackLink_t >::iterator end =
      result->trackParticleLinks.end();
   for( ; itr != end; ++itr ) {
      xAOD::VertexAuxContainer::TrackLink_t::iterator iitr = itr->begin();
      xAOD::VertexAuxContainer::TrackLink_t::iterator iend = itr->end();
      for( ; iitr != iend; ++iitr ) {
         iitr->toPersistent();
      }
   }

   //same for neutrals
   std::vector< xAOD::VertexAuxContainer::NeutralLink_t >::iterator itrN = result->neutralParticleLinks.begin();
   std::vector< xAOD::VertexAuxContainer::NeutralLink_t >::iterator endN = result->neutralParticleLinks.end();
   for( ; itrN != endN; ++itrN ) {
     xAOD::VertexAuxContainer::NeutralLink_t::iterator iitr = itrN->begin();
     xAOD::VertexAuxContainer::NeutralLink_t::iterator iend = itrN->end();
     for( ; iitr != iend; ++iitr ) {
       iitr->toPersistent();
     }
   }

   return result;
}

xAOD::VertexAuxContainer*
xAODVertexAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "B1F73A82-9B4E-4508-8EB0-EF7D6E05BA57" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::VertexAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::VertexAuxContainer found" );
   return 0;
}
