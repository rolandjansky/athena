// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StrFormat.h 599890 2014-06-02 13:52:58Z ssnyder $
/**
 * @file CxxUtils/StrFormat.h
 * @author Sebastien Binet <binet@cern.ch>
 * @date Jun 2010
 * @brief Provide helper functions to create formatted strings
 */


#ifndef CXXUTILS_STRFORMAT_H
#define CXXUTILS_STRFORMAT_H 1

// stl includes
#include <string>

namespace CxxUtils {

  /** @brief return a `std::string` according to a format `fmt` and varargs
   */
  std::string strformat(const char* fmt, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    ;

} //> namespace CxxUtils

#endif // not CXXUTILS_STRFORMAT_H
