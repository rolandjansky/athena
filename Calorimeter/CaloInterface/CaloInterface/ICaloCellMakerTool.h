/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOCELLMAKERTOOL_H
#define CALOINTERFACE_ICALOCELLMAKERTOOL_H


class CaloCellContainer ;

// Includes for Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_ICaloCellMakerTool("ICaloCellMakerTool", 1 , 0);

class ICaloCellMakerTool: virtual public IAlgTool {

public:
  // update theCellContainer, fill more cells  for example
  virtual StatusCode process( CaloCellContainer * theCellContainer)=0 ;

  static const InterfaceID& interfaceID() { return IID_ICaloCellMakerTool;}
};

#endif 
