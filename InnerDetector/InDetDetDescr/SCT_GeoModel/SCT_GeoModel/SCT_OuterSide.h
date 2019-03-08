/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_OUTERSIDE_H
#define SCT_GEOMODEL_SCT_OUTERSIDE_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "SCT_GeoModel/SCT_ComponentFactory.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;
class GeoTransform;

class SCT_Identifier;
class SCT_Hybrid;
class SCT_Pigtail;
class SCT_Sensor;

class SCT_OuterSide: public SCT_UniqueComponentFactory

{
public:

  SCT_OuterSide(const std::string & name,
                InDetDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);

  ~SCT_OuterSide(); 
  //Explicitly disallow copy, assign to appease coverity
  SCT_OuterSide(const SCT_OuterSide &) = delete;
  SCT_OuterSide & operator=(const SCT_OuterSide &) = delete;
  virtual GeoVPhysVol * build(SCT_Identifier id);
  
public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  const GeoTrf::Vector3D * env1RefPointVector() const {return m_env1RefPointVector;}
  const GeoTrf::Vector3D * env2RefPointVector() const {return m_env2RefPointVector;}

  const SCT_Hybrid       * hybrid() const {return m_hybrid;}
  const SCT_Pigtail      * pigtail()      const {return m_pigtail;}
  const SCT_Sensor             * sensor()       const {return m_sensor;}

  double hybridOffsetX() const {return m_hybridOffsetX;}
  double hybridOffsetZ() const {return m_hybridOffsetZ;}

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  double m_thickness;
  double m_width;
  double m_length;

  double m_hybridOffsetX;
  double m_hybridOffsetZ;
  double m_safety;

  SCT_Hybrid  * m_hybrid;
  SCT_Pigtail * m_pigtail;
  SCT_Sensor        * m_sensor;

  GeoTransform * m_hybridPos;
  GeoTransform * m_pigtailPos;
  GeoTransform * m_sensorPos;
  GeoTrf::Vector3D * m_env1RefPointVector;
  GeoTrf::Vector3D * m_env2RefPointVector;

};

#endif // SCT_GEOMODEL_SCT_OUTERSIDE_H
