/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_COMPONENTFACTORY_H
#define SCT_SLHC_GeoModel_SCT_COMPONENTFACTORY_H

#include "SCT_SLHC_GeoModel/SCT_Identifier.h"
#include <string>

namespace InDetDD{class SCT_DetectorManager;}

class GeoLogVol;
class GeoVPhysVol;

namespace InDetDDSLHC {

class SCT_GeometryManager;

class SCT_ComponentFactory
{

public:
  SCT_ComponentFactory(const std::string & name);

  const std::string & getName() const {return m_name;}

  static void setDetectorManager(InDetDD::SCT_DetectorManager * manager);
  static void setGeometryManager(SCT_GeometryManager * manager);
 
  // utility function to covert int to string
  static std::string intToString(int i);

protected: 
  InDetDD::SCT_DetectorManager * detectorManager() const {return s_detectorManager;}
  const SCT_GeometryManager * geometryManager() const {return s_geometryManager;}

  double epsilon() const;

  virtual ~SCT_ComponentFactory();

private:
  std::string m_name;
  static double s_epsilon;

  static InDetDD::SCT_DetectorManager* s_detectorManager;
  static const SCT_GeometryManager* s_geometryManager;

};


class SCT_SharedComponentFactory : public SCT_ComponentFactory
{

public:
  SCT_SharedComponentFactory(const std::string & name) : 
    SCT_ComponentFactory(name) {};
  GeoVPhysVol * getVolume() const {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class SCT_UniqueComponentFactory : public SCT_ComponentFactory
{

public:

  SCT_UniqueComponentFactory(const std::string & name) : 
    SCT_ComponentFactory(name)
  {};

  virtual GeoVPhysVol * build(SCT_Identifier id) const = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};
}
#endif // SCT_SLHC_GeoModel_SCT_COMPONENTFACTORY_H
