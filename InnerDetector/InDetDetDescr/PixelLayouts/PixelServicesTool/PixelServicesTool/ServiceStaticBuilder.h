/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTATICSERVICES_H
#define GEOPIXELSTATICSERVICES_H

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class GeoTransform;
class GeoVPhysVol;
class IInDetServMatBuilderTool;

namespace InDetDD {
  class Zone;
  class VolumeGeoCompBuilder;
  class VolumeStandardBuilder;
  class ServiceVolume;
}

//  class GeoSimpleObject;

class ServiceStaticBuilder :  public PixelGeoBuilder  {
public:
  
  ServiceStaticBuilder(const PixelGeoBuilderBasics* basics,/*IInDetServMatBuilderTool * serviceBuilderTool,*/ InDetDD::Zone * envelopeZone = 0);
  virtual ~ServiceStaticBuilder();
  virtual GeoVPhysVol* Build();
  
  void initialize(const std::string &);
  void initializeOld(const std::string &);
  
  int getVolumeNumber() const { return m_pixServBuilder.size(); }
  InDetDD::VolumeStandardBuilder * getBuilder(int i) const {return m_pixServBuilder[i];}
  
  double getServiceNumber(int iBuilder=0);
  //  InDet::GeoSimpleObject* getServiceObject(int i);

  void addTopZone( InDetDD::Zone * envelopeZone);
  
 private:
  
  // Tool for building service  
  std::vector<InDetDD::VolumeStandardBuilder* > m_pixServBuilder;
  std::vector<const InDetDD::ServiceVolume *> m_services;

};


#endif
