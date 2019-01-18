/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAODFixAlg.cxx 298140 2013-11-19 11:32:49Z emoyse $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "xAODEventInfo/EventInfoAODFixAlg.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

// Misc
#include "CxxUtils/make_unique.h"
#include <memory>

namespace xAOD {

  EventInfoAODFixAlg::EventInfoAODFixAlg( const std::string& name,
    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
  {

    declareProperty( "RunNumber", m_runNumber = 999 );
  }

  StatusCode EventInfoAODFixAlg::initialize() {
      // Return gracefully:
    return StatusCode::SUCCESS;
    
  }

  StatusCode EventInfoAODFixAlg::execute() {
    
    std::string m_containerName = "EventInfo";
    const SG::DataProxy* proxy =
    evtStore()->proxy (ClassID_traits<xAOD::EventInfo>::ID(),
                       "EventInfo");
    if (!proxy) {
      ATH_MSG_WARNING( "No xAOD::EventInfo "
                       << "with key \"" << m_containerName << "\" found" );
      return StatusCode::SUCCESS;
    }
        
    xAOD::EventInfo* eventInfo=nullptr;
    
    if (proxy->isConst()) {
      const xAOD::EventInfo* originalEventInfo = nullptr;
      ATH_CHECK( evtStore()->retrieve (originalEventInfo,
                                       "EventInfo") );
    
      eventInfo = new xAOD::EventInfo(*originalEventInfo);
      auto store = CxxUtils::make_unique<xAOD::EventInfoAuxContainer>();
      eventInfo->setStore (store.get());
      eventInfo->setRunNumber(m_runNumber);
        
      ATH_CHECK( evtStore()->overwrite (eventInfo,
                                    "EventInfo",
                                    true, false) );
      ATH_CHECK( evtStore()->overwrite (std::move(store),
                                    "EventInfoAux.",
                                    true, false) );
    } else {
      ATH_CHECK( evtStore()->retrieve (eventInfo,
                                       "EventInfo") );
        eventInfo->setRunNumber(m_runNumber);
    }
    
    return StatusCode::SUCCESS;
  }

}
