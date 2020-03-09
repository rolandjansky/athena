/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
xAODTriggerMenuAuxContainerCnv::createTransientWithKey (const std::string& key) {

  std::unique_ptr<xAOD::TriggerMenuAuxContainer_v1> obj
    { xAODTriggerMenuAuxContainerCnvBase::createTransientWithKey (key) };

  size_t sz = obj->smk.size();

  // A little helper macro for bringing the detailed variables to the
  // right size:
#define FIX_SIZE( NAME )                                                \
  do {                                                                  \
    if( obj->NAME.size() < sz ) {                                       \
      obj->NAME.resize( sz );                                           \
      ATH_MSG_VERBOSE( "Fixed the size of " << #NAME << " to " << sz ); \
    }                                                                   \
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
  return obj.release();
}
