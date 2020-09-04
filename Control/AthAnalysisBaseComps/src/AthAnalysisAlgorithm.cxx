///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisAlgorithm.cxx 
// Implementation file for class AthAnalysisAlgorithm
// Exactly like an AthAlgorithm
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "TROOT.h"

AthAnalysisAlgorithm::AthAnalysisAlgorithm( const std::string& name )
  : AthAnalysisAlgorithm( name, Gaudi::svcLocator() )
{
  //we assume this constructor is used outside of the framework
  //therefore we must increment the ref count so that 
  //any SmartIF doesn't "release" the alg and therefore delete it
  addRef();
}

AthAnalysisAlgorithm::AthAnalysisAlgorithm( const std::string& name
					    , ISvcLocator* pSvcLocator
					    , const std::string& )
  : AthHistogramAlgorithm(name,pSvcLocator)
  , m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name )
  , m_outputMetaStore( "StoreGateSvc/MetaDataStore", name )
{
  
  //declare an update handler for the EvtStore property, to reset the ServiceHandle
  for(auto prop : getProperties()) {
    if(prop->name() != "EvtStore") continue;
    prop->declareUpdateHandler(&AthAnalysisAlgorithm::updateEvtStore, this );
    break;
  }

}

AthAnalysisAlgorithm::~AthAnalysisAlgorithm() { }

void AthAnalysisAlgorithm::updateEvtStore(Gaudi::Details::PropertyBase& prop) {
  evtStore().release().ignore(); 
  evtStore().setTypeAndName(prop.toString());
}


ServiceHandle<StoreGateSvc>& AthAnalysisAlgorithm::inputMetaStore() const {
  return m_inputMetaStore;
}

ServiceHandle<StoreGateSvc>& AthAnalysisAlgorithm::outputMetaStore() const {
  return m_outputMetaStore;
}


StatusCode AthAnalysisAlgorithm::sysInitialize() {

  // Let the base class do its thing:
  ATH_CHECK( AthHistogramAlgorithm::sysInitialize() );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode AthAnalysisAlgorithm::sysExecute(const EventContext& ctx) {
  if(!m_doneFirstEvent) {
    m_doneFirstEvent=true;
    if( firstExecute().isFailure() ) {
      ATH_MSG_FATAL("Failure in firstEvent method");
      return StatusCode::FAILURE;
    }
  }
  return AthHistogramAlgorithm::sysExecute(ctx);
}

/// Dummy implementation that can be overridden by the derived tool.
///
StatusCode AthAnalysisAlgorithm::firstExecute() {

   // Return gracefully:
   return StatusCode::SUCCESS;
}
