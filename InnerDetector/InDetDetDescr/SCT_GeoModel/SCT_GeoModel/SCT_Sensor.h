/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_SENSOR_H
#define SCT_GEOMODEL_SCT_SENSOR_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
namespace InDetDD{class SiDetectorDesign;}

class SCT_Sensor: public SCT_UniqueComponentFactory
{
public:
  SCT_Sensor(const std::string & name,
             InDetDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;} 
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  virtual GeoVPhysVol * build(SCT_Identifier id);
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void makeDesign(); 
 
  const GeoMaterial * m_material = nullptr;
  double m_thickness = 0.0;
  double m_width = 0.0;
  double m_length = 0.0;
  
  const InDetDD::SiDetectorDesign * m_design = nullptr;

  mutable std::atomic_bool m_noElementWarning;
};

#endif // SCT_GEOMODEL_SCT_SENSOR_H
