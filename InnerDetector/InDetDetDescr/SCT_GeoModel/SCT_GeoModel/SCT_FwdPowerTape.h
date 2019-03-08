/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDPOWERTAPE_H
#define SCT_GEOMODEL_SCT_FWDPOWERTAPE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdPowerTape : public SCT_SharedComponentFactory
{

public:
 SCT_FwdPowerTape(const std::string & name,   
		  int numModules, 
		  double innerRadius, 
		  double startPos, 
		  double endPos,
                  InDetDD::SCT_DetectorManager* detectorManager,
                  const SCT_GeometryManager* geometryManager,
                  SCT_MaterialManager* materials);


  const GeoMaterial * material() const {return m_material;}
  double crossSectArea() const {return m_crossSectArea;}

  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double thickness()   const {return m_thickness;} 
  double length()      const {return m_length;} 
  double zPosition()   const {return m_zPosition;}
 
private:
  void getParameters();
  virtual GeoVPhysVol * build();

  const GeoMaterial * m_material;
  int    m_numModules;
  double m_crossSectArea; // Cross sectional area per module
  double m_innerRadius;
  double m_outerRadius;
  double m_thickness;
  double m_length;
  double m_zPosition;

};

#endif // SCT_GEOMODEL_SCT_FWDPOWERTAPE_H

