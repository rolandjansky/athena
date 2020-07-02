/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file InDetPhysValMonitoring/test/IDPVM_GaudiFixtureBase.h
 * @author Shaun Roe
 * @date May 2020
 * @brief Base class for initialising Gaudi in fixtures
 */

#ifndef InDetPhysValMonitoring_IDPVM_GaudiFixtureBase_h
#define InDetPhysValMonitoring_IDPVM_GaudiFixtureBase_h


#include "TestTools/initGaudi.h"
#include "TInterpreter.h"
#include "CxxUtils/ubsan_suppress.h"
#include <string>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This test uses static gaudiIsInitialised, and is not thread safe.

struct IDPVM_GaudiFixtureBase{
  ISvcLocator* svcLoc{};
  static bool gaudiIsInitialised;
  const std::string jobOpts{};
  IDPVM_GaudiFixtureBase(const std::string & jobOptionFile = "IDPVM_Test.txt"):jobOpts(jobOptionFile){
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
    if (not gaudiIsInitialised){
      std::string fullJobOptsName="InDetPhysValMonitoring/" + jobOpts;
      gaudiIsInitialised=Athena_test::initGaudi(fullJobOptsName, svcLoc);
    }
  }
};

bool IDPVM_GaudiFixtureBase::gaudiIsInitialised=false;

#endif
