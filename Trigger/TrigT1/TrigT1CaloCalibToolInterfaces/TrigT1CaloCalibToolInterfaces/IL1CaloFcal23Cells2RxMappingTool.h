//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  ***************************************************************************/

//
// Pure abstract base class interface to the L1CaloFcal23Cells2RxMappingTool
//

#ifndef TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOFCAL23CELLS2RXMAPPINGTOOL_H
#define TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOFCAL23CELLS2RXMAPPINGTOOL_H

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
