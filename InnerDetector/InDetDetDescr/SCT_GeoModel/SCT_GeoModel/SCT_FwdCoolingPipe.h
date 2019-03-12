/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDCOOLINGPIPE_H
#define SCT_GEOMODEL_SCT_FWDCOOLINGPIPE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdCoolingPipe : public SCT_SharedComponentFactory
{

public:

public:
  SCT_FwdCoolingPipe(const std::string & name,   
		     int numPipes, 
		     double innerRadius, 
		     double startPos, 
		     double endPos,
                     InDetDD::SCT_DetectorManager* detectorManager,
                     const SCT_GeometryManager* geometryManager,
                     SCT_MaterialManager* materials);

  const GeoMaterial * material() const {return m_material;}
  double pipeRadius()  const {return m_pipeRadius;}

  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double thickness()   const {return m_thickness;} 
  double length()      const {return m_length;} 
  double zPosition()   const {return m_zPosition;}

private:
  void getParameters();
  virtual GeoVPhysVol * build();

  const GeoMaterial * m_material;
  int m_numPipes;
  double m_pipeRadius; 
  double m_innerRadius;
  double m_outerRadius;
  double m_thickness;
  double m_length;
  double m_zPosition;

};

#endif // SCT_GEOMODEL_SCT_FWDCOOLINGPIPE_H

