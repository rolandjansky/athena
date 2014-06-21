/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetAuxContainerCnv.cxx 588765 2014-03-21 13:30:28Z delsart $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetAuxContainerCnv.h"

#include <TClass.h>

xAODJetAuxContainerCnv::xAODJetAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJetAuxContainerCnvBase( svcLoc ) {

}

#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
    if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::JetAuxContainer*
xAODJetAuxContainerCnv::
createPersistent( xAOD::JetAuxContainer* trans ) {

  static bool dictLoaded = false;
  if( ! dictLoaded ) {
    LOAD_DICTIONARY( "ElementLink<DataVector<xAOD::MuonSegment_v1> >" );
    LOAD_DICTIONARY( "std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1> > >" );
    LOAD_DICTIONARY( "std::vector<std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1> > > >" );

    LOAD_DICTIONARY( "std::vector<ElementLink<DataVector<xAOD::BTagging_v1> > >" );

    
    dictLoaded = true;
  }  

  // Create a copy of the container:
  xAOD::JetAuxContainer* result =
    new xAOD::JetAuxContainer( *trans );
  
  return result;
}

xAOD::JetAuxContainer* xAODJetAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "17E48A73-2219-44A0-8913-D59DB61F4B15" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JetAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetAuxContainer found" );
   return 0;
}
