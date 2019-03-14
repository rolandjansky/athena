/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_BaseBoard_H
#define SCT_GEOMODEL_SCT_BaseBoard_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;

class SCT_BaseBoard: public SCT_SharedComponentFactory
{
public:
  SCT_BaseBoard(const std::string & name,
                InDetDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  //double baseBoardOffsetX() const {return m_baseBoardOffsetX;} // Thu 16th Jun 2005 D.Naito removed.
  //double baseBoardOffsetY() const {return m_baseBoardOffsetY;} // Thu 16th Jun 2005 D.Naito removed.
  //double baseBoardOffsetZ() const {return m_baseBoardOffsetZ;} // Thu 16th Jun 2005 D.Naito removed.

private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_length;

  //double m_baseBoardOffsetX; // Thu 16th Jun 2005 D.Naito removed.
  //double m_baseBoardOffsetY; // Thu 16th Jun 2005 D.Naito removed.
  //double m_baseBoardOffsetZ; // Thu 16th Jun 2005 D.Naito removed.
  double m_safety;

};

#endif // SCT_GEOMODEL_SCT_BaseBoard_H
