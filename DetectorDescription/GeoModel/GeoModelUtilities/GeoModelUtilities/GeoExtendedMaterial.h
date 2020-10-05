/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoExtendedMaterial_h
#define GeoExtendedMaterial_h 1

#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

// Physical constants
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::STP_Temperature'
#else
  #include "GaudiKernel/PhysicalConstants.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::STP_Temperature'
#endif


enum GeoMaterialState { stateUndefined, stateSolid, stateLiquid, stateGas };

class GeoExtendedMaterial : public GeoMaterial
{
 public:
  GeoExtendedMaterial(const std::string &Name,
		      double Density,
		      GeoMaterialState State = stateUndefined,
		      double Temperature = SYSTEM_OF_UNITS::STP_Temperature,
		      double Pressure  = SYSTEM_OF_UNITS::STP_Pressure);
  
  virtual ~GeoExtendedMaterial();
  
  const GeoMaterialState& getState() const;
  const double& getTemperature() const;
  const double& getPressure() const;
  
  void SetMaterialPropertiesTable(GeoMaterialPropertiesTable* MPT);
  GeoMaterialPropertiesTable* GetMaterialPropertiesTable();
  const GeoMaterialPropertiesTable* GetMaterialPropertiesTable() const;

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

inline GeoMaterialPropertiesTable* GeoExtendedMaterial::GetMaterialPropertiesTable()
{
  return m_properties;
}

inline const GeoMaterialPropertiesTable* GeoExtendedMaterial::GetMaterialPropertiesTable() const
{
  return m_properties;
}

#endif
