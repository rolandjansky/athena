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

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteCond::WriteCond(const std::string& name, ISvcLocator* pSvcLocator) : 
   Algorithm(name, pSvcLocator), 
   m_pEventStore("StoreGateSvc", name), 
   m_pCondStore("DetectorStore", name) {
   declareProperty("ConditionName", m_conditionName = "PedestalWriteData");
   declareProperty("Store",  m_pCondStore);
   declareProperty("Weight", m_weight = 0.0);
   declareProperty("Offset", m_offset = 0.0);
}
//___________________________________________________________________________
WriteCond::~WriteCond() {
}
//___________________________________________________________________________
StatusCode WriteCond::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = m_pEventStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pEventStore) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   }

   // locate the conditions' Store and initialize our local ptr
   sc = m_pCondStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pCondStore) {
      log << MSG::ERROR << "Could not find Store [" << m_pCondStore.typeAndName() << "]" << endreq;
      return(StatusCode::FAILURE);
   }
   ExampleHitContainer* pPedestal = new ExampleHitContainer();
   ExampleHit* pEntry = new ExampleHit();
   pEntry->setDetector("<");
   pPedestal->push_back(pEntry);

   sc = m_pCondStore->record(pPedestal, m_conditionName);
   if (sc.isFailure()) {
      log << MSG::ERROR << "could not register Pedestal Object" << endreq;
      return(StatusCode::FAILURE);
   }
   return(sc);
}
//___________________________________________________________________________
StatusCode WriteCond::execute() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   if (m_pEventStore->contains<ExampleHitContainer>("MyHits")) {
      const DataHandle<ExampleHitContainer> cont;
      StatusCode sc = m_pEventStore->retrieve(cont, "MyHits");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find ExampleHitContainer/MyHits" << endreq;
         return(StatusCode::FAILURE);
      }
      DataHandle<ExampleHitContainer> ep;
      sc = m_pCondStore->retrieve(ep, m_conditionName);
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find ExampleHitContainer/" << m_conditionName << endreq;
         return(StatusCode::FAILURE);
      }
      ExampleHit* pEntry = *ep->begin();
      for (ExampleHitContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
         log << MSG::INFO << "Hit x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " detector = " << (*obj)->getDetector() << endreq;
         pEntry->setX(pEntry->getX() + m_offset + (*obj)->getX() * (1.0 + m_weight));
         pEntry->setY(pEntry->getY() + m_offset + (*obj)->getY() * (1.0 + m_weight));
         pEntry->setZ(pEntry->getZ() + m_offset + (*obj)->getZ() * (1.0 + m_weight));
         pEntry->setDetector(pEntry->getDetector() + ".");
      }
      pEntry->setDetector(pEntry->getDetector() + "o");
   }

   log << MSG::INFO << "registered all data" << endreq;
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode WriteCond::stop() {
   MsgStream log(msgSvc(), name());
   DataHandle<ExampleHitContainer> ep;
   StatusCode sc = m_pCondStore->retrieve(ep, m_conditionName);
   if (!sc.isSuccess()) {
      log << MSG::ERROR << "Could not find DataObject" << endreq;
      return(StatusCode::FAILURE);
   }
   ExampleHit* pEntry = *ep->begin();
   pEntry->setDetector(pEntry->getDetector() + ">");
   log << MSG::INFO << "in finalize()" << endreq;
   log << MSG::INFO << "Pedestal x = " << pEntry->getX() << " y = " << pEntry->getY() << " z = " << pEntry->getZ() << " string = " << pEntry->getDetector() << endreq;
   return(StatusCode::SUCCESS);
}
