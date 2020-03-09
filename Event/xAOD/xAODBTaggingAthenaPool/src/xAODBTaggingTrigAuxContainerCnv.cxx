/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingTrigAuxContainerCnv.cxx 705820 2015-11-04 14:54:05Z krasznaa $

// System include(s):
#include <exception>

// ROOT include(s):
#include <TClass.h>

// Gaudi/Athena include(s):
#include "AthContainers/tools/copyThinned.h"

// EDM include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"

// Local include(s):
#include "xAODBTaggingTrigAuxContainerCnv.h"
#include "xAODBTaggingTrigAuxContainerOfflineCnv_v1.h"


#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
    if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::BTaggingTrigAuxContainer*
xAODBTaggingTrigAuxContainerCnv::
createPersistentWithKey( xAOD::BTaggingTrigAuxContainer* trans,
                         const std::string& key )
{
  // ??? Still needed?
  std::once_flag flag;
  std::call_once (flag,
                  [this] {
                    LOAD_DICTIONARY( "std::vector<std::vector<ElementLink<DataVector<xAOD::TrackParticle_v1> > > >" );
                  });
                                                
  // Create a copy of the container:
  return xAODBTaggingTrigAuxContainerCnvBase::createPersistentWithKey (trans, key);
}
