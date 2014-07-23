/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StrFormat_test.cxx 599890 2014-06-02 13:52:58Z ssnyder $
/**
 * @file CxxUtils/test/StrFormat_test.cxx
 * @author Sebastien Binet
 * @date Jun 2010
 * @brief Regression tests for StrFormat
 */


#undef NDEBUG


#include "CxxUtils/StrFormat.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

using namespace std;
using namespace CxxUtils;

#define MYASSERT(chk, expected)                     \
  do {                                              \
  if (chk != expected) {                            \
    std::cerr << "**error**:" << __LINE__ << ": "   \
            << "chk != expected\n"                  \
            << "** chk: [" << chk << "]\n"          \
            << "** exp: [" << expected << "]\n";    \
    assert(false);                                  \
  }                                                 \
  else if (0) {                                     \
    std::cout << "test ok (line:" << __LINE__       \
              << ") str = [" << chk << "]\n";       \
  }                                                 \
} while (0)

void test1()
{
  {
    std::string s = strformat("%%");
    MYASSERT(s, "%");
  }
  {
    std::string s = strformat("%d", 1);
    MYASSERT(s, "1");
  }
  {
    std::string s = strformat("%i", 1);
    MYASSERT(s, "1");
  }
  {
    std::string s = strformat("%u", 0);
    MYASSERT(s, "0");
  }
  {
    std::string s = strformat("%0.1f", 1.);
    MYASSERT(s, "1.0");
  }
  {
    std::string s = strformat("%0.1f", 1.);
    MYASSERT(s, "1.0");
  }
  {
    std::string s = strformat("%8.3f", 1.);
    MYASSERT(s, "   1.000");
  }
  {
    std::string s = strformat("%8.3e", 1.);
    MYASSERT(s, "1.000e+00");
  }
  {
    std::string s = strformat("%8.3E", 1.);
    MYASSERT(s, "1.000E+00");
  }
  {
    std::string tt = "foo";
    std::string s = strformat("%s", tt.c_str());
    MYASSERT(s, "foo");
    MYASSERT(s, tt);
  }
  {
    std::string s = strformat("%s", "foo");
    MYASSERT(s, "foo");
  }
}

int main()
{
  //std::cout << "=== strformat_test ===\n";
  test1();
  //std::cout << "=== strformat_test === [done]" << std::endl;
  return 0;
}
