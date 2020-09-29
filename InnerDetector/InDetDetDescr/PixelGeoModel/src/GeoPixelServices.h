/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSERVICES_H
#define GEOPIXELSERVICES_H

#include "GeoVPixelFactory.h"
class GeoTransform;

namespace InDetDD {
  class Zone;
  class VolumeBuilder;
  class ServiceVolume;
}

class GeoPixelServices : public GeoVPixelFactory {
public:
  GeoPixelServices(InDetDD::PixelDetectorManager* ddmgr,
                   PixelGeometryManager* mgr, InDetDD::Zone * envelopeZone = 0);
  ~GeoPixelServices();
  virtual GeoVPhysVol* Build() override;
  void initialize(const std::string &);
  void initializeOld(const std::string &);
  InDetDD::VolumeBuilder * getBuilder() {return m_pixServBuilder;}
  InDetDD::VolumeBuilder * getServMatBuilder() {return m_servMatBuilder;}

private:
  
  std::vector<const InDetDD::ServiceVolume *> m_services;
  InDetDD::VolumeBuilder * m_pixServBuilder;
  InDetDD::VolumeBuilder * m_servMatBuilder;
  std::vector<double> m_layerShift;
};
    

#endif
