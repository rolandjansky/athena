/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_INTERLINK_H
#define SCT_GEOMODEL_SCT_INTERLINK_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>
#include <vector>

class GeoMaterial;
class GeoTube;
class GeoLogVol;
class GeoPhysVol;
class GeoTubs;
class GeoTransform;

class SCT_InterLink : public SCT_SharedComponentFactory
{

public:
  SCT_InterLink(const std::string & name,
                InDetDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);
  ~SCT_InterLink();

public:
  //  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double length() const {return m_length;} 

private:
  void getParameters();
  virtual GeoVPhysVol * build();

  // Following are for interlink
  std::string m_materialName;
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_length = 0.0;
  double m_dPhi = 0.0;
  double m_phiPos = 0.0;
  int m_nRepeat = 0;

  // Following are for FSI flange
  bool m_includeFSIFlange = false;
  double m_innerRadiusFSIFlange = 0.0;
  double m_outerRadiusFSIFlange = 0.0;
  std::string m_materialNameFSIFlange;

  // Following are for B6 bearing
  std::string m_materialNameBearing;
  double m_innerRadiusBearing = 0.0;
  double m_outerRadiusBearing = 0.0;
  double m_lengthBearing = 0.0;
  double m_dPhiBearing = 0.0;
  double m_phiPosBearing = 0.0;
  int m_nRepeatBearing = 0;

  const GeoTube* m_interLinkShape;
  const GeoLogVol* m_interLinkLog;
  GeoPhysVol* m_interLink;
  const GeoTubs* m_interLinkSegShape;
  const GeoLogVol* m_interLinkSegLog;
  GeoPhysVol* m_interLinkSeg;
  const GeoTubs* m_bearingShape;
  const GeoLogVol* m_bearingLog;
  GeoPhysVol* m_bearing;
  const GeoTubs* m_FSIFlangeShape;
  const GeoLogVol* m_FSIFlangeLog;
  GeoPhysVol* m_FSIFlange;
  std::vector<GeoTransform*> m_geoTransforms;
};

#endif // SCT_GEOMODEL_SCT_INTERLINK_H

