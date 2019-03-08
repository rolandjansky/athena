/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDHYBRID_H
#define SCT_GEOMODEL_SCT_FWDHYBRID_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;

class SCT_FwdHybrid : public SCT_SharedComponentFactory
{
public:
  SCT_FwdHybrid(const std::string & name, int ringType,
                InDetDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);

public:
  //int hybridPart() const {return m_hybridPart;}

  // inline methods
  const GeoMaterial * material() const {return m_material;}
  double outerWidth() const { return m_outerWidth; }
  double innerWidth() const { return m_innerWidth; }
  double length()     const { return m_length; }
  double thickness()  const { return m_thickness; }

  double mountPointToOuterEdge() const { return m_mountPointToOuterEdge; }
  double mountPointToInnerEdge() const { return m_mountPointToInnerEdge; }
  
  virtual GeoVPhysVol * build();

private:
  void getParameters();

  int m_ringType;

  double m_outerWidth; // Outer width of hybrid. (When module is placed on
                       // inner edge). ie the long side with wire bonds.
  double m_innerWidth; // Inner width
  double m_length1;
  
  double m_length;     // Length (radially).
  double m_thickness;

  double m_width2;     // second part of hybrid - rectangular 
  double m_length2;     
  double m_thickness2;

  double m_mountPointToInnerEdge;
  double m_mountPointToOuterEdge;

  const GeoMaterial * m_material;
  std::string m_materialName;

};
    
#endif // SCT_GEOMODEL_SCT_FWDHYBRID_H
