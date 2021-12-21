/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDRINGCOOLING_H
#define SCT_GEOMODEL_SCT_FWDRINGCOOLING_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdRingCooling : public SCT_SharedComponentFactory
{

public:
  SCT_FwdRingCooling(const std::string & name, int ringType,
                     InDetDD::SCT_DetectorManager* detectorManager,
                     const SCT_GeometryManager* geometryManager,
                     SCT_MaterialManager* materials);

  //
  // Methods to return basic and derived parameters. 
  //
  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double thickness()   const {return m_thickness;}
 
 
private:
  void getParameters();
  virtual GeoVPhysVol * build();

  int m_ringType;

  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_thickness = 0.0;
  const GeoMaterial * m_material = nullptr;
  std::string m_materialName;
  
  int m_numModules = 0;

};

#endif // SCT_GEOMODEL_SCT_FWDRINGCOOLING_H

