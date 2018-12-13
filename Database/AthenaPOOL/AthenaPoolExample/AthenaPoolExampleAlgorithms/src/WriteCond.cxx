/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file WriteCond.cxx
 *  @brief This file contains the implementation for the WriteCond class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteCond.cxx,v 1.15 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "WriteCond.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "StoreGate/ReadHandle.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteCond::WriteCond(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{ 
   declareProperty("ConditionName", m_conditionName = "PedestalWriteData");
   declareProperty("Weight", m_weight = 0.0);
   declareProperty("Offset", m_offset = 0.0);
}
//___________________________________________________________________________
WriteCond::~WriteCond() {
}
//___________________________________________________________________________
StatusCode WriteCond::initialize() {
   ATH_MSG_INFO("in initialize()");

   ExampleHitContainer* pPedestal = new ExampleHitContainer();
   ExampleHit* pEntry = new ExampleHit();
   pEntry->setDetector("<");
   pPedestal->push_back(pEntry);

   if (detStore()->record(pPedestal, m_conditionName).isFailure()) {
      ATH_MSG_ERROR("could not register Pedestal Object");
      return StatusCode::FAILURE;
   }

   ATH_CHECK( m_exampleHitKey.initialize() );
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteCond::execute (const EventContext& ctx) const {
   ATH_MSG_DEBUG("in execute()");

   SG::ReadHandle<ExampleHitContainer> hits (m_exampleHitKey, ctx);
   ExampleHitContainer* ep = nullptr;
   if (detStore()->retrieve(ep, m_conditionName).isFailure()) {
     ATH_MSG_ERROR("Could not find ExampleHitContainer/" << m_conditionName);
     return StatusCode::FAILURE;
   }
   ExampleHit* pEntry = *ep->begin();
   for (const ExampleHit* hit : *hits) {
     ATH_MSG_INFO("Hit x = " << hit->getX() << " y = " << hit->getY() << " z = " << hit->getZ() << " detector = " << hit->getDetector());
     pEntry->setX(pEntry->getX() + m_offset + hit->getX() * (1.0 + m_weight));
     pEntry->setY(pEntry->getY() + m_offset + hit->getY() * (1.0 + m_weight));
     pEntry->setZ(pEntry->getZ() + m_offset + hit->getZ() * (1.0 + m_weight));
     pEntry->setDetector(pEntry->getDetector() + ".");
   }
   pEntry->setDetector(pEntry->getDetector() + "o");

   ATH_MSG_INFO("registered all data");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteCond::stop() {
   ExampleHitContainer* ep = nullptr;
   if (detStore()->retrieve(ep, m_conditionName).isFailure()) {
      ATH_MSG_ERROR("Could not find DataObject");
      return StatusCode::FAILURE;
   }
   ExampleHit* pEntry = *ep->begin();
   pEntry->setDetector(pEntry->getDetector() + ">");
   ATH_MSG_INFO("in finalize()");
   ATH_MSG_INFO("Pedestal x = " << pEntry->getX() << " y = " << pEntry->getY() << " z = " << pEntry->getZ() << " string = " << pEntry->getDetector());
   return StatusCode::SUCCESS;
}
