/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include <EventBookkeeperTools/FilterReporter.h>

#include "TestFilterReentrantAlg.h"

TestFilterReentrantAlg::TestFilterReentrantAlg(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode TestFilterReentrantAlg::initialize()
{
  ATH_MSG_DEBUG("initialize()");

  ATH_CHECK(m_filterParams.initialize());

  return StatusCode::SUCCESS;
}


StatusCode TestFilterReentrantAlg::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("execute()");

  FilterReporter filter(m_filterParams, false, ctx);
  if (ctx.eventID().event_number() % m_modulo.value() == 0) {
    filter.setPassed(true);
  }

  return StatusCode::SUCCESS;
}

StatusCode TestFilterReentrantAlg::finalize()
{
  ATH_MSG_DEBUG("finalize()");

  ATH_MSG_INFO(m_filterParams.summary());

  return StatusCode::SUCCESS;
}
