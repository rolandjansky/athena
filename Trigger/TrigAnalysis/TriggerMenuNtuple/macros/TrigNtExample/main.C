/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::string incpath = gSystem->GetIncludePath();
incpath += "-I/home/okuyama/athena/15.5.3.8/InstallArea/include/TriggerMenuNtuple";
gSystem->SetIncludePath(incpath.c_str());
gSystem->Load("/home/okuyama/athena/15.5.3.8/InstallArea/i686-slc4-gcc34-opt/lib/libTriggerMenuNtupleDict.so");

gROOT->LoadMacro("TrigMuon.C+");
gROOT->LoadMacro("myTrigVarAnalysis.cxx+");
gROOT->LoadMacro("example.C+");

example("ntuple/trigntuple.root");
}
