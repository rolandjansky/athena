#!/bin/sh

INCDIR1=" -I`cmt show versions Trigger/TrigAnalysis/TriggerMenuNtuple | sed -n 1p | awk '{print $3}'`/InstallArea/include/TriggerMenuNtuple"
INCDIR1="\"${INCDIR1}\";"

cat > run.cc <<eof
void run(){
  gSystem->Load("libCintex.so");
  Cintex::Enable();
  std::string incpath = gSystem->GetIncludePath();
  incpath += ${INCDIR1}
  gSystem->SetIncludePath(incpath.c_str());
  gSystem->Load("libTriggerMenuNtupleDict.so");

  gROOT->LoadMacro("TrigMuonD3PD.C+");
  gROOT->LoadMacro("TrigConfTree.C+");
  gROOT->LoadMacro("sample.cc+");
  sample();
}
eof
