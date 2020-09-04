/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_ISCHEDULERMONSVC_H
#define TRIGSTEERMONITOR_ISCHEDULERMONSVC_H

#include "GaudiKernel/IInterface.h"

/**@class ISchedulerMonSvc
 * @brief Defines interface for interaction with a service monitoring the Scheduler status
 */
class ISchedulerMonSvc: virtual public IInterface {
public:
  /// Interface ID
  DeclareInterfaceID(ISchedulerMonSvc, 1, 0);

  /// Start querying and monitoring Scheduler status
  virtual StatusCode startMonitoring() = 0;

  /// Stop querying and monitoring Scheduler status
  virtual StatusCode stopMonitoring() = 0;
};

#endif // TRIGSTEERMONITOR_ISCHEDULERMONSVC_H
