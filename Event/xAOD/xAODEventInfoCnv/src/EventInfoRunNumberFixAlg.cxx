/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventInfoRunNumberFixAlg.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

// Misc
#include "CxxUtils/make_unique.h"
#include <memory>

namespace xAOD {

  EventInfoRunNumberFixAlg::EventInfoRunNumberFixAlg( const std::string& name,
    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
  {

    declareProperty( "RunNumber"    , m_runNumber     = 999         );
    declareProperty( "ContainerName", m_containerName = "EventInfo" );
  }

  StatusCode EventInfoRunNumberFixAlg::initialize() {
      // Return gracefully:
    return StatusCode::SUCCESS;
    
  }

  StatusCode EventInfoRunNumberFixAlg::execute() {
    
    const SG::DataProxy* proxy =
    evtStore()->proxy (ClassID_traits<xAOD::EventInfo>::ID(),
                       m_containerName );
    if (!proxy) {
      ATH_MSG_WARNING( "No xAOD::EventInfo "
                       << "with key \"" << m_containerName << "\" found" );
      return StatusCode::SUCCESS;
    }
        
    xAOD::EventInfo* eventInfo=nullptr;
    
    if (proxy->isConst()) {
      const xAOD::EventInfo* originalEventInfo = nullptr;
      ATH_CHECK( evtStore()->retrieve (originalEventInfo,
                                       m_containerName) );
    
      eventInfo = new xAOD::EventInfo(*originalEventInfo);
      auto store = CxxUtils::make_unique<xAOD::EventInfoAuxContainer>();
      eventInfo->setStore (store.get());
      eventInfo->setRunNumber(m_runNumber);
        
      ATH_CHECK( evtStore()->overwrite (eventInfo,
                                    m_containerName,
                                    true, false) );
      ATH_CHECK( evtStore()->overwrite (std::move(store),
                                    m_containerName + "Aux.",
                                    true, false) );
    } else {
      ATH_CHECK( evtStore()->retrieve (eventInfo,
                                       "EventInfo") );
      eventInfo->setRunNumber(m_runNumber);
    }
    
    return StatusCode::SUCCESS;
  }

}
