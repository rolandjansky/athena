/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file CLIDRegistry_test.cxx 
 *  @brief unit test for CLIDRegistry
 *
 * $Id: CLIDRegistry_test.cxx,v 1.2 2009-01-19 17:02:55 binet Exp $
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 */

/* #define COMPILEFAIL 1 */

#undef NDEBUG

#include "SGTools/CLIDRegistry.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

#include <boost/tuple/tuple.hpp>
using boost::get;
#include "AthenaKernel/tools/AthenaPackageInfo.h"

int main () {
  cerr << "*** CLIDRegistry_test starts ***" <<endl;
  //reset newEntries before we start counting
  std::pair<CLIDRegistry::const_iterator, CLIDRegistry::const_iterator>  er =
    CLIDRegistry::newEntries();
  Athena::PackageInfo info(PACKAGE_VERSION);
#ifdef COMPILEFAIL
  CLIDRegistry::addEntry<12>("Foo", info, "Foo");
  //no upper limit  CLIDRegistry::addEntry<1245736740>("Foo", info);
#endif
  CLIDRegistry::addEntry<1234>("Foo", info, "Foo");
  CLIDRegistry::addEntry<4321>("Bar", info, "Bar");
  assert(CLIDRegistry::hasNewEntries());
  er = CLIDRegistry::newEntries();
  assert( distance(er.first, er.second) == 2 );
  assert( get<0>(*(CLIDRegistry::end()-2)) == 1234 );
  assert( get<1>(*(CLIDRegistry::end()-2)) == "Foo" );
  assert( get<2>(*(CLIDRegistry::end()-2)).name() == "SGTools" );
  CLIDRegistry::addEntry<43213>("Baz", info, "Baz");
  er = CLIDRegistry::newEntries();
  assert( distance(er.first, er.second) == 1 );
  assert( !CLIDRegistry::hasNewEntries());
  cerr << "*** CLIDRegistry_test OK ***" <<endl;
  return 0;
}
