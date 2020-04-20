/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetAuxContainerCnv.cxx 588765 2014-03-21 13:30:28Z delsart $

#if !(defined(GENERATIONBASE) || defined(SIMULATIONBASE))

// Local include(s):
#include "xAODJetTrigAuxContainerCnv.h"
#include "xAODJetTrigAuxContainerCnv_v1.h"

#include <TClass.h>
#include <mutex>

#define LOAD_DICTIONARY( name ) do {  TClass* cl = TClass::GetClass( name ); \
    if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {  ATH_MSG_ERROR( "Couldn't load dictionary for type: " << name ); } } while(0)


xAOD::JetTrigAuxContainer*
xAODJetTrigAuxContainerCnv::
createPersistentWithKey( xAOD::JetTrigAuxContainer* trans,
                         const std::string& key )
{
  // ??? Still needed?
  std::once_flag flag;
  std::call_once (flag,
                  [this] {
                    LOAD_DICTIONARY( "ElementLink<DataVector<xAOD::MuonSegment_v1> >" );
                    LOAD_DICTIONARY( "std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1> > >" );
                    LOAD_DICTIONARY( "std::vector<std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1> > > >" );
                    LOAD_DICTIONARY( "std::vector<ElementLink<DataVector<xAOD::BTagging_v1> > >" );
                  });

  // Create a copy of the container:
  return xAODJetTrigAuxContainerCnvBase::createPersistentWithKey (trans, key);
}


#endif // NOT SIMULATIONBASE OR GENERATIONBASE
