#ifndef PIXELINTERFACES_IGEOPIXELTRACKINGVOLUMEBUILDER_H
#define PIXELINTERFACES_IGEOPIXELTRACKINGVOLUMEBUILDER_H

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
namespace Trk{
  class TrackingVolume;
}

static const InterfaceID IID_IGeoPixelTrackingVolumeBuilder( "IGeoPixelTrackingVolumeBuilder", 1, 0 );

class IGeoPixelTrackingVolumeBuilder : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoPixelTrackingVolumeBuilder; }
   
   virtual void  buildTrackingVolume(const PixelGeoBuilderBasics*);
   virtual const Trk::TrackingVolume* trackingVolume();
   virtual void processMaterial(const Trk::TrackingVolume*);
};

inline void IGeoPixelTrackingVolumeBuilder::buildTrackingVolume( const PixelGeoBuilderBasics* )
{ }

inline const Trk::TrackingVolume* IGeoPixelTrackingVolumeBuilder::trackingVolume()
{ return 0; }

inline void IGeoPixelTrackingVolumeBuilder::processMaterial(const Trk::TrackingVolume* ) 
{ }

#endif 

