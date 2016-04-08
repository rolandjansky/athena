/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetAuxContainerCnv.cxx 588765 2014-03-21 13:30:28Z delsart $

#ifndef SIMULATIONBASE

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetTrigAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

#include <TClass.h>

xAODJetTrigAuxContainerCnv::xAODJetTrigAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJetTrigAuxContainerCnvBase( svcLoc ) {

}

#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
    if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::JetTrigAuxContainer*
xAODJetTrigAuxContainerCnv::
createPersistent( xAOD::JetTrigAuxContainer* trans ) {

  static bool dictLoaded = false;
  if( ! dictLoaded ) {
    LOAD_DICTIONARY( "ElementLink<DataVector<xAOD::MuonSegment_v1> >" );
    LOAD_DICTIONARY( "std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1> > >" );
    LOAD_DICTIONARY( "std::vector<std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1> > > >" );

    LOAD_DICTIONARY( "std::vector<ElementLink<DataVector<xAOD::BTagging_v1> > >" );

    
    dictLoaded = true;
  }  

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::JetTrigAuxContainer* xAODJetTrigAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "89AE2C6B-A862-499C-8BDA-11D24FAC83F1" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JetTrigAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetTrigAuxContainer found" );
   return 0;
}
#endif // SIMULATIONBASE
