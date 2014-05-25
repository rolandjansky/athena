/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICaloBadChanTool_H
#define ICaloBadChanTool_H

#include "Identifier/Identifier.h"
#include "CaloConditions/CaloBadChannel.h"
#include "GaudiKernel/IAlgTool.h"

class ICaloBadChanTool : public virtual IAlgTool {
public:
					  
  virtual ~ICaloBadChanTool() {}
  
  virtual CaloBadChannel caloStatus( Identifier id) const = 0;

  static const InterfaceID& interfaceID() { 
    static const InterfaceID id("ICaloBadChanTool", 1 , 0);
    return id; 
  }

};

#endif
