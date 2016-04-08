/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscValidationHelper.h

#ifndef CscValidationHelper_H
#define CscValidationHelper_H

// David Adams
// April 2007
//
// Utilities for CSC validation.

#include <string>

class CscValidationHelper {

public:  // static methods

  // Return a string description of a chamber (wlay=0) or layer.
  static std::string csc_name(int istation, int zsec, int phisec, int wlay=0, bool measphi=false);

};

#endif
