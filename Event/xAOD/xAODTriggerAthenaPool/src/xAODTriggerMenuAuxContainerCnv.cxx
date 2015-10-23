/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTriggerMenuAuxContainerCnv.cxx 664263 2015-04-30 22:29:56Z ssnyder $

// System include(s):
#include <stdexcept>

// Local include(s):
#include "xAODTriggerMenuAuxContainerCnv.h"

xAODTriggerMenuAuxContainerCnv::
xAODTriggerMenuAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTriggerMenuAuxContainerCnvBase( svcLoc ) {

}

xAOD::TriggerMenuAuxContainer*
xAODTriggerMenuAuxContainerCnv::
createPersistent( xAOD::TriggerMenuAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TriggerMenuAuxContainer* result =
      new xAOD::TriggerMenuAuxContainer( *trans );

   return result;
}

xAOD::TriggerMenuAuxContainer*
xAODTriggerMenuAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "B8614CC5-8696-4170-8CCC-496DA7671246" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // Read in the v1 object:
      xAOD::TriggerMenuAuxContainer_v1* obj =
         poolReadObject< xAOD::TriggerMenuAuxContainer_v1 >();

      size_t sz = obj->smk.size();

      // A little helper macro for bringing the detailed variables to the
      // right size:
#define FIX_SIZE( NAME )                                                \
      do {                                                              \
         if( obj->NAME.size() < sz ) {                                    \
            obj->NAME.resize( sz );                                      \
            ATH_MSG_VERBOSE( "Fixed the size of " << #NAME << " to " << sz ); \
         }                                                              \
      } while( 0 )

      // Make sure that the detailed variables have the right size:
      FIX_SIZE( chainSignatureCounters );
      FIX_SIZE( chainSignatureLogics );
      FIX_SIZE( chainSignatureOutputTEs );
      FIX_SIZE( chainSignatureLabels );

      FIX_SIZE( sequenceInputTEs );
      FIX_SIZE( sequenceOutputTEs );
      FIX_SIZE( sequenceAlgorithms );

      // We don't need the macro anymore:
#undef FIX_SIZE

      // Return the "massaged" object:
      return obj;
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TriggerMenuAuxContainer found" );
   return 0;
}
