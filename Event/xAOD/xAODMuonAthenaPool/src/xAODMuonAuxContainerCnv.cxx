/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODMuonAuxContainerCnv_v1.h"

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
   static const pool::Guid v2_guid( "AB53C81C-B10C-11E4-AC59-6C3BE51AB9F1" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
     // It's the latest version, read it directly:
     return poolReadObject< xAOD::MuonAuxContainer >();
   } else if( compareClassGuid( v1_guid ) ) {
     // Fix for 'charge' bug ATLASRECTS-1577
	// std::cout<<"EJWM v1_guid - about to convert"<<std::endl;
	 static xAODMuonAuxContainerCnv_v1 converter;
	 std::unique_ptr< xAOD::MuonAuxContainer_v1 >
	         old( poolReadObject< xAOD::MuonAuxContainer_v1 >() );
	 
//     xAOD::MuonAuxContainer* aux = poolReadObject< xAOD::MuonAuxContainer >();
     // if( old->charge.size() != aux->size() ) {
     //   for( size_t i = 0; i < aux->size(); ++i ) {
     //     // Figure out which link to take:
     //     const ElementLink< xAOD::TrackParticleContainer >& link = getPrimaryTrackParticleLink(aux, i);
     //     // Fill the charge:
     //     aux->charge.push_back( ( *link )->charge() );
     //   }
     // }
     // return aux;
	 return converter.createTransient( old.get(), msg() );
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
  return aux->muonSpectrometerTrackParticleLink[i];
  // Probably we should check this and fail properly if this last link is missing. TODO
}

