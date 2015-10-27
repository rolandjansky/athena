/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// File     : checkCalibrationFile.C
// Author   : Frank Filthaut
// Purpose  : inspect the calibration ROOT file (new format using CalibrationDataContainers),
//            listing all objects that the code knows about.

// Either load the library before running ".x checkCalibrationFile.C+" (i.e. compiled),
// then you'll need the include. Or run ".x checkCalibrationFile.C" and call
// gSystem->Load("libCalibrationDataInterface.so"); in here - but then the #include
// is in the way: CINT *replaces* elements from the dictionary when loading
// the #include :-(

// One easy way to run this macro is within the RootCore build environment:
// root -l $ROOTCOREDIR/scripts/load_packages.C+
//   .L checkCalibrationFile.C+
//   checkCalibrationFile()

#include <iostream>
#include <vector>
#include <utility>
#include <string.h>
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TMatrixDSym.h"
#include "TDirectory.h"
#include "TCollection.h"
#include "TMap.h"
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
using Analysis::CalibrationDataHistogramContainer;
using Analysis::CalibrationDataMappedHistogramContainer;

bool isMatched(TString& path, TString& search);

void checkDirectory(int lvl, TString& search);

void checkCalibrationFile(TString fileName = "TopCalibrations_rel17_MC11a.root",
			  TString search = "", int lvl = 0) {

#ifdef __CINT__
  gSystem->Load("libCalibrationDataInterface.so");
#endif

  TFile* f = TFile::Open(fileName.Data(), "READ");
  f->cd();
  checkDirectory(lvl, search);
  f->Close();
}

bool isMatched(TString& path, TString& search) {
  return (search == "" || path.Contains(search));
}

void checkDirectory(int lvl, TString& search) {

  // TString path( (char*)strstr( dir->GetPath(), ":" ) );
  // path.Remove( 0, 2 );
  // dir->cd();
  TDirectory *current_dir = gDirectory;
  
  // loop over all keys in this directory

  TIter nextkey( current_dir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {

    //keep only the highest cycle number for each key
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

    // read object from first source file
    TObject *obj = key->ReadObj();
    TString path(current_dir->GetPath()); path += "/"; path += key->GetName();

    if (obj->IsA()->InheritsFrom(TDirectory::Class())) {
      // it's a subdirectory
      if (lvl > 3) cout << "Found subdirectory " << obj->GetName() << endl;
      current_dir->cd(obj->GetName());
      checkDirectory(lvl, search);
    }
    else if (obj->IsA()->InheritsFrom(CalibrationDataContainer::Class())) {

      if (obj->IsA()->InheritsFrom(CalibrationDataFunctionContainer::Class())) {
	if (isMatched(path, search))
	  cout << "found CalibrationDataFunctionContainer object "
	       << current_dir->GetPath() << "/" << key->GetName() << endl;
      }
      else if (obj->IsA()->InheritsFrom(CalibrationDataMappedHistogramContainer::Class())) {
	if (isMatched(path, search))
	  cout << "found CalibrationDataMappedHistogramContainer object "
	       << current_dir->GetPath() << "/" << key->GetName() << endl;
      }
      else if (obj->IsA()->InheritsFrom(CalibrationDataHistogramContainer::Class())) {
	if (isMatched(path, search))
	  cout << "found CalibrationDataHistogramContainer object "
	       << current_dir->GetPath() << "/" << key->GetName() << endl;
      }
      else {
	cout << "\tUNKNOWN container type for object " << current_dir->GetPath() << "/" << key->GetName() << "!" << endl;
	continue;
      }
      CalibrationDataContainer* cnt = dynamic_cast<CalibrationDataContainer*>(obj);
      if (lvl > 0) {
	std::string onoff = cnt->isRangeRestricted() ? "on " : "off";
	cout << "\t(1)range restriction switched " << onoff << endl;
	cout << "\t(1)validity bounds: ";
	std::vector<std::pair<double, double> > bounds = cnt->getBounds();
	for (unsigned int t = 0; t < bounds.size(); ++t)
	  cout << " " << t << "(" << bounds[t].first << "," << bounds[t].second << ")";
	cout << endl;
      }
    }
    else if (obj->IsA()->InheritsFrom(TMap::Class())) {
      if (isMatched(path, search))
	cout << "found TMap object "
	     << current_dir->GetPath() << "/" << key->GetName() << endl;
    }

  } // while ( ( TKey *key = (TKey*)nextkey() ) )
}
