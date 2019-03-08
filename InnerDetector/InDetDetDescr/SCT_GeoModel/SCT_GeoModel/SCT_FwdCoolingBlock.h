/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDCOOLINGBLOCK_H
#define SCT_GEOMODEL_SCT_FWDCOOLINGBLOCK_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;


class SCT_FwdCoolingBlock: public SCT_SharedComponentFactory

{


public:
  SCT_FwdCoolingBlock(const std::string & name, int hiLo, int mainOrSecondary,
                      InDetDD::SCT_DetectorManager* detectorManager,
                      const SCT_GeometryManager* geometryManager,
                      SCT_MaterialManager* materials);

  enum types {UPPER = 1,
	      LOWER = -1,
	      MAIN = 0,
	      SECONDARY = 1};

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double deltaR()    const {return m_deltaR;}
  double rphi()      const {return m_rphi;}
  double offsetFromDisc() const {return m_offset;}
  double index()     const {return m_coolingBlockIndex;}

private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material;
  std::string m_materialName;
  double m_thickness;
  double m_deltaR;
  double m_rphi;
  double m_offset;
  int m_hiLo;
  int m_mainSec;
  int m_coolingBlockIndex;

};

#endif // SCT_GEOMODEL_SCT_COOLINGBLOCK_H
