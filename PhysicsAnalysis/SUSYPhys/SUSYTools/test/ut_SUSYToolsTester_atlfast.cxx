/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

#include <AsgTools/MessageCheck.h>

int main()
{
  using namespace asg::msgUserCode;

  ATH_MSG_INFO ("Unit test for SUSYTools on MC");

  // Stored updated references in SUSY ART area for now
  std::string inputFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.AFII.46.mc16e.PHYSVAL.pool.root";

  std::string cmd("SUSYToolsTester " + inputFile + " maxEvents=100 isData=0 isAtlfast=1 Debug=0 NoSyst=0");
  cmd.append(" PRWFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc16_13TeV/pileup_mc16e_dsid410470_AFII.root");
  cmd.append(" ilumicalcFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190219/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root");

  ATH_MSG_INFO ("Will now run this command: " << cmd);
  int ret = system(cmd.c_str());

  if (ret != 0) {
    ATH_MSG_ERROR ("Test failed (return code was " << ret << ")");
    return 1;
  }

  ATH_MSG_INFO ("Finished (return code was " << ret << ")");
  return 0;
}
