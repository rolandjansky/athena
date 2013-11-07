/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FWDSPINE_H
#define SCT_SLHC_GeoModel_SCT_FWDSPINE_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;

namespace InDetDDSLHC {

class SCT_FwdSpine : public SCT_SharedComponentFactory{
  
 public:
  SCT_FwdSpine(const std::string & name, int moduleType);
  
 public:
  
  const GeoMaterial* material() const {return m_material;}
  //Main spine:
  double thickness() const {return m_thickness;}
  double length()    const {return m_length;}
  double innerRadius() const {return m_innerRadius;}
  double outerRadius() const {return m_outerRadius;}
  double innerWidth()      const {return m_innerWidth;}
  double outerWidth()      const {return m_outerWidth;}
  double deltaPhi()    const {return m_deltaPhi;}
  
  virtual GeoVPhysVol * build();
  
 private:
  void getParameters();
  
  int m_ring;//ring to which this module belongs 

  const GeoMaterial * m_material;
  std::string m_materialName;
  double m_thickness;
  double m_length;
  double m_innerRadius;
  double m_outerRadius;
  double m_middleRadius;
  double m_innerWidth;
  double m_outerWidth;
  double m_deltaPhi;
};
}
#endif //SCT_SLHC_GeoModel_SCT_FWDSPINE_H

