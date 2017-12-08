/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IROBDATAPROVIDERSVC_H
#define IROBDATAPROVIDERSVC_H

#include "GaudiKernel/IInterface.h"
#include "ByteStreamData/RawEvent.h"

#include <cstdint>
#include <vector>
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IROBDataProviderSvc("IROBDataProviderSvc", 1 , 0);

/** @class IROBDataProviderSvc
    @brief Interface class for managing ROB for both online and offline.
*/
class IROBDataProviderSvc : virtual public IInterface {

public:
   typedef std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> VROBFRAG;

   /// Retrieve interface ID
   static const InterfaceID& interfaceID() { return IID_IROBDataProviderSvc; }

   /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector
   virtual void addROBData(const std::vector<uint32_t>& robIds, const std::string callerName="UNKNOWN") = 0 ;

   /// Add a given LVL1/LVL2 ROBFragment to cache
   virtual void setNextEvent(const std::vector<OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment>& result) = 0 ;

   /// Add all ROBFragments of a RawEvent to cache
   virtual void setNextEvent(const RawEvent* re) = 0 ;

   /// Retrieve ROBFragments for given ROB ids from cache
   virtual void getROBData(const std::vector<uint32_t>& robIds, VROBFRAG& robFragments, const std::string callerName="UNKNOWN") = 0;

   /// Retrieve the whole event.
   virtual const RawEvent* getEvent() = 0;

   /// Store the status for the event.
   virtual void setEventStatus(uint32_t status) = 0;

   /// Retrieve the status for the event.
   virtual uint32_t getEventStatus() = 0;
};

#endif
