/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include <string>

struct GaudiKernelFixtureBase{
  ISvcLocator* svcLoc{};
  static bool gaudiIsInitialised;
  const std::string jobOpts{};
  GaudiKernelFixtureBase(const std::string & jobOptionFile = "IOVDbSvc_BoostTest.txt"):jobOpts(jobOptionFile){
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
    if (not gaudiIsInitialised){
      std::string fullJobOptsName="IOVDbSvc/" + jobOpts;
      gaudiIsInitialised=Athena_test::initGaudi(fullJobOptsName, svcLoc);
    }
  }
};

bool GaudiKernelFixtureBase::gaudiIsInitialised=false;

#endif
