/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysAuxContainerCnv.cxx

// System include(s):
#include <exception>
#include <mutex>

// Local include(s):
#include "xAODTrigBphysAuxContainerCnv.h"


#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
        if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::TrigBphysAuxContainer*
xAODTrigBphysAuxContainerCnv::
createPersistentWithKey( xAOD::TrigBphysAuxContainer* trans,
                         const std::string& key)
{
  // ??? Still needed?
  std::once_flag flag;
  std::call_once (flag,
                  [this] {
                    LOAD_DICTIONARY("ElementLink<DataVector<xAOD::TrigBphys_v1> >" );
                    LOAD_DICTIONARY("std::vector<ElementLink<DataVector<xAOD::TrackParticle_v1> >" );
                  });

  // Create a copy of the container:
  return xAODTrigBphysAuxContainerCnvBase::createPersistentWithKey (trans, key);
}
