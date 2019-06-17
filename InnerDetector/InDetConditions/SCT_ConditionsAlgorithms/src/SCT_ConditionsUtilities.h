// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsUtilities.h
 * header file for SCT conditions utilities
 * @author shaun.roe@cern.ch
 **/
#ifndef SCT_ConditionsUtilities_h
#define SCT_ConditionsUtilities_h

#include <string>

namespace SCT_ConditionsAlgorithms {
  namespace CoveritySafe {
    ///'getenv' which avoids possibly dereferencing a null pointer
    std::string getenv(const std::string& variableName);
  }
}

#endif // SCT_ConditionsUtilities_h
