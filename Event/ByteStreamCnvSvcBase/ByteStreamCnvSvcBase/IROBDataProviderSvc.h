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
#include <stdexcept>

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
   virtual void addROBData(const std::vector<uint32_t>& robIds, const std::string callerName="UNKNOWN") = 0 ;

   /// Add a given LVL1/LVL2 ROBFragment to cache
   virtual void setNextEvent(const std::vector<ROBF>& result) = 0 ;


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


   // variants for MT, it has an implementation for now in order not to require change in all implementations yet, they will all become pure virtual methods
   virtual void addROBData(const EventContext& /*context*/, const std::vector<uint32_t>& /*robIds*/, const std::string callerName="UNKNOWN") { 
     throw std::runtime_error( std::string( callerName+" is using unimplemented ") + __FUNCTION__ ) ; 
   }
   virtual void setNextEvent(const EventContext& /*context*/, const std::vector<ROBF>& /*result*/) {
     throw std::runtime_error( std::string("Unimplemented ") + __FUNCTION__ ); 
   }
   virtual void setNextEvent( const EventContext& /*context*/, const RawEvent* /*re*/) {
     throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ ); 
   }
   virtual void getROBData(const EventContext& /*context*/, const std::vector<uint32_t>& /*robIds*/, VROBFRAG& /*robFragments*/, const std::string callerName="UNKNOWN") { 
     throw std::runtime_error( std::string( callerName+" is using unimplemented ") + __FUNCTION__ ) ; 
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
   
  // additional interface for online use
  /// Return vector with all ROBFragments stored in the cache 
  // TB this name seems to be missleading, would getCachedROBs be better?
  virtual void getAllROBData(const EventContext& /*context*/, std::vector<const ROBF*>&) {
    throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ );
  }
  
  /// Dump the internal ROB cache
  // TB given the getAllROBData is available isn't this only a convenience function? 
  // i.e.: the implementation would be: svc->getCachedROBs(ctx,robs); .. print what is needed 
  virtual std::string dumpROBcache(const EventContext& /*context*/) const {
    throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ );      
    return "";
  }
  
  /// Return size of ROBFragments cache 
  virtual int sizeROBCache(const EventContext& /*context*/) {
    throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ );
    return 0;
  };

  /// Check if complete event data are already in cache
  virtual bool isEventComplete(const EventContext& /*context*/) {
    throw std::runtime_error(std::string("Unimplemented ") + __FUNCTION__ );      
  } 

  /// Collect all data for an event from the ROS and put them into the cache
  /// Return value: number of ROBs which were retrieved to complete the event
  /// Optinonally the name of the caller of this method can be specified for monitoring
  virtual int collectCompleteEventData(const EventContext& /*context*/, const std::string callerName="UNKNOWN") {
    throw std::runtime_error(std::string(callerName + " is using unimplemented ") + __FUNCTION__ );
    return 0;
  }




};

#endif
