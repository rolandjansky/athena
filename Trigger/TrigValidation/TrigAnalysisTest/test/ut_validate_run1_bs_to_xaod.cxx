/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Compare HLT variables from Run1 bytestream before and after the conversion to xAOD.
// The test will be running within the AnalysisBase ATN group, while the input xAOD file is
// produced in the AtlasAnalysis. Since we are not sure about the order of these tests, 
// let us use as the input the output of the yesterday's AtlasAnalysis nightly.
// Oleg Kuprash  <oleg.kuprash@cern.ch>
#include <iostream>
#include <ctime>

#include "TrigAnalysisTest/RootCoreTestHarness.h"

#include "TString.h"
#include "TSystem.h"



int main()
{
  
  std::time_t now = time(0);
  std::tm *gmtm = gmtime(&now);
  int i_today = gmtm->tm_wday;
  std::cout << "today is " << i_today << std::endl;
  int i_yesterday = (i_today+6)%7;
  TString s_nightly_aod_yesterday = "";
  s_nightly_aod_yesterday.Form("/afs/cern.ch/atlas/software/builds/nightlies/20.7.X-VAL/AtlasAnalysis/rel_%i/NICOS_area/NICOS_atntest207XVAL64BS6G49AnlOpt/triganalysistest_testconfiguration_work/TrigHLT_Run1BStoxAOD/myAOD.pool.root", i_yesterday);
  
  std::cout << "the path for AOD from yesterday's ATN is: " << s_nightly_aod_yesterday << std::endl;

  gSystem->Setenv("ROOTCORE_TEST_FILE", s_nightly_aod_yesterday);

  return TrigAnalysisTest::runTrigAnalysisTest("Run1BStoxAODTrigger");
}
