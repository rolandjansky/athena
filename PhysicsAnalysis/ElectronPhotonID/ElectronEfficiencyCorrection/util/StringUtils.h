/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __STRINGUtils__
#define __STRINGUtils__

#include "TString.h"
#include <sstream>

#include <iterator>

namespace StringUtils {


  template <typename T> static TString toTString(const T& t) { 
    std::ostringstream os; 
    os<<t; 
    return (TString)os.str(); 
  }
  template <typename T> static TString VToTString(const T& t, TString delim="," ) { 
    if (t.size()>0) { 
      std::ostringstream os; 
      std::copy(t.begin(), t.end(), std::ostream_iterator<typename T::value_type>(os,delim)); ; 
      TString val = (TString)os.str(); 
      return val.Remove(val.Length()-delim.Length()); 
    } 
    else return TString("");  
  }

  std::string TextBox   ( std::string text, int width );
  std::string NumberBox ( int number, int width );
	

}

#endif
