/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLAYERALPINETOOL_H
#define GEOPIXELLAYERALPINETOOL_H

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

#include "PixelGeoComponent/GeoLayerAlpine.h"

namespace InDet{
  class GeoComponent;
  class GeoLayerAlpine;
}

class GeoPhysVol;
class GeoTransform;
class PixelGeoBuilderBasics;

class GeoPixelLayerAlpineBuilder : public PixelGeoBuilder {

 public:
  GeoPixelLayerAlpineBuilder( const PixelGeoBuilderBasics*, int iLayer = -1, double rmin = 0., double rmax = 9999. );

  virtual GeoVPhysVol* Build();
  virtual InDet::GeoComponent* getGeoCompVol() const {return m_layerGeoComp;}
  
  GeoVPhysVol* getPhysVol() const { return m_physVol; }
  double Length() const { return m_length; }

 private:

  GeoVPhysVol* m_physVol;
  int m_layer;
  double m_length;
  double m_radiusMin;
  double m_radiusMax;

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;

  bool m_bGeoComp;
  InDet::GeoLayerAlpine *m_layerGeoComp;

};

#endif
