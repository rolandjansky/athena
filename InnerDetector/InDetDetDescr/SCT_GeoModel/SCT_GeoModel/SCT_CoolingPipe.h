/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_COOLINGPIPE_H
#define SCT_GEOMODEL_SCT_COOLINGPIPE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;


class SCT_CoolingPipe: public SCT_SharedComponentFactory

{

public:
  SCT_CoolingPipe(const std::string & name, double length);

public:
  const GeoMaterial * material() const {return m_material;}
  double pipeRadius() const {return m_pipeRadius;}
  double length()     const {return m_length;}
  
private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material;
  std::string m_materialName;
  double m_pipeRadius;
  double m_length;

};

#endif // SCT_GEOMODEL_SCT_COOLINGPIPE_H
