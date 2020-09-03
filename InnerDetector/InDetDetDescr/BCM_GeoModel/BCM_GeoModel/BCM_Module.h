/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_Module.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BCMGEOMODEL_BCMMODULE_H
#define BCMGEOMODEL_BCMMODULE_H

#include "BCM_GeoModel/BCM_Wall.h"
#include "BCM_GeoModel/BCM_ModuleParameters.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

class StoredMaterialManager;

  /** @class BCM_Module 

  Beam Condition Monitor module builder
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  

class BCM_Module
{
 public:
  GeoPhysVol* Build(const StoredMaterialManager* mat_mgr, const BCM_ModuleParameters* parameters, MsgStream* msg);
};

#endif
