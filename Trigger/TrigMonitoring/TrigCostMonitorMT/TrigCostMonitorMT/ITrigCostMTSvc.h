/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 /*
 */

#ifndef TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H 1

#include "GaudiKernel/IService.h"
#include "GaudiKernel/EventContext.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "StoreGate/WriteHandle.h"

#include <string>


/**
 * @class ITrigCostMTSvc
 * @brief Pure virtual interface to be implimented by Trig Cost MT service(s)
 */
class ITrigCostMTSvc :  public virtual IService {
public:
  /**
   * @brief Retrieve interface ID
   */
  DeclareInterfaceID( ITrigCostMTSvc, 1, 0);

  virtual ~ITrigCostMTSvc() {}

  /**
   * @brief Disambiguation enum for the start or the end of an algorithm's execution
   */
  enum AuditType { Before, After };

  /**
   * @brief To be used to signal to the service a new event in a given slot, and to send a monitor/don't monitor flag
   */
  virtual StatusCode startEvent(const EventContext&, const bool enableMonitoring) = 0; 

  /**
   * @brief To be used to signal the Before or After status of a named algorithm in a given context
   */
  virtual StatusCode processAlg(const EventContext&, const std::string&, const AuditType) = 0; 

  /**
   * @brief To be used to signal end of an event in a given slot, and to write an output payload to a given handle.
   */
  virtual StatusCode endEvent(const EventContext&, SG::WriteHandle<xAOD::TrigCompositeContainer>&) = 0; 

  /**
   * @brief To be used by external suppliers of data to know if they should spend the CPU to collate their monitoring data
   */
  virtual bool isMonitoredEvent(const EventContext& context) const = 0;

};

#endif // TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
