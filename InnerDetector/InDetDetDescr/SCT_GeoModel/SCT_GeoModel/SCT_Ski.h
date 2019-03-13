/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_SKI_H
#define SCT_GEOMODEL_SCT_SKI_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include "GeoModelKernel/GeoDefinitions.h"


#include <string>
#include <vector>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;
class GeoNameTag;
class GeoTransform;

class SCT_Identifier;
class SCT_Module;
class SCT_Dogleg;
class SCT_CoolingBlock;
class SCT_CoolingPipe;

class SCT_Ski: public SCT_UniqueComponentFactory

{
public:
  SCT_Ski(const std::string & name,
          SCT_Module * module,
          int stereoSign,
          double tilt,
          double length,
          InDetDD::SCT_DetectorManager* detectorManager,
          const SCT_GeometryManager* geometryManager,
          SCT_MaterialManager* materials);

  ~SCT_Ski();
  virtual GeoVPhysVol * build(SCT_Identifier id);

public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  double zPos(int i)       const {return m_zPos[i];}
  int    identifier(int i) const {return m_id[i];}
  double radialSep()       const {return m_radialSep;}
  int    firstStagger()    const {return m_firstStagger;}
  int    modulesPerSki()   const {return m_modulesPerSki;}

  double tilt() const {return m_tilt;}
  int    stereoSign()   const {return m_stereoSign;}

  const SCT_Module * module() const {return m_module;}   
  const SCT_Dogleg * dogleg() const {return m_dogleg;}   
  const SCT_CoolingBlock * coolingBlock() const {return m_coolingBlock;}   
  const SCT_CoolingPipe * coolingPipe() const {return m_coolingPipe;}   

  const GeoTransform * getRefPointTransform() const {return m_refPointTransform;}
  const GeoTrf::Vector3D * env1RefPointVector() const {return m_env1RefPointVector;}
  const GeoTrf::Vector3D * env2RefPointVector() const {return m_env2RefPointVector;}
  double env1Thickness() const {return m_env1Thickness;}
  double env1Width()     const {return m_env1Width;}
  double env2Thickness() const {return m_env2Thickness;}
  double env2Width()     const {return m_env2Width;}
  double coolingBlockOffsetX() const {return m_coolingBlockOffsetX;}
  double coolingBlockOffsetY() const {return m_coolingBlockOffsetY;}
  double coolingBlockOffsetZ() const {return m_coolingBlockOffsetZ;}
  double coolingPipeOffsetX() const {return m_coolingPipeOffsetX;}
  double coolingPipeOffsetY() const {return m_coolingPipeOffsetY;}

  double doglegOffsetX() const {return m_doglegOffsetX;}
  double doglegOffsetY() const {return m_doglegOffsetY;}

private:
  // Copy and assignment operations not supported, so are private
  SCT_Ski(const SCT_Ski& ski);
  SCT_Ski& operator = (const SCT_Ski& ski);

  void getParameters();
  virtual const GeoLogVol * preBuild();
  
  double m_radialSep;
  int    m_firstStagger;
  int    m_modulesPerSki;
  double m_safety;

  std::vector<double> m_zPos;
  std::vector<int>    m_id;

  int    m_stereoSign;
  double m_tilt;

  double m_length;
  double m_width;
  double m_thickness;
  

  // These are currently all set to zero.
  double m_doglegPhiOffset;
  double m_doglegRadialOffset;
  double m_coolingBlockPhiOffset;
  double m_coolingBlockRadialOffset;
  double m_coolingPipeRadialOffset;
  double m_coolingPipePhiOffset;

  // There are cooling block & cooling pipe offset parameters.
  double m_coolingBlockOffsetX;
  double m_coolingBlockOffsetY;
  double m_coolingBlockOffsetZ;
  double m_coolingPipeOffsetX;
  double m_coolingPipeOffsetY;

  double m_doglegOffsetX;
  double m_doglegOffsetY;

  SCT_Module* m_module;
  SCT_Dogleg* m_dogleg;
  SCT_CoolingBlock* m_coolingBlock;
  SCT_CoolingPipe* m_coolingPipe;

  GeoTransform * m_refPointTransform;
  GeoTransform * m_coolingPipePos;

  //! For calculations of envelopes of SCT_DetailLayer.
  GeoTrf::Vector3D * m_env1RefPointVector;
  //! For calculations of envelopes of SCT_DetailLayer.
  GeoTrf::Vector3D * m_env2RefPointVector;

  double m_env1Thickness;
  double m_env1Width;
  double m_env2Thickness;
  double m_env2Width;

  std::vector<GeoNameTag *> m_nameTag;
  std::vector<GeoTrf::Transform3D> m_modulePos;
  std::vector<GeoTransform *> m_doglegPos;
  std::vector<GeoTransform *> m_coolingBlockPos;

};

#endif // SCT_GEOMODEL_SCT_SKI_H
