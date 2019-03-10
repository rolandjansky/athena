/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDDISCFIXATION_H
#define SCT_GEOMODEL_SCT_FWDDISCFIXATION_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdDiscFixation : public SCT_SharedComponentFactory
{

public:
  SCT_FwdDiscFixation(const std::string & name,
                      InDetDD::SCT_DetectorManager* detectorManager,
                      const SCT_GeometryManager* geometryManager,
                      SCT_MaterialManager* materials);

  //
  // Methods to return basic and derived parameters. 
  //
  const GeoMaterial * material() const {return m_material;}
  double radius() const {return m_radius;} 
  double thickness() const {return m_thickness;}


private:
  void getParameters();
  virtual GeoVPhysVol * build();

  // Basic parameters
  double m_radius;
  double m_thickness;
  const GeoMaterial * m_material;
  std::string m_materialName;

};

#endif // SCT_GEOMODEL_SCT_FWDDISCFIXATION_H

