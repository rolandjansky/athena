/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolatorComparisonTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkExAlgs/ExtrapolatorComparisonTest.h"

// Tracking
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParametersBase/ParametersT.h"
#include "EventPrimitives/EventPrimitives.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/RNGWrapper.h"

// ACTS
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsInterop/Logger.h"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/ParameterDefinitions.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"

// OTHER
#include "CLHEP/Random/RandomEngine.h"

// STL
#include <string>
#include <fstream>
#include <chrono>
#include <cmath>

using xclock = std::chrono::steady_clock;

//================ Constructor =================================================

Trk::ExtrapolatorComparisonTest::ExtrapolatorComparisonTest(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthReentrantAlgorithm(name,pSvcLocator),
  m_gaussDist(0),
  m_flatDist(0),
  m_sigmaD0(17.*Gaudi::Units::micrometer),                   
  m_sigmaZ0(50.*Gaudi::Units::millimeter),
  m_minPhi(-M_PI),                    
  m_maxPhi(M_PI),                    
  m_minEta(-3.),                    
  m_maxEta(3.),                    
  m_minPt(0.5*Gaudi::Units::GeV),                      
  m_maxPt(50000.*Gaudi::Units::GeV),
  m_particleType(2),
  m_referenceSurfaces(0),
  m_eventsPerExecute(1),
  m_atlasPropResultWriterSvc("ATLASPropResultRootWriterSvc", name),
  m_actsPropResultWriterSvc("ACTSPropResultRootWriterSvc", name)
{
  // algorithm steering
  declareProperty("StartPerigeeSigmaD0"       , m_sigmaD0                   );
  declareProperty("StartPerigeeSigmaZ0"       , m_sigmaZ0                   );
  declareProperty("StartPerigeeMinPhi"        , m_minPhi                    );
  declareProperty("StartPerigeeMaxPhi"        , m_maxPhi                    );
  declareProperty("StartPerigeeMinEta"        , m_minEta                    );
  declareProperty("StartPerigeeMaxEta"        , m_maxEta                    );
  declareProperty("StartPerigeeMinPt"         , m_minPt                     );
  declareProperty("StartPerigeeMaxPt"         , m_maxPt                     );
  declareProperty("ParticleType"              , m_particleType              );
  declareProperty("ReferenceSurfaceRadius"    , m_referenceSurfaceRadius    );
  declareProperty("ReferenceSurfaceHalfZ"     , m_referenceSurfaceHalflength);
  declareProperty("EventsPerExecute"          , m_eventsPerExecute          );
  declareProperty("ATLASPropResultRootWriter" , m_atlasPropResultWriterSvc  );
  declareProperty("ACTSPropResultRootWriter"  , m_actsPropResultWriterSvc   );
}

//================ Destructor =================================================

Trk::ExtrapolatorComparisonTest::~ExtrapolatorComparisonTest()
{
  delete m_gaussDist;
  delete m_flatDist;
  
  // cleanup of the Trk::Surfaces
  std::vector< std::vector< const Trk::Surface* > >::const_iterator atlasSurfaceTripleIter    = m_atlasReferenceSurfaceTriples.begin();
  std::vector< std::vector< const Trk::Surface* > >::const_iterator atlasSurfaceTripleIterEnd = m_atlasReferenceSurfaceTriples.end();
  for ( ; atlasSurfaceTripleIter != atlasSurfaceTripleIterEnd; ++atlasSurfaceTripleIter) {
    std::vector<const Trk::Surface*>::const_iterator surfIter = (*atlasSurfaceTripleIter).begin();
    std::vector<const Trk::Surface*>::const_iterator surfIterEnd = (*atlasSurfaceTripleIter).end();
    for ( ; surfIter != surfIterEnd; delete (*surfIter), ++surfIter);
  }    
}


//================ Initialisation =================================================

StatusCode Trk::ExtrapolatorComparisonTest::initialize()
{
  // Code entered here will be executed once at program start.
  
  ATH_MSG_INFO(" initialize()");
  
  ATH_CHECK( m_extrapolationTool.retrieve() );
  ATH_CHECK( m_atlasExtrapolator.retrieve() );

  // Create the destination surfaces for extrapolation
  // --> you need the Trk::Surfaces and the Acts::Surfaces
  if (m_referenceSurfaceRadius.size() == m_referenceSurfaceHalflength.size()) {
     // assign the size
     m_referenceSurfaces = m_referenceSurfaceRadius.size();
     // loop over it and create the 
     for (unsigned int surface = 0; surface < m_referenceSurfaces; surface++) {
       
       double radius = m_referenceSurfaceRadius.at(surface);
       double halfZ  = m_referenceSurfaceHalflength.at(surface);
       
       // create the Surface triplet
       std::vector< const Trk::Surface*> trkSurfaceTriplet;
       trkSurfaceTriplet.push_back(new Trk::DiscSurface    (new Amg::Transform3D(Amg::Translation3D(0.,0., halfZ)),    0.,radius));
       trkSurfaceTriplet.push_back(new Trk::CylinderSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,    0.)),radius, halfZ));
       trkSurfaceTriplet.push_back(new Trk::DiscSurface    (new Amg::Transform3D(Amg::Translation3D(0.,0.,-halfZ)),    0.,radius));
       ATH_MSG_INFO("Creating Trk::Surface at: R " << radius << " Z " << halfZ);
       m_atlasReferenceSurfaceTriples.push_back(trkSurfaceTriplet);
       
       // create the Surface triplet
       std::vector<std::shared_ptr<const Acts::Surface>> actsSurfaceTriplet;
       
       Acts::Transform3D posTransf(Acts::Transform3D::Identity()*Acts::Translation3D(Acts::Vector3D(0.,0., halfZ)));
       Acts::Transform3D   cTransf(Acts::Transform3D::Identity()*Acts::Translation3D(Acts::Vector3D(0.,0.,    0.)));
       Acts::Transform3D negTransf(Acts::Transform3D::Identity()*Acts::Translation3D(Acts::Vector3D(0.,0.,-halfZ)));
       
       auto posSurface = Acts::Surface::makeShared<Acts::DiscSurface>    (posTransf ,    0.,radius);
       auto cSurface   = Acts::Surface::makeShared<Acts::CylinderSurface>(  cTransf ,radius, halfZ);
       auto negSurface = Acts::Surface::makeShared<Acts::DiscSurface>    (negTransf ,    0.,radius);
       
       actsSurfaceTriplet.push_back(posSurface);
       actsSurfaceTriplet.push_back(cSurface  );
       actsSurfaceTriplet.push_back(negSurface);
       ATH_MSG_INFO("Creating Acts::Surface at: R " << radius << " Z " << halfZ);
       m_actsReferenceSurfaceTriples.push_back(actsSurfaceTriplet);
       
       m_referenceSurfaceNegativeBoundary.push_back(atan2(radius,-halfZ));
       m_referenceSurfacePositiveBoundary.push_back(atan2(radius, halfZ));
     }
  } else { 
    ATH_MSG_WARNING("Not compatible size of ReferenceSurfaceRadius and ReferenceSurfaceHalfZ!! Returning FAILURE!");
    return StatusCode::FAILURE;
  }

  m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
  m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));
  
  ATH_CHECK( m_atlasPropResultWriterSvc.retrieve() );
  ATH_CHECK( m_actsPropResultWriterSvc.retrieve() );
  
  ATH_MSG_INFO("initialize() successful!!");

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::ExtrapolatorComparisonTest::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::ExtrapolatorComparisonTest::execute(const EventContext& ctx) const {
  
  float milliseconds_to_seconds = 1000.;
  
  // generate perigees with random number generator 
  
  std::vector<perigeeParameters> parameters = {};
  for (int ext = 0; ext<m_eventsPerExecute; ext++) {
    // generate with random number generator
    double d0 = m_gaussDist->shoot() * m_sigmaD0;
    double z0 = m_gaussDist->shoot() * m_sigmaZ0;
    double phi = m_minPhi + (m_maxPhi-m_minPhi)* m_flatDist->shoot();
    double eta = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
    double pt = m_minPt + m_flatDist->shoot()*(m_maxPt-m_minPt);
    double charge = (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
    parameters.push_back(perigeeParameters(d0, z0, phi, eta, pt, charge));
  }
  
  int n_extraps = 0;  
  auto start = xclock::now();
  for (auto& perigee : parameters) {
    
    Acts::Vector3D momentum(perigee.m_pt * std::cos(perigee.m_phi), perigee.m_pt * std::sin(perigee.m_phi), perigee.m_pt * std::sinh(perigee.m_eta));
    double theta = Acts::VectorHelpers::theta(momentum);
    double qOverP = perigee.m_charge / momentum.norm();
    
    const Trk::PerigeeSurface atlPerigeeSurface;
    const Trk::Perigee * atlPerigee = new const Trk::Perigee(perigee.m_d0, perigee.m_z0, perigee.m_phi, theta, qOverP, atlPerigeeSurface);
    
    for (unsigned int surface = 0; surface < m_atlasReferenceSurfaceTriples.size(); surface++) {
      n_extraps++;

      double negRef = m_referenceSurfaceNegativeBoundary.at(surface);
      double posRef = m_referenceSurfacePositiveBoundary.at(surface);
    
      // decide which reference surface to take
      int refSurface = theta < posRef ? 2 : 1;
      refSurface     = theta > negRef ? 0 : 1;
      
      const Trk::Surface* destinationSurface = m_atlasReferenceSurfaceTriples.at(surface).at(refSurface);
      
      ATH_MSG_VERBOSE("Starting extrapolation " << n_extraps << " from : "       << *atlPerigee << " to : " << *destinationSurface);
      
      auto start_fwd = xclock::now();
      const Trk::TrackParameters* destParameters = m_atlasExtrapolator->extrapolate(*atlPerigee,
                                                                                    *destinationSurface, 
                                                                                    Trk::alongMomentum,
                                                                                    true,
                                                                                    (Trk::ParticleHypothesis)m_particleType);
      auto end_fwd = xclock::now();
      float ms_fwd = std::chrono::duration_cast<std::chrono::milliseconds>(end_fwd-start_fwd).count();
      
      if (destParameters) {
        ATH_MSG_VERBOSE(" ATLAS Extrapolator succeded!! --> Forward" );           
        ATH_MSG_VERBOSE(" [ intersection ] with surface at (x,y,z) = " << destParameters->position().x() << ", " << destParameters->position().y() << ", " << destParameters->position().z() );   
        ATH_MSG_VERBOSE(" [ intersection ] parameters: " << destParameters->parameters() );   
        ATH_MSG_VERBOSE(" [ intersection ] cov matrix: " << destParameters->covariance() );
        
        // now try backward extrapolation
        auto start_bkw = xclock::now();
        const Trk::TrackParameters* finalperigee = m_atlasExtrapolator->extrapolate(*destParameters,
                                                                                    atlPerigee->associatedSurface(), 
                                                                                    Trk::oppositeMomentum,
                                                                                    true,
                                                                                    (Trk::ParticleHypothesis)m_particleType);
        auto end_bkw = xclock::now();
        float ms_bkw = std::chrono::duration_cast<std::chrono::milliseconds>(end_bkw-start_bkw).count();
        
         if (finalperigee) {
           ATH_MSG_VERBOSE(" ATLAS Extrapolator succeded!! --> Backward" );           
           ATH_MSG_VERBOSE(" [extrapolation to perigee]      input: " << atlPerigee->parameters() );   
           ATH_MSG_VERBOSE(" [extrapolation to perigee]     output: " << finalperigee->parameters() );   
           ATH_MSG_VERBOSE(" [extrapolation to perigee] cov matrix: " << finalperigee->covariance() );
           
         } else if (!finalperigee) {
           ATH_MSG_DEBUG(" ATLAS Extrapolation to perigee failed for input parameters: " << destParameters->parameters());
         }
         
         m_atlasPropResultWriterSvc->write<Trk::TrackParameters>(atlPerigee, destParameters, ms_fwd, finalperigee, ms_bkw);
         delete finalperigee;
      } else if (!destParameters) {
        ATH_MSG_DEBUG(" ATLAS Extrapolation not successful! " );
        m_atlasPropResultWriterSvc->write<Trk::TrackParameters>(atlPerigee);
      }
      delete destParameters;
    }    
    delete atlPerigee;
  }
  auto end = xclock::now();   
  auto secs = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() / milliseconds_to_seconds;
  double secs_per_ex = secs / n_extraps;
  
  ATH_MSG_INFO("ATLAS : Time for " << n_extraps << " iterations: " << secs << "s (" << secs_per_ex << "s per extrapolation)");
  
  n_extraps = 0;  
  start = xclock::now();
  for (auto& perigee : parameters) {
    Acts::Vector3D momentum(perigee.m_pt * std::cos(perigee.m_phi), perigee.m_pt * std::sin(perigee.m_phi), perigee.m_pt * std::sinh(perigee.m_eta));
    double theta = Acts::VectorHelpers::theta(momentum);
    double qOverP = perigee.m_charge / momentum.norm();
    
    std::shared_ptr<Acts::PerigeeSurface> actsPerigeeSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(Acts::Vector3D(0, 0, 0));
    double t = 0.;
    Acts::BoundVector pars;
    pars << perigee.m_d0, perigee.m_z0, perigee.m_phi, theta, qOverP, t;
    std::optional<Acts::BoundSymMatrix> cov = std::nullopt;
    
    // Perigee, no alignment -> default geo context
    ActsGeometryContext gctx = m_extrapolationTool->trackingGeometryTool()->getNominalGeometryContext();
    auto anygctx = gctx.any();
    const Acts::BoundTrackParameters* startParameters = new const Acts::BoundTrackParameters(std::move(actsPerigeeSurface), std::move(pars), std::move(cov));
    
    for (unsigned int surface = 0; surface < m_actsReferenceSurfaceTriples.size(); surface++) {
      n_extraps++;
      
      double negRef = m_referenceSurfaceNegativeBoundary.at(surface);
      double posRef = m_referenceSurfacePositiveBoundary.at(surface);
    
      // decide which reference surface to take
      int refSurface = theta < posRef ? 2 : 1;
      refSurface     = theta > negRef ? 0 : 1;
      
      auto destinationSurface = m_actsReferenceSurfaceTriples.at(surface).at(refSurface);
      
     ATH_MSG_VERBOSE("Starting extrapolation " << n_extraps << " from : "       << pars << " to : " << destinationSurface);
      
      auto start_fwd = xclock::now();
      auto destParameters = m_extrapolationTool->propagate(ctx, *startParameters, *destinationSurface, Acts::forward);
      auto end_fwd = xclock::now();
      float ms_fwd = std::chrono::duration_cast<std::chrono::milliseconds>(end_fwd-start_fwd).count();
      
      if (destParameters) {
        ATH_MSG_VERBOSE(" ACTS Extrapolator succeded!! --> Forward" );           
        ATH_MSG_VERBOSE(" [ intersection ] with surface at (x,y,z) = " << destParameters->position(anygctx).x() << ", " << destParameters->position(anygctx).y() << ", " << destParameters->position(anygctx).z() );   
        ATH_MSG_VERBOSE(" [ intersection ] parameters: " << destParameters->parameters() );   
        ATH_MSG_VERBOSE(" [ intersection ] cov matrix: " << *destParameters->covariance() );
        
        // now try backward extrapolation
        auto start_bkw = xclock::now();
        auto finalperigee = m_extrapolationTool->propagate(ctx, *destParameters, startParameters->referenceSurface(), Acts::backward);
        auto end_bkw = xclock::now();
        float ms_bkw = std::chrono::duration_cast<std::chrono::milliseconds>(end_bkw-start_bkw).count();
        
        if (finalperigee) {
           ATH_MSG_VERBOSE(" ACTS Extrapolator succeded!! --> Backward" );           
           ATH_MSG_VERBOSE(" [extrapolation to perigee]      input: " << startParameters->parameters() );   
           ATH_MSG_VERBOSE(" [extrapolation to perigee]     output: " << finalperigee->parameters() );   
           ATH_MSG_VERBOSE(" [extrapolation to perigee] cov matrix: " << *finalperigee->covariance() );
           
         } else if (!finalperigee) {
           ATH_MSG_DEBUG(" ACTS Extrapolation to perigee failed for input parameters: " << destParameters->parameters());
         }

         // Construct wrappers for Acts track parameters
         const ActsTrackWrapper* startWrapper = new ActsTrackWrapper(startParameters, anygctx);
         const ActsTrackWrapper* destWrapper = new ActsTrackWrapper(destParameters.release(), anygctx);
         const ActsTrackWrapper* finalWrapper = new ActsTrackWrapper(finalperigee.release(), anygctx);

         m_actsPropResultWriterSvc->write<ActsTrackWrapper>(startWrapper, destWrapper, ms_fwd, finalWrapper, ms_bkw);

         delete startWrapper;
         delete destWrapper;
         delete finalWrapper;
      } else if (!destParameters) {
        ATH_MSG_DEBUG(" ACTS Extrapolation not successful! " );
        const ActsTrackWrapper* startWrapper = new ActsTrackWrapper(startParameters, anygctx);
        m_actsPropResultWriterSvc->write<ActsTrackWrapper>(startWrapper);
        delete startWrapper;
      }
    }
    delete startParameters;
  }
  
  end = xclock::now();   
  secs = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() / milliseconds_to_seconds;
  secs_per_ex = secs / n_extraps;
  
  ATH_MSG_INFO("ACTS : Time for " << n_extraps << " iterations: " << secs << "s (" << secs_per_ex << "s per extrapolation)");
  
  return StatusCode::SUCCESS;
}

//============================================================================================


 
