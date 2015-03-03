/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void run(){
  gSystem->Load("libCintex.so");
  Cintex::Enable();
  std::string incpath = gSystem->GetIncludePath();
  incpath += " -I/home/kanno/athena/AtlasProduction-15.6.11.4/InstallArea/include/TriggerMenuNtuple";
  gSystem->SetIncludePath(incpath.c_str());
  gSystem->Load("libTriggerMenuNtupleDict.so");

  gROOT->LoadMacro("TrigMuonD3PD.C+");
  gROOT->LoadMacro("TrigConfTree.C+");
  gROOT->LoadMacro("sample.cc+");
  sample();
}
