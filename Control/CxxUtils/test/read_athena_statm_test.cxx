/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file read_athena_statm_test.cxx
 * @brief  unit test for read_athena_statm 
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: read_athena_statm_test.cxx 592256 2014-04-10 00:20:45Z calaf $
 **/

#undef NDEBUG

#include <cassert>
#include <iostream>

#include "CxxUtils/read_athena_statm.h"

using std::cerr;
using std::cout;
using std::endl;



int main() {
  const std::string appName = "read_statm_test";
  cout << "*** " << appName << " starts ***" <<endl;

  athena_statm s = read_athena_statm();
  cout << "read_athena_statm reports process size (in pages): VM " << s.vm_pages 
       << " RSS " << s.rss_pages << endl;
  //let's do something...
  assert(s.vm_pages >= s.rss_pages);
  //all done
  cout << "*** " << appName << " OK ***" <<endl;
  return 0;
}
