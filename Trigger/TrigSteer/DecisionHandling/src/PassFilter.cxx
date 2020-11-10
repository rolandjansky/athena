/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PassFilter.h"

PassFilter::PassFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

PassFilter::~PassFilter()
{
}

StatusCode PassFilter::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode PassFilter::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode PassFilter::execute(const EventContext&) const
{
  return StatusCode::SUCCESS;
}

