/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TString.h"

int SUSYTools_RTT(TString dataFile="")
{
  Int_t status;

  TString myTag = gSystem->GetFromPipe("cmt show versions PhysicsAnalysis/SUSYPhys/SUSYTools | head -1 | awk '{print $2}'");
  cout << "SUSYTools tag is " << myTag << endl;
  
  TString cmd = "cmt co -r " + myTag + " PhysicsAnalysis/SUSYPhys/SUSYTools";
  status = gSystem->Exec(cmd.Data());

  status = gSystem->Exec("mv PhysicsAnalysis/SUSYPhys/SUSYTools .");

  cmd = "python SUSYTools/python/install.py; cd RootCore; ./configure; source scripts/setup.sh; cd ..; $ROOTCOREDIR/scripts/find_packages.sh; $ROOTCOREDIR/scripts/compile.sh;";
  cmd += " python SUSYTools/macros/susycutflow_1lep.py --infile=" + dataFile;
  status = gSystem->Exec(cmd.Data());

  return 0;
  
}
