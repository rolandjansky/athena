/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODVertexAuxContainerCnv.cxx 652739 2015-03-09 17:09:47Z nstyles $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODVertexAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODVertexAuxContainerCnv::
xAODVertexAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODVertexAuxContainerCnvBase( svcLoc ) {

}

xAOD::VertexAuxContainer*
xAODVertexAuxContainerCnv::
createPersistent( xAOD::VertexAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::VertexAuxContainer*
xAODVertexAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "B1F73A82-9B4E-4508-8EB0-EF7D6E05BA57" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
    

     xAOD::VertexAuxContainer_v1* aux = poolReadObject< xAOD::VertexAuxContainer >();

     if( aux->neutralParticleLinks.size() != aux->size() ) {
       
       aux->neutralParticleLinks.resize( aux->size() );
       
     }
     
     if( aux->neutralWeights.size() != aux->size() ) {
       
       aux->neutralWeights.resize( aux->size() );
       
     }


  // It's the latest version, read it directly:
      return aux;
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::VertexAuxContainer found" );
   return 0;
}
