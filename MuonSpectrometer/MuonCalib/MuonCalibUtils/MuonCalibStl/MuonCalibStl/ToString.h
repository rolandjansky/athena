/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOSTRING
#define TOSTRING

#include <sstream>
#include <string>

namespace MuonCalib {

/**
@class ToString
provides an operator converting a class T object into a std::string.
*/

 class ToString {
  public:
   template< class T >
    std::string operator()( const T& i )
    {
      std::ostringstream os;
      os << i;
      return os.str();
    }
 };
}

#endif
