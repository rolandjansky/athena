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
  // locate the output of the yesterday's ATN test TrigHLT_Run1BStoxAOD
  std::time_t now = time(0);
  std::tm *gmtm = gmtime(&now);
  int i_today = gmtm->tm_wday;
  std::cout << "today is " << i_today << std::endl;
  int i_yesterday = (i_today+6)%7;
  TString s_nightly_aod_yesterday = "";
  s_nightly_aod_yesterday.Form("/afs/cern.ch/atlas/software/builds/nightlies/20.7.X-VAL/AtlasAnalysis/rel_%i/NICOS_area/NICOS_atntest207XVAL64BS6G49AnlOpt/triganalysistest_testconfiguration_work/TrigHLT_Run1BStoxAOD/myAOD.pool.root", i_yesterday);
  std::cout << "the path for AOD from yesterday's ATN is: " << s_nightly_aod_yesterday << std::endl;
  // use this file if the ATN test upstream was not successful
  // it is also used for ROOTCORE_RELEASE_SERIES = 23
  TString s_ref_file = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data12_xAODmadeWithAtlasAnalysis207XVAL151124_forATNAnalysisBase.root";
  
  std::cout << "check if the ATN output file exists..." << std::endl;
  Long_t id,size,flags,modtime;
  TString s_file = "";
  if(gSystem->GetPathInfo(s_nightly_aod_yesterday,&id,&size,&flags,&modtime)){ // returns 1 if the file could not be stat'ed
    std::cout << "WARNING: it seems that the yesterday's ATN TrigHLT_Run1BStoxAOD in AtlasAnalysis,20.7.X-VAL was unsuccessful, going to use input file from the eos space" << std::endl;
    s_file = s_ref_file;
  } else {
    s_file = s_nightly_aod_yesterday;
  }

  // if this is AnalysisBase,2.3, it won't work with the root file prepared in current 20.7.X-VAL due to different xAOD schema version
  int rc_release = atoi(gSystem->Getenv("ROOTCORE_RELEASE_SERIES"));
  if(rc_release == 23) {
    std::cout << "This is ROOTCORE_RELEASE_SERIES = 23, will use the corresponding input file" << std::endl;
    s_file = s_ref_file;
  }

  std::cout << s_file << " will be used as the input for this test" << std::endl;
  
  gSystem->Setenv("ROOTCORE_TEST_FILE", s_file);

  return TrigAnalysisTest::runTrigAnalysisTest("Run1BStoxAODTrigger");
}
