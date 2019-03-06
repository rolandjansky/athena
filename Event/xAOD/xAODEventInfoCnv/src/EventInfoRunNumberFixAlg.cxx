/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventInfoRunNumberFixAlg.h"
#include "xAODEventInfo/EventAuxInfo.h"

// Misc
#include "CxxUtils/make_unique.h"
#include <memory>

namespace xAOD {

  EventInfoRunNumberFixAlg::EventInfoRunNumberFixAlg( const std::string& name,
    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
  {

    declareProperty( "McChannelNumber" , m_mcChannelNumber = 0         );
    declareProperty( "ContainerName"   , m_containerName = "EventInfo" );
  }

  StatusCode EventInfoRunNumberFixAlg::initialize() {
      // Return gracefully:
    return StatusCode::SUCCESS;
    
  }

  StatusCode EventInfoRunNumberFixAlg::execute() {
    
    const xAOD::EventInfo* originalEventInfo = nullptr;
    ATH_CHECK( evtStore()->retrieve (originalEventInfo,
                                     m_containerName) );

    // Only run if a difference is found
    if( originalEventInfo->mcChannelNumber()==m_mcChannelNumber ){
      return StatusCode::SUCCESS;
    }

    //
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
      eventInfo = new xAOD::EventInfo();
      *eventInfo = *originalEventInfo;
      auto store = CxxUtils::make_unique<xAOD::EventAuxInfo>();
      eventInfo->setStore (store.get());
      eventInfo->setMCChannelNumber(m_mcChannelNumber);
      ATH_CHECK( evtStore()->overwrite (eventInfo,
                                    m_containerName,
                                    true, false) );
      ATH_CHECK( evtStore()->overwrite (std::move(store),
                                    m_containerName + "Aux.",
                                    true, false) );
    } else {
      ATH_CHECK( evtStore()->retrieve (eventInfo,
                                       m_containerName) );
      eventInfo->setMCChannelNumber(m_mcChannelNumber);
    }
    return StatusCode::SUCCESS;
  }

} // xAOD namespace
