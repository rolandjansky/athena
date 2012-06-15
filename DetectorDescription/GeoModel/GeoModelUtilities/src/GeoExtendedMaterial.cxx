/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

GeoExtendedMaterial::GeoExtendedMaterial(const std::string &Name,
					 double Density,
					 GeoMaterialState State,
					 double Temperature,
					 double Pressure):
  GeoMaterial(Name,Density),
  state(State),
  temperature(Temperature),
  pressure(Pressure),
  properties(0)
{
}

GeoExtendedMaterial::~GeoExtendedMaterial()
{
  properties->unref();
}

