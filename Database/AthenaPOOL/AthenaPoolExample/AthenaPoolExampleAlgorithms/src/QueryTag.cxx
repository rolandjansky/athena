/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file QueryTag.cxx
 *  @brief This file contains the implementation for the QueryTag class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: QueryTag.cxx,v 1.5 2009-03-17 09:44:46 gemmeren Exp $
 **/

#include "QueryTag.h"

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

using namespace AthPoolEx;

//___________________________________________________________________________
QueryTag::QueryTag(const std::string& type, const std::string& name, const IInterface* parent) : 
	AlgTool(type, name, parent), m_eventStore("StoreGateSvc", name), m_attrListKey() {
   declareInterface<IAthenaSelectorTool>(this);
}
//___________________________________________________________________________
QueryTag::~QueryTag() {
}
//___________________________________________________________________________
StatusCode QueryTag::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;
   const IService* parentSvc = dynamic_cast<const IService*>(this->parent());
   if (parentSvc != 0) {
      const IProperty* propertyServer = dynamic_cast<const IProperty*>(parentSvc);
      if (propertyServer != 0) {
         StringProperty attrKeyProperty("AttributeListKey", "");
         StatusCode status = propertyServer->getProperty(&attrKeyProperty);
         if (status.isSuccess()) {
            m_attrListKey = attrKeyProperty.value();
         }
      }
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode QueryTag::postInitialize() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode QueryTag::preNext() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode QueryTag::postNext() {
   MsgStream log(msgSvc(), name());
   if (!m_eventStore->contains<AthenaAttributeList>(m_attrListKey)) {
      log << MSG::DEBUG << "Can't get attributeList for preselection" << endreq;
      return(StatusCode::SUCCESS);
   }

   const DataHandle<AthenaAttributeList> attrList;
   StatusCode status = m_eventStore->retrieve(attrList, m_attrListKey);
   if (status.isFailure()) {
      log << MSG::ERROR << "Could not retrieve AthenaAttributeList" << endreq;
      return(StatusCode::FAILURE);
   }

   try {
      unsigned int eventNumber = (*attrList)["EventNumber"].data<unsigned int>();
      unsigned int runNumber = (*attrList)["RunNumber"].data<unsigned int>();
      unsigned int magicNumber = (*attrList)["MagicNumber"].data<unsigned int>();

      log << MSG::DEBUG << "EventNumber = " << eventNumber << endreq;
      log << MSG::DEBUG << "RunNumber = " << runNumber << endreq;
      log << MSG::DEBUG << "MagicNumber = " << magicNumber << endreq;
      if (eventNumber < 10 && magicNumber > 17) {
         return(StatusCode::RECOVERABLE);
      }
   } catch (...) {
      log << MSG::DEBUG << "Can't apply attributeList preselection" << endreq;
      return(StatusCode::SUCCESS);
   }

   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode QueryTag::preFinalize() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode QueryTag::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
