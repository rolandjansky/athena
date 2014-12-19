/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_MODULE_H
#define SCT_GEOMODEL_SCT_MODULE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

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

  SCT_Module(const std::string & name);

  ~SCT_Module(); 
  //Explicitly disallow copy, assign to appease coverity
  SCT_Module(const SCT_Module &) = delete;
  SCT_Module & operator=(const SCT_Module &) = delete;
  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;
  
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

  CLHEP::Hep3Vector * env1RefPointVector() const {return m_env1RefPointVector;}
  CLHEP::Hep3Vector * env2RefPointVector() const {return m_env2RefPointVector;}

  double sensorGap()    const {return m_sensorGap;}
  double stereoInner()  const {return m_stereoInner;}
  double stereoOuter()  const {return m_stereoOuter;}
  double stereoAngle()  const {return m_stereoAngle;}
  
  double activeWidth()     const {return m_activeWidth;}

  double baseBoardOffsetY() const {return m_baseBoardOffsetY;}
  double baseBoardOffsetZ() const {return m_baseBoardOffsetZ;}

  const SCT_InnerSide * innerSide() const {return m_innerSide;}
  const SCT_OuterSide * outerSide() const {return m_outerSide;}
  const SCT_BaseBoard * baseBoard() const {return m_baseBoard;}

 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  
  double m_thickness;
  double m_width;
  double m_length;

  double m_env1Thickness;
  double m_env1Width;
  double m_env1Length;
  double m_env2Thickness;
  double m_env2Width;
  double m_env2Length;

  double m_activeWidth;

  double m_baseBoardOffsetY;
  double m_baseBoardOffsetZ;

  double m_sensorGap;
  double m_stereoInner;
  double m_stereoOuter;
  double m_stereoAngle;
  //int  m_stereoSign;
  int    m_upperSide;
  double m_safety;

  const SCT_InnerSide * m_innerSide;
  const SCT_OuterSide * m_outerSide;
  const SCT_BaseBoard * m_baseBoard;
  //const SCT_Sensor          * m_sensor; // 14:00 Thu 14th Jul 2005 D.Naito removed.

  HepGeom::Transform3D * m_innerSidePos;
  HepGeom::Transform3D * m_outerSidePos;
  HepGeom::Translate3D * m_baseBoardPos; // 6th Apr 2005 S.Mima

  CLHEP::Hep3Vector * m_env1RefPointVector;
  CLHEP::Hep3Vector * m_env2RefPointVector;

};

#endif // SCT_GEOMODEL_SCT_MODULE_H
