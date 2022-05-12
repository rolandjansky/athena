/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#include <EventBookkeeperTools/FilterReporter.h>
#include <xAODCutFlow/CutBookkeeper.h>

#include "AllExecutedEventsCounterAlg.h"


AllExecutedEventsCounterAlg::AllExecutedEventsCounterAlg(const std::string& name,
                                                         ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode AllExecutedEventsCounterAlg::initialize ATLAS_NOT_THREAD_SAFE ()
{
  ATH_MSG_DEBUG("Initializing " << this->name() << "...");

  ATH_CHECK(m_filterParams.initialize());

  m_filterParams.cutFlowSvc()->registerTopFilter(m_filterParams.key(),
                                                 "Number of processed events before any cut",
                                                 xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
                                                 "AllStreams",
                                                 false);

  return StatusCode::SUCCESS;
}


StatusCode AllExecutedEventsCounterAlg::execute(const EventContext& ctx) const
{
  FilterReporter filter(m_filterParams, true, ctx);

  return StatusCode::SUCCESS;
}


StatusCode AllExecutedEventsCounterAlg::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << this->name() << "...");

  ATH_MSG_INFO(m_filterParams.summary());

  return StatusCode::SUCCESS;
}
