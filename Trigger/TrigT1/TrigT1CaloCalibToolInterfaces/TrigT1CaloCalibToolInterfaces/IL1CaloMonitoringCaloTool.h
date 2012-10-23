//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: Peter Faulkner
//  *                                                                         *
//  ***************************************************************************/

//
// Pure abstract base class interface to L1CaloMonitoringCaloTool
//

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMONITORINGCALOTOOL_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMONITORINGCALOTOOL_H_

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"

class Identifier;
class StatusCode;

namespace LVL1{

 static const InterfaceID IID_IL1CaloMonitoringCaloTool("LVL1::IL1CaloMonitoringCaloTool",1,0);

  class IL1CaloMonitoringCaloTool : virtual public IAlgTool{
    public:
      static const InterfaceID& interfaceID();

      virtual ~IL1CaloMonitoringCaloTool(){};

      virtual StatusCode loadCaloCells() = 0;
      virtual float et(const Identifier& TTid) const = 0;
      virtual float caloQuality(const Identifier& TTid) const = 0;
  };

  inline const InterfaceID& IL1CaloMonitoringCaloTool::interfaceID(){
    return IID_IL1CaloMonitoringCaloTool;
  }
} // end of namespace
#endif
