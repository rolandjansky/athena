/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// 14th Aug 2005 S.Mima modified.
//
#ifndef SCT_GEOMODEL_SCT_DOGLEG_H
#define SCT_GEOMODEL_SCT_DOGLEG_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;


class SCT_Dogleg: public SCT_SharedComponentFactory

{

public:
  SCT_Dogleg(const std::string & name,
             InDetDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}
  
private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material = nullptr;
  double m_thickness = 0.0;
  double m_width = 0.0;
  double m_length = 0.0;

};

#endif // SCT_GEOMODEL_SCT_DOGLEG_H
