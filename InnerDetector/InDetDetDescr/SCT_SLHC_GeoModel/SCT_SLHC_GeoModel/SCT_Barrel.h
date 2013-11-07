/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_BARREL_H
#define SCT_SLHC_GeoModel_SCT_BARREL_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"

class GeoVPhysVol;
class GeoLogVol;

namespace InDetDDSLHC {

class SCT_InnerSide;
class SCT_OuterSide;
class SCT_BaseBoard;
class SCT_ServiceBuilder;

class SCT_Barrel : public SCT_UniqueComponentFactory
{

public:
  SCT_Barrel(const std::string & name);
  virtual GeoVPhysVol * build(SCT_Identifier id) const;

public:
  int    numLayers()   const {return m_numLayers;}
  double innerRadius() const {return m_innerRadius;}
  double outerRadius() const {return m_outerRadius;}
  double length()      const {return m_length;}
  void   setServiceBuilder(const SCT_ServiceBuilder & builder) {m_serviceBuilder = &builder;}
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  int    m_numLayers;
  double m_innerRadius;
  double m_intermediateRadius;
  double m_outerRadius;
  double m_length;
  double m_intermediateLength;

  const SCT_ServiceBuilder * m_serviceBuilder;
};
}
#endif // SCT_SLHC_GeoModel_SCT_BARREL_H
