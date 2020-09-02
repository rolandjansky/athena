/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BLM_Module.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BLMGEOMODEL_BLMMODULE_H
#define BLMGEOMODEL_BLMMODULE_H

#include "BLM_GeoModel/BLM_Wall.h"
#include "BLM_GeoModel/BLM_ModuleParameters.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

class StoredMaterialManager;

  /** @class BLM_Module

  Beam Loss Monitor module builder
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */

class BLM_Module
{
 public:
  GeoPhysVol* Build(const StoredMaterialManager* mat_mgr, const BLM_ModuleParameters* parameters, MsgStream* msg);

};

#endif
