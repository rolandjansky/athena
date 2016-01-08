/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENVELOPEALPINETOOL_H
#define GEOPIXELENVELOPEALPINETool_H

#include "PixelInterfaces/IGeoPixelEnvelopeTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelGeoComponent/GeoEnvelopeAlpine.h"

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

class GeoPixelEnvelopeAlpineTool : virtual public IGeoPixelEnvelopeTool, public AthAlgTool {

 public:
  //  GeoPixelEnvelopeAlpineTool(const GeoPixelServices * pixServices);
  GeoPixelEnvelopeAlpineTool(const std::string&,const std::string&,const IInterface*);
  virtual GeoVPhysVol* Build();

  virtual StatusCode initialize();
  virtual StatusCode create();
  virtual StatusCode finalize();
  
  // Register callback function on ConDB object
  virtual StatusCode registerCallback( StoreGateSvc* detStore );
  
  GeoVPhysVol* buildEnvelope(const PixelGeoBuilderBasics*);
  InDet::GeoEnvelopeAlpine* buildEnvelopeGeoComp(const PixelGeoBuilderBasics*);

 private:
  std::vector<InDetDD::TubeZone*> SortServiceRegions(std::vector<InDetDD::TubeZone*>& svcRegions );

  ToolHandle<IGeoPixelBarrelTool> m_barrelTool;
  ToolHandle<IGeoPixelEndcapTool> m_endcapTool;
  ToolHandle<IPixelServicesTool> m_IDserviceTool;

  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;
    
};

#endif
