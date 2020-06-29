/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataUtilities                                                                 //
//                                                                                          //
// This file collects a few utility functions allowing access to information stored in a    //
// CalibrationDataInterface ROOT file.                                                      //
//                                                                                          //
// CalibrationDataUtilities.cxx, (c) ATLAS Detector software                                //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataUtilities.h"
#include "TMap.h"
#include "TFile.h"
#include "TObjString.h"

#include <iostream>
#include <iomanip>
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::setfill;

void
Analysis::showHadronisations(const char* filename, const char* directory)
{
  // Print a summary of the fragmentation and decay settings for the specified
  // file and internal directory

  std::map<string, string> refs = getHadronisations(filename, directory);
  if (refs.size() == 0) {
    cerr << "unable to retrieve hadronisation references for directory " << directory << endl;
    return;
  }

  cout << setw(27) << "hadronisation specification" << " reference calibration" << endl;
  cout << setfill('-') << setw(60) << "-" << endl;
  cout << setfill(' ');
  for (std::map<string, string>::iterator it = refs.begin(); it != refs.end(); ++it)
    cout << setw(27) << it->first << " " << it->second << endl;
}

std::map<string, string>
Analysis::getHadronisations(const char* filename, const char* directory)
{
  // Retrieve the fragmentation and decay settings for the specified
  // file and internal directory. An empty map will be returned in case
  // of error.

  std::map<string, string> final;

  TFile* file = TFile::Open(filename, "READ");
  if (file->IsZombie()) return final;

  string refname(directory); refname += "/MChadronisation_ref";
  TMap* original;
  file->GetObject(refname.c_str(), original);
  if (!original) return final;

  TMapIter next(original); TObjString* spec;
  while ((spec = (TObjString*) next())) {
    TObjString* ref = (TObjString*) original->GetValue(spec);
    final[string(spec->GetName())] = string(ref->GetName());
  }
  return final;
}

void
Analysis::showBuildNumber(const char* filename)
{
  // Print the CDI file's build number (if it was an "official" CDI file)

  TFile* file = TFile::Open(filename, "READ");
  if (file->IsZombie()) {
    cout << "Error opening file " << filename << endl;
    return;
  }

  TObjString* s;
  file->GetObject("VersionInfo/BuildNumber", s);
  if (!s) {
    cout << "No build number found in file " << filename << endl;
    return;
  }
  cout << "File " << filename << " has build number " << s->GetName() << endl;
}
