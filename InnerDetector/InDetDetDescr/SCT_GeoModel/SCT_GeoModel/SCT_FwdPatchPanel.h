/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDPATCHPANEL_H
#define SCT_GEOMODEL_SCT_FWDPATCHPANEL_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdPatchPanel : public SCT_SharedComponentFactory
{

public:
  SCT_FwdPatchPanel(const std::string & name, int type,
                    InDetDD::SCT_DetectorManager* detectorManager,
                    const SCT_GeometryManager* geometryManager,
                    SCT_MaterialManager* materials);

  //
  // Methods to return basic and derived parameters. 
  //
  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_midRadius - 0.5*m_deltaR;} 
  double outerRadius() const {return m_midRadius + 0.5*m_deltaR;} 
  double midRadius() const {return m_midRadius;} 
  double deltaR() const {return m_deltaR;} 
  double rphi() const {return m_rphi;} 
  double thickness() const {return m_thickness;}


private:
  void getParameters();
  virtual GeoVPhysVol * build();

  int m_type;

  // Basic parameters
  double m_midRadius;
  double m_deltaR;
  double m_rphi;
  double m_thickness;
  const GeoMaterial * m_material;
  std::string m_materialName;

};

#endif // SCT_GEOMODEL_SCT_FWDPATCHPANEL_H

