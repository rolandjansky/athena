/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CablingUtilities_H
#define SCT_CablingUtilities_H
/**
 *  @file SCT_CablingUtilities.h
 *  Header file for utility functions used in SCT cabling code
 *
 *  @author Shaun Roe 
 *  @date 14/11/2008.
 *
 */

#include <sstream>
#include <string>

namespace SCT_Cabling{
  /// @name Utility functions for this unit
  //@{
  
  ///Convert a string (decimal or hex) to an int; -1 indicates an error
  int stringToInt(const std::string &hexOrDecString);
  
  ///Check range and give error if out of range
  bool inRange(const int value, const int lowerBound, const int upperBound, const std::string & valueName="");
  
  ///calculate link, normal and swapped (from Kondo)
  int calculateLink(const int MURorder, const int ModID, const int theSide,const bool isSwapped = false);
  //@}
  //End of utility functions
  namespace CoveritySafe{
    /// @name Equivalent functions which avoid coverity check warnings
    //@{
    ///'getenv' which avoids possibly dereferencing a null pointer
    std::string getenv(const std::string & variableName);
      
    //@}
     
  }
}
#endif

