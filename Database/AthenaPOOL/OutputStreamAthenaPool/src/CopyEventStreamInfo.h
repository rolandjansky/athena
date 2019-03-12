/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYEVENTSTREAMINFO_H
#define COPYEVENTSTREAMINFO_H
/** @file CopyEventStreamInfo.h
 *  @brief This file contains the class definition for the CopyEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaKernel/GenericMetadataToolNoAux.h"
#include "EventInfo/EventStreamInfo.h"

#include <string>

class StoreGateSvc;

/** @class CopyEventStreamInfo 
 *  @brief This class provides an algorithm to make the EventStreamInfo object and update it.
 **/
class CopyEventStreamInfo : public GenericMetadataToolNoAux <EventStreamInfo> {
public:
   /// Standard AlgTool Constructor
   CopyEventStreamInfo(const std::string& type, 
                       const std::string& name, 
                       const IInterface* parent);
   /// Destructor
   virtual ~CopyEventStreamInfo();

   /// AthAlgTool Interface method implementations:
   //StatusCode initialize();
   //StatusCode finalize();
   /// Helper class to update a container with information from another one
   virtual StatusCode updateContainer(EventStreamInfo* evtStrInfo_out,
                                const EventStreamInfo* evtStrInfo_in );

};
#endif
