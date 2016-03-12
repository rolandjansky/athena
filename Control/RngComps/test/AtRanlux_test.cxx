/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AtRanlux_test.cxx
 * @brief  unit test for AtRanluxSvc
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: AtRanlux_test.cxx 729159 2016-03-11 12:58:15Z krasznaa $
 **/

#undef NDEBUG

#include <cassert>
#include <iostream>
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "StoreGate/tools/hash_functions.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

using std::cerr;
using std::cout;
using std::endl;
using namespace Athena_test;

int main() {
  ISvcLocator* pSvcLoc(0);
  if (!initGaudi("AtRanlux_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  ServiceHandle<IAtRndmGenSvc> pAtRanlux("AtRanluxGenSvc", "AtRanlux_test" );
  assert(pAtRanlux.retrieve().isSuccess());
  assert(pAtRanlux);

  //seeds from job opts
  CLHEP::HepRandomEngine* pEng(pAtRanlux->GetEngine("PITHIA"));
  assert(pEng);
  cout << "Initial status of PITHIA engine" << endl;
  pEng->showStatus();   //  this depends on AtRanlux_test.txt


  //defined seeds
  pEng=pAtRanlux->setOnDefinedSeeds(13,"PITHIA");
  cout << "Status of PITHIA engine after 1st setOnDefinedSeeds" << endl;
  pEng->showStatus();
  pEng=pAtRanlux->setOnDefinedSeeds(13, 10000, "PITHIA");
  cout << "Status of PITHIA engine after 2nd setOnDefinedSeeds" << endl;
  pEng->showStatus();

  //snapshot engine status
  ServiceHandle<IIncidentSvc> pIISvc("IncidentSvc",  "AtRanlux_test" );
  assert(pIISvc.retrieve().isSuccess());
  assert(pIISvc);
  pIISvc->fireIncident(Incident("EndEvent","EndEvent"));
  
  //get a random number for later comparison
  double saveRnd(pEng->flat());

  //dump status to file and to screen
  cout << "Final status of PITHIA engine" << endl;
  pEng->showStatus();
  assert( pAtRanlux->finalize().isSuccess() );

  //init new generator using first one as input (see job opts)
  ServiceHandle<IAtRndmGenSvc> pAtRanlux2("AtRanluxGenSvc2", "AtRanlux_test" );
  assert(pAtRanlux2.retrieve().isSuccess());
  assert(pAtRanlux2);
  pEng = pAtRanlux2->GetEngine("PITHIA");
  assert(pEng);
  double newRand(pEng->flat());
  FLOAT_EQassert( saveRnd, newRand );
  cout << "Final status of PITHIA engine for AtRanluxGenSvc2" << endl;
  pEng->showStatus(); 
  assert( pAtRanlux2->finalize().isSuccess() );
  
  //init third generator using first one as input (see job opts)
  ServiceHandle<IAtRndmGenSvc> pAtRanlux3("AtRanluxGenSvc3", "AtRanlux_test" );
  assert(pAtRanlux3.retrieve().isSuccess());
  assert(pAtRanlux3);
  pEng = pAtRanlux3->GetEngine("PITHIA");
  assert(pEng);
  double new3Rand(pEng->flat());
  FLOAT_EQassert( saveRnd, new3Rand );
  assert( pAtRanlux3->finalize().isSuccess() );
  
  cout << "*** AtRanlux_test OK ***" <<endl;
  return 0;
}
