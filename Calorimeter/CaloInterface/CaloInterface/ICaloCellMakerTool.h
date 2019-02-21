/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOCELLMAKERTOOL_H
#define CALOINTERFACE_ICALOCELLMAKERTOOL_H


class CaloCellContainer;
class EventContext;

// Includes for Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h"


class ICaloCellMakerTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (ICaloCellMakerTool, 1, 0);

  virtual StatusCode process ( CaloCellContainer* theCellContainer,
                               const EventContext& ctx ) const = 0;
};

#endif 
