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
   ATH_MSG_INFO( "UserMetadataExample::initialize"  );
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataExample::execute() 
{
   ATH_MSG_INFO( "UserMetadataExampleA::execute"  );
   if (!m_done) {
      m_done = true;
      const DataHandle< AthenaAttributeList > digiParam;
      //StatusCode sc = detStore()->retrieve( digiParam, "/Digitization/Parameters" );
      StatusCode sc = detStore()->retrieve( digiParam, "/Digitization/Parameters" );
      if (sc.isSuccess()) {
         //ATH_MSG_DEBUG("Found /Digitization/Parameters in detStore");
         ATH_MSG_INFO( "Found /Digitization/Parameters in detStore"  );
         ByteStream::FreeMetadata* metacont = new ByteStream::FreeMetadata();
         coral::AttributeList::const_iterator first1 = digiParam->begin();
         coral::AttributeList::const_iterator last1  = digiParam->end();
         for (; first1 != last1; ++first1) {
            const std::string name = (*first1).specification().name();
            const std::string type = (*first1).specification().typeName();
            const std::string value = first1->data<std::string>();
            ATH_MSG_INFO( "About to push back " << name << ", " << value  );
            metacont->push_back(ByteStreamFreeMetadataString(name+"/"+type,value));
         }
         //sc = detStore()->record(metacont, "/Digitization/Parameters" );
         sc = detStore()->record(metacont, "/Digitization/Parameters" );
         if (sc.isFailure()) {
            //ATH_MSG_WARNING("Unable to store ByteStreamFreeMetadataString for /Digitization/Parameters" );
            ATH_MSG_WARNING( "Unable to store ByteStreamFreeMetadataString for /Digitization/Parameters"  );
         }
      }
      else {
         //ATH_MSG_WARNING("Did NOT find /Digitization/Parameters in detStore");
         ATH_MSG_WARNING( "Did NOT find /Digitization/Parameters in detStore"  );
      }
   }
   setFilterPassed(true);	
   return StatusCode::SUCCESS; 
}

StatusCode UserMetadataExample::finalize() { return StatusCode::SUCCESS; }
