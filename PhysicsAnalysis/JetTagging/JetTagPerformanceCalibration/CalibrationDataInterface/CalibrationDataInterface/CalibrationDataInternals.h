/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// CalibrationDataInternals.h, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAINTERNALS_H
#define ANALYSISCALIBRATIONDATAINTERNALS_H

#include <vector>
#include <string>

namespace Analysis {

  // It's somewhat cumbersome to have a nested namespace here... this is because a mere Analysis::split() could clash with other code
  namespace CalibrationDataInterface {

    /** local utility function: split string into a vector of substrings separated by a specified separator, and with whitespace in the results trimmed */
    std::vector<std::string> split(const std::string& str, const char token = ';');

  }
}

#endif
