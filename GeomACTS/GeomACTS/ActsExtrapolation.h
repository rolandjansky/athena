#ifndef GeomACTS_ActsExtrapolation_h
#define GeomACTS_ActsExtrapolation_h

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Utilities/BFieldMapUtils.hpp"

// PACKAGE
#include "GeomACTS/ObjWriterTool.h"
#include "GeomACTS/IExCellWriterSvc.h"

// STL
#include <memory>
#include <vector>

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingGeometrySvc;
  class IExtrapolationTool;
  
  template<typename>
  class ExtrapolationCell;
}

template<typename>
class RootExCellWriter;

class ParticleGun;

class EventContext;



class ActsExtrapolation : public AthReentrantAlgorithm {
public:
  ActsExtrapolation (const std::string& name, ISvcLocator* pSvcLocator);
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

};

#endif // GeomACTS_ActsExtrapolation_h
