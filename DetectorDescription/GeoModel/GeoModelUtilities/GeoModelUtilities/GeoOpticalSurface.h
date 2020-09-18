/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoOpticalSurface_h
#define GeoOpticalSurface_h 1

#include "GeoModelKernel/RCBase.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"
#include <string>


class GeoOpticalSurface : public RCBase
{
 public:
  // Enums ---------
  enum GeoSurfaceType
  {
    dielectric_metal,            // dielectric-metal interface
    dielectric_dielectric,       // dielectric-dielectric interface
    firsov,                      // for Firsov Process
    x_ray                        // for x-ray mirror process
  };
  
  enum GeoOpticalSurfaceFinish
  {
    polished,                    // smooth perfectly polished surface
    polishedfrontpainted,        // smooth top-layer (front) paint
    polishedbackpainted,         // same is 'polished' but with a back-paint
    ground,                      // rough surface
    groundfrontpainted,          // rough top-layer (front) paint
    groundbackpainted            // same as 'ground' but with a back-paint
  };

  enum GeoOpticalSurfaceModel
  {
    glisur,                      // original GEANT3 model
    unified                      // UNIFIED model
  };
  // Enums ---------

  GeoOpticalSurface(const std::string& name,
		    GeoOpticalSurfaceModel model = glisur,
		    GeoOpticalSurfaceFinish finish = polished,
		    GeoSurfaceType type = dielectric_dielectric,
		    double parameter = 1.0);

  ~GeoOpticalSurface();

  // accessor methods
  std::string GetName() const {return m_name;}
  GeoSurfaceType GetType() const {return m_type;};
  GeoOpticalSurfaceFinish GetFinish() const {return m_finish;};
  GeoOpticalSurfaceModel GetModel() const {return m_model;};
  double GetParameter() const {return m_parameter;}

  void SetMaterialPropertiesTable(GeoMaterialPropertiesTable *mpt)
  { m_materialPropertiesTable = mpt; if(mpt) m_materialPropertiesTable->ref(); };

  GeoMaterialPropertiesTable* GetMaterialPropertiesTable()
    { return m_materialPropertiesTable;};
  const GeoMaterialPropertiesTable* GetMaterialPropertiesTable() const
    { return m_materialPropertiesTable;};
 
 private:
  std::string m_name;
  GeoOpticalSurfaceModel m_model;  
  GeoOpticalSurfaceFinish m_finish;
  GeoSurfaceType m_type; 
  double m_parameter;
  GeoMaterialPropertiesTable* m_materialPropertiesTable;
};

#endif 
