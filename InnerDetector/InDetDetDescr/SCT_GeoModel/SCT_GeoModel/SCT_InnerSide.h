/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// 13rd Mar 2005 D.Naito
// 28th Mar 2005 S.Mima
// Wed 15th Jun 2005 D.Naito modified.
//
#ifndef SCT_GEOMODEL_SCT_INNERSIDE_H
#define SCT_GEOMODEL_SCT_INNERSIDE_H

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
class SCT_Sensor;

class SCT_InnerSide: public SCT_UniqueComponentFactory

{
public:

  SCT_InnerSide(const std::string & name,
                InDetDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);
  
  //explicitly disallow copy, assign, to appease coverity
  SCT_InnerSide(const SCT_InnerSide &) = delete;
  SCT_InnerSide & operator=(const SCT_InnerSide &) = delete;

  ~SCT_InnerSide();  
  virtual GeoVPhysVol * build(SCT_Identifier id);
  
public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  const GeoTrf::Vector3D * env1RefPointVector() const {return m_env1RefPointVector;}
  const GeoTrf::Vector3D * env2RefPointVector() const {return m_env2RefPointVector;}
  // *** End of modified lines. ------------------ (00)*********************************


  const SCT_Hybrid       * hybrid() const {return m_hybrid;}
  const SCT_Sensor       * sensor() const {return m_sensor;}

  double hybridOffsetX() const {return m_hybridOffsetX;}
  //double hybridOffsetY() const {return m_hybridOffsetY;} // 16:30 Wed 15th Jun 2005 D.Naito removed.
  double hybridOffsetZ() const {return m_hybridOffsetZ;}


private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  double m_thickness;
  double m_width;
  double m_length;

  double m_hybridOffsetX;
  //double m_hybridOffsetY; // 16:30 Wed 15th Jun 2005 D.Naito removed.
  double m_hybridOffsetZ;
  double m_safety;

  SCT_Hybrid       * m_hybrid;
  SCT_Sensor       * m_sensor;

  GeoTransform * m_hybridPos;
  GeoTransform * m_sensorPos;

  // *** 16:30 Wed 15th Jun 2005 D.Naito modified. (02)*********************************
  // *** -->>                                      (02)*********************************
  GeoTrf::Vector3D * m_env1RefPointVector;
  GeoTrf::Vector3D * m_env2RefPointVector;
  // *** End of modified lines. ------------------ (02)*********************************

};

#endif // SCT_GEOMODEL_SCT_INNERSIDE_H
