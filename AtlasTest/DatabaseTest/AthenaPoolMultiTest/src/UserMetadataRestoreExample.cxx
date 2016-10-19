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
   ATH_MSG_INFO( "initialize"  );
   // Add empty container for callback registrations
   AthenaAttributeList* attlist;
   StatusCode sc = detStore()->retrieve(attlist,"/Digitization/Parameters" );
   if (sc.isFailure()) {
      coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
      attlist = new AthenaAttributeList(*spec);
      StatusCode sc2 = detStore()->record(attlist,"/Digitization/Parameters" );
      if (sc2.isFailure()) {
        ATH_MSG_WARNING( "Unable to store AthenaAttributeList for /Digitization/Parameters"  );
      }
   }
   else ATH_MSG_INFO( "Stored initial version"  );
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataRestoreExample::execute() { return StatusCode::SUCCESS; }

StatusCode UserMetadataRestoreExample::start() 
{
   ATH_MSG_DEBUG( "UserMetadataRestoreExample::execute"  );
   if (!m_done) {
      m_done = true;

      // First check for metadata from the bytestream file
      StoreGateSvc* mdStore = nullptr;
      ATH_CHECK( service("InputMetaDataStore",mdStore) );
      const DataHandle< ByteStream::FreeMetadata > metacont;
      StatusCode sc = mdStore->retrieve( metacont, "/Digitization/Parameters" );

      if (sc.isSuccess()) {
         ATH_MSG_DEBUG( "Found /Digitization/Parameters in detStore"  );

         // Check for attlist in detector store (should be there from initialize)
         AthenaAttributeList* attlist;
         bool found(false);
         sc = detStore()->retrieve(attlist,"/Digitization/Parameters" );
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
            ATH_MSG_INFO( "name = " << name << ", type = " << type << ", value = " << first1->second  );
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
              ATH_MSG_ERROR( "Not a string, ---> SKIPPING!"  );
            }
         }

         // If it was not found (ERROR) recover by recording it
         if (!found) {
           sc = detStore()->record(attlist, "/Digitization/Parameters" );
            if (sc.isFailure()) {
              ATH_MSG_WARNING( "Unable to store AthenaAttributeList for /Digitization/Parameters"  );
            }
         }
      }
      else {
        ATH_MSG_WARNING( "Did NOT find /Digitization/Parameters in detStore"  );
      }
   }
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataRestoreExample::finalize() { return StatusCode::SUCCESS; }

