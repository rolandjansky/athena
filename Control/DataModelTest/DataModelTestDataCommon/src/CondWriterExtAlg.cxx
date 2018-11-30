/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CondWriterExtAlg.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventIncident.h"
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"

#include <stdlib.h>

namespace DMTest {

CondWriterExtAlg::CondWriterExtAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_iovSvc("IOVSvc", name),
  m_iovDbSvc("IOVDbSvc", name),
  m_incidentSvc("IncidentSvc", name)
{
}

StatusCode CondWriterExtAlg::initialize()
{
  ATH_CHECK( m_eventInfoKey.initialize() );    
  ATH_CHECK( m_iovSvc.retrieve() );
  ATH_CHECK( m_iovDbSvc.retrieve() );
  ATH_CHECK( m_incidentSvc.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode CondWriterExtAlg::execute()
{
  SG::ReadHandle<EventInfo> eventInfo (m_eventInfoKey);

  ATH_MSG_INFO ("Event " << eventInfo->event_ID()->event_number() <<
                " LBN " << eventInfo->event_ID()->lumi_block());

  // Check if we need to execute a command
  auto it = m_cmd.find(eventInfo->event_ID()->lumi_block());
  if (it != m_cmd.end()) {
    ATH_MSG_INFO("Executing: " << it->second);
    if ( system(it->second.c_str()) != 0 ) {
      ATH_MSG_ERROR("Error executing command");
      return StatusCode::FAILURE;
    }
    // Remove this command
    m_cmd.erase(it);

    ATH_MSG_INFO("Resetting and dropping payload of folder " << m_attrListKey.value());

    // Set IOV in the past to trigger reload in IOVSvc
    IOVRange iov_range(IOVTime(0, 0), IOVTime(0, 0));
    ATH_CHECK( m_iovSvc->setRange(detStore()->clid(m_attrListKey), m_attrListKey, iov_range, "StoreGateSvc") );
  }

    // Drop folder payload
    ATH_CHECK( m_iovDbSvc->dropObject(m_attrListKey, /*resetCache=*/true), StatusCode::FAILURE );

    return StatusCode::SUCCESS;
}

} // namespace DMTest
