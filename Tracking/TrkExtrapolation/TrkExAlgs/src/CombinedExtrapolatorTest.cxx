/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinedExtrapolatorTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Tracking
#include "TrkExAlgs/CombinedExtrapolatorTest.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "GaudiKernel/SystemOfUnits.h"

//================ Constructor =================================================
Trk::CombinedExtrapolatorTest::CombinedExtrapolatorTest(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_gaussDist(nullptr),
  m_flatDist(nullptr),
  m_sigmaD0(17.*Gaudi::Units::micrometer),                   
  m_minZ0(-25000.),
  m_maxZ0(+25000.),
  m_minEta(-3.),                    
  m_maxEta(3.),                    
  m_minP(0.5*Gaudi::Units::GeV),                      
  m_maxP(50000.*Gaudi::Units::GeV),
  m_outerBoundary(nullptr),
  m_trackingGeometry(nullptr),
  m_particleType(Trk::muon)
{
  // used algorithms and alg tools
  declareProperty("Extrapolator",                m_extrapolator);

  // algorithm steering
  declareProperty("StartPerigeeSigmaD0"       , m_sigmaD0);
  declareProperty("StartPerigeeMinZ0"         , m_minZ0);
  declareProperty("StartPerigeeMaxZ0"         , m_maxZ0);
  declareProperty("StartPerigeeMinEta"        , m_minEta);
  declareProperty("StartPerigeeMaxEta"        , m_maxEta);
  declareProperty("StartPerigeeMinP"          , m_minP);
  declareProperty("StartPerigeeMaxP"          , m_maxP);
  declareProperty("ParticleType"              , m_particleType);

}

//================ Destructor =================================================

Trk::CombinedExtrapolatorTest::~CombinedExtrapolatorTest()
{
  delete m_gaussDist;
  delete m_flatDist;
}


//================ Initialisation =================================================

StatusCode Trk::CombinedExtrapolatorTest::initialize()
{
  // Code entered here will be executed once at program start.
  msg(MSG::INFO) << " initialize()" << endmsg;

  // Get Extrapolator from ToolService   
  if (m_extrapolator.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve Tool " << m_extrapolator << ". Exiting."<<endmsg;
        return StatusCode::FAILURE;
  }
  
  m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
  m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));

  msg(MSG::INFO) << "initialize() successful in " << endmsg;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::CombinedExtrapolatorTest::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::CombinedExtrapolatorTest::execute()
{
  msg(MSG::INFO) << " execute()" << endmsg;
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // retrieve outer boundary
  if (!m_outerBoundary) {
    m_trackingGeometry = m_extrapolator->trackingGeometry();
    m_outerBoundary = m_trackingGeometry->highestTrackingVolume();
    if (!m_outerBoundary) {
      msg(MSG::FATAL) << "Could not retrieve geometry boundary  from " << m_extrapolator << ". Exiting."<<endmsg;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << " boundary retrieved " << endmsg;
  }
  
  // generate with random number generator
  double d0 = m_gaussDist->shoot() * m_sigmaD0;
  double z0 = m_minZ0 + m_flatDist->shoot() * (m_maxZ0-m_minZ0);
  double phi = 2.*M_PI * m_flatDist->shoot() - M_PI;
  //double eta = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
  double ctheta = -1. + 2* m_flatDist->shoot();
  double theta = acos(ctheta);
  double p = m_minP + m_flatDist->shoot()*(m_maxP-m_minP);
  double charge = (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
  double qOverP = charge/(p);

  // the initial perigee with random numbers
  const Trk::PerigeeSurface perSurface;
  Trk::Perigee initialPerigee(d0, z0, phi, theta, qOverP,perSurface); 
  // input covariance matrix
  const Trk::TrackParameters* seed = initialPerigee.clone();

  const Trk::PerigeeSurface pSf = initialPerigee.associatedSurface();
  
  const Trk::TrackParameters* destParameters = m_extrapolator->extrapolateToVolume(
    ctx, 
    *seed,
    *m_outerBoundary, 
    Trk::alongMomentum,
    (Trk::ParticleHypothesis)m_particleType);

  if (!destParameters || !m_extrapolator->trackingGeometry()->atVolumeBoundary(destParameters->position(),m_outerBoundary,0.001) ) {
    msg(MSG::ERROR) << " extrapolation to outer boundary failed for input parameters: " << initialPerigee.parameters() << endmsg;

  } else {
    // forward extrapolation ok
    msg(MSG::INFO) << " outer boundary reached at: " << destParameters->position().perp() <<","<<destParameters->position().z() << endmsg;
     msg(MSG::INFO) << "cov matrix:"<< destParameters->covariance() << endmsg;
 
    const Trk::TrackParameters* peri = m_extrapolator->extrapolate(
      ctx, 
      *destParameters,
      pSf, 
      Trk::oppositeMomentum,
      false,
      (Trk::ParticleHypothesis)m_particleType);
    if ( peri) {
      msg(MSG::INFO) << " extrapolation to perigee:input: " << initialPerigee.parameters() << endmsg;
      msg(MSG::INFO) << " extrapolation to perigee:output: " << peri->parameters() << endmsg;
       msg(MSG::INFO) << "cov matrix:"<< peri->covariance() << endmsg;
    } else {
      msg(MSG::ERROR) << " extrapolation to perigee failed for input parameters: " << destParameters->parameters() << endmsg;
    }
    delete peri;
  }
  
  delete destParameters;
  
  return StatusCode::SUCCESS;
}

//============================================================================================

