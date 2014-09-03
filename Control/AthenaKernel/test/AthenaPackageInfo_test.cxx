/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPackageInfo_test.cxx
 * @author Paolo Calafiura
 * @brief Regression tests for PackageInfo (if only it worked...)
 *
 * verify that constructor parses correctly PACKAGE_VERSION macro strings
 * of the format PACKAGE-xx-yy-zz
 *
 * $Id: AthenaPackageInfo_test.cxx,v 1.5 2008-07-25 23:37:34 calaf Exp $
 */
#undef NDEBUG
#include <cassert>
#include <iostream>
#include "AthenaKernel/tools/AthenaPackageInfo.h"
using namespace std;
using Athena::PackageInfo;
int main() {
  cout << "*** AthenaPackageInfo_test OK ***" << endl;
  PackageInfo info(PACKAGE_VERSION);
  assert( info.name() == "AthenaKernel" );
  assert( (info.version().size() == 7 && info.version()[0] == 'r') || //r123456
	  info.version().size() == 8 || //00-00-00
	  info.version().size() == 11 ); //00-00-00-00
  cerr << "Now we expect to see an error message:\n"
       << "----Error Message Starts--->>" << endl; 
  try {
    PackageInfo bad("BaD");
    assert(!"BaD should never get here");
  } catch (...) {}
  cerr << "<<----Error Message Ends-----" << endl;
  cerr << "Now we expect to see an error message:\n"
       << "----Error Message Starts--->>" << endl; 
  try {
    PackageInfo bad("BaD-");
    assert(!"BaD- should never get here");
  } catch (...) {}
  cerr << "<<----Error Message Ends-----" << endl;
  cerr << "Now we expect to see an error message:\n"
       << "----Error Message Starts--->>" << endl; 
  try {
    PackageInfo bad("-BaD");
    assert(!"-BaD should never get here");
  } catch (...) {}
  cerr << "<<----Error Message Ends-----" << endl;
  cout << "*** AthenaPackageInfo_test OK ***" << endl;
  return 0;
}
