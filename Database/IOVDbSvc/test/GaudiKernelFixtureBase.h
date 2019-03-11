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
  const std::string here;
  GaudiKernelFixtureBase(const std::string & derivedLocation):here(derivedLocation){
    //The following is to ensure I can run the Boost test executable outside ctest
    //const std::string here=__FILE__;
    const size_t lastSlash=here.find_last_of("/");
    const std::string dir=here.substr(0,lastSlash+1);
    const std::string searchPath=dir+"../share/";
    //
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
    if (not gaudiIsInitialised){
      gaudiIsInitialised=Athena_test::initGaudi(searchPath+"IOVDbSvc_BoostTest.txt", svcLoc);
    }
  }
};

bool GaudiKernelFixtureBase::gaudiIsInitialised=false;

#endif
