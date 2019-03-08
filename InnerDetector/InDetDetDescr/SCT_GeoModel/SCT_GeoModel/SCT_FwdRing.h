/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDRING_H
#define SCT_GEOMODEL_SCT_FWDRING_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class SCT_FwdModule;
class SCT_FwdCoolingBlock;

#include <vector>

class GeoNameTag;
class GeoTransform;
class GeoLogVol;
class GeoPhysVol;

class SCT_FwdRing : public SCT_UniqueComponentFactory
{

public:
  SCT_FwdRing(const std::string & name, 
	      SCT_FwdModule * module, 
	      int iWheel,
	      int iRing,
	      int ec,
              InDetDD::SCT_DetectorManager* detectorManager,
              const SCT_GeometryManager* geometryManager,
              SCT_MaterialManager* materials);

  ~SCT_FwdRing();
  
  //explicitly disallow copy, assignment to appease coverity
  SCT_FwdRing(const SCT_FwdRing &) = delete;
  SCT_FwdRing & operator=(const SCT_FwdRing &) = delete;

  // Ring type
  //int ringType() const {return m_ringType;}  
  //int ringType() const {return m_module->ringType();}

  //
  // Retrieve parameters
  //
  int    numModules() const {return m_numModules;}
  double moduleStagger()    const {return m_moduleStagger;} 
  int    identifier() const {return m_identifier;}
  int    ringSide()   const {return m_ringSide;}
  double ringOffset() const {return m_ringOffset;}

  double thickness() const {return m_thickness;} 
  double thicknessInner() const {return m_thicknessInner;} 
  double thicknessOuter() const {return m_thicknessOuter;} 
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 

  int stereoSign() const {return m_stereoSign;}

  bool discRotated() const {return m_discRotated;}
 
  virtual GeoVPhysVol * build(SCT_Identifier id);

  // Child elements
  const SCT_FwdModule* module() const {return m_module;}

private:
  void makeModuleServices();
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  int    m_iWheel;
  int    m_iRing;
  int    m_endcap;

  // Basic parameters
  int     m_numModules;
  double  m_moduleStagger;
  double  m_safety;
  int     m_identifier;
  int     m_ringSide;
  int     m_stereoSign;
  double  m_refStartAngle;
  int     m_refFirstStagger;
  double  m_ringOffset;
  double  m_discSupportThickness;

  // Derived parameters
  double m_innerRadius;
  double m_outerRadius;
  double m_thickness;
  double m_thicknessInner;
  double m_thicknessOuter;
  double m_startAngle;
  int    m_firstStagger;
  bool   m_discRotated;
  int    m_moduleZero;

  double m_moduleServicesHiRPos;
  double m_moduleServicesLoRPos;
  double m_moduleServicesHiZPos;
  double m_moduleServicesLoZPos;

  double m_moduleServicesHiOuterZPos;
  double m_moduleServicesLoOuterZPos;

  double m_maxModuleServicesBaseToRingCenter;

  // Child detector elements
  SCT_FwdModule* m_module;
  SCT_FwdCoolingBlock * m_coolingBlockHiMain;
  SCT_FwdCoolingBlock * m_coolingBlockHiSec;
  SCT_FwdCoolingBlock * m_coolingBlockLoMain;
  SCT_FwdCoolingBlock * m_coolingBlockLoSec;

  GeoPhysVol * m_moduleServicesHi;
  GeoPhysVol * m_moduleServicesLo;

};

#endif // SCT_GEOMODEL_SCT_FWDRING_H
