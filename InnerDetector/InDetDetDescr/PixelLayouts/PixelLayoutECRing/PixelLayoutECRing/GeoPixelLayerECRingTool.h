/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLAYERECRINGF_H
#define GEOPIXELLAYERECRINGF_H

#include "PixelInterfaces/IGeoPixelEndcapLayerTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

#include "PixelLayoutECRing/GeoPixelRingECRing.h"

namespace InDet{
  class XMLReaderSvc;
}

class GeoPixelServices;
class PixelGeoBuilderBasics;
class IPixelServicesTool;

class GeoPixelLayerECRingTool : virtual public IGeoPixelEndcapLayerTool, public AthAlgTool {
 public:
  GeoPixelLayerECRingTool(const std::string&,const std::string&,const IInterface*);
  ~GeoPixelLayerECRingTool();

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  virtual GeoVPhysVol* buildLayer(const PixelGeoBuilderBasics*, int, int );
  void preBuild(const PixelGeoBuilderBasics*, int);

  double getLayerZMin() const { return m_layerZMin; }
  double getLayerZMax() const { return m_layerZMax; }
  double getLayerRMin() const { return m_layerRMin; }
  double getLayerRMax() const { return m_layerRMax; }
  double getZPosition() const {return m_layerPosition; }

 private:

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;
  
  int m_layer;
  int m_endcapSide;
  double m_layerZMin;
  double m_layerZMax;
  double m_layerRMin;
  double m_layerRMax;
  double m_layerPosition;

  std::vector<GeoPixelRingECRing> m_ringListF;
  std::vector<GeoPixelRingECRing> m_ringListB;
  std::vector<double> m_ringPos;

  double getValueFromVector(std::vector<double> v, int i);
  int getValueFromVector(std::vector<int> v, int i);
  std::string getValueFromVector(std::vector<std::string> v, int i);

  double m_rmin;
  double m_rmax;

  ToolHandle<IPixelServicesTool> m_IDserviceTool;
  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
};

#endif
