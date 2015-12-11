/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_SENSOR_H
#define SCT_SLHC_GeoModel_SCT_SENSOR_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
namespace InDetDD{class SiDetectorDesign;}

namespace InDetDDSLHC {

class SCT_Sensor: public SCT_UniqueComponentFactory{
 public:
  SCT_Sensor(const std::string & name, int moduleType);
  
 public:
  const GeoMaterial * material() const {return m_material;} 
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}
  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;
  
 private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void makeDesign(); 
  
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_length;
  double m_stripLength;
  double m_segmentGap;
  double m_subSensorLength;
  int m_numSegments;
  int m_moduleType;
  int m_chargeCarrier;

  GeoLogVol * m_subSensorLog;

  InDetDD::SiDetectorDesign * m_design;
};
}
#endif //SCT_SLHC_GeoModel_SCT_SENSOR_H
