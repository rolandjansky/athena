/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELINTERFACES_IGEOPIXELENVELOPETOOL_H
#define PIXELINTERFACES_IGEOPIXELENVELOPETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StoreGateSvc;
class OraclePixGeoAccessor;
class GeoVPixelBasics;
class InDetMaterialManager;
class GeoVPhysVol;
class PixelGeoBuilderBasics;
namespace InDet{
  class GeoComponent;
}

static const InterfaceID IID_IGeoPixelEnvelopeTool( "IGeoPixelEnvelopeTool", 1, 0 );

class IGeoPixelEnvelopeTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoPixelEnvelopeTool; }
   
   //    virtual GeoEnvelope*  build( const OraclePixGeoAccessor*, InDetMaterialManager*) = 0;

   virtual GeoVPhysVol*  buildEnvelope(const PixelGeoBuilderBasics* )=0;
   virtual InDet::GeoComponent*  buildEnvelopeGeoComp(const PixelGeoBuilderBasics* )=0;
};

#endif 

