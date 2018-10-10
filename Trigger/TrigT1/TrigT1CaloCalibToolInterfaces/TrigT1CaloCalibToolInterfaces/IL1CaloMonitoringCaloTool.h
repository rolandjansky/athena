/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: Peter Faulkner
//  *                                                                         *

//
// Pure abstract base class interface to L1CaloMonitoringCaloTool
//

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMONITORINGCALOTOOL_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMONITORINGCALOTOOL_H_

// Framework include(s):
#include "AsgTools/IAsgTool.h"

class Identifier;
class StatusCode;

namespace LVL1{

  class IL1CaloMonitoringCaloTool : virtual public IAlgTool{
    
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( LVL1::IL1CaloMonitoringCaloTool )    
    
    public:

      virtual StatusCode loadCaloCells() = 0;
      virtual float et(const Identifier& TTid) const = 0;
      virtual float caloQuality(const Identifier& TTid) const = 0;
  };
} // end of namespace
#endif
