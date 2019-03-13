/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigROBDataProviderSvc_H
#define ITrigROBDataProviderSvc_H
#include "GaudiKernel/IInterface.h"
#include "ByteStreamData/RawEvent.h" 
#include <inttypes.h>
#include <vector>
#include <map>
#include <string>
#include <string_view>

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ITrigROBDataProviderSvc("ITrigROBDataProviderSvc", 2 , 0);

/** @class ITrigROBDataProviderSvc 
    @brief Interface class with additional methods to interface IROBDataProviderSvc for managing ROBs in online.
*/
class ITrigROBDataProviderSvc : virtual public IInterface {

public:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF; 

  //---------------------------------------------------
  // Methods to access the data in the online ROB cache
  //---------------------------------------------------

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrigROBDataProviderSvc; }

  /// Return vector with all ROBFragments stored in the cache 
  virtual void getAllROBData(std::vector<const ROBF*>&) = 0;

  /// Dump the internal ROB cache
  virtual std::string dumpROBcache() const = 0;

  /// Return size of ROBFragments cache 
  virtual int sizeROBCache() = 0;

  /// iterators over cache entries
  virtual std::map<uint32_t, ROBF>::iterator beginROBCache() = 0;
  virtual std::map<uint32_t, ROBF>::iterator endROBCache() = 0;

  //--------------------------------------------------------------------------------------------------
  // Methods for the merged L2/EF system to initiate or check the retrieval of the complete event data
  //--------------------------------------------------------------------------------------------------

  /// Check if complete event data are already in cache
  virtual bool isEventComplete() = 0;

  /// Collect all data for an event from the ROS and put them into the cache
  /// Return value: number of ROBs which were retrieved to complete the event
  /// Optinonally the name of the caller of this method can be specified for cost monitoring
  virtual int collectCompleteEventData(const std::string_view callerName="UNKNOWN") = 0;

  //-------------------------------------
  // Methods useful for (cost) monitoring
  //-------------------------------------

  /// set the name of the program which uses the ROBDataProviderSvc
  virtual void setCallerName(const std::string_view) = 0;

  /// get the name of the program which is presently registered in the ROBDataProviderSvc
  virtual std::string getCallerName() = 0;
};

#endif
