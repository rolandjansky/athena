/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StrFormat.cxx 579482 2014-01-22 09:14:16Z krasznaa $
/**
 * @file CxxUtils/src/StrFormat.cxx
 * @author Sebastien Binet <binet@cern.ch>
 * @date Jun, 2010
 * @brief Provide helper functions to create formatted strings
 */

#include "CxxUtils/StrFormat.h"

// c includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

namespace {
  // little helper to ensure a char* is freed
  class CharLiberator
  {
    char* m_buf;

  public:
    explicit
    CharLiberator(char* buf) : m_buf(buf)
    {}
    CharLiberator() : m_buf (NULL) 
    {}
    ~CharLiberator()
    {
      free(m_buf);
    }

    CharLiberator (const CharLiberator&) = delete;
    CharLiberator& operator= (const CharLiberator&) = delete;
  };

}

namespace CxxUtils {

/** @brief return a `std::string` according to a format `fmt` and varargs
 */
std::string
strformat(const char* fmt, ...)
{
  char *buf = NULL;
  int nbytes = -1;

  va_list ap;
  va_start(ap, fmt); /* Initialize the va_list */

  nbytes = vasprintf(&buf, fmt, ap);
  va_end(ap); /* Cleanup the va_list */

  if (nbytes < 0) {
    /*buf is undefined when allocation failed
     * see: http://linux.die.net/man/3/asprintf
     */
    // free(buf); 
    throw std::runtime_error("problem while calling vasprintf");
  }

  CharLiberator guard(buf);

  // help compiler to apply RVO
  return std::string(buf);
}

} //> namespace CxxUtils

