/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELBARRELLOI_H
#define GEOPIXELBARRELLOI_H

#include "PixelInterfaces/IGeoPixelBarrelTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

namespace InDet{
  class XMLReaderSvc;
}
namespace InDet{
  class GeoComponent;
}
//class GeoPixelServices;
class PixelGeoBuilderBasics;
class IPixelServicesTool;

class GeoPixelBarrelLoITool : virtual public IGeoPixelBarrelTool, public AthAlgTool {

 public:
  //  GeoPixelBarrelLoITool(const GeoPixelServices * pixServices);
  GeoPixelBarrelLoITool(const std::string&,const std::string&,const IInterface*);
  void preBuild(const PixelGeoBuilderBasics*);
  virtual InDet::GeoComponent* getGeoCompVol() const {return 0; }

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  virtual GeoVPhysVol* buildBarrel(const PixelGeoBuilderBasics*);
  std::vector<InDetDD::TubeZone*> getSvcRegions() const { return m_barrelSvcRegions; }
 private:

  ToolHandle<IPixelServicesTool> m_IDserviceTool;
  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;

  std::vector<std::string> m_barrelRegion;
  std::vector<InDetDD::TubeZone*> m_barrelSvcRegions;
};

#endif
