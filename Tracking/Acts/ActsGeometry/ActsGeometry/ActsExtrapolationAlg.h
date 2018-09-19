/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSEXTRAPOLATIONALG_H
#define ACTSGEOMETRY_ACTSEXTRAPOLATIONALG_H

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Utilities/BFieldMapUtils.hpp"
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Plugins/MaterialMapping/MaterialStep.hpp"
#include "Acts/Plugins/MaterialMapping/MaterialTrack.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Plugins/MaterialMapping/MaterialTrack.hpp"

// PACKAGE
#include "ActsGeometry/ActsObjWriterTool.h"
#include "ActsGeometry/IActsExCellWriterSvc.h"
#include "ActsGeometry/ActsExtrapolationTool.h"

// STL
#include <memory>
#include <vector>


namespace Acts {
  class TrackingGeometry;
  
  template<typename>
  class ExtrapolationCell;
}

class IActsMaterialTrackWriterSvc;

template<typename>
class RootExCellWriter;

class EventContext;

class ActsExtrapolationAlg : public AthReentrantAlgorithm {
public:
  ActsExtrapolationAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute_r(const EventContext& ctx) const override;
  StatusCode finalize() override;
  
private:
  ServiceHandle<IActsExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;

  ToolHandle<ActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "ActsExtrapolationTool"};

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

  // poor-mans Particle Gun is included here right now
  Gaudi::Property<std::vector<double>> m_etaRange{this, "EtaRange", {-3, 3}, ""};
  Gaudi::Property<std::vector<double>> m_ptRange{this, "PtRange", {0.1, 1000}, ""};

  Gaudi::Property<bool> m_writeMaterialTracks{this, "WriteMaterialTracks", false, ""};
  ServiceHandle<IActsMaterialTrackWriterSvc> m_materialTrackWriterSvc;
  
  // we need alignment, even though it should be nominal
  SG::ReadCondHandleKey<GeoAlignmentStore> m_rch {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};

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
