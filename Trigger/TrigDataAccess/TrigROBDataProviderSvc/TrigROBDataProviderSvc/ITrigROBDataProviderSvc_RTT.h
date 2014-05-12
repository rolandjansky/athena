/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigROBDataProviderSvc_RTT_H
#define ITrigROBDataProviderSvc_RTT_H
#include "GaudiKernel/IInterface.h"

#include <inttypes.h>
#include <vector>
#include <map>

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ITrigROBDataProviderSvc_RTT("ITrigROBDataProviderSvc_RTT", 1 , 0);

/** @class ITrigROBDataProviderSvc_RTT 
    @brief Interface class with additional methods to interface ITrigROBDataProviderSvc for managing ROBs 
*/
class ITrigROBDataProviderSvc_RTT : virtual public IInterface {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrigROBDataProviderSvc_RTT; }

  /// Return vector with all ROBFragments stored in the cache 
  virtual void addROBData(const std::vector<uint32_t>& robIds,
			  const std::string callerName="UNKNOWN") = 0;

   //return status of prefecthing
  virtual bool  isMissingPrefetching()  = 0;

  // return setup of pre-fetching
  virtual bool isPrefetchingAtAlgoLevel() = 0;

  /// set the name of the program which uses the ROBDataProviderSvc
  virtual void setCallerName(const std::string) = 0;

  /// Check if complete event data are already in cache
  virtual bool isEventComplete() = 0;

  /// Collect all data for an event from the ROS and put them into the cache
  /// Return value: number of ROBs which were retrieved to complete the event
  /// Optinonally the name of the caller of this method can be specified for cost monitoring
  virtual int collectCompleteEventData(const std::string callerName="UNKNOWN") = 0;


};

#endif
