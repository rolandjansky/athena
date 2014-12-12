/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_MODULE_H
#define SCT_SLHC_GeoModel_SCT_MODULE_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;

namespace InDetDDSLHC {

class SCT_InnerSide;
class SCT_OuterSide;
class SCT_BaseBoard;

class SCT_Module: public SCT_UniqueComponentFactory

{
public:

  SCT_Module(const std::string & name, bool doubleSided, bool staveLayout, int moduleType);

  ~SCT_Module();
  //disable copy, assign  
  SCT_Module & operator=(const SCT_Module &) = delete;
  SCT_Module(const SCT_Module &) = delete;
  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;
  
public:
  int    moduleType()const {return m_moduleType;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}


  double stereoAngle()  const {return m_stereoAngle;}

  const SCT_InnerSide * innerSide() const {return m_innerSide;}
  const SCT_OuterSide * outerSide() const {return m_outerSide;}
  const SCT_BaseBoard * baseBoard() const {return m_baseBoard;}
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  bool m_doubleSided;
  bool m_staveLayout;
  int m_moduleType;
  double m_thickness;
  double m_width;
  double m_length;
  double m_stereoAngle;
  double m_interSidesGap;
  const SCT_InnerSide * m_innerSide;
  const SCT_OuterSide * m_outerSide;
  const SCT_BaseBoard * m_baseBoard;
};
}
#endif // SCT_SLHC_GeoModel_SCT_MODULE_H
