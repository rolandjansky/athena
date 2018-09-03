 /*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/EventContext.h"

#include <string>

/// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ITrigCostMTSvc("ITrigCostMTSvc", 1 , 0);

class ITrigCostMTSvc :  public virtual IInterface {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return(IID_ITrigCostMTSvc); }
  virtual ~ITrigCostMTSvc() {}

  enum AuditType { Before, After };

  virtual StatusCode processAlg(const std::string&, const EventContext& context, const AuditType type) = 0; 

  virtual StatusCode endEvent(const EventContext& context) = 0; 

};

#endif // TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
