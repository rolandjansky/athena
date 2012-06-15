/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoExtendedMaterial_h
#define GeoExtendedMaterial_h 1

#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

enum GeoMaterialState { stateUndefined, stateSolid, stateLiquid, stateGas };

class GeoExtendedMaterial : public GeoMaterial
{
 public:
  GeoExtendedMaterial(const std::string &Name,
		      double Density,
		      GeoMaterialState State = stateUndefined,
		      double Temperature = CLHEP::STP_Temperature,
		      double Pressure  = CLHEP::STP_Pressure);
  
  virtual ~GeoExtendedMaterial();
  
  const GeoMaterialState& getState() const;
  const double& getTemperature() const;
  const double& getPressure() const;
  
  void SetMaterialPropertiesTable(GeoMaterialPropertiesTable* MPT);
  GeoMaterialPropertiesTable* GetMaterialPropertiesTable() const;

 private:
  GeoExtendedMaterial(const GeoExtendedMaterial &right);
  GeoExtendedMaterial & operator=(const GeoExtendedMaterial &right);
  
  GeoMaterialState state;
  double temperature;
  double pressure;

  GeoMaterialPropertiesTable* properties;
};


inline const GeoMaterialState& GeoExtendedMaterial::getState() const
{
  return state;
}

inline const double& GeoExtendedMaterial::getTemperature() const
{
  return temperature;
}
 	  	 
inline const double& GeoExtendedMaterial::getPressure() const
{
  return pressure;
}

inline void GeoExtendedMaterial::SetMaterialPropertiesTable(GeoMaterialPropertiesTable* MPT)
{
  properties = MPT;
  properties->ref();
}

inline GeoMaterialPropertiesTable* GeoExtendedMaterial::GetMaterialPropertiesTable() const
{
  return properties;
}

#endif
