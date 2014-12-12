/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_OUTERSIDE_H
#define SCT_SLHC_GeoModel_SCT_OUTERSIDE_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;
class GeoTransform;
#include "CLHEP/Vector/ThreeVector.h"

namespace InDetDDSLHC {

class SCT_Sensor;

class SCT_OuterSide: public SCT_UniqueComponentFactory{
public:

  SCT_OuterSide(const std::string & name, int moduleType);
  ~SCT_OuterSide();  
  //disable copy, assign
  SCT_OuterSide & operator=(const SCT_OuterSide &) = delete;
  SCT_OuterSide(const SCT_OuterSide &) = delete;
  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;
  
public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  const SCT_Sensor*  sensor() const {return m_sensor;}

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  int m_moduleType;
  double m_thickness;
  double m_width;
  double m_length;

  double m_safety;

  SCT_Sensor       * m_sensor;

  GeoTransform * m_sensorPos;
};
}
#endif //SCT_SLHC_GeoModel_SCT_OUTERSIDE_H
