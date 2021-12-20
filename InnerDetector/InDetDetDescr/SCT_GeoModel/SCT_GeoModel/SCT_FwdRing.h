/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDRING_H
#define SCT_GEOMODEL_SCT_FWDRING_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class SCT_FwdModule;
class SCT_FwdCoolingBlock;

#include <memory>
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
  int     m_numModules = 0;
  double  m_moduleStagger = 0.0;
  double  m_safety = 0.0;
  int     m_identifier = 0;
  int     m_ringSide = 0;
  int     m_stereoSign = 0;
  double  m_refStartAngle = 0.0;
  int     m_refFirstStagger = 0;
  double  m_ringOffset = 0.0;
  double  m_discSupportThickness = 0.0;

  // Derived parameters
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_thickness = 0.0;
  double m_thicknessInner = 0.0;
  double m_thicknessOuter = 0.0;
  double m_startAngle = 0.0;
  int    m_firstStagger = 0;
  bool   m_discRotated = false;
  int    m_moduleZero = 0;

  double m_moduleServicesHiRPos = 0.0;
  double m_moduleServicesLoRPos = 0.0;
  double m_moduleServicesHiZPos = 0.0;
  double m_moduleServicesLoZPos = 0.0;

  double m_moduleServicesHiOuterZPos = 0.0;
  double m_moduleServicesLoOuterZPos = 0.0;

  double m_maxModuleServicesBaseToRingCenter = 0.0;

  // Child detector elements
  SCT_FwdModule* m_module;
  std::unique_ptr<SCT_FwdCoolingBlock> m_coolingBlockHiMain;
  std::unique_ptr<SCT_FwdCoolingBlock> m_coolingBlockHiSec;
  std::unique_ptr<SCT_FwdCoolingBlock> m_coolingBlockLoMain;
  std::unique_ptr<SCT_FwdCoolingBlock> m_coolingBlockLoSec;

  GeoPhysVol * m_moduleServicesHi = nullptr;
  GeoPhysVol * m_moduleServicesLo = nullptr;

};

#endif // SCT_GEOMODEL_SCT_FWDRING_H
