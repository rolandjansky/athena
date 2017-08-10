/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file WriteTag.cxx
 *  @brief This file contains the implementation for the WriteTag class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteTag.cxx,v 1.4 2009-03-17 09:44:46 gemmeren Exp $
 **/

#include "WriteTag.h"

#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

using namespace AthPoolEx;

//___________________________________________________________________________
WriteTag::WriteTag(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_attribListSpec(0) {
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
   ATH_MSG_INFO("in initialize()");

   ATH_MSG_DEBUG("Defining the attribute list specification.");
   m_attribListSpec = new AthenaAttributeListSpecification();
   m_attribListSpec->extend("RunNumber", "unsigned int");
   m_attribListSpec->extend("EventNumber", "unsigned int");
   if (m_magic > 0) {
      m_attribListSpec->extend("MagicNumber", "unsigned int");
   }
   ATH_MSG_DEBUG("Printing out attribute list specification:");
   for (AthenaAttributeListSpecification::const_iterator first = m_attribListSpec->begin(), last = m_attribListSpec->end(); first != last; ++first) {
      ATH_MSG_DEBUG(" name " << (*first).name() << " type " << (*first).typeName());
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteTag::execute() {
   ATH_MSG_DEBUG("in execute()");

   const DataHandle<EventInfo> evt;
   if (evtStore()->retrieve(evt).isFailure()) {
      ATH_MSG_FATAL("Could not find event");
      return StatusCode::FAILURE;
   }
   unsigned int eventNumber = evt->event_ID()->event_number();
   unsigned int runNumber = evt->event_ID()->run_number();
   ATH_MSG_INFO("EventInfo event: " << eventNumber << "  run: " << runNumber);

   auto tagAttribList = std::make_unique<TagAthenaAttributeList>(*m_attribListSpec);
   (*tagAttribList)["RunNumber"].data<unsigned int>() = runNumber;
   (*tagAttribList)["EventNumber"].data<unsigned int>() = eventNumber;
   if (m_magic > 0) {
      (*tagAttribList)["MagicNumber"].data<unsigned int>() = m_magic.value();
   }

   coral::AttributeListSpecification* cspec = new coral::AttributeListSpecification;
   m_attribListSpec->coralSpec (*cspec);
   auto attribList = std::make_unique<AthenaAttributeList>(*cspec);
   cspec->release();
   (*attribList)["RunNumber"].data<unsigned int>() = runNumber;
   (*attribList)["EventNumber"].data<unsigned int>() = eventNumber;
   if (m_magic > 0) {
      (*attribList)["MagicNumber"].data<unsigned int>() = m_magic.value();
   }
   ATH_CHECK( evtStore()->record (std::move(attribList), m_key.value()) );
   ATH_CHECK( evtStore()->record (std::move(tagAttribList), m_key.value()) );

   ATH_MSG_INFO("registered all data");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode WriteTag::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
