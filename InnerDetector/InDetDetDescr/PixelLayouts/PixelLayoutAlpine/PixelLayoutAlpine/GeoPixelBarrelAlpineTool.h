/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELBARRELALPINETOOL_H
#define GEOPIXELBARRELALPINETOOL_H

#include "PixelInterfaces/IGeoPixelBarrelTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

#include "PixelGeoComponent/GeoBarrelAlpine.h"
#include "GeoModelKernel/GeoShape.h"

namespace InDet{
  class GeoComponent;
  class GeoBarrelAlpine;
}

//class GeoPixelServices;
class PixelGeoBuilderBasics;
class IPixelServicesTool;
class IInDetServicesTool;
namespace InDetDD{
  class TubeZone;
}

class GeoPixelBarrelAlpineTool : virtual public IGeoPixelBarrelTool, public AthAlgTool{ 

 public:
  //  GeoPixelBarrelAlpineTool(const GeoPixelServices * pixServices);
  GeoPixelBarrelAlpineTool(const std::string&,const std::string&,const IInterface*);
  virtual GeoVPhysVol* Build();
  virtual InDet::GeoComponent* getGeoCompVol() const {return m_barrelGeoComp;}

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  virtual GeoVPhysVol* buildBarrel(const PixelGeoBuilderBasics*);
  void preBuild();
 
  std::vector<InDetDD::TubeZone*> getSvcRegions() const { return m_barrelSvcRegions; }

 private:

  const GeoShape* addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans);

  ToolHandle<IPixelServicesTool> m_IDserviceTool;
  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;

  bool m_bGeoComp;
  InDet::GeoBarrelAlpine *m_barrelGeoComp;

  std::vector<std::string> m_barrelRegion;
  std::vector<InDetDD::TubeZone*> m_barrelSvcRegions;
  bool m_bBuildStaticSvc;
};

#endif
