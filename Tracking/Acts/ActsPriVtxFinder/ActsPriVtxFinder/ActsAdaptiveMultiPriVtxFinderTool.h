/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSPRIVTXFINDER_ACTSADAPTIVEMULTIPRIVTXFINDERTOOL_H
#define ACTSPRIVTXFINDER_ACTSADAPTIVEMULTIPRIVTXFINDERTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/EventContext.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrkTrackLink/ITrackLink.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkTrack/LinkToTrack.h"

// PACKAGE
#include "ActsPriVtxFinderInterfaces/IActsPriVtxFinderTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
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

namespace MagField {
  class IMagFieldSvc;
}

namespace Acts {
class Surface;
class BoundaryCheck;
}


class ActsAdaptiveMultiPriVtxFinderTool : public extends<AthAlgTool, IActsPriVtxFinderTool>
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

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
     findVertex(const TrackCollection* trackTES, const EventContext& ctx) const override;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
     findVertex(const Trk::TrackParticleBaseCollection* trackTES, const EventContext& ctx) const override;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
     findVertex(const xAOD::TrackParticleContainer* trackParticles, const EventContext& ctx) const override;


private:

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
  findVertex(const std::vector<const Trk::ITrackLink*>& trackVector, const EventContext& ctx) const;

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
  using VertexSeedFinder = Acts::TrackDensityVertexFinder<VertexFitter, Acts::GaussianTrackDensity>;
  using VertexFinder = Acts::AdaptiveMultiVertexFinder<VertexFitter, VertexSeedFinder>;

  std::shared_ptr<VertexFinder> m_vertexFinder = nullptr;

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};
  ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  // Configuration variables
  // For details check ACTS documentation
  //
  // Annealing tool config
  std::vector<double> m_annealingTemps;
  double m_annealingCutOff;
  // Fitter config variables
  unsigned int m_fitterMaxIterations;
  double m_fitterMaxDistToLinPoint;
  double m_fitterMaxRelativeShift;
  bool m_fitterDoSmoothing;
  // Finder config variables
  bool m_useBeamConstraint;
  double m_tracksMaxZinterval;
  double m_tracksMaxSignificance;
  double m_maxVertexChi2;
  bool m_doRealMultiVertex;
  bool m_useFastCompatibility;
  double m_maxMergeVertexSignificance;
  double m_minWeight;
  int m_maxIterations;
  bool m_addSingleTrackVertices;
  bool m_do3dSplitting;
  double m_maximumVertexContamination;
  double m_looseConstrValue;
  bool m_refitAfterBadVertex;
  bool m_useVertexCovForIPEstimation;
  bool m_useSeedConstraint;
  // Final vertex selection variables
  double m_finalCutMaxVertexChi2;
  // Gaussian seed finder variables
  double m_gaussianMaxD0Significance;
  double m_gaussianMaxZ0Significance;
  // IP Estimator config
  int m_ipEstMaxIterations;
  double m_ipEstPrecision;

};

#endif
