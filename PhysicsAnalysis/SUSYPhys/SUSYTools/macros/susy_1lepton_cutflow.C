/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  cout << "First part: Check if file can be read" << endl << endl;

  gSystem->CompileMacro("Test_dpd.C", "f");
  Test_dpd(0,0);

  cout << endl << "Second part: Produce cutflow" << endl << endl;

  gSystem->Load("../StandAlone/libSUSYTools.so");
  Reco_dpd2 t;
  t.Loop();
}
