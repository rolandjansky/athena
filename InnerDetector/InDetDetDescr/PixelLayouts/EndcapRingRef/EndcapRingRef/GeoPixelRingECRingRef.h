/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELRINGECRING_H
#define GEOPIXELRINGECRING_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"

#include "GaudiKernel/ServiceHandle.h"
#include "PixelInterfaces/IPixelModuleSvc.h"
#include "PixelInterfaces/IPixelDesignSvc.h"

class GeoPixelServices;
class PixelGeoBuilderBasics;

class GeoPixelRingECRingRef  {
 public:
  GeoPixelRingECRingRef(int iDisk,int iRing, double ringRadius, double ringOuterRadius, double zOffset, double phiOffset,
		     int iSide, int numModules, std::string moduleType, int firstRing, int back_front);
  ~GeoPixelRingECRingRef();
  virtual GeoVPhysVol* Build(const PixelGeoBuilderBasics* basics, int endcapSide);
  void preBuild(const PixelGeoBuilderBasics* basics);

  double getRingZMin() const { return m_ringZMin; }
  double getRingZMax() const { return m_ringZMax; }
  double getRingZShift() const { return m_ringZShift; }
  double getRingRMin() const { return m_ringRMin; }
  double getRingRMax() const { return m_ringRMax; }
  double getZposition() const {return m_zPosition; }

 private:
  //  const GeoPixelServices * m_pixServices;
  int m_endcapSide;
  int m_layer;
  int m_ring;
  int m_ringId;
  double m_radius;
  double m_outerRadius;
  double m_zOffset;
  double m_phiOffset;
  int m_ringSide;
  int m_numModules;
  std::string m_moduleType;
  int m_diskId;
  int m_front_back;
 
  double ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth);
  double m_ringZMin;
  double m_ringZMax;
  double m_ringZShift;
  double m_ringRMin;
  double m_ringRMax;
  double m_zPosition;
  double m_halfLength;
  bool m_bPrebuild;  

  ServiceHandle<IPixelModuleSvc> m_pixelModuleSvc;
  ServiceHandle<IPixelDesignSvc> m_pixelDesignSvc;

};

#endif
