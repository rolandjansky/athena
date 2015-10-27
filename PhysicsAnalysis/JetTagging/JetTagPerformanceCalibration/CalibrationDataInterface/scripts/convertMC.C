/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// File     : convertMC.C
// Author   : Frank Filthaut
// Purpose  : take old-format MC b-tagging efficiency parametrisations and write them as CalibrationDataFunctionContainer objects

// Either load the library before running ".x convertMC.C+" (i.e. compiled),
// then you'll need the include. Or run ".x convertMC.C" and call
// gSystem->Load("libCalibrationDataInterface.so"); in here - but then the #include
// is in the way: CINT *replaces* elements from the dictionary when loading
// the #include :-(

// The easiest way to run this macro is within the RootCore build environment:
// root -l $ROOTCOREDIR/scripts/load_packages.C+
//   .L convertMC.C+
//   convertMC()

#include <iostream>
#include <string.h>
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TMatrixDSym.h"
#include "TDirectory.h"
#include "TCollection.h"
#include "TKey.h"
#include "TString.h"
#include "TClass.h"

using std::cout;
using std::endl;

#ifndef __CINT__
#  include "CalibrationDataInterface/CalibrationDataContainer.h"
#endif

using Analysis::CalibrationDataContainer;
using Analysis::CalibrationDataFunctionContainer;

void convertDirectory(TDirectory* dir, TFile* in);

void convertMC(TString InFileName = "TopCalibrations_rel17_MC11a_Eff.root",
	       TString OutFileName = "TopCalibrations_rel17_MC11a.root") {

#ifdef __CINT__
  gSystem->Load("libCalibrationDataInterface.so");
#endif

  TFile* fin = TFile::Open(InFileName.Data(), "READ");
  TFile* fout = TFile::Open(OutFileName.Data(), "NEW");

  convertDirectory(fout, fin);

  fin->Close();
  fout->Close();
}


void convertDirectory(TDirectory *dir, TFile* in) {

  TString path( (char*)strstr( dir->GetPath(), ":" ) );
  path.Remove( 0, 2 );
  in->cd(path);
  TDirectory *current_dir = gDirectory;
  
  // loop over all keys in this directory

  TIter nextkey( current_dir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {

    //keep only the highest cycle number for each key
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

    // read object from first source file
    in->cd(path);
    TObject *obj = key->ReadObj();

    if (obj->IsA()->InheritsFrom(TDirectory::Class())) {
      // it's a subdirectory

      cout << "Found subdirectory " << obj->GetName() << endl;

      // create a new subdir of same name and title in the target file
      dir->cd();
      TDirectory *newdir = dir->mkdir( obj->GetName(), obj->GetTitle() );

      // newdir is now the starting point of another round of merging
      // newdir still knows its depth within the target file via
      // GetPath(), so we can still figure out where we are in the recursion
      convertDirectory(newdir, in);
    }
    else if (! strcmp(key->GetName(), "top_Eff")) {
    // else if (! strcmp(obj->GetName(), "top_Eff")) {
      // Found the expected scale factor parametrisation name.
      // Also retrieve the corresponding systematic and statistical uncertainties

      cout << "found top_Eff object" << endl;

      TF1* f = (TF1*)obj;
      TF1* fSyst; gDirectory->GetObject("top_Eff_syst", fSyst);
      TMatrixDSym* fStat; gDirectory->GetObject("top_Eff_stat", fStat);

      // create the Container object
      CalibrationDataFunctionContainer* cnt = new CalibrationDataFunctionContainer("top_Eff");
      cnt->setResult(f);
      cnt->setUncertainty("statistics", fStat);
      cnt->setComment("efficiency parametrisation to ttbar MC");
      cnt->setLowerBound(CalibrationDataContainer::kPt, 20.0);
      cnt->setUpperBound(CalibrationDataContainer::kPt, 200.0);
      cnt->setLowerBound(CalibrationDataContainer::kAbsEta, 0.0);
      cnt->setUpperBound(CalibrationDataContainer::kAbsEta, 2.5);
      cnt->restrictToRange(true);

      // and write it to the output file
      dir->cd();
      gDirectory->WriteObject(cnt, "top_Eff");
      // and write it also as the "default" calibration
      gDirectory->WriteObject(cnt, "default_Eff");

      // cnt->Write("top_Eff");
      // dir->SaveSelf(kTRUE);

      // and clean up again...
      delete cnt;
    }

  } // while ( ( TKey *key = (TKey*)nextkey() ) )
}
