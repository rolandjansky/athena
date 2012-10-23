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

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOFCAL23CELLS2RXMAPPINGTOOL_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOFCAL23CELLS2RXMAPPINGTOOL_H_

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include <vector>

namespace LVL1 {

  static const InterfaceID IID_IL1CaloFcal23Cells2RxMappingTool("LVL1::IL1CaloFcal23Cells2RxMappingTool",1,0);

  class IL1CaloFcal23Cells2RxMappingTool : virtual public IAlgTool{
    public:
      static const InterfaceID& interfaceID();

      virtual ~IL1CaloFcal23Cells2RxMappingTool(){};

      virtual unsigned int offlineCell2RxId(const unsigned int& cellId) const = 0;
      virtual unsigned int onlineCell2RxId(const unsigned int& cellId) const = 0;

  };

  inline const InterfaceID& IL1CaloFcal23Cells2RxMappingTool::interfaceID(){
    return IID_IL1CaloFcal23Cells2RxMappingTool;
  }  

} // end of namespace

#endif
