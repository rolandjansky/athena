/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SimplePOSIXTimeKeeperSvc_test.cxx
 * @brief  unit test for ITimeKeeper simplest implementation
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: SimplePOSIXTimeKeeperSvc_test.cxx,v 1.4 2007-02-15 06:13:52 binet Exp $
 **/

#include <cassert>
#include <cmath>  /* sqrt */
#include <iostream>
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/ITimeKeeper.h"
#include "SimplePOSIXTimeKeeperSvc.h"

using std::cerr;
using std::cout;
using std::endl;
using namespace Athena_test;

int main() {
  const std::string appName = "SimplePOSIXTimeKeeperSvc_test";
  cout << "*** " << appName << " starts ***" <<endl;
  ISvcLocator* pSvcLoc(0);
  if (!initGaudi("SimplePOSIXTimeKeeperSvc_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  //locate time keeper
  ServiceHandle<ITimeKeeper> pITK( "SimplePOSIXTimeKeeperSvc", appName );
  assert( pITK.retrieve().isSuccess() );
  assert( pITK );
  //  cout <<  *pITK <<endl;

  SimplePOSIXTimeKeeperSvc& tk(dynamic_cast<SimplePOSIXTimeKeeperSvc&>(*pITK));
  assert(tk.timeX() + tk.timeL() == tk.allocTime());

  double x = 0;
  while (pITK->nextIter()) {
    for (int i=0; i<20000000; ++i) x += atan(i); //waste some time
  }

  assert(tk.timeX() + tk.timeL() == tk.allocTime());

  tk.finalize();

  //all done
  cout << "*** " << appName << " OK ***" <<endl;
  return 0;
}
