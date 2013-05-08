/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  //////////////////////////////////////////////////////////////////////////////////////////
  // Name    : TauToolCompiledARAExample.C
  // Package : offline/PhysicsAnalysis/TauID/TauTools
  // Author  : Jyothsna Komaragiri and Dugan O'Neil
  //
  // Description: 
  //          This example shows how to run compiled C++ ARA example in AthenaROOTAccess.
  //
  // Usage:
  // % root -l TauToolCompiledARAExample.C
  // Don't forget to change the input ROOT file in test.py!!!!
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  //Create the transient tree in ARA
  TPython::ExecScript("test.py");

  TauToolCompiledARAExample example;

  // initialize method needs Tree as argument
  example.initialize((TTree *)gROOT->Get("CollectionTree_trans"));

  example.execute();

  example.finalize();

}
