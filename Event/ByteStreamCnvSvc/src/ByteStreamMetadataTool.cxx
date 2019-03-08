/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamMetadataTool.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadataTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataTool.cxx,v 1.4 2009-05-13 22:06:51 cranshaw Exp $
 **/

#include "ByteStreamMetadataTool.h"

//___________________________________________________________________________
ByteStreamMetadataTool::ByteStreamMetadataTool(const std::string& type,
     	                                       const std::string& name,
	                                       const IInterface* parent)
    : GenericMetadataToolNoAux <ByteStreamMetadataContainer>(type, name, parent)
{
  m_inputCollName="ByteStreamMetadata";
  m_outputCollName="ByteStreamMetadata";
  m_markIncomplete=false;
}

//___________________________________________________________________________
ByteStreamMetadataTool::~ByteStreamMetadataTool() {
}

//___________________________________________________________________________
StatusCode ByteStreamMetadataTool::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode ByteStreamMetadataTool::finalize() {
   ATH_MSG_INFO("in finalize()");
   return(StatusCode::SUCCESS);
}


StatusCode ByteStreamMetadataTool::updateContainer(
                      ByteStreamMetadataContainer* bsmdc_out,
                const ByteStreamMetadataContainer* bsmdc_in )
{
   std::vector<ByteStreamMetadata*> copy;
   std::set<std::string> transGuids;

   // Put input into vector of components
   for (const ByteStreamMetadata* md : *bsmdc_in) {
     copy.push_back(new ByteStreamMetadata(*md));
   }
   // If anything was in the input, copy to output
   if (!copy.empty()) {
      transGuids.clear();
      // Check for existing guid in output container
      for (auto iter = bsmdc_out->begin(), iterEnd = bsmdc_out->end();
	               iter != iterEnd; iter++) {
         transGuids.insert((*iter)->getGuid());
      }
      for (auto iter = copy.begin(), iterEnd = copy.end();
	            iter != iterEnd; iter++) {
         // Only insert new metadata records (with GUID not yet in container)
         if (transGuids.insert((*iter)->getGuid()).second) {
            bsmdc_out->push_back(*iter);
            *iter = 0;
         } else {
            delete *iter; *iter = 0;
         }
      }
   }
   return StatusCode::SUCCESS;
}

