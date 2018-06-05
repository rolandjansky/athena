#ifndef ActsGeometry_ActsExtrapolation_h
#define ActsGeometry_ActsExtrapolation_h

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
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Plugins/MaterialPlugins/MaterialStep.hpp"
#include "Acts/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Plugins/MaterialPlugins/MaterialTrack.hpp"

// PACKAGE
#include "ActsGeometry/ObjWriterTool.h"
#include "ActsGeometry/IExCellWriterSvc.h"
#include "ActsGeometry/IExtrapolationTool.h"

// STL
#include <memory>
#include <vector>

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingGeometrySvc;
  //class IExtrapolationTool;
  
  class IMaterialTrackWriterSvc;
  
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

  Gaudi::Property<bool> m_writeMaterialTracks{this, "WriteMaterialTracks", false, ""};
  ServiceHandle<Acts::IMaterialTrackWriterSvc> m_materialTrackWriterSvc;

  template <class T>
  Acts::MaterialTrack
  makeMaterialTrack(const T& ecell) const
  {

    double totDInX0 = 0;
    double totDInL0 = 0;

    std::vector<Acts::MaterialStep> mSteps;
    for (const auto &es : ecell.extrapolationSteps) {
      if (es.configuration.checkMode(Acts::ExtrapolationMode::CollectMaterial)) {
        // this is a material step
        const Acts::GeometryID& geoID = es.surface->geoID();
        const Acts::MaterialProperties& matProp = *es.material;
        totDInX0 += matProp.thicknessInX0();
        totDInL0 += matProp.thicknessInL0();
        Acts::MaterialStep::Position pos(es.position);
        mSteps.emplace_back(matProp, pos, geoID.value());
      }
    }

    const Acts::Vector3D& mom = ecell.startParameters->momentum();
    double theta = mom.theta();
    double phi = mom.phi();
    Acts::MaterialTrack mTrack(ecell.startParameters->position(),
                         theta,
                         phi,
                         mSteps,
                         totDInX0,
                         totDInL0);

    return mTrack;

  }

};

#endif // ActsGeometry_ActsExtrapolation_h
