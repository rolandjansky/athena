/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// CPW 17/8/06
// Version using new model of services from A. Tricoli
// 
#ifndef SCT_GEOMODEL_SCT_LAYER_H
#define SCT_GEOMODEL_SCT_LAYER_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <memory>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;

class SCT_Bracket;
class SCT_Clamp;
class SCT_CoolingEnd;
class SCT_Flange;
class SCT_Harness;
class SCT_Module;
class SCT_Ski;
class SCT_SkiAux;
class SCT_SkiPowerTape;
class SCT_SupportCyl;
class SCT_FSIEndJewel;
class SCT_FSIScorpion;
class SCT_FSIFibreMask;
class SCT_Identifier;

class SCT_Layer: public SCT_UniqueComponentFactory
{
public:
  SCT_Layer(const std::string & name,
	    int iLayer,
            SCT_Module * module,
            InDetDD::SCT_DetectorManager* detectorManager,
            const SCT_GeometryManager* geometryManager,
            SCT_MaterialManager* materials);

  virtual ~SCT_Layer();
  //Explicitly disallow copy, assign to appease coverity
  SCT_Layer(const SCT_Layer &) = delete;
  SCT_Layer & operator=(const SCT_Layer &) = delete;

  virtual GeoVPhysVol * build(SCT_Identifier id);


public:
  int    skisPerLayer() const {return m_skisPerLayer;}  
  double tilt()         const {return m_tilt;}
  int    stereoSign()   const {return m_stereoSign;}
  double radius()       const {return m_radius;}
  
  double innerRadius() const {return m_innerRadius;}
  double outerRadius() const {return m_outerRadius;}
  double length()      const {return m_cylinderLength;}

  double skiPhiStart() const {return m_skiPhiStart;}

 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  void activeEnvelopeExtent(double & rmin, double & rmax);
  double calcSkiPhiOffset();

  // Layer number
  int    m_iLayer;

  int    m_skisPerLayer = 0;
  double m_tilt = 0.0;
  int    m_stereoSign = 0;
  double m_radius = 0.0;
  double m_safety = 0.0;
  
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_cylinderLength = 0.0;
  double m_activeLength = 0.0;

  double m_innerRadiusActive = 0.0;
  double m_outerRadiusActive = 0.0;

  double m_outerRadiusOfSupport = 0.0;

  double m_skiPhiStart = 0.0;
  double m_skiAuxPhiStart = 0.0;
  
  double m_bracketPhiOffset = 0.0;
  double m_phiRefModule = 0.0;

  bool   m_includeFSI = false;
  int    m_nRepeatEndJewel = 0;
  double m_phiEndJewel = 0.0;
  double m_zEndJewel = 0.0;
  int    m_nRepeatScorpion = 0;
  double m_phiScorpion = 0.0;
  double m_zScorpion = 0.0;

  SCT_Module* m_module;
  std::unique_ptr<SCT_Ski> m_ski;
  std::unique_ptr<SCT_Clamp> m_clamp;
  std::unique_ptr<SCT_CoolingEnd> m_coolingEnd;
  std::unique_ptr<SCT_Bracket> m_bracket;
  std::unique_ptr<SCT_Harness> m_harness;
  std::unique_ptr<SCT_SkiPowerTape> m_skiPowerTape;
  std::unique_ptr<SCT_SkiAux> m_skiAux;
  std::unique_ptr<SCT_Flange> m_flange;
  std::unique_ptr<SCT_SupportCyl> m_supportCyl;
  std::unique_ptr<SCT_FSIEndJewel> m_endJewel;
  std::unique_ptr<SCT_FSIScorpion> m_scorpion;
  std::unique_ptr<SCT_FSIFibreMask> m_fibreMask;

};

#endif // SCT_GEOMODEL_SCT_LAYER_H
