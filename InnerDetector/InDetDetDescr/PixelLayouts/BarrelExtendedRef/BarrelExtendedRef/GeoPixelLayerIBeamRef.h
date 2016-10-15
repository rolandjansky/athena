/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLAYERIBEAMREF_H
#define GEOPIXELLAYERIBEAMREF_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "BarrelExtendedRef/GeoPixelLadderExtRef.h"
#include "BarrelExtendedRef/GeoPixelLadderIBeamRef.h"

#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

/*namespace InDet{
  class XMLReaderSvc;
  }*/

class GeoPhysVol;
class GeoTransform;

class GeoPixelLayerIBeamRef : public PixelGeoBuilder {

 public:
  GeoPixelLayerIBeamRef(const PixelGeoBuilderBasics* basics, int iLayer, std::string mode);  
  virtual GeoVPhysVol* Build();
  
  GeoVPhysVol* getPhysVol() const { return m_physVol; }

 private:

  GeoVPhysVol* m_physVol;
  int m_layer;
  std::string m_mode;

  std::pair<double,double> ComputeLayerThickness(const GeoPixelLadderIBeamRef& pixelLadder, double ladderTilt, double layerRadius);

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
};

#endif
