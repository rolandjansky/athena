/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationDataUtilities.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAUTILITIES_H
#define ANALYSISCALIBRATIONDATAUTILITIES_H

#include <map>
#include <string>

namespace Analysis {

  // Note that the following naming conventions are assumed here:
  //   directory:   <tagger name>/<jet author name>/<operating point>/<flavour>
  //        path:   <directory>/<calibration object name>

  // Print a summary of the fragmentation and decay settings for the specified
  // file and internal directory
  void showHadronisations(const char* filename, const char* directory);

  // Retrieve the fragmentation and decay settings for the specified
  // file and internal directory
  std::map<std::string, std::string>
    getHadronisations(const char* filename, const char* directory);

  // Print the CDI file's build number (if it was an "official" CDI file)
  void showBuildNumber(const char* filename);
}

#endif
