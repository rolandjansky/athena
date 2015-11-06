/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonAuxContainerCnv.cxx 706293 2015-11-06 05:34:41Z ssnyder $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2StandAloneMuonAuxContainerCnv.h"
#include "xAODL2StandAloneMuonAuxContainerCnv_v1.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODL2StandAloneMuonAuxContainerCnv::
xAODL2StandAloneMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODL2StandAloneMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::L2StandAloneMuonAuxContainer*
xAODL2StandAloneMuonAuxContainerCnv::
createPersistent( xAOD::L2StandAloneMuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::L2StandAloneMuonAuxContainer*
xAODL2StandAloneMuonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "69F56941-D2B5-4C70-BFBD-605CB64A3DB8" );
   static const pool::Guid v2_guid( "39CA616A-16BF-4CCE-8885-3D68CE7BC3B0" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::L2StandAloneMuonAuxContainer >();
   }

   if( compareClassGuid( v1_guid ) ) {
     static xAODL2StandAloneMuonAuxContainerCnv_v1 converter;
     std::unique_ptr<xAOD::L2StandAloneMuonAuxContainer_v1>
       old (poolReadObject<xAOD::L2StandAloneMuonAuxContainer_v1>());
     return converter.createTransient (old.get(), msg());
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2StandAloneMuonAuxContainer found" );
   return 0;
}
