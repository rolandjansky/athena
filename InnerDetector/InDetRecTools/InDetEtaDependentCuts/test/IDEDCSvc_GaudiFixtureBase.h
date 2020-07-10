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
#include "CxxUtils/ubsan_suppress.h"
#include <string>


struct IDEDC_GaudiFixtureBase{
  ISvcLocator* svcLoc{};
  const std::string jobOpts{};
  static bool initGaudi (const std::string & jobOptionFile, ISvcLocator*& svcLoc)
  {
    CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
    std::string fullJobOptsName="InDetEtaDependentCuts/" + jobOptionFile;
    return Athena_test::initGaudi(fullJobOptsName, svcLoc);
  }
  IDEDC_GaudiFixtureBase(const std::string & jobOptionFile = "InDetEtaDependentCutsTestJobOpts.txt"):jobOpts(jobOptionFile){
    static const bool gaudiIsInitialized [[maybe_unused]]
      = initGaudi (jobOptionFile, svcLoc);
  }
};


#endif
