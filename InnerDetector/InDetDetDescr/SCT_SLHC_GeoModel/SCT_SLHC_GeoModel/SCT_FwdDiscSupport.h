/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FWDDISCSUPPORT_H
#define SCT_SLHC_GeoModel_SCT_FWDDISCSUPPORT_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

namespace InDetDDSLHC {

class SCT_FwdDiscSupport : public SCT_SharedComponentFactory{

public:
  SCT_FwdDiscSupport(const std::string & name, int iWheel);

  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double thickness() const {return m_thickness;}

 
private:
  void getParameters();
  virtual GeoVPhysVol* build();

  int m_iWheel;

  // Basic parameters
  double m_innerRadius;
  double m_outerRadius;
  double m_thickness;
  const GeoMaterial* m_material;
};
}
#endif//SCT_SLHC_GeoModel_SCT_FWDDISCSUPPORT_H
