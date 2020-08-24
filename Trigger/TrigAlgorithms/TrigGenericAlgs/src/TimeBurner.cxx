/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TimeBurner.h"

#include <thread>
#include <chrono>

TimeBurner::TimeBurner(const std::string& name, ISvcLocator* pSvcLocator)
: ::HypoBase(name, pSvcLocator) {}

StatusCode TimeBurner::execute(const EventContext& /*eventContext*/) const {

  std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTimeMillisec.value()));

  return StatusCode::SUCCESS;
}
