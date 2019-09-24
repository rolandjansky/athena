/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleAlg_step1.h"

ExampleAlg_step1::ExampleAlg_step1 (const std::string& name, ISvcLocator* svcLocator)
  : AthReentrantAlgorithm (name, svcLocator)
{ }

StatusCode ExampleAlg_step1::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode ExampleAlg_step1::execute(const EventContext& ctx) const
{
  ATH_MSG_VERBOSE("Events processed: "<<ctx.evt());
  return StatusCode::SUCCESS;
}
