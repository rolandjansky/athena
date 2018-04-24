/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENDCAPECRING_H
#define GEOPIXELENDCAPECRING_H

#include "PixelInterfaces/IGeoPixelEndcapTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "GeoModelKernel/GeoShape.h"

class GeoPixelServices;
class PixelGeoBuilderBasics;
class IGeoPixelEndcapLayerTool;
class IPixelServicesTool;
class GeoFullPhysVol;
namespace InDetDD{
  class TubeZone;
}
 
class GeoPixelEndcapECRingRefTool : virtual public IGeoPixelEndcapTool, public AthAlgTool {
 public:
  GeoPixelEndcapECRingRefTool(const std::string&,const std::string&,const IInterface*);
  virtual GeoVPhysVol* buildEndcap(const PixelGeoBuilderBasics*, int);

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  double getEndcapZMin() const { return m_endcapZMin; }
  double getEndcapZMax() const { return m_endcapZMax; }
  double getEndcapRMin() const { return 0.; }
  double getEndcapRMax() const { return 0.; }

  double getNbLayers() const { return m_nlayers; }
  void preBuild(const PixelGeoBuilderBasics* basics);

  std::vector<InDetDD::TubeZone*> getSvcRegions() const { return m_endcapSvcRegions; }

 private:
  //  const GeoPixelServices * m_pixServices;

  const GeoShape* addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans);

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;

  int m_endcapSide;
  double m_endcapZMin;
  double m_endcapZMax;
  double m_endcapZOffset;

  double m_endcapRMin;
  double m_endcapRMax;
  double m_nlayers;

  bool m_bPrebuild;
 
  std::vector<std::string> m_ecPosSvcRegion;
  std::vector<std::string> m_ecNegSvcRegion;
  std::vector<std::string> m_ecSvcRegion;
  
  std::vector<InDetDD::TubeZone*> m_endcapSvcRegions;
  std::vector<std::string> m_layerPosSvcRegion;
  std::vector<std::string> m_layerNegSvcRegion;

  ToolHandle<IGeoPixelEndcapLayerTool> m_layerTool;
  ToolHandle<IPixelServicesTool> m_IDserviceTool;
  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;

};

#endif
