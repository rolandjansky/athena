/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_STAVESUPPORT_H
#define SCT_SLHC_GeoModel_SCT_STAVESUPPORT_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;

namespace InDetDDSLHC {

class SCT_StaveSupport: public SCT_SharedComponentFactory{
  
 public:
  SCT_StaveSupport(const std::string & name, int iLayer, double length);
  
 public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}
  
 private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_length;
  int m_iLayer;
};
}
#endif//SCT_SLHC_GeoModel_SCT_STAVESUPPORT_H
