/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsAdaptiveMultiPriVtxFinderTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// PACKAGE
#include "ActsGeometry/ActsTrackingGeometrySvc.h"
#include "ActsInterop/Logger.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"

// ACTS
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Utilities/AnnealingUtility.hpp"

// STL
#include <iostream>
#include <memory>

namespace
{
  // Helper struct for vertex signal compatibility
  struct VertexAndSignalComp {
    xAOD::Vertex* first;
    double second;
    VertexAndSignalComp(xAOD::Vertex* p1, double p2)
    : first(p1), second(p2) {}
    bool
    operator < (const VertexAndSignalComp& other) const
    {return second > other.second;}
  };
  } //anonymous namespace

  ActsAdaptiveMultiPriVtxFinderTool::ActsAdaptiveMultiPriVtxFinderTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
  {}

StatusCode
ActsAdaptiveMultiPriVtxFinderTool::initialize()
{
	using namespace std::literals::string_literals;

    ATH_CHECK(m_beamSpotKey.initialize());
    ATH_CHECK(m_trkFilter.retrieve());

    ATH_MSG_INFO("Initializing ACTS AMVF tool");
    ATH_CHECK( m_trackingGeometryTool.retrieve() );
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
    = m_trackingGeometryTool->trackingGeometry();

    ATH_CHECK( m_extrapolationTool.retrieve() );

    Acts::Navigator navigator(trackingGeometry);

    using BField_t = ATLASMagneticFieldWrapper;
    BField_t bField;
    auto stepper = Acts::EigenStepper<BField_t>(std::move(bField));
    auto propagator = std::make_shared<Propagator>(std::move(stepper), 
      std::move(navigator));
  	// IP Estimator
    using IPEstimator = Acts::ImpactPointEstimator<TrackWrapper, Propagator>;
    IPEstimator::Config ipEstCfg(bField, propagator);
    ipEstCfg.maxIterations = m_ipEstMaxIterations;
    ipEstCfg.precision = m_ipEstPrecision;
    IPEstimator ipEst(ipEstCfg);

    Acts::AnnealingUtility::Config annealingConfig(m_annealingTemps);
    annealingConfig.cutOff = m_annealingCutOff;
    Acts::AnnealingUtility annealingUtility(annealingConfig);

    // Create a custom std::function to extract BoundParameters 
    // from TrackWrapper
    std::function<Acts::BoundTrackParameters(TrackWrapper)> extractParameters =
        [](TrackWrapper params) { return params.parameters(); };

    // Vertex fitter configuration
    VertexFitter::Config fitterCfg(ipEst);
    fitterCfg.annealingTool = annealingUtility;
    fitterCfg.maxIterations = m_fitterMaxIterations;
    fitterCfg.maxDistToLinPoint = m_fitterMaxDistToLinPoint;
    fitterCfg.minWeight = m_minWeight;
    fitterCfg.maxRelativeShift = m_fitterMaxRelativeShift;
    fitterCfg.doSmoothing = m_fitterDoSmoothing;
    VertexFitter fitter(fitterCfg, extractParameters);

  	// Linearizer for Acts::BoundParameters type test
    TrackLinearizer::Config ltConfig(bField, propagator);
    TrackLinearizer linearizer(ltConfig);

    // Set up Gaussian track density
    Acts::GaussianTrackDensity<TrackWrapper>::Config trackDensityConfig;
    trackDensityConfig.d0MaxSignificance = m_gaussianMaxD0Significance;
    trackDensityConfig.z0MaxSignificance = m_gaussianMaxZ0Significance;
    Acts::GaussianTrackDensity<TrackWrapper> trackDensity(trackDensityConfig);

    // Vertex seed finder
    VertexSeedFinder::Config seedFinderConfig;
    seedFinderConfig.trackDensityEstimator = trackDensity;
    VertexSeedFinder seedFinder(seedFinderConfig, extractParameters);
    VertexFinder::Config finderConfig(std::move(fitter), seedFinder,
      ipEst, linearizer);

    // Vertex finder config
    finderConfig.useBeamSpotConstraint = m_useBeamConstraint;
    finderConfig.tracksMaxZinterval = m_tracksMaxZinterval;
    finderConfig.tracksMaxSignificance = m_tracksMaxSignificance;
    finderConfig.maxVertexChi2 = m_maxVertexChi2;
    finderConfig.doRealMultiVertex = m_doRealMultiVertex;
    finderConfig.useFastCompatibility = m_useFastCompatibility;
    finderConfig.maxMergeVertexSignificance = m_maxMergeVertexSignificance;
    finderConfig.minWeight = m_minWeight;
    finderConfig.maxIterations = m_maxIterations;
    finderConfig.addSingleTrackVertices = m_addSingleTrackVertices;
    finderConfig.do3dSplitting = m_do3dSplitting;
    finderConfig.maximumVertexContamination = m_maximumVertexContamination;
    finderConfig.looseConstrValue = m_looseConstrValue;
    finderConfig.useVertexCovForIPEstimation = m_useVertexCovForIPEstimation;
    finderConfig.useSeedConstraint = m_useSeedConstraint;
    m_vertexFinder = std::make_shared<VertexFinder>(finderConfig, extractParameters);

    ATH_MSG_INFO("ACTS AMVF tool successfully initialized");
    return StatusCode::SUCCESS;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
ActsAdaptiveMultiPriVtxFinderTool::findVertex(const EventContext& ctx, const TrackCollection* trackTES) const
{
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx};
  const Trk::RecVertex& beamposition(beamSpotHandle->beamVtx());

  std::vector<std::unique_ptr<Trk::ITrackLink>> selectedTracks;

  typedef DataVector<Trk::Track>::const_iterator TrackDataVecIter;

  bool selectionPassed;
  for (TrackDataVecIter itr = (*trackTES).begin(); itr != (*trackTES).end(); itr++) {
    if (m_useBeamConstraint) {
      selectionPassed = static_cast<bool>(m_trkFilter->accept(**itr, &beamposition));
    } else {
        Trk::Vertex null(Amg::Vector3D(0, 0, 0));
        selectionPassed = static_cast<bool>(m_trkFilter->accept(**itr, &null));
    }
    if (selectionPassed) {
        ElementLink<TrackCollection> link;
        link.setElement(const_cast<Trk::Track*>(*itr));
        auto trkPtr = std::make_unique<Trk::LinkToTrack>(link);
        trkPtr->setStorableObject(*trackTES);
        selectedTracks.push_back(std::move(trkPtr));
    }
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = findVertex(ctx, std::move(selectedTracks));

  return returnContainers;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
ActsAdaptiveMultiPriVtxFinderTool::findVertex(const EventContext& ctx, const xAOD::TrackParticleContainer* trackParticles) const
{
  std::vector<std::unique_ptr<Trk::ITrackLink>> selectedTracks;
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx};
  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();
  beamposition.setPosition(beamSpotHandle->beamVtx().position());
  beamposition.setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());

  typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;

  bool selectionPassed;
  for (TrackParticleDataVecIter itr = (*trackParticles).begin(); itr != (*trackParticles).end(); itr++) {
    if (m_useBeamConstraint) {
      selectionPassed = static_cast<bool>(m_trkFilter->accept(**itr, &beamposition));
    } else {
        xAOD::Vertex null;
        null.makePrivateStore();
        null.setPosition(Amg::Vector3D(0, 0, 0));
        AmgSymMatrix(3) vertexError;
        vertexError.setZero();
        null.setCovariancePosition(vertexError);
        selectionPassed = static_cast<bool>(m_trkFilter->accept(**itr, &null));
    }

    if (selectionPassed) {
        ElementLink<xAOD::TrackParticleContainer> link;
        link.setElement(const_cast<xAOD::TrackParticle*>(*itr));
        auto trkPtr = std::make_unique<Trk::LinkToXAODTrackParticle>(link);
        trkPtr->setStorableObject(*trackParticles);
        selectedTracks.push_back(std::move(trkPtr));
    }
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = findVertex(ctx, std::move(selectedTracks));

  return returnContainers;
}


std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
ActsAdaptiveMultiPriVtxFinderTool::findVertex(const EventContext& ctx, std::vector<std::unique_ptr<Trk::ITrackLink>> trackVector) const
{
    using namespace Acts::UnitLiterals;

    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx};
    const Acts::Vector3D& beamSpotPos = beamSpotHandle->beamVtx().position();
    Acts::Vertex<TrackWrapper> beamSpotConstraintVtx(beamSpotPos);
    beamSpotConstraintVtx.setCovariance(beamSpotHandle->beamVtx().covariancePosition());

    std::shared_ptr<Acts::PerigeeSurface> perigeeSurface =
    Acts::Surface::makeShared<Acts::PerigeeSurface>(beamSpotPos);

    // Get the magnetic field context
    Acts::MagneticFieldContext magFieldContext = m_extrapolationTool->getMagneticFieldContext(ctx);

    const auto& geoContext
    = m_trackingGeometryTool->getGeometryContext(ctx).any();

    // Convert tracks to Acts::BoundParameters
    std::vector<TrackWrapper> allTracks;

    for (const auto& trk : trackVector) {

      const auto& trkParams = trk->parameters();
      const auto& params = trkParams->parameters();

      Acts::BoundVector actsParams;
      actsParams << params(0), params(1), params(2), params(3), params(4)*1./(1_MeV), 0.;

      if(trkParams->covariance() == nullptr){
          continue;
      }
      auto cov = *(trkParams->covariance());
      
      // TODO: check if the following works as well:
      // cov->col(4) *= 1./1_MeV;
      // cov->row(4) *= 1./1_MeV;
      Acts::BoundSymMatrix covMat;
      covMat << cov(0,0) , cov(0,1) , cov(0,2) , cov(0,3) , cov(0,4) *1./(1_MeV), 0      
      , cov(1,0) , cov(1,1) , cov(1,2) , cov(1,3) , cov(1,4) *1./(1_MeV) , 0
      , cov(2,0) , cov(2,1) , cov(2,2) , cov(2,3) , cov(2,4) *1./(1_MeV) , 0
      , cov(3,0) , cov(3,1) , cov(3,2) , cov(3,3) , cov(3,4) *1./(1_MeV) , 0 
      , cov(4,0) *1./(1_MeV) , cov(4,1) *1./(1_MeV) , cov(4,2) *1./(1_MeV) , cov(4,3) *1./(1_MeV) , cov(4,4) *1./(1_MeV*1_MeV), 0
      , 0. , 0. , 0. , 0., 0., 1.;
      
      allTracks.emplace_back(trk.get(),Acts::BoundTrackParameters(perigeeSurface, actsParams, covMat));
    }

    std::vector<const TrackWrapper*> allTrackPtrs;
    for(const auto& trk : allTracks){
      allTrackPtrs.push_back(&trk);
    }

    Acts::VertexingOptions<TrackWrapper> vertexingOptions(geoContext,
       magFieldContext);

    if(!m_useBeamConstraint){
      beamSpotConstraintVtx.setPosition(Acts::Vector3D::Zero());
      beamSpotConstraintVtx.setCovariance(Acts::ActsSymMatrixD<3>::Zero());
    }

    vertexingOptions.vertexConstraint = beamSpotConstraintVtx;

    VertexFinder::State finderState;

    auto findResult = m_vertexFinder->find(allTrackPtrs, vertexingOptions, finderState);

    xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
    xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
    theVertexContainer->setStore(theVertexAuxContainer);

    if(!findResult.ok()){
      xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back(dummyxAODVertex);
      dummyxAODVertex->setPosition(beamSpotHandle->beamVtx().position());
      dummyxAODVertex->setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);

      return std::make_pair(theVertexContainer, theVertexAuxContainer);
  }

  std::vector<Acts::Vertex<TrackWrapper>> allVertices = *findResult;

  std::vector<VertexAndSignalComp> vtxList;
    // Reserve memory for all vertices
  vtxList.reserve(allVertices.size());

  for(const auto& vtx : allVertices){
      xAOD::Vertex* xAODVtx = new xAOD::Vertex;
      xAODVtx->makePrivateStore();
      xAODVtx->setPosition(vtx.position());
      xAODVtx->setCovariancePosition(vtx.covariance());
      xAODVtx->setFitQuality(vtx.fitQuality().first, vtx.fitQuality().second);

      const auto& tracks = vtx.tracks();
      std::vector<Trk::VxTrackAtVertex>* trkAtVtxVec = &(xAODVtx->vxTrackAtVertex());
      for(const auto& trk : tracks){

        // Clean up incompatible tracks
        if ((trk.vertexCompatibility > m_maxVertexChi2 && m_useFastCompatibility) ||
          ((trk.trackWeight < m_minWeight
            || trk.chi2Track > m_maxVertexChi2)
          && !m_useFastCompatibility)) {
          continue;
        }

        Trk::Perigee* fittedPerigee = actsBoundToTrkPerigee(trk.fittedParams, beamSpotPos);
        Trk::Perigee* originalPerigee = actsBoundToTrkPerigee((trk.originalParams)->parameters(), beamSpotPos);

        Trk::VxTrackAtVertex trkAtVtx(trk.chi2Track, fittedPerigee, originalPerigee);
        trkAtVtx.setVtxCompatibility(trk.vertexCompatibility);
        trkAtVtx.setWeight(trk.trackWeight);
        trkAtVtxVec->push_back(trkAtVtx);

        Trk::LinkToXAODTrackParticle* linkToXAODTP =
        dynamic_cast<Trk::LinkToXAODTrackParticle*>((trk.originalParams)->trackLink()->clone());
        if (linkToXAODTP) {
          xAODVtx->addTrackAtVertex(*linkToXAODTP, trk.trackWeight);
        }
      }
      // Find signal compatibility
      double sigComp = estimateSignalCompatibility(xAODVtx);
            // Insert vertex at correct position according to sigComp value
      VertexAndSignalComp vertexAndSig(xAODVtx, sigComp);
      auto it = std::lower_bound( vtxList.begin(), vtxList.end(), vertexAndSig );
      vtxList.insert( it, vertexAndSig );
  }

for(unsigned int i = 0; i < vtxList.size(); i++){
    auto vtx = vtxList[i].first;
    theVertexContainer->push_back(vtx);
    if(i == 0){
      vtx->setVertexType(xAOD::VxType::PriVtx);
  }
  else{
      vtx->setVertexType(xAOD::VxType::PileUp);
  }
}

// Add dummy vertex to collection
xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
theVertexContainer->push_back(dummyxAODVertex);

if(!vtxList.empty()){
      // If HS vertex exists, create dummy with same position/covariance
    xAOD::Vertex* primaryVtx = theVertexContainer->front();
    dummyxAODVertex->setPosition(primaryVtx->position());
    dummyxAODVertex->setCovariancePosition(primaryVtx->covariancePosition());
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
}
else{
    dummyxAODVertex->setPosition(beamSpotHandle->beamVtx().position());
    dummyxAODVertex->setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
}

return std::make_pair(theVertexContainer, theVertexAuxContainer);
}


Trk::Perigee* ActsAdaptiveMultiPriVtxFinderTool::actsBoundToTrkPerigee(
  const Acts::BoundTrackParameters& bound, const Acts::Vector3D& surfCenter) const {
  using namespace Acts::UnitLiterals;
  AmgSymMatrix(5)* cov =  new AmgSymMatrix(5)(bound.covariance()->block<5,5>(0,0));
  cov->col(Trk::qOverP) *= 1_MeV;
  cov->row(Trk::qOverP) *= 1_MeV;
  Acts::ActsVectorD<5> params = bound.parameters().head<5>();
  params[Trk::qOverP] *= 1_MeV;

  return new Trk::Perigee(params, Trk::PerigeeSurface(surfCenter), cov);
}

double
ActsAdaptiveMultiPriVtxFinderTool::estimateSignalCompatibility(xAOD::Vertex* vtx) const {
  double totalPt2 = 0;
  unsigned int nTracks = 0;

  for(const auto& trk : vtx->vxTrackAtVertex()){
    if ((trk.vtxCompatibility() < m_finalCutMaxVertexChi2 && m_useFastCompatibility) ||
      (trk.weight() > m_minWeight
         && trk.trackQuality().chiSquared() < m_finalCutMaxVertexChi2
         && !m_useFastCompatibility)) {
      const Trk::TrackParameters* perigee = nullptr;
      if (trk.perigeeAtVertex() != nullptr) {
          perigee = trk.perigeeAtVertex();
      } else {
          ATH_MSG_VERBOSE("Only initialPerigee is available");
          perigee = trk.initialPerigee();
      }
      if (perigee == nullptr) {
          ATH_MSG_ERROR("Neutrals are not supported. Skipping track in pT calculation...");
          continue;
      }
      totalPt2 +=
      std::pow(std::fabs(1. / perigee->parameters()[Trk::qOverP]) * std::sin(perigee->parameters()[Trk::theta]), 2);
      nTracks += 1;
    }
  }
  return totalPt2 * std::sqrt((double) nTracks);
}
