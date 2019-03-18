/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  void buildThermalShield(GeoFullPhysVol * parent) const;
  void buildEMIShield(GeoFullPhysVol * parent) const;

  
  int    m_numLayers;

  double m_innerRadius;
  double m_outerRadius;
  double m_length;

  double m_thermalShieldEndWallThickness;

  double m_zClearance;

  bool   m_isOldGeometry;

};

#endif // SCT_GEOMODEL_SCT_BARREL_H
