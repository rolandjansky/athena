#ifndef GeomACTS_ACTSTrackingGeometry_h
#define GeomACTS_ACTSTrackingGeometry_h

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"

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

class IGeoModelSvc;

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

namespace InDetDD {
  class InDetDetectorManager;
}


class ACTSTrackingGeometry : public AthAlgorithm {
public:
  ACTSTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  bool isClonable() const override { return true; }
  //unsigned int cardinality() const override { return m_cardinality; }

private:
  bool m_firstEvent;
  
  // alg properties
  int m_nParticles;
  bool m_writeTrackingGeometry;

  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;
  ServiceHandle<Acts::IExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::IExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "Acts__ExtrapolationTool"};
  ToolHandle<Acts::ObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "Acts__ObjWriterTool"};

  std::unique_ptr<ParticleGun> m_particleGun;

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> m_exCells;
  std::shared_ptr<RootExCellWriter<Acts::TrackParameters>> m_rootEccWriter;
};

#endif // GeomACTS_ACTSTrackingGeometry_h
