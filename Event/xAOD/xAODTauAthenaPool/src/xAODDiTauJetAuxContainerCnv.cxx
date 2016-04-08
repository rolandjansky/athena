/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODDiTauJetAuxContainerCnv.cxx 631921 2015-09-23 23:30:59Z dkirchme $

// System include(s):
#include <exception>
#include <memory>

// Local include(s):
#include "xAODDiTauJetAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODDiTauJetAuxContainerCnv::xAODDiTauJetAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODDiTauJetAuxContainerCnvBase( svcLoc ) {

}

xAOD::DiTauJetAuxContainer*
xAODDiTauJetAuxContainerCnv::
createPersistent( xAOD::DiTauJetAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::DiTauJetAuxContainer* xAODDiTauJetAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "52A49486-FE88-4448-87E0-5878B69975D1" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::DiTauJetAuxContainer >();

   } 
   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::DiTauJetAuxContainer found" );
   return 0;
}
