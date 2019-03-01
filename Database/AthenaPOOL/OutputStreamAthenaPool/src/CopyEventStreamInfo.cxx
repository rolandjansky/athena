/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file CopyEventStreamInfo.cxx
 *  @brief This file contains the implementation for the CopyEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: CopyEventStreamInfo.cxx,v 1.7 2009-02-09 22:48:31 gemmeren Exp $
 **/

#include "CopyEventStreamInfo.h"


//___________________________________________________________________________
CopyEventStreamInfo::CopyEventStreamInfo(const std::string& type,
              	                         const std::string& name,
	                                 const IInterface* parent) 
       : GenericMetadataToolNoAux <EventStreamInfo>(type, name, parent)
{
}
//___________________________________________________________________________
CopyEventStreamInfo::~CopyEventStreamInfo() {
}


StatusCode CopyEventStreamInfo::updateContainer(
                 EventStreamInfo* evtStrInfo_out,
           const EventStreamInfo* evtStrInfo_in ) 
{
   evtStrInfo_out->addEvent(evtStrInfo_in->getNumberOfEvents());
   for (auto elem = evtStrInfo_in->getRunNumbers().begin(),
         lastElem = evtStrInfo_in->getRunNumbers().end(); 
            elem != lastElem; elem++) {
      evtStrInfo_out->insertRunNumber(*elem);
   }
   for (auto elem = evtStrInfo_in->getLumiBlockNumbers().begin(),
         lastElem = evtStrInfo_in->getLumiBlockNumbers().end(); 
            elem != lastElem; elem++) {
      evtStrInfo_out->insertLumiBlockNumber(*elem);
   }
   for (auto elem = evtStrInfo_in->getProcessingTags().begin(),
         lastElem = evtStrInfo_in->getProcessingTags().end(); 
            elem != lastElem; elem++) {
      evtStrInfo_out->insertProcessingTag(*elem);
   }
   for (auto elem = evtStrInfo_in->getItemList().begin(),
         lastElem = evtStrInfo_in->getItemList().end(); 
            elem != lastElem; elem++) {
      evtStrInfo_out->insertItemList((*elem).first, (*elem).second);
   }
   for (auto elem = evtStrInfo_in->getEventTypes().begin(),
         lastElem = evtStrInfo_in->getEventTypes().end(); 
            elem != lastElem; elem++) {
      evtStrInfo_out->insertEventType(*elem);
   }
   return(StatusCode::SUCCESS);
}
