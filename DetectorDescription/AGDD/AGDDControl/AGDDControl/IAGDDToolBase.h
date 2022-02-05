/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDCONTROL_IAGDDToolBase_H
#define AGDDCONTROL_IAGDDToolBase_H

#include "GaudiKernel/IAlgTool.h"
#include "CxxUtils/checker_macros.h"

//  Interface to define an AGDD-constructing tool to be used from AGDD2GeoSvc

class IAGDDToolBase: virtual public IAlgTool
{
public:
  virtual ~IAGDDToolBase() {}
  DeclareInterfaceID(IAGDDToolBase,1,0);

  virtual StatusCode construct ATLAS_NOT_THREAD_SAFE () = 0;
};

#endif
