/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file StoreDump.cxx
 * @brief Implementation of class StoreDump
 */
  
#include <string>
#include "StoreDump.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
    
StoreDump::StoreDump(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthAlgorithm(name, pSvcLocator)
{
   declareProperty("StoreName", m_storename="DetectorStore");
   declareProperty("Key", m_key="*");
}

StoreDump::~StoreDump()
{}

StatusCode StoreDump::initialize() { return StatusCode::SUCCESS; }

StatusCode StoreDump::execute() 
{
   MsgStream log(msgSvc(), name());
   StoreGateSvc* detStore;
   StatusCode sc = service(m_storename,detStore);
   if (sc.isFailure()) return StatusCode::FAILURE;
   log << MSG::INFO << detStore->dump() << endreq;

   AthenaAttributeList* test;
   sc = detStore->retrieve(test,m_key);
   if (sc.isSuccess()) {
      log << MSG::INFO << "AttributeList has size " << test->size() << endreq;
   }
   else {
      log << MSG::ERROR << "AthenaAttributeList not retrievable" << endreq;
   }

   return StatusCode::SUCCESS; 
}

StatusCode StoreDump::finalize() { return StatusCode::SUCCESS; }
