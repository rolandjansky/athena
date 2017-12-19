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
// Pure abstract base class interface to L1CaloLArTowerEnergy
//

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOLARTOWERENERGY_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOLARTOWERENERGY_H_


// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"

class CaloCellContainer;
class Identifier;

namespace LVL1{

  class IL1CaloLArTowerEnergy : virtual public asg::IAsgTool
  {      
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( LVL1::IL1CaloLArTowerEnergy )
    
    public:
      virtual bool initL1CaloLArTowerEnergy(const CaloCellContainer& cellContainer, const TriggerTowerCollection &triggerTowerCollection) = 0;
      virtual float EtLArg(const Identifier& TTid) const = 0;
      virtual bool hasMissingFEB(const Identifier& TTid) const = 0;

    protected:
      virtual void reset() = 0;
      virtual double IDeta(const Identifier& TTid) const = 0;
  };
} // end of namespace
#endif
