/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class IAthRNGSvc
  * @brief manage multiple RandomEngines in thread-safe way.
  * 
  * Access to the RandomEngine is wrapped in the RNGWrapper
  * in order to ensure thread-safe access in the event loop.
  *
  * @author Sami Kama <sami.kama@cern.ch>
  */

#ifndef ATHENAKERNEL_IATHRNGSVC_H
#define ATHENAKERNEL_IATHRNGSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/INamedInterface.h"

#include <string>
#include "stdint.h"

namespace ATHRNG {
  class RNGWrapper;
}

class IAthRNGSvc : virtual public IService
{

public:

  /// Interface to retrieve the CLHEP engine
  virtual ATHRNG::RNGWrapper* getEngine(const INamedInterface* client,
                                        const std::string& streamName) = 0;

  /// out-of-line destructor
  virtual ~IAthRNGSvc();

  /// Declare interface to the framework
  DeclareInterfaceID( IAthRNGSvc, 1, 0 );

};

#endif // ATHENAKERNEL_IATHRNGSVC_H
