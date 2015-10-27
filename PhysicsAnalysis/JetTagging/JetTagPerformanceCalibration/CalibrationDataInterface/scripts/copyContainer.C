/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Either load the library before running ".x copyContainer.C+" (i.e. compiled),
// then you'll need the include. Or run ".x copyContainer.C" and call
// gSystem->Load("libCalibrationDataInterface.so"); in here - but then the #include
// is in the way: CINT *replaces* elements from the dictionary when loading
// the #include :-(
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
#  include "CalibrationDataInterface/CalibrationDataContainer.h"
#endif

#include "CalibrationDataInterface/CalibrationDataBackend.h"
#include "TFile.h"
#include "TSystem.h"
#include "TString.h"
#include <iostream>

using Analysis::CalibrationDataContainer;

void copyContainer()
{
  // You seem to prefer histograms to behave like all other objects when it
  // comes to object lifetime, i.e. you want to delete them instead of TFile
  // deleting them:
  //  TH1::AddDirectory(kFALSE);

#ifdef __CINT__
  gSystem->Load("libCalibrationDataInterface.so");
#endif

  // example: open the 2013 LP file
  TFile* fin = TFile::Open("2013-LP-MC12.root");

  // retrieve two of its containers
  CalibrationDataContainer* cb;
  fin->GetObject("MV1/AntiKt4TopoEMJVF0_5/0_8119/B/default_SF", cb);
  CalibrationDataContainer* cl;
  fin->GetObject("MV1/AntiKt4TopoEMJVF0_5/0_8119/Light/negative tags_SF", cl);
  
  // create a new ROOT file
  TString fname("containerFile.root");
  TFile* f = TFile::Open(fname, "RECREATE");
  if (f->IsZombie()) {
    std::cout << "error: cannot create file " << fname << std::endl;
    delete f;
    return;
  }

  // The utility below for writing the calibration objects to file uses the individual
  // tagger, jet author, operating point, and flavour components rather than a full path
  // name. This is to help ensure that no ordering mistakes are made.

  // Write the b-jet container with its own name
  addContainer(cb, f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "B");
  // Write the light-jet container with a different name (the CDI code normally expects a "default" object)
  addContainer(cl, f, "MV1", "AntiKt4TopoEMJVF0_5", "0_8119", "Light", "default_SF");
  f->ls();
  // close the file, and delete all objects
  f->Close();
  delete cb;
  delete cl;
  delete f;
}
