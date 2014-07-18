/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigBphysAuxContainerCnv.h"

xAODTrigBphysAuxContainerCnv::xAODTrigBphysAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigBphysAuxContainerCnvBase( svcLoc ) {

}


#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
        if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::TrigBphysAuxContainer*
xAODTrigBphysAuxContainerCnv::
createPersistent( xAOD::TrigBphysAuxContainer* trans ) {

    static bool dictLoaded(false);
    if (!dictLoaded) {
        LOAD_DICTIONARY("ElementLink<DataVector<xAOD::TrigBphys_v1> >" );
        LOAD_DICTIONARY("std::vector<ElementLink<DataVector<xAOD::TrackParticle_v1> >" );
        dictLoaded = true;
    }
    
    
   // Create a copy of the container:
   xAOD::TrigBphysAuxContainer* result =
      new xAOD::TrigBphysAuxContainer( *trans );

   return result;
}

xAOD::TrigBphysAuxContainer* xAODTrigBphysAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   //static const pool::Guid v1_guid( "4A2F55AF-D465-42A8-A4CF-2DB84D9628E5" ); // original
   static const pool::Guid v1_guid( "C7246162-DB5D-4ACA-BF20-838A1B2BC4A3"); // new

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigBphysAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigBphysAuxContainer found" );
   return 0;
}
