/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

//
// Pure abstract base class interface to the L1CaloFcal23Cells2RxMappingTool
//

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOFCAL23CELLS2RXMAPPINGTOOL_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOFCAL23CELLS2RXMAPPINGTOOL_H_

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include <vector>

namespace LVL1 {

  class IL1CaloFcal23Cells2RxMappingTool : virtual public asg::IAsgTool{
    
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( LVL1::IL1CaloFcal23Cells2RxMappingTool )  
    
    public:

      virtual unsigned int offlineCell2RxId(const unsigned int& cellId) const = 0;
      virtual unsigned int onlineCell2RxId(const unsigned int& cellId) const = 0;

  };
} // end of namespace

#endif
