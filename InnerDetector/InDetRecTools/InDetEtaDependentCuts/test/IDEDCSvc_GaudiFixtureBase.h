/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file InDetEtaDependentCuts/test/IDEDC_GaudiFixtureBase.h
 * @author Shaun Roe
 * @date June 2020
 * @brief Base class for initialising Gaudi in fixtures
 */

#ifndef InDetPhysValMonitoring_IDPVM_GaudiFixtureBase_h
#define InDetPhysValMonitoring_IDPVM_GaudiFixtureBase_h


#include "TestTools/initGaudi.h"
#include "TInterpreter.h"
#include "CxxUtils/checker_macros.h"
#include "CxxUtils/ubsan_suppress.h"
#include <string>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This is for unit tests and a static variable IDEDC_GaudiFixtureBase::gaudiIsInitialised is used.

struct IDEDC_GaudiFixtureBase{
  ISvcLocator* svcLoc{};
  static bool gaudiIsInitialised;
  const std::string jobOpts{};
  IDEDC_GaudiFixtureBase(const std::string & jobOptionFile = "InDetEtaDependentCutsTestJobOpts.txt"):jobOpts(jobOptionFile){
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
    if (not gaudiIsInitialised){
      std::string fullJobOptsName="InDetEtaDependentCuts/" + jobOpts;
      gaudiIsInitialised=Athena_test::initGaudi(fullJobOptsName, svcLoc);
    }
  }
};

bool IDEDC_GaudiFixtureBase::gaudiIsInitialised=false;

#endif
