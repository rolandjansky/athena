/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef InDet_GeoPixelLayerTool_H
#define InDet_GeoPixelLayerTool_H

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

static const InterfaceID IID_IGeoPixelLayerTool( "InDet::IGeoPixelLayerTool", 1, 0 );

class IGeoPixelLayerTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoPixelLayerTool; }
   
   //   virtual GeoBarrelStd*  buildLayer() = 0;
   virtual GeoVPhysVol*  buildLayer(const PixelGeoBuilderBasics*, int )=0;
   virtual void preBuild(const PixelGeoBuilderBasics*, int) =0;

   virtual std::vector<InDetDD::TubeZone*> getSvcRegions() const =0;
   //   virtual InDet::GeoComponent* getGeoCompVol() const =0;

   // neeeded for HGTD, added by ncalace in 20.20, see https://svn-atlas.web.cern.ch/websvn/comp.php?repname=repository&compare[]=/InnerDetector/InDetDetDescr/PixelLayouts/PixelInterfaces/branches/PixelInterfaces-00-00-09-branch/PixelInterfaces/IGeoPixelLayerTool.h@815871&compare[]=/InnerDetector/InDetDetDescr/PixelLayouts/PixelInterfaces/branches/PixelInterfaces-00-00-09-branch/PixelInterfaces/IGeoPixelLayerTool.h@815872
   virtual const PixelGeoBuilderBasics* getPixelGeoBuilderBasics() const =0;

};


#endif
