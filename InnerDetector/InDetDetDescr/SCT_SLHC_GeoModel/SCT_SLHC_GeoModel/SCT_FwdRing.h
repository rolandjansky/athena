/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FWDRING_H
#define SCT_SLHC_GeoModel_SCT_FWDRING_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include "CLHEP/Geometry/Transform3D.h"
#include <vector>

class GeoNameTag;
class GeoTransform;
class GeoLogVol;
class GeoPhysVol;

namespace InDetDDSLHC {

class SCT_FwdModule;

class SCT_FwdRing : public SCT_UniqueComponentFactory{
  
 public:
  
  SCT_FwdRing(const std::string & name, int iWheel, int iRing, double offset);
  
  ~SCT_FwdRing();
  //disable copy and assign
  SCT_FwdRing & operator=(const SCT_FwdRing &) = delete;
  SCT_FwdRing(const SCT_FwdRing &) = delete;

  int    numModules() const {return m_numModules;}
  double moduleStagger()    const {return m_moduleStagger;} 

  double thickness() const {return m_thickness;} 
  double innerRadius() const {return m_innerRadius;} 
  double middleRadius() const {return m_middleRadius;} 
  double outerRadius() const {return m_outerRadius;} 

  int stereoSign() const {return m_stereoSign;}

  virtual GeoVPhysVol* build(SCT_Identifier id) const;

  const SCT_FwdModule* module() const {return m_module;}

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  int     m_iWheel;
  int     m_iRing;
  int     m_numModules;
  int     m_doubleSided;
  double  m_moduleStagger;
  int     m_stereoSign;
  double  m_refStartAngle;
  int     m_refFirstStagger;

  double m_innerRadius;
  double m_middleRadius;
  double m_outerRadius;
  short  m_ringSide;
  double m_length;
  double m_thickness;
  int    m_firstStagger;

  const SCT_FwdModule* m_module;
};
}
#endif //SCT_SLHC_GeoModel_SCT_FWDRING_H
