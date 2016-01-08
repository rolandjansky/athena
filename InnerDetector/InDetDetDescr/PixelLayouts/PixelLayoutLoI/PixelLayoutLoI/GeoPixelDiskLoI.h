/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISKLOI_H
#define GEOPIXELDISKLOI_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

class GeoPixelServices;

class GeoPixelDiskLoI : public PixelGeoBuilder {
 public:
  GeoPixelDiskLoI(const PixelGeoBuilderBasics* basics, int endcap);
  ~GeoPixelDiskLoI();
  virtual GeoVPhysVol* Build();
  void preBuild();

  void setDiskNumber(int idisk) { m_disk = idisk; }
  double getDiskZMin() const { return m_diskZMin; }
  double getDiskZMax() const { return m_diskZMax; }
  double getZPosition() const {return m_diskPosition; }

 private:
  //  const GeoPixelServices * m_pixServices;
  int m_endcapSide;
  int m_disk;
  double m_diskZMin;
  double m_diskZMax;
  double m_diskPosition;

  double m_rmin;
  double m_rmax;

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
};

#endif
