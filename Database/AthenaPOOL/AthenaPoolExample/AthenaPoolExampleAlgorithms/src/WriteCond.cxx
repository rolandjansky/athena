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

using namespace AthPoolEx;

//___________________________________________________________________________
WriteCond::WriteCond(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) { 
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
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteCond::execute() {
   ATH_MSG_DEBUG("in execute()");

   if (evtStore()->contains<ExampleHitContainer>("MyHits")) {
      const DataHandle<ExampleHitContainer> cont;
      if (!evtStore()->retrieve(cont, "MyHits").isSuccess()) {
         ATH_MSG_ERROR("Could not find ExampleHitContainer/MyHits");
         return StatusCode::FAILURE;
      }
      DataHandle<ExampleHitContainer> ep;
      if (detStore()->retrieve(ep, m_conditionName).isFailure()) {
         ATH_MSG_ERROR("Could not find ExampleHitContainer/" << m_conditionName);
         return StatusCode::FAILURE;
      }
      ExampleHit* pEntry = *ep->begin();
      for (ExampleHitContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
         ATH_MSG_INFO("Hit x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " detector = " << (*obj)->getDetector());
         pEntry->setX(pEntry->getX() + m_offset + (*obj)->getX() * (1.0 + m_weight));
         pEntry->setY(pEntry->getY() + m_offset + (*obj)->getY() * (1.0 + m_weight));
         pEntry->setZ(pEntry->getZ() + m_offset + (*obj)->getZ() * (1.0 + m_weight));
         pEntry->setDetector(pEntry->getDetector() + ".");
      }
      pEntry->setDetector(pEntry->getDetector() + "o");
   }

   ATH_MSG_INFO("registered all data");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteCond::stop() {
   DataHandle<ExampleHitContainer> ep;
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
