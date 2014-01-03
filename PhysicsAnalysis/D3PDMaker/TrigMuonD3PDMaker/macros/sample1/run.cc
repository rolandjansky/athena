/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void run(){
  gSystem->Load("libCintex.so");
  Cintex::Enable();
  gROOT->LoadMacro("TrigMuonD3PD.C+");
  gROOT->LoadMacro("TrigConfTree.C+");
  gROOT->LoadMacro("sample.cc+");
  sample();
}
