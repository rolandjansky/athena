/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  init_tmn.C
  ----------
  Initialize the ROOT environment to use the TriggerMenuNtuple library
  root> .x init_tmn.C("<TMN_package_location>");
*/

#include "TSystem.h"

void init_tmn(const std::string& tmndir="./TriggerMenuNtuple") {
  std::string s = tmndir + "/" + "lib/libTriggerMenuNtuple.so";
  gSystem->Load(s.c_str());
  gROOT->LoadMacro("extradict.C+");
  gROOT->LoadMacro("D3PD_NTUP.C+");
  s = " -I" + tmndir + " " + gSystem->GetIncludePath();
  gSystem->SetIncludePath(s.c_str());
}
