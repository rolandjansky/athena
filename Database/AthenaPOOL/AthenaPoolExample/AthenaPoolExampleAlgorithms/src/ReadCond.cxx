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

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

using namespace AthPoolEx;

//___________________________________________________________________________
ReadCond::ReadCond(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), m_pCondStore("DetectorStore", name) {
}
//___________________________________________________________________________
ReadCond::~ReadCond() {
}
//___________________________________________________________________________
StatusCode ReadCond::initialize() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the DetectorStore and initialize our local ptr
   sc = m_pCondStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pCondStore) {
      log << MSG::ERROR << "Could not find Store [" << m_pCondStore.typeAndName() << "]" << endreq;
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode ReadCond::execute() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   if (m_pCondStore->contains<ExampleHitContainer>("PedestalWriteData")) {
      const DataHandle<ExampleHitContainer> ep;
      sc = m_pCondStore->retrieve(ep, "PedestalWriteData");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find DataObject: PedestalWriteData" << endreq;
         return(StatusCode::FAILURE);
      }
      for (ExampleHitContainer::const_iterator obj = ep->begin(); obj != ep->end(); obj++) {
         log << MSG::INFO << "Pedestal x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " string = " << (*obj)->getDetector() << endreq;
      }
   }
   if (m_pCondStore->contains<ExampleHitContainer>("PedestalAppendData")) {
      const DataHandle<ExampleHitContainer> ep;
      sc = m_pCondStore->retrieve(ep, "PedestalAppendData");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find DataObject: PedestalAppendData" << endreq;
         return(StatusCode::FAILURE);
      }
      for (ExampleHitContainer::const_iterator obj = ep->begin(); obj != ep->end(); obj++) {
         log << MSG::INFO << "Pedestal (2) x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " string = " << (*obj)->getDetector() << endreq;
      }
   }

   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode ReadCond::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
