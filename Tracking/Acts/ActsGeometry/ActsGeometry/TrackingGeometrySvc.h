#ifndef GEOMACTS_TRACKINGGEOMETRY_SERVICE
#define GEOMACTS_TRACKINGGEOMETRY_SERVICE

#include "AthenaBaseComps/AthService.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/EventContext.h"

#include "ActsGeometry/ITrackingGeometrySvc.h"

#include "ActsGeometry/ShiftCondObj.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"


#include <map>
#include <boost/thread/shared_mutex.hpp>

namespace InDetDD {
  class InDetDetectorManager;
  class SiDetectorManager;
  class TRT_DetectorManager;
}

class TRT_ID;
class GeoAlignmentStore;

namespace Acts {

class TrackingGeometry;
class CylinderVolumeHelper;
class ITrackingVolumeBuilder;
class GeoModelDetectorElement;

class GeometryID;
class BinnedSurfaceMaterial;


class TrackingGeometrySvc : public extends<AthService, ITrackingGeometrySvc> {
public:

  virtual StatusCode initialize() override;
  //virtual StatusCode finalize() override;
    
  TrackingGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() override;
  
  void 
  setGeoAlignmentStore(const GeoAlignmentStore* gas, const EventContext& ctx) override;
    
  const GeoAlignmentStore*
  getGeoAlignmentStore(const EventContext& ctx) const override;


private:
  std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
  makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline = false);

  ServiceHandle<StoreGateSvc> m_detStore;
  const InDetDD::SiDetectorManager* p_pixelManager;
  const InDetDD::SiDetectorManager* p_SCTManager;
  const InDetDD::TRT_DetectorManager* p_TRTManager;
    
  std::shared_ptr<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>> m_elementStore;
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;
  
  const TRT_ID *m_TRT_idHelper;
    
  Gaudi::Property<bool> m_useMaterialMap{this, "UseMaterialMap", false, ""};
  Gaudi::Property<std::string> m_materialMapInputFile{this, "MaterialMapInputFile", "", ""};
  Gaudi::Property<std::vector<size_t>> m_barrelMaterialBins{this, "BarrelMaterialBins", {10, 10}};
  Gaudi::Property<std::vector<size_t>> m_endcapMaterialBins{this, "EndcapMaterialBins", {5, 20}};
  
  mutable std::unordered_map<EventContext::ContextID_t, const GeoAlignmentStore*> m_gasMap;
  mutable boost::shared_mutex m_gasMapMutex;


};

}


#endif 
