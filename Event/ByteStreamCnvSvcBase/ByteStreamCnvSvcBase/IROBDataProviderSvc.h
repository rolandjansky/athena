/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IROBDATAPROVIDERSVC_H
#define IROBDATAPROVIDERSVC_H

#include "GaudiKernel/IInterface.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/EventContext.h"

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <functional>

// Declaration of the interface ID ( interface id, major version, minor version)
//static const InterfaceID IID_IROBDataProviderSvc("IROBDataProviderSvc", 1 , 0);

/** @class IROBDataProviderSvc
    @brief Interface class for managing ROB for both online and offline.
*/
class IROBDataProviderSvc : virtual public IInterface {

public:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;
  typedef std::vector<const ROBF*> VROBFRAG;

   /// Retrieve interface ID
  //   static const InterfaceID& interfaceID() { return IID_IROBDataProviderSvc; }
  DeclareInterfaceID(IROBDataProviderSvc, 1, 1);

   /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector
   virtual void addROBData(const std::vector<uint32_t>& robIds, const std::string_view callerName="UNKNOWN") = 0 ;

   /// Add a given LVL1/LVL2 ROBFragment to cache
   virtual void setNextEvent(const std::vector<ROBF>& result) = 0 ;


   /// Add all ROBFragments of a RawEvent to cache
   virtual void setNextEvent(const RawEvent* re) = 0 ;


   /// Retrieve ROBFragments for given ROB ids from cache
   virtual void getROBData(const std::vector<uint32_t>& robIds, VROBFRAG& robFragments, const std::string_view callerName="UNKNOWN") = 0;


   /// Retrieve the whole event.
   virtual const RawEvent* getEvent() = 0;


   /// Store the status for the event.
   virtual void setEventStatus(uint32_t status) = 0;


   /// Retrieve the status for the event.
   virtual uint32_t getEventStatus() = 0;


   // variants for MT, it has an implementation for now in order not to require change in all implementations yet, they will all become pure virtual methods
   virtual void addROBData(const EventContext& /*context*/, const std::vector<uint32_t>& /*robIds*/, const std::string_view callerName="UNKNOWN") { 
     throw std::runtime_error( std::string(callerName)+ std::string(" is using unimplemented ") + __FUNCTION__ ) ; 
   }
   virtual void setNextEvent(const EventContext& /*context*/, const std::vector<ROBF>& /*result*/) {
     throw std::runtime_error( std::string("Unimplemented ") + __FUNCTION__ ); 
   }
   virtual void setNextEvent( const EventContext& /*context*/, const RawEvent* /*re*/) {
     throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ ); 
   }
   virtual void getROBData(const EventContext& /*context*/, const std::vector<uint32_t>& /*robIds*/, VROBFRAG& /*robFragments*/, 
			   const std::string_view callerName="UNKNOWN") { 
     throw std::runtime_error( std::string(callerName)+ std::string(" is using unimplemented ") + __FUNCTION__ ) ; 
   }
   virtual const RawEvent* getEvent(const EventContext& /*context*/) {
     throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ ); 
   }
   virtual void setEventStatus(const EventContext& /*context*/, uint32_t /*status*/) {
     throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ ); 
   }
   virtual uint32_t getEventStatus(const EventContext& /*context*/) {
     throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ ); 
     return 0;
   }
   
   /// @brief Interface to access cache of ROBs (it is a full event in case of offline)
   /// In online implementation the cache will contain only a subset of ROBs. 
   /// This method allows read access to the cache. 
   /// @warning in case the cache is updated in the meantime the iteration is guaranteed to be safe 
   /// but may not give access to all the ROBs available n the very moment
   /// Example of counting: size_t counter = 0; svc->processCahcedROBs(ctx, [&](const ROBF*){ counter ++; })
   /// Example of printout: svc->processCahcedROBs(ctx, [&](const ROBF* rob){ log() << MSG::DEBUG << "ROB " << rob->source_id() << endmsg; })
   virtual void processCachedROBs(const EventContext& /*context*/, 
				  const std::function< void(const ROBF* )>& /*fn*/ ) const {
     throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ ); 
   }
  
  /// Check if complete event data are already in cache
  virtual bool isEventComplete(const EventContext& /*context*/) const {
    throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ );      
  } 
  
  /// @brief Collect all data for an event from the ROS and put them into the cache
  /// @return value: number of ROBs which were retrieved to complete the event
  /// Optionally the name of the caller of this method can be specified for monitoring
  virtual int collectCompleteEventData(const EventContext& /*context*/, const std::string_view callerName="UNKNOWN") {
    throw std::runtime_error(std::string(callerName) + std::string(" is using unimplemented ") + __FUNCTION__ );
    return 0;
  }

};

#endif
