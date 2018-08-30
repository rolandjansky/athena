/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <chrono>
#include <ctime>

#include "TrigCostMonitorMT/TrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////

TrigCostMTSvc::TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator) :
AthService(name, pSvcLocator) {
  ATH_MSG_INFO ("regular constructor");

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigCostMTSvc::~TrigCostMTSvc() {
  ATH_MSG_INFO ("destructor()");

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::initialize(){
  ATH_MSG_INFO ("initialize()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::finalize() {
  ATH_MSG_INFO ("finalize()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::beginAlg(const std::string& caller) {
  std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
  std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
  ATH_MSG_INFO ("Caller " << caller << " began at " << std::ctime(&time_now_t));
  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endAlg(const std::string& caller) {
  std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
  std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
  ATH_MSG_INFO ("Caller " << caller << " ended at " << std::ctime(&time_now_t));
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
