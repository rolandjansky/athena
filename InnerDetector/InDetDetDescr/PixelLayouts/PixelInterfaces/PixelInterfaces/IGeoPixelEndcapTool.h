/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoPixelEndcapTool_H
#define InDet_GeoPixelEndcapTool_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"


//  class GeoComponent;
  //  class GeoBarrelAlpine;
//class GeoBarrelStd;

class GeoVPixelBasics;
class GeoVPhysVol;
class PixelGeoBuilderBasics;
namespace InDetDD{
  class TubeZone;
}

static const InterfaceID IID_IGeoPixelEndcapTool( "InDet::IGeoPixelEndcapTool", 1, 0 );

class IGeoPixelEndcapTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoPixelEndcapTool; }
   
   //   virtual GeoBarrelStd*  buildBarrel() = 0;
   virtual GeoVPhysVol*  buildEndcap(const PixelGeoBuilderBasics* , int side)=0;
   virtual void preBuild(const PixelGeoBuilderBasics* ) =0;

   virtual double getEndcapZMin() const =0;
   virtual double getEndcapZMax() const =0;
   virtual double getEndcapRMin() const =0;
   virtual double getEndcapRMax() const =0;
   virtual std::vector<InDetDD::TubeZone*> getSvcRegions() const =0;


};


#endif


