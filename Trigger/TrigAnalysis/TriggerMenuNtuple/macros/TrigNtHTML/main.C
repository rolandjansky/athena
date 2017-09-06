/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::string incpath = gSystem->GetIncludePath();
incpath += "-I/home/okuyama/athena/15.5.3.8/InstallArea/include/TriggerMenuNtuple";
gSystem->SetIncludePath(incpath.c_str());
gSystem->Load("/home/okuyama/athena/15.5.3.8/InstallArea/i686-slc4-gcc34-opt/lib/libTriggerMenuNtupleDict.so");

gROOT->LoadMacro("TrigMenu.C+");
gROOT->LoadMacro("L1_Cluster.C+");
gROOT->LoadMacro("L2_Cluster.C+");
gROOT->LoadMacro("L2_Track.C+");
gROOT->LoadMacro("TrigMuon.C+");
gROOT->LoadMacro("TrigVarInElectronChain.cxx+");
gROOT->LoadMacro("TrigVarInMuonChain.cxx+");
gROOT->LoadMacro("TrigVarInJetChain.cxx+");
gROOT->LoadMacro("HTMLUtil.cxx+");
gROOT->LoadMacro("createMenuPlot.C+");

createMenuPlot();
}
