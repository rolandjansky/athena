/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICaloBadChanTool_H
#define ICaloBadChanTool_H

#include "Identifier/Identifier.h"
#include "CaloConditions/CaloBadChannel.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"


class ICaloBadChanTool : public virtual IAlgTool {
public:
					  
  virtual ~ICaloBadChanTool() {}

  virtual CaloBadChannel caloStatus(const EventContext& ctx,
                                    Identifier id) const = 0;

  virtual CaloBadChannel caloStatus(Identifier id) const
  {
    return caloStatus(Gaudi::Hive::currentContext(), id);
  }

  static const InterfaceID& interfaceID() { 
    static const InterfaceID id("ICaloBadChanTool", 1 , 0);
    return id; 
  }

};

#endif
