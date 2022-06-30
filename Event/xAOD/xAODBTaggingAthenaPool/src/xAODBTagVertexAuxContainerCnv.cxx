/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingAuxContainerCnv.cxx 566967 2013-10-24 13:24:31Z krasznaa $

// System include(s):
#include <exception>

// ROOT include(s):
#include <TClass.h>

// Local include(s):
#include "xAODBTagVertexAuxContainerCnv.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "AthContainers/tools/copyThinned.h"


#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
    if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::BTagVertexAuxContainer*
xAODBTagVertexAuxContainerCnv::
createPersistentWithKey( xAOD::BTagVertexAuxContainer* trans,
                         const std::string& key)
{
  // ??? Still needed?
  static std::once_flag flag;
  std::call_once (flag,
                  [this] {
                    LOAD_DICTIONARY( "std::vector<std::vector<ElementLink<DataVector<xAOD::TrackParticle_v1> > > >" );
                  });
                                                
  // Create a copy of the container:
  return xAODBTagVertexAuxContainerCnvBase::createPersistentWithKey (trans, key);
}

