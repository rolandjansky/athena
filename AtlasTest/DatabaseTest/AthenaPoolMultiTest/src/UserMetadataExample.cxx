/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file UserMetadataExample.cxx
 * @brief Implementation of class UserMetadataExample
 */
  
#include <string>
#include "UserMetadataExample.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "ByteStreamData/ByteStreamUserMetadata.h"

UserMetadataExample::UserMetadataExample(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthAlgorithm(name, pSvcLocator), m_done(false)
{}

UserMetadataExample::~UserMetadataExample()
{}

StatusCode UserMetadataExample::initialize() 
{ 
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "UserMetadataExample::initialize" << endreq;
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataExample::execute() 
{
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "UserMetadataExampleA::execute" << endreq;
   if (!m_done) {
      m_done = true;
      const DataHandle< AthenaAttributeList > digiParam;
      //StatusCode sc = detStore()->retrieve( digiParam, "/Digitization/Parameters" );
      StoreGateSvc* detStore;
      StatusCode sc = service("DetectorStore",detStore);
      if (sc.isFailure()) return StatusCode::FAILURE;
      sc = detStore->retrieve( digiParam, "/Digitization/Parameters" );
      if (sc.isSuccess()) {
         //ATH_MSG_DEBUG("Found /Digitization/Parameters in detStore");
         log << MSG::INFO << "Found /Digitization/Parameters in detStore" << endreq;
         ByteStream::FreeMetadata* metacont = new ByteStream::FreeMetadata();
         coral::AttributeList::const_iterator first1 = digiParam->begin();
         coral::AttributeList::const_iterator last1  = digiParam->end();
         for (; first1 != last1; ++first1) {
            const std::string name = (*first1).specification().name();
            const std::string type = (*first1).specification().typeName();
            const std::string value = first1->data<std::string>();
            log << MSG::INFO << "About to push back " << name << ", " << value << endreq;
            metacont->push_back(ByteStreamFreeMetadataString(name+"/"+type,value));
         }
         //sc = detStore()->record(metacont, "/Digitization/Parameters" );
         sc = detStore->record(metacont, "/Digitization/Parameters" );
         if (sc.isFailure()) {
            //ATH_MSG_WARNING("Unable to store ByteStreamFreeMetadataString for /Digitization/Parameters" );
            log << MSG::WARNING << "Unable to store ByteStreamFreeMetadataString for /Digitization/Parameters" << endreq;
         }
      }
      else {
         //ATH_MSG_WARNING("Did NOT find /Digitization/Parameters in detStore");
         log << MSG::WARNING << "Did NOT find /Digitization/Parameters in detStore" << endreq;
      }
   }
   setFilterPassed(true);	
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataExample::finalize() { return StatusCode::SUCCESS; }
