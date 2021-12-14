/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDDISCSUPPORT_H
#define SCT_GEOMODEL_SCT_FWDDISCSUPPORT_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdDiscSupport : public SCT_SharedComponentFactory
{

public:
  SCT_FwdDiscSupport(const std::string & name, int iWheel,
                     InDetDD::SCT_DetectorManager* detectorManager,
                     const SCT_GeometryManager* geometryManager,
                     SCT_MaterialManager* materials);

  //
  // Methods to return basic and derived parameters. 
  //
  // Retrieve basic parameters
  //
  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double thickness() const {return m_thickness;}

 
private:
  void getParameters();
  virtual GeoVPhysVol * build();

  int m_iWheel;

  // Basic parameters
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_thickness = 0.0;
  const GeoMaterial * m_material = nullptr;
  std::string m_materialName;

};

#endif // SCT_GEOMODEL_SCT_FWDDISCSUPPORT_H

