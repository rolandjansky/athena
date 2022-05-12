/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolExampleAlgorithms/src/WriteData.cxx
 *  @brief This file contains the implementation for the WriteData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteData.cxx,v 1.25 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "WriteData.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "StoreGate/WriteHandle.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteData::WriteData(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}
//___________________________________________________________________________
WriteData::~WriteData() {
}
//___________________________________________________________________________
StatusCode WriteData::initialize() {
   ATH_MSG_INFO("in initialize()");
   ATH_CHECK( m_exampleHitKey.initialize() );
   ATH_CHECK( m_aliasKey.initialize() );
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteData::execute (const EventContext& ctx) const {
   ATH_MSG_DEBUG("in execute()");

   int eventNumber = ctx.eventID().event_number();
   int runNumber = ctx.eventID().run_number();
   ATH_MSG_INFO("EventInfo event: " << eventNumber << "  run: " << runNumber);

   SG::WriteHandle<ExampleHitContainer> hits (m_exampleHitKey, ctx);
   ATH_CHECK( hits.record( std::make_unique<ExampleHitContainer>() ) );
   for (int i = 0; i < 10; i++) {
      auto hitObj = std::make_unique<ExampleHit>();
      hitObj->setX(1.2346 + eventNumber * 100 + 3.21 * i);
      hitObj->setY(-2.346 + runNumber * 100 - 5.6789 * i);
      hitObj->setZ(34.567 - eventNumber * 100 + 87.6 / (i + 0.456));
      hitObj->setDetector("DummyHitDetector");
      hits->push_back(std::move (hitObj));
   }

   if (eventNumber == 14) {
      ATH_MSG_INFO("Now creating ExampleHit alias for event: " << eventNumber);
      ATH_CHECK( hits.alias (m_aliasKey) );
   }
   ATH_MSG_INFO("registered all data");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteData::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
