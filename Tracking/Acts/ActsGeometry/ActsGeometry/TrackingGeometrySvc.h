#ifndef GEOMACTS_TRACKINGGEOMETRY_SERVICE
#define GEOMACTS_TRACKINGGEOMETRY_SERVICE

#include "AthenaBaseComps/AthService.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreGateSvc.h"

#include "ActsGeometry/ITrackingGeometrySvc.h"

#include "ActsGeometry/ShiftCondObj.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"

#include "GeoModelUtilities/GeoAlignmentStore.h"

#include <map>

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
  
  SG::ReadCondHandleKey<GeoAlignmentStore> alignmentCondHandleKey() const;


private:
  std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
  makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline = false);

  ServiceHandle<StoreGateSvc> m_detStore;
  const InDetDD::SiDetectorManager* p_pixelManager;
  const InDetDD::SiDetectorManager* p_SCTManager;
  const InDetDD::TRT_DetectorManager* p_TRTManager;
    
  std::shared_ptr<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>> m_elementStore;
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;
    
  Gaudi::Property<bool> m_useMaterialMap{this, "UseMaterialMap", false, ""};
  Gaudi::Property<std::string> m_materialMapInputFile{this, "MaterialMapInputFile", "", ""};

  Gaudi::Property<std::vector<size_t>> m_barrelMaterialBins{this, "BarrelMaterialBins", {10, 10}};
  Gaudi::Property<std::vector<size_t>> m_endcapMaterialBins{this, "EndcapMaterialBins", {5, 20}};
  
  SG::ReadCondHandleKey<GeoAlignmentStore> m_rch {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};


};

}

inline SG::ReadCondHandleKey<GeoAlignmentStore>
Acts::TrackingGeometrySvc::alignmentCondHandleKey() const {
  return m_rch;
}

#endif 
