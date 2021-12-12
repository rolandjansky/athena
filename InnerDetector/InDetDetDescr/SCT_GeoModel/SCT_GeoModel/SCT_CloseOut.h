/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_CLOSEOUT_H
#define SCT_GEOMODEL_SCT_CLOSEOUT_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"
#include <string>

class GeoMaterial;

class SCT_CloseOut : public SCT_SharedComponentFactory
{

public:
  SCT_CloseOut(const std::string & name, int iLayer,
               InDetDD::SCT_DetectorManager* detectorManager,
               const SCT_GeometryManager* geometryManager,
               SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double length() const {return m_length;} 

 
private:
  void getParameters();
  virtual GeoVPhysVol * build();

  int m_iLayer;

  const GeoMaterial * m_material = nullptr;
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_length = 0.0;
};

#endif // SCT_GEOMODEL_SCT_CLOSEOUT_H

