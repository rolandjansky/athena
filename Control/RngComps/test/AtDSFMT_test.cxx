/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AtDSFMT_test.cxx
 * @brief  unit test for AtDSFMTSvc
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: AtDSFMT_test.cxx 535391 2013-01-30 12:04:52Z binet $
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
  if (!initGaudi("AtDSFMT_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  ServiceHandle<IAtRndmGenSvc> pAtDSFMT("AtDSFMTGenSvc", "AtDSFMT_test" );
  assert(pAtDSFMT.retrieve().isSuccess());
  assert(pAtDSFMT);

  //seeds from job opts
  CLHEP::HepRandomEngine* pEng(pAtDSFMT->GetEngine("PITHIA"));
  assert(pEng);
  cout << "Initial status of PITHIA engine" << endl;
  pEng->showStatus();   //  this depends on AtDSFMT_test.txt


  //defined seeds
  pEng=pAtDSFMT->setOnDefinedSeeds(13,"PITHIA");
  cout << "Status of PITHIA engine after 1st setOnDefinedSeeds" << endl;
  pEng->showStatus();
  pEng=pAtDSFMT->setOnDefinedSeeds(13, 10000, "PITHIA");
  cout << "Status of PITHIA engine after 2nd setOnDefinedSeeds" << endl;
  pEng->showStatus();

  //snapshot engine status
  ServiceHandle<IIncidentSvc> pIISvc("IncidentSvc",  "AtDSFMT_test" );
  assert(pIISvc.retrieve().isSuccess());
  assert(pIISvc);
  pIISvc->fireIncident(Incident("EndEvent","EndEvent"));
  
  //get a random number for later comparison
  double saveRnd(pEng->flat());

  //dump status to file and to screen. Notice they are different (one extra rndm)
  cout << "Final status of PITHIA engine" << endl;
  pEng->showStatus(); //after saveRnd generated
  assert( pAtDSFMT->finalize().isSuccess() ); //write status at EndEvent

  //init new generator using first one as input (see job opts)
  ServiceHandle<IAtRndmGenSvc> pAtDSFMT2("AtDSFMTGenSvc2", "AtDSFMT_test" );
  assert(pAtDSFMT2.retrieve().isSuccess());
  assert(pAtDSFMT2);
  cout << "Initial status of PITHIA2 engine" << endl;
  pEng->showStatus();
  pEng = pAtDSFMT2->GetEngine("PITHIA");
  assert(pEng);
  double newRand(pEng->flat());
  FLOAT_EQassert( saveRnd, newRand );
  cout << "Final status of PITHIA engine for AtDSFMTGenSvc2" << endl;
  pEng->showStatus(); 
  assert( pAtDSFMT2->finalize().isSuccess() );
  
  //init third generator using first one as input (see job opts)
  ServiceHandle<IAtRndmGenSvc> pAtDSFMT3("AtDSFMTGenSvc3", "AtDSFMT_test" );
  assert(pAtDSFMT3.retrieve().isSuccess());
  assert(pAtDSFMT3);
  pEng = pAtDSFMT3->GetEngine("PITHIA");
  assert(pEng);
  double new3Rand(pEng->flat());
  FLOAT_EQassert( saveRnd, new3Rand );
  assert( pAtDSFMT3->finalize().isSuccess() );
  
  cout << "*** AtDSFMT_test OK ***" <<endl;
  return 0;
}
