/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDSPINE_H
#define SCT_GEOMODEL_SCT_FWDSPINE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;

class SCT_FwdSpine : public SCT_SharedComponentFactory
{

public:
  SCT_FwdSpine(const std::string & name, int ringType,
               InDetDD::SCT_DetectorManager* detectorManager,
               const SCT_GeometryManager* geometryManager,
               SCT_MaterialManager* materials);

public:

  const GeoMaterial * material() const {return m_material;}
  //Main spine:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  double moduleCenterToEnd() const {return m_moduleCenterToEnd;}

  virtual GeoVPhysVol * build();

private:
  void getParameters();

  int m_ringType;

  const GeoMaterial * m_material = nullptr;
  std::string m_materialName;
  double m_thickness = 0.0;
  double m_width = 0.0;
  double m_length = 0.0;
  double m_moduleCenterToEnd = 0.0;
  double m_moduleCenterToEndTmp = 0.0;
};

#endif // SCT_GEOMODEL_SCT_FWDSPINE_H

