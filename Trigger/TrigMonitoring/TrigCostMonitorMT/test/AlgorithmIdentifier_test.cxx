/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"

#include "TrigCostMonitorMT/AlgorithmIdentifier.h"

/// @brief Unit test for AlgorithmIdentifier class
///
int main ATLAS_NOT_THREAD_SAFE () {

  errorcheck::ReportMessage::hideFunctionNames (true);

  // initialize Gaudi, SG
  ISvcLocator* pSvcLoc;
  Athena_test::initGaudi(pSvcLoc); 
  StoreGateSvc* pSG(nullptr);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  // Create a context
  IProxyDict* xdict = &*pSG;
  xdict = pSG->hiveProxyDict();
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);

  // Create a log
  MsgStream log(nullptr, "AlgorithmIdentifier");
  log.setLevel( MSG::DEBUG );

  // Test slot override
  AlgorithmIdentifier ai = AlgorithmIdentifierMaker::make(Gaudi::Hive::currentContext(), "ALG_A", log, 10);

  ai.dump();

  // Test hash collision. These two strings are known to collide.
  AlgorithmIdentifier collision_a = AlgorithmIdentifierMaker::make(Gaudi::Hive::currentContext(), "APP_HLT:HLTMPPU-36:HLT-36:tpu-rack-73:pc-tdq-tpu-73012-30", log);
  AlgorithmIdentifier collision_b = AlgorithmIdentifierMaker::make(Gaudi::Hive::currentContext(), "APP_HLT:HLTMPPU-36:HLT-36:tpu-rack-75:pc-tdq-tpu-75033-11", log);

  collision_a.dump();
  collision_b.dump();
}
