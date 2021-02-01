/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MbtsHypoAlg.h"

MbtsHypoAlg::MbtsHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

MbtsHypoAlg::~MbtsHypoAlg()
{
}

StatusCode MbtsHypoAlg::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode MbtsHypoAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode MbtsHypoAlg::execute(const EventContext& context) const
{
  return StatusCode::SUCCESS;
}

