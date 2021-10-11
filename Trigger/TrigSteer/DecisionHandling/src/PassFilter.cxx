/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PassFilter.h"
#include "AthenaKernel/Timeout.h"
#include "AthenaKernel/AthStatusCode.h"

PassFilter::PassFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PassFilter::execute(const EventContext& ctx) const
{
  if (Athena::Timeout::instance(ctx).reached()) {
    ATH_MSG_ERROR("Timeout reached before " << name());
    return Athena::Status::TIMEOUT;
  }
  return StatusCode::SUCCESS;
}

