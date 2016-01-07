/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoPixelEndcapLayerTool_H
#define InDet_GeoPixelEndcapLayerTool_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"


//  class GeoComponent;
  //  class GeoBarrelAlpine;
//class GeoBarrelStd;

class GeoVPixelBasics;
class GeoVPhysVol;
class PixelGeoBuilderBasics;


static const InterfaceID IID_IGeoPixelEndcapLayerTool( "InDet::IGeoPixelEndcapLayerTool", 1, 0 );

class IGeoPixelEndcapLayerTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoPixelEndcapLayerTool; }
   
   virtual GeoVPhysVol*  buildLayer(const PixelGeoBuilderBasics* , int iLayer, int side)=0;
   virtual void preBuild(const PixelGeoBuilderBasics*, int)=0;

   virtual double getLayerZMin() const =0;
   virtual double getLayerZMax() const =0;
   virtual double getLayerRMin() const =0;
   virtual double getLayerRMax() const =0;
   virtual double getZPosition() const =0;
};


#endif


