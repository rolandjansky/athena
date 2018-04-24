#ifndef InDet_PixelLayerValidationTool_H
#define InDet_PixelLayerValidationTool_H

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

static const InterfaceID IID_IPixelLayerValidationTool( "InDet::IPixelLayerValidationTool", 1, 0 );

class IPixelLayerValidationTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IPixelLayerValidationTool; }
   
   //   virtual GeoBarrelStd*  buildLayer() = 0;
   //   virtual GeoVPhysVol*  buildLayer(const PixelGeoBuilderBasics*, int )=0;
   virtual void printInfo(const GeoVPhysVol*, int) const =0;

   //   virtual InDet::GeoComponent* getGeoCompVol() const =0;
};


#endif


