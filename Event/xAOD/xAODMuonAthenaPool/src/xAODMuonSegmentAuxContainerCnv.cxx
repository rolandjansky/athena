/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <exception>

// ROOT include(s):
#include <TClass.h>

// Local include(s):
#include "xAODMuonSegmentAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODMuonSegmentAuxContainerCnv::
xAODMuonSegmentAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODMuonSegmentAuxContainerCnvBase( svcLoc ) {

}

xAOD::MuonSegmentAuxContainer*
xAODMuonSegmentAuxContainerCnv::
createPersistent( xAOD::MuonSegmentAuxContainer* trans ) {

#ifndef XAOD_ANALYSIS
   // Make sure that the dictionary for the Athena-only dynamic variable
   // is loaded:
   static const char* muonSegmentsType =
      "std::vector<ElementLink<DataVector<Trk::Segment> > >";
   static bool dictLoaded = false;
   if( ! dictLoaded ) {
      TClass* cl = TClass::GetClass( muonSegmentsType );
      if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {
         ATH_MSG_ERROR( "Couldn't load the dictionary for \""
                        << muonSegmentsType << "\"" );
      } else {
         dictLoaded = true;
      }
   }
#endif 

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::MuonSegmentAuxContainer*
xAODMuonSegmentAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "51739E92-98A5-11E3-B7F4-6C3BE51AB9F1" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::MuonSegmentAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MuonSegmentAuxContainer found" );
   return 0;
}
