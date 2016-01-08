/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELRINGLOI_H
#define GEOPIXELRINGLOI_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelInterfaces/IPixelModuleSvc.h"
#include "PixelInterfaces/IPixelDesignSvc.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

#include "GeoModelKernel/GeoPhysVol.h"

namespace InDet{
  class XMLReaderSvc;
}

class GeoPixelServices;

class GeoPixelRingLoI : public PixelGeoBuilder {
 public:
  GeoPixelRingLoI(const PixelGeoBuilderBasics* basics, int endcap, int iDisk,int iRing, double ringRadius, 
		  double zOffset, int iSide, int numModules, std::string moduleType, int front_back);
  ~GeoPixelRingLoI();
  virtual GeoVPhysVol* Build();
  void preBuild();

  double getRingZMin() const { return m_ringZMin; }
  double getRingZMax() const { return m_ringZMax; }
  double getRingRMin() const { return m_ringRMin; }
  double getRingRMax() const { return m_ringRMax; }
  double getZposition() const {return m_zPosition; }

 private:
  //  const GeoPixelServices * m_pixServices;
  int m_endcapSide;
  int m_disk;
  int m_ring;
  double m_radius;
  double m_zOffset;
  int m_ringSide;
  int m_numModules;
  std::string m_moduleType;
  int m_front_back;

  double ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth);
  double m_ringZMin;
  double m_ringZMax;
  double m_ringRMin;
  double m_ringRMax;
  double m_zPosition;

  ServiceHandle<IPixelModuleSvc> m_pixelModuleSvc;
  ServiceHandle<IPixelDesignSvc> m_pixelDesignSvc;
  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;

};

#endif
