/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/GaudiKernelFixtureBase.h
 * @author Shaun Roe
 * @date Feb, 2019
 * @brief Base class for initialising Gaudi in fixtures
 */

#ifndef IOVDBSVC_GaudiKernelFixtureBase
#define IOVDBSVC_GaudiKernelFixtureBase


#include "TestTools/initGaudi.h"
#include "TInterpreter.h"
#include "CxxUtils/ubsan_suppress.h"
#include "CxxUtils/checker_macros.h"
#include <string>
#include <mutex>

struct GaudiKernelFixtureBase{
  static ISvcLocator* svcLoc ATLAS_THREAD_SAFE;
  static std::mutex m;
  const std::string jobOpts{};
  GaudiKernelFixtureBase(const std::string & jobOptionFile = "IOVDbSvc_BoostTest.txt"):jobOpts(jobOptionFile){
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
    std::scoped_lock lock (m);
    if (svcLoc==nullptr){
      std::string fullJobOptsName="IOVDbSvc/" + jobOpts;
      Athena_test::initGaudi(fullJobOptsName, svcLoc);
    }
  }
};

ISvcLocator* GaudiKernelFixtureBase::svcLoc = nullptr;
std::mutex GaudiKernelFixtureBase::m;

#endif
