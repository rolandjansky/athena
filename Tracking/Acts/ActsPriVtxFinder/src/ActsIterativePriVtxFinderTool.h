/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSPRIVTXFINDER_ACTSITERATIVEPRIVTXFINDERTOOL_H
#define ACTSPRIVTXFINDER_ACTSITERATIVEPRIVTXFINDERTOOL_H 

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrkTrackLink/ITrackLink.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkTrack/LinkToTrack.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"

// Need to include this early; otherwise, we run into errors with
// ReferenceWrapperAnyCompat in clang builds due the is_constructable
// specialization defined there getting implicitly instantiated earlier.
#include "Acts/Propagator/Propagator.hpp"

// PACKAGE
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Vertexing/IterativeVertexFinder.hpp"
#include "Acts/Vertexing/FullBilloirVertexFitter.hpp"
#include "Acts/Vertexing/HelicalTrackLinearizer.hpp"
#include "Acts/Vertexing/TrackDensityVertexFinder.hpp"
#include "Acts/Vertexing/Vertex.hpp"
#include "Acts/Vertexing/ImpactPointEstimator.hpp"
#include "Acts/Utilities/Logger.hpp"

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <cmath>

namespace Acts {
class Surface;
class BoundaryCheck;
}

class ActsIterativePriVtxFinderTool : public extends<AthAlgTool, InDet::IVertexFinder>
{

// Track wrapper input for the Acts vertexing
class TrackWrapper {
public:
  TrackWrapper(const Trk::ITrackLink* trkLink, const Acts::BoundTrackParameters& boundParams)

    : m_trkLink(trkLink)
    , m_boundParams(boundParams)
  {}

  const Acts::BoundTrackParameters& parameters() const {return m_boundParams;}

  const Trk::ITrackLink* trackLink() const {return m_trkLink;}

private:
  const Trk::ITrackLink* m_trkLink;
  Acts::BoundTrackParameters m_boundParams;
};

public:
  virtual StatusCode initialize() override;

  /**
   * Constructor
   */
  ActsIterativePriVtxFinderTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  using InDet::IVertexFinder::findVertex;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
     findVertex(const EventContext& ctx, const TrackCollection* trackTES) const override;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
     findVertex(const EventContext& ctx, const xAOD::TrackParticleContainer* trackParticles) const override;

private:

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
  findVertex(const EventContext& ctx, std::vector<std::unique_ptr<Trk::ITrackLink>> trackVector) const;

  Trk::Perigee* actsBoundToTrkPerigee(
  const Acts::BoundTrackParameters& bound, const Acts::Vector3& surfCenter) const;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const
  {
    return m_trackingGeometryTool.get();
  }

  using Propagator = Acts::Propagator<Acts::EigenStepper<ATLASMagneticFieldWrapper>, Acts::Navigator>;
  using TrackLinearizer = Acts::HelicalTrackLinearizer<Propagator>;
  using VertexFitter = Acts::FullBilloirVertexFitter<TrackWrapper, TrackLinearizer>;
  using VertexSeedFinder = Acts::TrackDensityVertexFinder<VertexFitter, Acts::GaussianTrackDensity<TrackWrapper>>;
  using VertexFinder = Acts::IterativeVertexFinder<VertexFitter, VertexSeedFinder>;

  std::shared_ptr<VertexFinder> m_vertexFinder = nullptr;

  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "", "ActsTrackingGeometryTool"};
  ToolHandle<IActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "", "ActsExtrapolationTool"};
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkFilter{this, "TrackSelector", "", "InDetTrackSelectionTool"};
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey {this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

  // Configuration variables
  // For details check ACTS documentation

  // Full-Billoir Fitter config variables
  UnsignedIntegerProperty m_fitterMaxIterations{this, "fitterMaxIterations", 5, "Vertex fitter max. iterations"};

  // Iterative vertex Finder config variables
  BooleanProperty m_useBeamConstraint{this, "useBeamConstraint", true, "Use beam constraint"};
  DoubleProperty m_significanceCutSeeding{this, "significanceCutSeeding", 10, "Seeding Cut Significance"};
  DoubleProperty m_maximumChi2cutForSeeding{this, "maximumChi2cutForSeeding", 36., "Max. Chi2 cut for seeding"};
  UnsignedIntegerProperty m_maxVertices{this, "maxVertices", 50, "Max number of vertices"};
  BooleanProperty m_createSplitVertices{this, "createSplitVertices", false, "Create split vertices or not"};
  UnsignedIntegerProperty m_splitVerticesTrkInvFraction{this, "splitVerticesTrkInvFraction", 2, "Track Inv. fraction for split vertices"};
  BooleanProperty m_reassignTracksAfterFirstFit{this, "reassignTracksAfterFirstFit", false, "Whether re-assign tracks after first vertex fit or not"};
  BooleanProperty m_doMaxTracksCut{this, "doMaxTracksCut", false, "Whether use max. tracks cut or not"};
  UnsignedIntegerProperty m_maxTracks{this, "maxTracks", 5000, "Max. number of tracks to use for vertex finding"};
  DoubleProperty m_cutOffTrackWeight{this, "cutOffTrackWeight", 0.01, "Min. track weight allowed"}; 

  // Gaussian seed finder variables
  DoubleProperty m_gaussianMaxD0Significance{this, "gaussianMaxD0Significance", 3.5, "Gaussian seeder max d0 track significance"};
  DoubleProperty m_gaussianMaxZ0Significance{this, "gaussianMaxDZSignificance", 12.0, "Gaussian seeder max z0 track significance"};

  // IP Estimator config
  UnsignedIntegerProperty m_ipEstMaxIterations{this, "ipEstMaxIterations", 20, "IpEstimator max. iterations"};
  DoubleProperty m_ipEstPrecision{this, "ipEstPrecision", 1e-10, "IpEstimator precision"};

};

#endif
