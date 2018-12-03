/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigROBDataProviderSvcPrefetch_H
#define ITrigROBDataProviderSvcPrefetch_H
#include "GaudiKernel/IInterface.h"

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ITrigROBDataProviderSvcPrefetch("ITrigROBDataProviderSvcPrefetch", 1 , 0);

namespace HLT {
  class RobRequestInfo;
}

/** @class ITrigROBDataProviderSvcPrefetch 
    @brief Interface class with additional methods to interface IROBDataProviderSvc for prefetching ROBs in online.
*/
class ITrigROBDataProviderSvcPrefetch : virtual public IInterface {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrigROBDataProviderSvcPrefetch; }

  //-----------------------------------
  // Methods useful for ROB prefetching
  //-----------------------------------

  // Set access to ROB prefetching information from steering
  virtual void setRobRequestInfo(HLT::RobRequestInfo*) = 0;

  // Get access to ROB prefetching information from steering
  virtual HLT::RobRequestInfo* robRequestInfo() const = 0;
};
#endif
