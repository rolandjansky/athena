/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXEL_LayerPlanarRefTool_H
#define GEOPIXEL_LayerPlanarRefTool_H

#include "PixelInterfaces/IGeoPixelLayerTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "BarrelInclinedRef/GeoPixelLadderPlanarRef.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

class GeoPhysVol;
class GeoTransform;

class GeoPixelLayerPlanarRefTool : virtual public IGeoPixelLayerTool, public AthAlgTool {

 public:
  GeoPixelLayerPlanarRefTool(const std::string&,const std::string&,const IInterface*);
  ~GeoPixelLayerPlanarRefTool();

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );

  virtual GeoVPhysVol* buildLayer(const PixelGeoBuilderBasics* basics, int iLayer);
  void preBuild(const PixelGeoBuilderBasics*, int);
  
  GeoVPhysVol* getPhysVol() const { return m_physVol; }
  std::vector<InDetDD::TubeZone*> getSvcRegions() const { return m_layerSvcRegions; }

 private:

  GeoVPhysVol* m_physVol;
  int m_layer;
  bool m_bPreBuild;

  void ComputeLayerThickness(const GeoPixelLadderPlanarRef& pixelLadder, double ladderTilt, double layerRadius);

  double m_layerThicknessN;
  double m_layerThicknessP;

  std::vector<std::string> m_layerRegion;
  std::vector<InDetDD::TubeZone*> m_layerSvcRegions;

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
};

#endif
