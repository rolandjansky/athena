/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDYNAMICSERVICES_H
#define GEOPIXELDYNAMICSERVICES_H

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelServicesTool/ServiceDynVolume.h"

class GeoTransform;
class GeoVPhysVol;
class GeoMaterial;

namespace InDetDD {
  class Zone;
  class VolumeGeoCompBuilder;
  class VolumeStandardBuilder;
  class ServiceVolume;
}

//  class GeoSimpleObject;

class ServiceDynamicBuilder :  public PixelGeoBuilder {
public:
  
  ServiceDynamicBuilder(const PixelGeoBuilderBasics* basics, InDetDD::Zone * envelopeZone = 0, bool bSvcDynAuto=true, bool bBarrelModuleMaterial=false);
  virtual ~ServiceDynamicBuilder();

  int getVolumeNumber() const { return m_volStdBuilder.size(); }
  InDetDD::VolumeStandardBuilder * getBuilder(int i) const {return m_volStdBuilder[i];}

  void addTopZone( InDetDD::Zone * envelopeZone);
  double getServiceNumber();
  std::map<std::string,std::string> getSvcMaterialNameTable() const { return m_svcMaterialNameTable; }
  
 private:
  
  // Tool for building service
  std::vector<InDetDD::VolumeStandardBuilder*> m_volStdBuilder;
  std::vector<const InDetDD::ServiceVolume *> m_services;
  std::map<std::string,std::string> m_svcMaterialNameTable;

  void addServiceDynVolume( const ServiceDynVolume& vol);
  std::string addPrefix( const std::string& prefix, const std::string& name) const;

  /// General service adding method.
  void addService(InDetDD::ServiceVolume * param);
  void addService(const std::string & name, const GeoMaterial * material, double rmin, double rmax, double zmin, double zmax, bool zsymm);
  void addService(const std::string & name, const std::string & materialName, double rmin, double rmax, double zmin, double zmax, bool zsymm);
  void printNewVolume( const ServiceDynVolume& vol, const GeoMaterial& mat,
		       const InDetDD::ServiceVolume& param) const;


};


#endif
