/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsIterativePriVtxFinderTool.h"

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

  ActsIterativePriVtxFinderTool::ActsIterativePriVtxFinderTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
  {}

StatusCode
ActsIterativePriVtxFinderTool::initialize()
{
  using namespace std::literals::string_literals;

  ATH_CHECK(m_beamSpotKey.initialize());
  ATH_CHECK(m_trkFilter.retrieve());

  ATH_MSG_INFO("Initializing ACTS Iterative Vertex Finder tool");
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

  // Create a custom std::function to extract BoundParameters 
  // from TrackWrapper
  std::function<Acts::BoundTrackParameters(TrackWrapper)> extractParameters =
    [](TrackWrapper params) { return params.parameters(); };

  // Full Billoir Vertex fitter setup
  VertexFitter::Config fitterCfg;
  fitterCfg.maxIterations = m_fitterMaxIterations;
  VertexFitter fitter(fitterCfg, extractParameters);

  // Linearizer for Acts::BoundParameters type test
  TrackLinearizer::Config ltConfig(bField, propagator);
  TrackLinearizer linearizer(ltConfig);

  // Seed finder setup
  // Set up Gaussian track density
  Acts::GaussianTrackDensity<TrackWrapper>::Config trackDensityConfig;
  trackDensityConfig.d0MaxSignificance = m_gaussianMaxD0Significance;
  trackDensityConfig.z0MaxSignificance = m_gaussianMaxZ0Significance;
  Acts::GaussianTrackDensity<TrackWrapper> trackDensity(trackDensityConfig);

  // Vertex seed finder
  VertexSeedFinder::Config seedFinderConfig;
  seedFinderConfig.trackDensityEstimator = trackDensity;
  VertexSeedFinder seedFinder(seedFinderConfig, extractParameters);
    
  // Iterative Vertex Finder setup
  VertexFinder::Config finderConfig(fitter, linearizer, std::move(seedFinder), ipEst);
  finderConfig.useBeamConstraint = m_useBeamConstraint;
  finderConfig.significanceCutSeeding = m_significanceCutSeeding;
  finderConfig.maximumChi2cutForSeeding = m_maximumChi2cutForSeeding;
  finderConfig.maxVertices = m_maxVertices;
  finderConfig.createSplitVertices = m_createSplitVertices;
  finderConfig.splitVerticesTrkInvFraction = m_splitVerticesTrkInvFraction;
  finderConfig.reassignTracksAfterFirstFit = m_reassignTracksAfterFirstFit;
  finderConfig.doMaxTracksCut = m_doMaxTracksCut;
  finderConfig.maxTracks = m_maxTracks;
  finderConfig.cutOffTrackWeight = m_cutOffTrackWeight;
  m_vertexFinder = std::make_shared<VertexFinder>(finderConfig, extractParameters); 

  ATH_MSG_INFO("ACTS Iterative Vertex Finder tool successfully initialized");
  return StatusCode::SUCCESS;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
ActsIterativePriVtxFinderTool::findVertex(const EventContext& ctx, const TrackCollection* trackTES) const
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
ActsIterativePriVtxFinderTool::findVertex(const EventContext& ctx, const xAOD::TrackParticleContainer* trackParticles) const
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
ActsIterativePriVtxFinderTool::findVertex(const EventContext& ctx, std::vector<std::unique_ptr<Trk::ITrackLink>> trackVector) const
{

  using namespace Acts::UnitLiterals;

  // Vertex finding algorithm begins 
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx};

  // The output vertex containers
  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
  theVertexContainer->setStore(theVertexAuxContainer);
            
  // bail out early with only Dummy vertex if multiplicity cut is applied and exceeded
  if((m_doMaxTracksCut && (trackVector.size() > m_maxTracks)) || trackVector.empty()) {
    ATH_MSG_WARNING(trackVector.size()
                    << " tracks - exceeds maximum (" << m_maxTracks
                    << "), skipping vertexing and returning only dummy...");
    xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
    theVertexContainer->push_back(
				  dummyxAODVertex); // have to add vertex to container here first so it can use its aux store
    dummyxAODVertex->setPosition(beamSpotHandle->beamVtx().position());
    dummyxAODVertex->setCovariancePosition(
					   beamSpotHandle->beamVtx().covariancePosition());
    dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    return std::make_pair(theVertexContainer, theVertexAuxContainer);
  }

  const Acts::Vector3D& beamSpotPos = beamSpotHandle->beamVtx().position();
  Acts::Vertex<TrackWrapper> beamSpotConstraintVtx(beamSpotPos);
  beamSpotConstraintVtx.setCovariance(beamSpotHandle->beamVtx().covariancePosition());

  std::shared_ptr<Acts::PerigeeSurface> perigeeSurface =
    Acts::Surface::makeShared<Acts::PerigeeSurface>((trackVector[0])->parameters()->associatedSurface().transform());

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

  //Adding 4th dimensional timing info to vertex constraint as needed by ACTS
  Acts::Vector4D vtxConstraintPos;
  Acts::SymMatrix4D vtxConstraintCov;

  auto beamSpotCov = beamSpotHandle->beamVtx().covariancePosition();

  vtxConstraintPos << beamSpotPos(0), beamSpotPos(1), beamSpotPos(2), 0.;
  vtxConstraintCov << beamSpotCov(0,0), beamSpotCov(0,1), beamSpotCov(0,2), 0.
    , beamSpotCov(1,0), beamSpotCov(1,1), beamSpotCov(1,2), 0.
    , beamSpotCov(2,0), beamSpotCov(2,1), beamSpotCov(2,2), 0.
    , 0., 0., 0., 1.;
                  
  vertexingOptions.vertexConstraint.setFullPosition(vtxConstraintPos);
  vertexingOptions.vertexConstraint.setFullCovariance(vtxConstraintCov);

  VertexFinder::State finderState(magFieldContext);

  auto findResult = m_vertexFinder->find(allTrackPtrs, vertexingOptions, finderState);

  if(!findResult.ok()){
    xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
    theVertexContainer->push_back(dummyxAODVertex);
    dummyxAODVertex->setPosition(beamSpotHandle->beamVtx().position());
    dummyxAODVertex->setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());
    dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);

    return std::make_pair(theVertexContainer, theVertexAuxContainer);
  }

  std::vector<Acts::Vertex<TrackWrapper>> allVertices = *findResult;

  for(const auto& vtx : allVertices){
    xAOD::Vertex* xAODVtx = new xAOD::Vertex;
    xAODVtx->makePrivateStore();
    xAODVtx->setPosition(vtx.position());
    xAODVtx->setCovariancePosition(vtx.covariance());
    xAODVtx->setFitQuality(vtx.fitQuality().first, vtx.fitQuality().second);
    
    const auto& tracks = vtx.tracks();
    std::vector<Trk::VxTrackAtVertex>* trkAtVtxVec = &(xAODVtx->vxTrackAtVertex());
    for(const auto& trk : tracks){

      Trk::Perigee* fittedPerigee = actsBoundToTrkPerigee(trk.fittedParams, beamSpotPos);
      //Trk::Perigee* originalPerigee = actsBoundToTrkPerigee((trk.originalParams)->parameters(), beamSpotPos);

      //Trk::VxTrackAtVertex trkAtVtx(trk.chi2Track, fittedPerigee, originalPerigee);
      Trk::VxTrackAtVertex trkAtVtx((trk.originalParams)->trackLink()->clone());
      trkAtVtx.setPerigeeAtVertex(fittedPerigee);
      trkAtVtx.setTrackQuality(Trk::FitQuality(trk.chi2Track, trk.ndf));
      trkAtVtx.setVtxCompatibility(trk.vertexCompatibility);
      trkAtVtx.setWeight(trk.trackWeight);
      trkAtVtxVec->push_back(trkAtVtx);

      const Trk::LinkToXAODTrackParticle* linkToXAODTP =
        dynamic_cast<const Trk::LinkToXAODTrackParticle*>((trk.originalParams)->trackLink());
      if (linkToXAODTP) {
	xAODVtx->addTrackAtVertex(*linkToXAODTP, trk.trackWeight);
      }
    }

    theVertexContainer->push_back(xAODVtx);
  }

  if (!m_createSplitVertices) {
    if (!theVertexContainer->empty()) {
      xAOD::Vertex* primaryVtx = theVertexContainer->front();
      if (!primaryVtx->vxTrackAtVertex().empty()) {
	primaryVtx->setVertexType((xAOD::VxType::VertexType)Trk::PriVtx);
	xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
	theVertexContainer->push_back(dummyxAODVertex);
	dummyxAODVertex->setPosition(primaryVtx->position());
	dummyxAODVertex->setCovariancePosition(primaryVtx->covariancePosition());
	dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
	dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
      } else {
	primaryVtx->setVertexType(xAOD::VxType::NoVtx);
      }
    } else {
      xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back(dummyxAODVertex);
      dummyxAODVertex->setPosition(beamSpotHandle->beamVtx().position());
      dummyxAODVertex->setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    }
  
    // loop over the pile up to set it as pile up (EXCLUDE first and last
    // vertex, do not do that in split mode)
    for (unsigned int i = 0; i < theVertexContainer->size() - 1; i++) {

      ATH_MSG_DEBUG(
        " Vtx: " << i << " x= " << (*theVertexContainer)[i]->position().x()
                 << " y= " << (*theVertexContainer)[i]->position().y() << " z= "
                 << (*theVertexContainer)[i]->position().z() << " ntracks= "
                 << (*theVertexContainer)[i]->vxTrackAtVertex().size()
                 << " chi2= " << (*theVertexContainer)[i]->chiSquared()
                 << " ndf = " << (*theVertexContainer)[i]->numberDoF());
      if (i > 0) {
        (*theVertexContainer)[i]->setVertexType(xAOD::VxType::PileUp);
      }
    }
  }

  return std::make_pair(theVertexContainer, theVertexAuxContainer);
}

Trk::Perigee* ActsIterativePriVtxFinderTool::actsBoundToTrkPerigee(
  const Acts::BoundTrackParameters& bound, const Acts::Vector3D& surfCenter) const {
  using namespace Acts::UnitLiterals;
  AmgSymMatrix(5)* cov =  new AmgSymMatrix(5)(bound.covariance()->block<5,5>(0,0));
  cov->col(Trk::qOverP) *= 1_MeV;
  cov->row(Trk::qOverP) *= 1_MeV;
  Acts::ActsVectorD<5> params = bound.parameters().head<5>();
  params[Trk::qOverP] *= 1_MeV;

  return new Trk::Perigee(params, Trk::PerigeeSurface(surfCenter), cov);
}
