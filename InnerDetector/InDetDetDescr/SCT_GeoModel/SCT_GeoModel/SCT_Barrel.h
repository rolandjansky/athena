/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_BARREL_H
#define SCT_GEOMODEL_SCT_BARREL_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class SCT_Identifier;

class SCT_Barrel : public SCT_UniqueComponentFactory
{

public:
  SCT_Barrel(const std::string & name,
             InDetDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);
  virtual GeoVPhysVol * build(SCT_Identifier id);

public:
  int    numLayers()   const {return m_numLayers;}
  double innerRadius() const {return m_innerRadius;}
  double outerRadius() const {return m_outerRadius;}
  double length()      const {return m_length;}
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  void buildThermalShield(GeoFullPhysVol * parent);
  void buildEMIShield(GeoFullPhysVol * parent);

  
  int    m_numLayers = 0;

  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_length = 0.0;

  double m_thermalShieldEndWallThickness = 0.0;

  double m_zClearance = 0.0;

  bool   m_isOldGeometry = false;

};

#endif // SCT_GEOMODEL_SCT_BARREL_H
