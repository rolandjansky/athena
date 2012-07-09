/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_INTERLINK_H
#define SCT_GEOMODEL_SCT_INTERLINK_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;

class SCT_InterLink : public SCT_SharedComponentFactory
{

public:
  SCT_InterLink(const std::string & name);

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
  double m_innerRadius;
  double m_outerRadius;
  double m_length;
  double m_dPhi;
  double m_phiPos;
  int m_nRepeat;

  // Following are for FSI flange
  bool m_includeFSIFlange;
  double m_innerRadiusFSIFlange;
  double m_outerRadiusFSIFlange;
  std::string m_materialNameFSIFlange;

  // Following are for B6 bearing
  std::string m_materialNameBearing;
  double m_innerRadiusBearing;
  double m_outerRadiusBearing;
  double m_lengthBearing;
  double m_dPhiBearing;
  double m_phiPosBearing;
  int m_nRepeatBearing;

};

#endif // SCT_GEOMODEL_SCT_INTERLINK_H

