/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENDCAPLOI_H
#define GEOPIXELENDCAPLOI_H

#include "PixelInterfaces/IGeoPixelEndcapTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

class GeoPixelServices;
class PixelGeoBuilderBasics;
class IGeoPixelEndcapLayerTool;
class IPixelServicesTool;
namespace InDetDD{
  class TubeZone;
} 

class GeoPixelEndcapLoITool : virtual public IGeoPixelEndcapTool, public AthAlgTool {
 public:
  GeoPixelEndcapLoITool(const std::string&,const std::string&,const IInterface*);
  virtual GeoVPhysVol* buildEndcap(const PixelGeoBuilderBasics*, int);
  void preBuild(const PixelGeoBuilderBasics*);

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  double getEndcapZMin() const { return m_endcapZMin; }
  double getEndcapZMax() const { return m_endcapZMax; }
  double getEndcapRMin() const { return m_endcapRMin; }
  double getEndcapRMax() const { return m_endcapRMax; }

  double getNbLayers() const { return 0; }
  std::vector<InDetDD::TubeZone*> getSvcRegions() const { return m_endcapSvcRegions; }

 private:

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;
  bool m_bPrebuild;

  //  const GeoPixelServices * m_pixServices;
  //  int m_endcapSide;
  //  int m_side;
  double m_endcapRMin;
  double m_endcapRMax;
  double m_endcapZMin;
  double m_endcapZMax;

  double m_ndisks;
  std::vector<int>m_numRings;

  std::vector<std::string> m_ecPosSvcRegion;
  std::vector<std::string> m_ecNegSvcRegion;
  std::vector<std::string> m_ecSvcRegion;
  std::vector<InDetDD::TubeZone*> m_endcapSvcRegions;

  ToolHandle<IPixelServicesTool> m_IDserviceTool;
  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;

};

#endif
