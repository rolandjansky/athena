/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  // Run this macro from the CalibrationDataInterfance's parent directory:
  //     root -l CalibrationDataInterface/scripts/makeHtml.C
  // The documentation goes per default into "$PWD/htmldoc/"

  // need to be loaded since your package depends on it and for some reason it is not loaded per default...
  gSystem->Load("$ROOTSYS/lib/libHist.so");
  gSystem->Load("$ROOTSYS/lib/libMatrix.so");

  // put in your path to the package library and the header files
  gSystem->Load("./CalibrationDataInterface/StandAlone/libCalibrationDataInterface.so");
  gSystem->AddIncludePath(" -I./CalibrationDataInterface/CalibrationDataInterface ");
  gInterpreter->AddIncludePath("./CalibrationDataInterface/CalibrationDataInterface");

  THtml html;
  //html.SetInputDir("$ROOTSYS/include/:../CalibrationDataInterface/"); // if you want to have all root classes in your documentation, use this line
  html.SetInputDir("./CalibrationDataInterface/");
  html.SetProductName("Your name here:");
  html.MakeAll(kFALSE, "*");
}
