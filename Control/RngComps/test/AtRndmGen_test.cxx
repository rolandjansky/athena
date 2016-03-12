/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AtRndmGen_test.cxx
 * @brief  unit test for AtRndGenSvc
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: AtRndmGen_test.cxx 729159 2016-03-11 12:58:15Z krasznaa $
 **/

#undef NDEBUG

#include <cassert>
#include <iostream>
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "StoreGate/tools/hash_functions.h"
#include "CLHEP/Random/RandomEngine.h"

using std::cerr;
using std::cout;
using std::endl;
using namespace Athena_test;

int main() {
  ISvcLocator* pSvcLoc(0);
  if (!initGaudi("AtRndmGen_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  ServiceHandle<IAtRndmGenSvc> pAtRndmGen("AtRndmGenSvc", "AtRndmGen_test" );
  assert(pAtRndmGen.retrieve().isSuccess());
  assert(pAtRndmGen);
  //  this depends on AtRndmGen_test.txt
  CLHEP::HepRandomEngine* pEng(pAtRndmGen->GetEngine("PITHIA_INIT"));
  assert(pEng);
  cout << "Initial status of PITHIA engine" << endl;
  pEng->showStatus();   //  this depends on AtRndmGen_test.txt


  //defined seeds
  pEng=pAtRndmGen->setOnDefinedSeeds(13,"PITHIA");
  cout << "Status of PITHIA engine after 1st setOnDefinedSeeds" << endl;
  pEng->showStatus();
  pEng=pAtRndmGen->setOnDefinedSeeds(13, 10000, "PITHIA");
  cout << "Status of PITHIA engine after 2nd setOnDefinedSeeds" << endl;
  pEng->showStatus();

  //snapshot engine status
  ServiceHandle<IIncidentSvc> pIISvc("IncidentSvc",  "AtRndmGen_test" );
  assert(pIISvc.retrieve().isSuccess());
  assert(pIISvc);
  pIISvc->fireIncident(Incident("EndEvent","EndEvent"));
  
  //get a random number for later comparison
  double saveRnd(pEng->flat());

  //dump status to file and to screen
  cout << "Final status of PITHIA engine" << endl;
  pEng->showStatus();
  assert( pAtRndmGen->finalize().isSuccess() );

  //init new generator using first one as input (see job opts)
  ServiceHandle<IAtRndmGenSvc> pAtRndmGen2("AtRndmGenSvc2", "AtRndmGen_test" );
  assert(pAtRndmGen2.retrieve().isSuccess());
  assert(pAtRndmGen2);
  pEng = pAtRndmGen2->GetEngine("PITHIA");
  assert(pEng);
  double newRand(pEng->flat());
  FLOAT_EQassert(saveRnd, newRand);
  cout << "Final status of PITHIA engine for AtRndmGenSvc2" << endl;
  pEng->showStatus(); 
  assert( pAtRndmGen2->finalize().isSuccess() );
  
  //init third generator using first one as input (see job opts)
  ServiceHandle<IAtRndmGenSvc> pAtRndmGen3("AtRndmGenSvc3", "AtRndmGen_test" );
  assert(pAtRndmGen3.retrieve().isSuccess());
  assert(pAtRndmGen3);
  pEng = pAtRndmGen3->GetEngine("PITHIA");
  assert(pEng);
  double new3Rand(pEng->flat());
  FLOAT_EQassert(saveRnd, new3Rand);
  assert( pAtRndmGen3->finalize().isSuccess() );
  
  cout << "*** AtRndmGen_test OK ***" <<endl;
  return 0;
}
