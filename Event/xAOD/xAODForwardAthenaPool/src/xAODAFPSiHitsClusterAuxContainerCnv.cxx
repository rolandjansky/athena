/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPSiHitsClusterAuxContainerCnv.cxx
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Implementation file of xAODAFPSiHitsClusterAuxContainerCnv.
 * 
 */

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODAFPSiHitsClusterAuxContainerCnv.h"

xAODAFPSiHitsClusterAuxContainerCnv::xAODAFPSiHitsClusterAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPSiHitsClusterAuxContainerCnvBase( svcLoc ) {

}

xAOD::AFPSiHitsClusterAuxContainer* xAODAFPSiHitsClusterAuxContainerCnv::createPersistent( xAOD::AFPSiHitsClusterAuxContainer* trans ) {

  // Access the thinning svc, if thinning is defined for this object:
  IThinningSvc* thinSvc = IThinningSvc::instance();

  // Create the persistent object using the helper function from AthContainers:
  return SG::copyThinned( *trans, thinSvc );
}

xAOD::AFPSiHitsClusterAuxContainer* xAODAFPSiHitsClusterAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "1B4F1207-38F5-42ED-BC5B-F9125F845C3F" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPSiHitsClusterAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPSiHitsClusterAuxContainer found" );
  return 0;
}


