#ifndef GeomACTS_ACTSTrackingGeometry_h
#define GeomACTS_ACTSTrackingGeometry_h

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/

// ACTS
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Utilities/BFieldMapUtils.hpp"

// PACKAGE
#include "GeomACTS/ObjWriterTool.h"
#include "GeomACTS/IExCellWriterSvc.h"

// STL
#include <fstream>
#include <memory>
#include <vector>

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingVolumeBuilder;
  class CylinderVolumeHelper;
  class GeoModelDetectorElement;
  class ITrackingGeometrySvc;
  class IExtrapolationTool;
  
  template<typename>
  class ExtrapolationCell;
}

template<typename>
class RootExCellWriter;

class ParticleGun;

class EventContext;

namespace InDetDD {
  class InDetDetectorManager;
}


class ACTSTrackingGeometry : public AthReentrantAlgorithm {
public:
  ACTSTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute_r(const EventContext& ctx) const override;
  StatusCode finalize() override;
  
  //bool isClonable() const override { return true; }
  //unsigned int cardinality() const override { return m_cardinality; }

private:
  bool m_firstEvent;
  

  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;
  ServiceHandle<Acts::IExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::IExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "Acts__ExtrapolationTool"};
  ToolHandle<Acts::ObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "Acts__ObjWriterTool"};

  std::unique_ptr<ParticleGun> m_particleGun;

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> m_exCells;
  std::shared_ptr<RootExCellWriter<Acts::TrackParameters>> m_rootEccWriter;

  
  Gaudi::Property<int> m_searchMode{this, "SearchMode", 1, ""};
  Gaudi::Property<bool> m_collectSensitive{this, "CollectSensitive", true, ""};
  Gaudi::Property<bool> m_collectPassive{this, "CollectPassive", true, ""};
  Gaudi::Property<bool> m_collectBoundary{this, "CollectBoundary", true, ""};
  Gaudi::Property<bool> m_collectMaterial{this, "CollectMaterial", true, ""};
  Gaudi::Property<bool> m_stopAtBoundary{this, "StopAtBoundary", true, ""};
  Gaudi::Property<bool> m_FATRAS{this, "FATRAS", true, ""};
  Gaudi::Property<size_t> m_nParticles{this, "nParticles", 1, ""};
  Gaudi::Property<bool> m_writeTrackingGeometry{this, "WriteTrackingGeometry", false, ""};

};

#endif // GeomACTS_ACTSTrackingGeometry_h
