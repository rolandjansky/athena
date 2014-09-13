/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsUtilities.cxx
 * implementation file for SCT conditions utilities
 * @author shaun.roe@cern.ch
 **/

#include "SCT_ConditionsUtilities.h"
#include <cstdlib>

namespace SCT_ConditionsServices{
  namespace CoveritySafe{
    ///get an environment variable
    std::string getenv(const std::string & variableName){
      std::string result;
      const char * pChar=std::getenv(variableName.c_str());
      if (pChar) result=pChar;
      return result;
    }
  }
}

