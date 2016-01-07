/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoPixelBarrelTool_H
#define InDet_GeoPixelBarrelTool_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class GeoVPixelBasics;
class GeoVPhysVol;
class PixelGeoBuilderBasics;
namespace InDet{
  class GeoComponent;
}
namespace InDetDD{
  class TubeZone;
}

static const InterfaceID IID_IGeoPixelBarrelTool( "InDet::IGeoPixelBarrelTool", 1, 0 );

class IGeoPixelBarrelTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoPixelBarrelTool; }
   
   //   virtual GeoBarrelStd*  buildBarrel() = 0;
   virtual GeoVPhysVol*  buildBarrel(const PixelGeoBuilderBasics* )=0;
   virtual void preBuild() =0;
   virtual std::vector<InDetDD::TubeZone*> getSvcRegions() const =0;
   virtual InDet::GeoComponent* getGeoCompVol() const =0;
};


#endif


