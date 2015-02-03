/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODMuonAuxContainerCnv::
xAODMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::MuonAuxContainer*
xAODMuonAuxContainerCnv::
createPersistent( xAOD::MuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::MuonAuxContainer*
xAODMuonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "EC9B677A-B3BA-4C75-87D3-373FC478291E" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:

     xAOD::MuonAuxContainer* aux = poolReadObject< xAOD::MuonAuxContainer >();
     if( aux->charge.size() != aux->size() ) {
       for( size_t i = 0; i < aux->size(); ++i ) {
         // Figure out which link to take:
         const ElementLink< xAOD::TrackParticleContainer >& link = getPrimaryTrackParticleLink(aux, i);
         // Fill the charge:
         aux->charge.push_back( ( *link )->charge() );
       }
     }
	 return aux;
   }


   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MuonAuxContainer found" );
   return 0;
}

const ElementLink< xAOD::TrackParticleContainer >&
xAODMuonAuxContainerCnv::getPrimaryTrackParticleLink(xAOD::MuonAuxContainer* aux,size_t i) {
  if (aux->combinedTrackParticleLink.at(i).isValid()) return aux->combinedTrackParticleLink[i];
  if (aux->inDetTrackParticleLink.at(i).isValid()) return aux->inDetTrackParticleLink[i];
  if (aux->muonSpectrometerTrackParticleLink.at(i).isValid()) return aux->muonSpectrometerTrackParticleLink[i];
}

