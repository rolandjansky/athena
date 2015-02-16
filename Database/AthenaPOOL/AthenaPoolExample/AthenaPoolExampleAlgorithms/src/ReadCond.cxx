/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ReadCond.cxx
 *  @brief This file contains the implementation for the ReadCond class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadCond.cxx,v 1.13 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "ReadCond.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"

using namespace AthPoolEx;

//___________________________________________________________________________
ReadCond::ReadCond(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {
}
//___________________________________________________________________________
ReadCond::~ReadCond() {
}
//___________________________________________________________________________
StatusCode ReadCond::initialize() {
   ATH_MSG_INFO("in initialize()");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadCond::execute() {
   ATH_MSG_DEBUG("in execute()");

   if (detStore()->contains<ExampleHitContainer>("PedestalWriteData")) {
      const DataHandle<ExampleHitContainer> ep;
      if (detStore()->retrieve(ep, "PedestalWriteData").isFailure()) {
         ATH_MSG_ERROR("Could not find DataObject: PedestalWriteData");
         return StatusCode::FAILURE;
      }
      for (ExampleHitContainer::const_iterator obj = ep->begin(); obj != ep->end(); obj++) {
         ATH_MSG_INFO("Pedestal x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " string = " << (*obj)->getDetector());
      }
   }
   if (detStore()->contains<ExampleHitContainer>("PedestalAppendData")) {
      const DataHandle<ExampleHitContainer> ep;
      if (detStore()->retrieve(ep, "PedestalAppendData").isFailure()) {
         ATH_MSG_ERROR("Could not find DataObject: PedestalAppendData");
         return StatusCode::FAILURE;
      }
      for (ExampleHitContainer::const_iterator obj = ep->begin(); obj != ep->end(); obj++) {
         ATH_MSG_INFO("Pedestal (2) x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " string = " << (*obj)->getDetector());
      }
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadCond::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
