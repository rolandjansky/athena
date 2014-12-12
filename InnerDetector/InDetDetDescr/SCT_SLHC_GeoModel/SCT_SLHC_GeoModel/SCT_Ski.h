/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_SKI_H
#define SCT_SLHC_GeoModel_SCT_SKI_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"

#include "CLHEP/Geometry/Transform3D.h"

#include <string>
#include <vector>

class GeoMaterial;
class GeoVPhysVol;
class GeoPhysVol;
class GeoLogVol;
class GeoNameTag;
class GeoTransform;
#include "CLHEP/Vector/ThreeVector.h"

namespace InDetDDSLHC {

class SCT_Module;
class SCT_StaveSupport;

class SCT_Ski: public SCT_UniqueComponentFactory{
 public:
  SCT_Ski(const std::string & name,
	  int modulesPerSki,
	  int layer,
	  int stereoSign,
	  double length, 
	  const SCT_Module* module);

  ~SCT_Ski();
  //disable copy, assign
  SCT_Ski & operator=(const SCT_Ski &) = delete;
  SCT_Ski(const SCT_Ski &) = delete;
  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;

 public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  double zPos(int i)       const {return m_zPos[i];}
  int    identifier(int i) const {return m_id[i];}
  double radialSep()       const {return m_radialSep;}
  int    firstStagger()    const {return m_firstStagger;}
  int    modulesPerSki()   const {return m_modulesPerSki;}

  int    stereoSign()   const {return m_stereoSign;}

  const SCT_Module * module() const {return m_module;}   

 private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void placeModule(GeoPhysVol * ski, SCT_Identifier id, int iModule, int side,
		   double xModulePos,  double zModulePos, double stereoAngle, bool flip) const;  
  double m_radialSep;
  int    m_firstStagger;
  int    m_modulesPerSki;
  double m_safety;
  int    m_layer;
  std::vector<double> m_zPos;
  std::vector<int>    m_id;
  int    m_stereoSign;
  double m_stereoOuter;
  double m_stereoInner;
  int   m_staveLayout;
  double m_length;
  double m_width;
  double m_thickness;
  double m_cylInnerZMin;
  
  const SCT_Module* m_module;
  const SCT_StaveSupport* m_staveSupport;
  const SCT_StaveSupport* m_staveSupport_pos;
  const SCT_StaveSupport* m_staveSupport_neg;
};
}
#endif // SCT_SLHC_GeoModel_SCT_SKI_H
