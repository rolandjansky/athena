/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSPRIVTXFINDER_ACTSADAPTIVEMULTIPRIVTXFINDERTOOL_H
#define ACTSPRIVTXFINDER_ACTSADAPTIVEMULTIPRIVTXFINDERTOOL_H

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
#include "Acts/Utilities/Units.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Vertexing/AdaptiveMultiVertexFinder.hpp"
#include "Acts/Vertexing/AdaptiveMultiVertexFitter.hpp"
#include "Acts/Vertexing/HelicalTrackLinearizer.hpp"
#include "Acts/Vertexing/TrackDensityVertexFinder.hpp"
#include "Acts/Vertexing/Vertex.hpp"
#include "Acts/Vertexing/ImpactPointEstimator.hpp"

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <cmath>

namespace Acts {
class Surface;
class BoundaryCheck;
}


class ActsAdaptiveMultiPriVtxFinderTool : public extends<AthAlgTool, InDet::IVertexFinder>
{

// Track wrapper input for the Acts vertexing
class TrackWrapper {
public:
  TrackWrapper(const Trk::ITrackLink* trkLink, const Acts::BoundParameters& boundParams)
    : m_trkLink(trkLink)
    , m_boundParams(boundParams)

  {}

  const Acts::BoundParameters& parameters() const {return m_boundParams;}

  const Trk::ITrackLink* trackLink() const {return m_trkLink;}

private:
  const Trk::ITrackLink* m_trkLink;
  Acts::BoundParameters m_boundParams;
};

public:
  virtual StatusCode initialize() override;

  ActsAdaptiveMultiPriVtxFinderTool(const std::string& type, const std::string& name,
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
  const Acts::BoundParameters& bound, const Acts::Vector3D& surfCenter) const;

  double estimateSignalCompatibility(xAOD::Vertex* vtx) const;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const
  {
    return m_trackingGeometryTool.get();
  }

  using Propagator = Acts::Propagator<Acts::EigenStepper<ATLASMagneticFieldWrapper>, Acts::Navigator>;
  using TrackLinearizer = Acts::HelicalTrackLinearizer<Propagator>;
  using VertexFitter = Acts::AdaptiveMultiVertexFitter<TrackWrapper, TrackLinearizer>;
  using VertexSeedFinder = Acts::TrackDensityVertexFinder<VertexFitter, Acts::GaussianTrackDensity<TrackWrapper>>;
  using VertexFinder = Acts::AdaptiveMultiVertexFinder<VertexFitter, VertexSeedFinder>;

  std::shared_ptr<VertexFinder> m_vertexFinder = nullptr;

  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "", "ActsTrackingGeometryTool"};
  ToolHandle<IActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "", "ActsExtrapolationTool"};
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkFilter{this, "TrackSelector", "", "InDetTrackSelectionTool"};
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey {this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};


  // Configuration variables
  // For details check ACTS documentation
  //
  // Annealing tool config
  DoubleArrayProperty m_annealingTemps{this, "annealingTemps", {8.0, 4.0, 2.0, 1.4142136, 1.2247449, 1.0}, "Annealing temperatures"};
  DoubleProperty m_annealingCutOff{this, "annealingCutOff", 9., "Annealing cut-off value"};
  // Fitter config variables
  UnsignedIntegerProperty m_fitterMaxIterations{this, "fitterMaxIterations", 30, "Vertex fitter max. iterations"};
  DoubleProperty m_fitterMaxDistToLinPoint{this, "fitterMaxDistToLinPoint", 0.5, "Vertex fitter max. distance to LinPoint"};
  DoubleProperty m_fitterMaxRelativeShift{this, "fitterMaxRelativeShift", 0.01, "Vertex fitter max. relative shift"};
  BooleanProperty m_fitterDoSmoothing{this, "fitterDoSmoothing", true, "Vertex fitter doSmoothing"};
  // Finder config variables
  BooleanProperty m_useBeamConstraint{this, "useBeamConstraint", true, "Use beam constraint"};
  DoubleProperty m_tracksMaxZinterval{this, "tracksMaxZinterval", 1., "Tracks max. Z-interval"};
  DoubleProperty m_tracksMaxSignificance{this, "tracksMaxSignificance", 5., "Tracks max. significance"};
  DoubleProperty m_maxVertexChi2{this, "maxVertexChi2", 18.42, "Max vertex chi2"};
  BooleanProperty m_doRealMultiVertex{this, "doRealMultiVertex", true, "Do real multivertex fit"};
  BooleanProperty m_useFastCompatibility{this, "useFastCompatibility", true, "Use fast compatibility estimation"};
  DoubleProperty m_maxMergeVertexSignificance{this, "maxMergeVertexSignificance", 3., "Max merge vertex significance"};
  DoubleProperty m_minWeight{this, "minWeight", 0.0001, "Min track weight for finder and fitter"};
  UnsignedIntegerProperty m_maxIterations{this, "maxIterations", 100, "Vertex finder max. iterations"};
  BooleanProperty m_addSingleTrackVertices{this, "addSingleTrackVertices", false, "Add single-track vertices"};
  BooleanProperty m_do3dSplitting{this, "do3dSplitting", false, "Do 3d-splitting"};
  DoubleProperty m_maximumVertexContamination{this, "maximumVertexContamination", 0.5, "Max. vertex contamination"};
  DoubleProperty m_looseConstrValue{this, "looseConstrValue", 1e+8, "Loose constraint value"};
  BooleanProperty m_useVertexCovForIPEstimation{this, "useVertexCovForIPEstimation", false, "Use seed vertex cov for IPEstimation"};
  BooleanProperty m_useSeedConstraint{this, "useSeedConstraint", true, "Use seed constraint in fit"};
  // Final vertex selection variables
  DoubleProperty m_finalCutMaxVertexChi2{this, "finalCutMaxVertexChi2", 18.42, "Final cut max. vertex chi2"};
  // Gaussian seed finder variables
  DoubleProperty m_gaussianMaxD0Significance{this, "gaussianMaxD0Significance", 3.5, "Gaussian seeder max d0 track significance"};
  DoubleProperty m_gaussianMaxZ0Significance{this, "gaussianMaxDZSignificance", 12.0, "Gaussian seeder max z0 track significance"};
  // IP Estimator config
  UnsignedIntegerProperty m_ipEstMaxIterations{this, "ipEstMaxIterations", 20, "IpEstimator max. iterations"};
  DoubleProperty m_ipEstPrecision{this, "ipEstPrecision", 1e-10, "IpEstimator precision"};

};

#endif
