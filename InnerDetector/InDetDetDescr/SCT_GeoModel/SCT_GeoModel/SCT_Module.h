/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_MODULE_H
#define SCT_GEOMODEL_SCT_MODULE_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "SCT_GeoModel/SCT_ComponentFactory.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include <memory>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;

class SCT_InnerSide;
class SCT_OuterSide;
class SCT_BaseBoard;

class SCT_Module: public SCT_UniqueComponentFactory

{
public:

  SCT_Module(const std::string & name,
             InDetDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);

  ~SCT_Module(); 
  //Explicitly disallow copy, assign to appease coverity
  SCT_Module(const SCT_Module &) = delete;
  SCT_Module & operator=(const SCT_Module &) = delete;
  
  virtual GeoVPhysVol * build(SCT_Identifier id);
  
public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  double env1Thickness() const {return m_env1Thickness;}
  double env1Width()     const {return m_env1Width;}
  double env1Length()    const {return m_env1Length;}
  double env2Thickness() const {return m_env2Thickness;}
  double env2Width()     const {return m_env2Width;}
  double env2Length()    const {return m_env2Length;}

  const GeoTrf::Vector3D * env1RefPointVector() const {return m_env1RefPointVector.get();}
  const GeoTrf::Vector3D * env2RefPointVector() const {return m_env2RefPointVector.get();}

  double sensorGap()    const {return m_sensorGap;}
  double stereoInner()  const {return m_stereoInner;}
  double stereoOuter()  const {return m_stereoOuter;}
  double stereoAngle()  const {return m_stereoAngle;}
  
  double activeWidth()     const {return m_activeWidth;}

  double baseBoardOffsetY() const {return m_baseBoardOffsetY;}
  double baseBoardOffsetZ() const {return m_baseBoardOffsetZ;}

  const SCT_InnerSide * innerSide() const {return m_innerSide.get();}
  const SCT_OuterSide * outerSide() const {return m_outerSide.get();}
  const SCT_BaseBoard * baseBoard() const {return m_baseBoard.get();}

 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  
  double m_thickness = 0.0;
  double m_width = 0.0;
  double m_length = 0.0;

  double m_env1Thickness = 0.0;
  double m_env1Width = 0.0;
  double m_env1Length = 0.0;
  double m_env2Thickness = 0.0;
  double m_env2Width = 0.0;
  double m_env2Length = 0.0;

  double m_activeWidth = 0.0;

  double m_baseBoardOffsetY = 0.0;
  double m_baseBoardOffsetZ = 0.0;

  double m_sensorGap = 0.0;
  double m_stereoInner = 0.0;
  double m_stereoOuter = 0.0;
  double m_stereoAngle = 0.0;
  int    m_upperSide = 0;
  double m_safety = 0.0;

  std::unique_ptr<SCT_InnerSide> m_innerSide;
  std::unique_ptr<SCT_OuterSide> m_outerSide;
  std::unique_ptr<SCT_BaseBoard> m_baseBoard;

  std::unique_ptr<GeoTrf::Transform3D> m_innerSidePos;
  std::unique_ptr<GeoTrf::Transform3D> m_outerSidePos;
  std::unique_ptr<GeoTrf::Translate3D> m_baseBoardPos;

  std::unique_ptr<GeoTrf::Vector3D> m_env1RefPointVector;
  std::unique_ptr<GeoTrf::Vector3D> m_env2RefPointVector;

};

#endif // SCT_GEOMODEL_SCT_MODULE_H
