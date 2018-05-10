/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPProtonAuxContainerCnv.cxx
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Implementation file of xAODAFPProtonAuxContainerCnv. Based on xAODAFPSiHitsClusterAuxContainerCnv.cxx
 * 
 */

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODAFPProtonAuxContainerCnv.h"

xAODAFPProtonAuxContainerCnv::xAODAFPProtonAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPProtonAuxContainerCnvBase( svcLoc ) {

}

xAOD::AFPProtonAuxContainer* xAODAFPProtonAuxContainerCnv::createPersistent( xAOD::AFPProtonAuxContainer* trans ) {

  // Access the thinning svc, if thinning is defined for this object:
  IThinningSvc* thinSvc = IThinningSvc::instance();

  // Create the persistent object using the helper function from AthContainers:
  return SG::copyThinned( *trans, thinSvc );
}

xAOD::AFPProtonAuxContainer* xAODAFPProtonAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "A5E4D2CC-2541-49D8-BAC7-3C5DFD51779B" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPProtonAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPProtonAuxContainer found" );
  return 0;
}


