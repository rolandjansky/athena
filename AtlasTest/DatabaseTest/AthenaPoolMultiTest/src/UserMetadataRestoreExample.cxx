/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file UserMetadataRestoreExample.cxx
 * @brief Implementation of class UserMetadataRestoreExample
 */
  
#include <string>
#include "UserMetadataRestoreExample.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "ByteStreamData/ByteStreamUserMetadata.h"
    
UserMetadataRestoreExample::UserMetadataRestoreExample(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthAlgorithm(name, pSvcLocator), m_done(false)
{
   declareProperty("DetStoreWrite", m_detStoreWrite);
}

UserMetadataRestoreExample::~UserMetadataRestoreExample()
{}

StatusCode UserMetadataRestoreExample::initialize() 
{ 
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "initialize" << endreq; 
   // Add empty container for callback registrations
   StoreGateSvc* mdStore;
   StatusCode sc = service("DetectorStore",mdStore);
   if (sc.isFailure()) return StatusCode::FAILURE;
   AthenaAttributeList* attlist;
   sc = mdStore->retrieve(attlist,"/Digitization/Parameters" );
   if (sc.isFailure()) {
      coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
      attlist = new AthenaAttributeList(*spec);
      StatusCode sc2 = mdStore->record(attlist,"/Digitization/Parameters" );
      if (sc2.isFailure()) {
         log << MSG::WARNING << "Unable to store AthenaAttributeList for /Digitization/Parameters" << endreq;
      }
   }
   else log << MSG::INFO << "Stored initial version" << endreq;
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataRestoreExample::execute() { return StatusCode::SUCCESS; }

StatusCode UserMetadataRestoreExample::start() 
{
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "UserMetadataRestoreExample::execute" << endreq;
   if (!m_done) {
      m_done = true;

      // First check for metadata from the bytestream file
      StoreGateSvc* mdStore;
      StatusCode sc = service("InputMetaDataStore",mdStore);
      if (sc.isFailure()) return StatusCode::FAILURE;
      const DataHandle< ByteStream::FreeMetadata > metacont;
      sc = mdStore->retrieve( metacont, "/Digitization/Parameters" );

      if (sc.isSuccess()) {
         log << MSG::DEBUG << "Found /Digitization/Parameters in detStore" << endreq;

         // Check for attlist in detector store (should be there from initialize)
         AthenaAttributeList* attlist;
         bool found(false);
         StoreGateSvc* detStore;
         sc = service("DetectorStore",detStore);
         if (sc.isFailure()) return StatusCode::FAILURE;
         sc = detStore->retrieve(attlist,"/Digitization/Parameters" );
         if (sc.isFailure()) {
            // build spec
            // fill att list spec
            coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
            attlist = new AthenaAttributeList(*spec);
         }
         else found=true;

         // Add attributes name,types to the spec through attlist
         ByteStream::FreeMetadata::const_iterator first1 = metacont->begin();
         ByteStream::FreeMetadata::const_iterator last1  = metacont->end();
         for (; first1 != last1; ++first1) {
            std::string name = first1->first.substr(0,first1->first.find('/'));
            std::string type = first1->first.substr(first1->first.find('/')+1);
            log << MSG::INFO
                << "name = " << name << ", type = " << type << ", value = " << first1->second << endreq;
            attlist->extend(name,type);
         }

         // Yes we have to loop twice to add the data
         first1 = metacont->begin();
         for (; first1 != last1; ++first1) {
            std::string name = first1->first.substr(0,first1->first.find('/'));
            std::string type = first1->first.substr(first1->first.find('/')+1);
            if (type == "string") {
               (*attlist)[name].data<std::string>() = first1->second;
            }
            else {
               log << MSG::ERROR << "Not a string, ---> SKIPPING!" << endreq;
            }
         }

         // If it was not found (ERROR) recover by recording it
         if (!found) {
            sc = detStore->record(attlist, "/Digitization/Parameters" );
            if (sc.isFailure()) {
               log << MSG::WARNING << "Unable to store AthenaAttributeList for /Digitization/Parameters" << endreq;
            }
         }
      }
      else {
         log << MSG::WARNING << "Did NOT find /Digitization/Parameters in detStore" << endreq;
      }
   }
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataRestoreExample::finalize() { return StatusCode::SUCCESS; }

