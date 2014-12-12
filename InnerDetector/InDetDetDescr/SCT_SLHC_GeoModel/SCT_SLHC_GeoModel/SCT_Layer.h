/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_LAYER_H
#define SCT_SLHC_GeoModel_SCT_LAYER_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;

namespace InDetDDSLHC {

class SCT_Module;
class SCT_Ski;

class SCT_Layer: public SCT_UniqueComponentFactory{
public:
  SCT_Layer(const std::string & name,
		  int iLayer,
		  const SCT_Module * module,
		  double cylLength);

  //new constructor
  SCT_Layer(const std::string & name,
	    int iLayer,
	    double cylLength);
	    
	//disable copy, assign
	SCT_Layer & operator=(const SCT_Layer &) = delete;
	SCT_Layer(const SCT_Layer &) = delete;

  ~SCT_Layer();

  virtual GeoVPhysVol * build(SCT_Identifier id) const;


public:
  int    skisPerLayer() const {return m_skisPerLayer;}  
  double tilt()         const {return m_tilt;}
  int    stereoSign()   const {return m_stereoSign;}
  double radius()       const {return m_radius;}
  double cylLength()    const {return m_cylLength;}
  double innerRadius()  const {return m_innerRadius;}
  double outerRadius()  const {return m_outerRadius;}
  double skiPhiStart()  const {return m_skiPhiStart;}

 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  void activeEnvelopeExtent(double & rmin, double & rmax);
  double calcSkiPhiOffset();
  // Layer number
  int    m_iLayer;
  bool   m_doubleSided;
  int   m_staveLayout;
  int    m_moduleType;
  int    m_modulesPerSki;
  int    m_skisPerLayer;
  double m_tilt;
  int    m_stereoSign;
  double m_radius;
  double m_safety;
  
  double m_cylLength;
  double m_innerRadius;
  double m_outerRadius;

  double m_activeInnerRadius;
  double m_activeOuterRadius;
  double m_skiPhiStart;

  const SCT_Module* m_module;
  const SCT_Ski* m_ski;
  //support stuff
  const GeoMaterial* m_supportMaterial;
  double m_supportInnerRadius;
  double m_supportOuterRadius;
};
}
#endif // SCT_GEOMODEL_SCT_LAYER_H
