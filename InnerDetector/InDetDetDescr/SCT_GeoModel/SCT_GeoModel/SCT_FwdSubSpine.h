/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDSUBSPINE_H 
#define SCT_GEOMODEL_SCT_FWDSUBSPINE_H 


#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

#define SUBSPINE_LEFT     1
#define SUBSPINE_RIGHT   -1

class GeoMaterial;
class GeoVPhysVol;

class SCT_FwdSubSpine : public SCT_SharedComponentFactory
{

public:
  SCT_FwdSubSpine(const std::string & name, int ringType, int spineSide,
                  InDetDD::SCT_DetectorManager* detectorManager,
                  const SCT_GeometryManager* geometryManager,
                  SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;}

  //Sub spines:
  double innerThickness() const {return m_thickness1;}
  double innerWidth()     const {return m_width1;}
  double innerLength()    const {return m_length1;}

  double middleThickness() const {return m_thickness2;}
  double middleWidth()     const {return m_width2;}
  double middleLength()    const {return m_length2;}

  double outerThickness() const {return m_thickness3;}
  double outerWidth()     const {return m_width3;}
  double outerLength()    const {return m_length3;}

  virtual GeoVPhysVol * build();

private:
  void getParameters();

  int m_ringType;
  int m_spineSide;
  const GeoMaterial * m_material;

  double m_thickness1;
  double m_width1;
  double m_length1;

  double m_thickness2;
  double m_width2;
  double m_length2;

  double m_thickness3;
  double m_width3;
  double m_length3;

};

#endif // SCT_GEOMODEL_SCT_FWDSUBSPINE_H 


