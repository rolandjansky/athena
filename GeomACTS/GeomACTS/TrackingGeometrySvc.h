#ifndef GEOMACTS_TRACKINGGEOMETRY_SERVICE
#define GEOMACTS_TRACKINGGEOMETRY_SERVICE

#include "AthenaBaseComps/AthService.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeomACTS/ITrackingGeometrySvc.h"

namespace InDetDD {
  class InDetDetectorManager;
  class SiDetectorManager;
  class TRT_DetectorManager;
}

namespace Acts {

class TrackingGeometry;
class CylinderVolumeHelper;
class ITrackingVolumeBuilder;
class GeoModelDetectorElement;

class TrackingGeometrySvc : public extends<AthService, ITrackingGeometrySvc> {
public:

  virtual StatusCode initialize() override;
  //virtual StatusCode finalize() override;
    
  TrackingGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() override;


private:
std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline = false);

ServiceHandle<StoreGateSvc> m_detStore;
const InDetDD::SiDetectorManager* p_pixelManager;
const InDetDD::SiDetectorManager* p_SCTManager;
const InDetDD::TRT_DetectorManager* p_TRTManager;
  
std::shared_ptr<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>> m_elementStore;
std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;


};

}


#endif 
