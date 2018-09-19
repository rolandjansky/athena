 /*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H 1

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/EventContext.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "StoreGate/WriteHandle.h"

#include <string>

/// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ITrigCostMTSvc("ITrigCostMTSvc", 1 , 0);

class ITrigCostMTSvc :  public virtual IInterface {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return(IID_ITrigCostMTSvc); }
  virtual ~ITrigCostMTSvc() {}

  enum AuditType { Before, After };

  virtual StatusCode startEvent(const EventContext&, const bool enableMonitoring) = 0; 

  virtual StatusCode processAlg(const EventContext&, const std::string&, const AuditType) = 0; 

  virtual StatusCode endEvent(const EventContext&, SG::WriteHandle<xAOD::TrigCompositeContainer>&) = 0; 

};

#endif // TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
