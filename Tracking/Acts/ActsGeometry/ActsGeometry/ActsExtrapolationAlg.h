/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSEXTRAPOLATIONALG_H
#define ACTSGEOMETRY_ACTSEXTRAPOLATIONALG_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Plugins/MaterialMapping/MaterialStep.hpp"
#include "Acts/Plugins/MaterialMapping/MaterialTrack.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Plugins/MaterialMapping/MaterialTrack.hpp"
#include "Acts/Utilities/Helpers.hpp"

// PACKAGE

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>


namespace Acts {
  class TrackingGeometry;
  
  namespace detail {
    class Step;
  }
}

//class IActsMaterialTrackWriterSvc;

template<typename>
class RootExCellWriter;
class EventContext;
class IAthRNGSvc;
class ActsExtrapolationTool;
class IActsPropStepRootWriterSvc;

class ActsExtrapolationAlg : public AthReentrantAlgorithm {
public:
  ActsExtrapolationAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
  
private:
  ServiceHandle<IActsPropStepRootWriterSvc> m_propStepWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;

  ToolHandle<ActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "ActsExtrapolationTool"};

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> m_exCells;
  std::shared_ptr<RootExCellWriter<Acts::TrackParameters>> m_rootEccWriter;
  
  // poor-mans Particle Gun is included here right now
  Gaudi::Property<std::vector<double>> m_etaRange{this, "EtaRange", {-3, 3}, ""};
  Gaudi::Property<std::vector<double>> m_ptRange{this, "PtRange", {0.1, 1000}, ""};

  // this does not work right now
  Gaudi::Property<bool> m_writeMaterialTracks{this, "WriteMaterialTracks", false, ""};
  //ServiceHandle<IActsMaterialTrackWriterSvc> m_materialTrackWriterSvc;
  
  mutable std::mutex m_writeMutex{};
  mutable std::unique_ptr<std::ofstream> m_objOut;
  mutable size_t m_objVtxCount{0};

  void writeStepsObj(std::vector<Acts::detail::Step> steps) const;

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
    double theta = Acts::VectorHelpers::theta(mom);
    double phi = Acts::VectorHelpers::phi(mom);
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
