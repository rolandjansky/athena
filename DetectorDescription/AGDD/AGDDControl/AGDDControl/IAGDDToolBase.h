/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDCONTROL_IAGDDToolBase_H
#define AGDDCONTROL_IAGDDToolBase_H

#include "GaudiKernel/IAlgTool.h"

//  Interface to define an AGDD-constructing tool to be used from AGDD2GeoSvc

class IAGDDToolBase: virtual public IAlgTool
{
public:
  virtual ~IAGDDToolBase() {}
  DeclareInterfaceID(IAGDDToolBase,1,0);

  virtual StatusCode construct() = 0;
};

#endif
