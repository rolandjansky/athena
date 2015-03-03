/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gSystem->Load("../lib/libMuonTriggerTool.so");
  gROOT->ProcessLine(".include ..");

  gROOT->LoadMacro("physics.C+");
  gROOT->LoadMacro("example.cxx+");

  example();
}
