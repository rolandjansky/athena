/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoUnlocker.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "CxxUtils/make_unique.h"
//================ Constructor =================================================

EventInfoUnlocker::EventInfoUnlocker(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator)
{

}

//================ Initialisation =================================================

StatusCode EventInfoUnlocker::initialize()
{
  ATH_MSG_DEBUG( "initialize: Setting up EventInfo Unlocker - **ONLY needed ror running on ESD!**");
  
  // retrieve the StoreGate Service (delete if not needed)
  if (!evtStore().retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve StoreGateSvc!");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode EventInfoUnlocker::execute()
{

  const SG::DataProxy* proxy =
    evtStore()->proxy (ClassID_traits<xAOD::EventInfo>::ID(),
		       "EventInfo");
  if (!proxy) {
    ATH_MSG_WARNING( "No xAOD::EventInfoContainer "
		     << "with key EventInfo found" );
    return StatusCode::SUCCESS;
  }
  
  if (proxy->isConst()) {
    const xAOD::EventInfo* ccoll = nullptr;
    const xAOD::EventAuxInfo* cauxcoll = nullptr;
    ATH_CHECK( evtStore()->retrieve (ccoll,
				     "EventInfo") );
    ATH_CHECK( evtStore()->retrieve (cauxcoll,
    				     "EventInfoAux.") );
    
    xAOD::EventInfo * eventInfo = new xAOD::EventInfo;
    xAOD::EventAuxInfo * store = new xAOD::EventAuxInfo;

    eventInfo->setStore(store);
    *eventInfo = *ccoll;

    CHECK( evtStore()->overwrite (eventInfo,
				  "EventInfo",
				  true, false) );
    CHECK( evtStore()->overwrite (store,
    			  "EventInfoAux.",
    				  true, false) );



   
  } 
 
  return StatusCode::SUCCESS;
}
