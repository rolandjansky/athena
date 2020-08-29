/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_Wall.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BCMGEOMODEL_BCMWALL_H
#define BCMGEOMODEL_BCMWALL_H

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"

class StoredMaterialManager;

  /** @class BCM_Wall

  Beam Condition Monitor wall builder
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  
  
class BCM_Wall
{
 public:
  GeoPhysVol* Build(double halfX, double halfY, double thick, std::string logname, const GeoMaterial* material);
  GeoPhysVol* Build(double halfX, double halfY, double thick_one, double thick_two, const GeoMaterial* material_one, const GeoMaterial* material_two, const StoredMaterialManager* mat_mgr);
  GeoPhysVol* Build(double halfX, double halfY, double thick_one, double thick_two, double thick_three, const GeoMaterial* material_one, const GeoMaterial* material_two, const GeoMaterial* material_three, const StoredMaterialManager* mat_mgr);
};

#endif
