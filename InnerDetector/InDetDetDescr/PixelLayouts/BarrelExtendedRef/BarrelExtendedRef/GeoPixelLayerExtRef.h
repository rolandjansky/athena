/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLAYERLOI_H
#define GEOPIXELLAYERLOI_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "BarrelExtendedRef/GeoPixelLadderExtRef.h"

#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

/*namespace InDet{
  class XMLReaderSvc;
  }*/

class GeoPhysVol;
class GeoTransform;

class GeoPixelLayerExtRef : public PixelGeoBuilder {

 public:
  GeoPixelLayerExtRef(const PixelGeoBuilderBasics* basics, int iLayer);
  virtual GeoVPhysVol* Build();
  
  GeoVPhysVol* getPhysVol() const { return m_physVol; }

 private:

  GeoVPhysVol* m_physVol;
  int m_layer;

  void ComputeLayerThickness(const GeoPixelLadderExtRef& pixelLadder, double ladderTilt, double layerRadius);

  double m_layerThicknessN;
  double m_layerThicknessP;

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
};

#endif
