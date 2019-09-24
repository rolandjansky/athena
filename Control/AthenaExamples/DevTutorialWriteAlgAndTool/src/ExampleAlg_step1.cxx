/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleAlg_step1.h"

StatusCode ExampleAlg_step1::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode ExampleAlg_step1::execute(const EventContext& ctx) const
{
  ATH_MSG_VERBOSE("Events processed: "<<ctx.evt());
  return StatusCode::SUCCESS;
}
