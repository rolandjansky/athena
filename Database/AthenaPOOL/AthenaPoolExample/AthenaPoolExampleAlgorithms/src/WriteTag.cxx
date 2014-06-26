/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file WriteTag.cxx
 *  @brief This file contains the implementation for the WriteTag class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteTag.cxx,v 1.4 2009-03-17 09:44:46 gemmeren Exp $
 **/

#include "WriteTag.h"

// the user data-class defintions
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"

#include "GaudiKernel/MsgStream.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteTag::WriteTag(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), p_SGevent("StoreGateSvc", name), m_attribListSpec(0) {
   declareProperty("Key",  m_key = "RunEventTag");
   declareProperty("Magic", m_magic = 0);
}
//___________________________________________________________________________
WriteTag::~WriteTag() {
   if (m_attribListSpec != 0) {
      m_attribListSpec->release();
   }
}
//___________________________________________________________________________
StatusCode WriteTag::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = p_SGevent.retrieve();
   if (!sc.isSuccess() || 0 == p_SGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   }
   log << MSG::DEBUG << "Defining the attribute list specification." << endreq;
   m_attribListSpec = new AthenaAttributeListSpecification();
   m_attribListSpec->extend("RunNumber", "unsigned int");
   m_attribListSpec->extend("EventNumber", "unsigned int");
   if (m_magic > 0) {
      m_attribListSpec->extend("MagicNumber", "unsigned int");
   }
   log << MSG::DEBUG << "Printing out attribute list specification:" << endreq;
   for (AthenaAttributeListSpecification::const_iterator first = m_attribListSpec->begin(), last = m_attribListSpec->end(); first != last; ++first) {
      log << MSG::DEBUG << " name " << (*first).name() << " type " << (*first).typeName() << endreq;
   }
   return(sc);
}
//___________________________________________________________________________
StatusCode WriteTag::execute() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   const DataHandle<EventInfo> evt;
   sc = p_SGevent->retrieve(evt);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   if (!evt) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   unsigned int eventNumber = evt->event_ID()->event_number();
   unsigned int runNumber = evt->event_ID()->run_number();
   log << MSG::INFO << "EventInfo event: " << eventNumber << "  run: " << runNumber << endreq;

   TagAthenaAttributeList* attribList = new TagAthenaAttributeList(*m_attribListSpec);
   (*attribList)["RunNumber"].data<unsigned int>() = runNumber;
   (*attribList)["EventNumber"].data<unsigned int>() = eventNumber;
   if (m_magic > 0) {
      (*attribList)["MagicNumber"].data<unsigned int>() = m_magic.value();
   }
   sc = p_SGevent->record(attribList, m_key.value());
   if (sc.isFailure()) {
      log << MSG::ERROR << "Could not record TagAthenaAttributeList object." << endreq;
      return(StatusCode::FAILURE);
   }

   log << MSG::INFO << "registered all data" << endreq;
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode WriteTag::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
