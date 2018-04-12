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

   //return status of prefecthing
  virtual bool  isMissingPrefetching()  = 0;

  // return setup of pre-fetching
  virtual bool isPrefetchingAtAlgoLevel() = 0;

};

#endif
