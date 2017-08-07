/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENVELOPELOI_H
#define GEOPIXELENVELOPELOI_H

#include "PixelInterfaces/IGeoPixelEnvelopeTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/MsgStreamMember.h"

class IGeoPixelBarrelTool;
class IGeoPixelEndcapTool;
class IPixelServicesTool;
namespace InDet{
  class GeoComponent;
}
namespace InDetDD{
  class TubeZone;
}

class GeoPixelEnvelopeInclRefTool : virtual public IGeoPixelEnvelopeTool, public AthAlgTool {

 public:
  GeoPixelEnvelopeInclRefTool(const std::string&,const std::string&,const IInterface*);
  virtual GeoVPhysVol* Build();

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  virtual GeoVPhysVol* buildEnvelope(const PixelGeoBuilderBasics*);
  InDet::GeoComponent* buildEnvelopeGeoComp(const PixelGeoBuilderBasics*) { return 0; };

 private:
  ToolHandle<IGeoPixelBarrelTool> m_barrelTool;
  ToolHandle<IGeoPixelEndcapTool> m_endcapTool;
  ToolHandle<IPixelServicesTool> m_IDserviceTool;

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;

  std::vector<InDetDD::TubeZone*> SortServiceRegions(std::vector<InDetDD::TubeZone*>& svcRegions );    
};


#endif
