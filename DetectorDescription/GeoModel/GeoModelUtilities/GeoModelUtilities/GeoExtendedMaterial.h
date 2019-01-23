/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoExtendedMaterial_h
#define GeoExtendedMaterial_h 1

#include "GeoModelKernel/GeoMaterial.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

enum GeoMaterialState { stateUndefined, stateSolid, stateLiquid, stateGas };

class GeoExtendedMaterial : public GeoMaterial
{
 public:
  GeoExtendedMaterial(const std::string &Name,
		      double Density,
		      GeoMaterialState State = stateUndefined,
		      double Temperature = Gaudi::Units::STP_Temperature,
		      double Pressure  = Gaudi::Units::STP_Pressure);
  
  virtual ~GeoExtendedMaterial();
  
  const GeoMaterialState& getState() const;
  const double& getTemperature() const;
  const double& getPressure() const;
  
  void SetMaterialPropertiesTable(GeoMaterialPropertiesTable* MPT);
  GeoMaterialPropertiesTable* GetMaterialPropertiesTable() const;

 private:
  GeoExtendedMaterial(const GeoExtendedMaterial &right);
  GeoExtendedMaterial & operator=(const GeoExtendedMaterial &right);
  
  GeoMaterialState m_state;
  double m_temperature;
  double m_pressure;

  GeoMaterialPropertiesTable* m_properties;
};


inline const GeoMaterialState& GeoExtendedMaterial::getState() const
{
  return m_state;
}

inline const double& GeoExtendedMaterial::getTemperature() const
{
  return m_temperature;
}
 	  	 
inline const double& GeoExtendedMaterial::getPressure() const
{
  return m_pressure;
}

inline void GeoExtendedMaterial::SetMaterialPropertiesTable(GeoMaterialPropertiesTable* MPT)
{
  m_properties = MPT;
  m_properties->ref();
}

inline GeoMaterialPropertiesTable* GeoExtendedMaterial::GetMaterialPropertiesTable() const
{
  return m_properties;
}

#endif
