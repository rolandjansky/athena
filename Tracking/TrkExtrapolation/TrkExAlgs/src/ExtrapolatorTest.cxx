/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolatorTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Tracking
#include "TrkExAlgs/ExtrapolatorTest.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "EventPrimitives/EventPrimitives.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "GaudiKernel/SystemOfUnits.h"
// std
#include <cmath>

//================ Constructor =================================================

Trk::ExtrapolatorTest::ExtrapolatorTest(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_propagator("Trk::RungeKuttaPropagator/RungeKuttaPropagator"),
  m_magFieldSvc("AtlasFieldSvc",name),
  m_magFieldProperties(0),
  m_gaussDist(0),
  m_flatDist(0),
  m_sigmaD0(17.*Gaudi::Units::micrometer),                   
  m_sigmaZ0(50.*Gaudi::Units::millimeter),
  m_minPhi(-M_PI),                    
  m_maxPhi(M_PI),                    
  m_minEta(-3.),                    
  m_maxEta(3.),                    
  m_minP(0.5*Gaudi::Units::GeV),                      
  m_maxP(50000.*Gaudi::Units::GeV),
  m_direction(1),
  m_particleType(2),
  m_referenceSurfaces(0),
  m_eventsPerExecute(-1),
  m_useExtrapolator(false)
{
  // used algorithms and alg tools
  declareProperty("Extrapolator",                m_extrapolator);
  declareProperty("Propagator",                  m_propagator);
  declareProperty("MagFieldSvc",   m_magFieldSvc );

  // algorithm steering
  declareProperty("StartPerigeeSigmaD0"       , m_sigmaD0);
  declareProperty("StartPerigeeSigmaZ0"       , m_sigmaZ0);
  declareProperty("StartPerigeeMinPhi"        , m_minPhi);
  declareProperty("StartPerigeeMaxPhi"        , m_maxPhi);
  declareProperty("StartPerigeeMinEta"        , m_minEta);
  declareProperty("StartPerigeeMaxEta"        , m_maxEta);
  declareProperty("StartPerigeeMinP"          , m_minP);
  declareProperty("StartPerigeeMaxP"          , m_maxP);
  declareProperty("StartDirection"            , m_direction);
  declareProperty("ParticleType"              , m_particleType);

  //  template for property decalration
  declareProperty("ReferenceSurfaceRadius",    m_referenceSurfaceRadius);
  declareProperty("ReferenceSurfaceHalfZ",     m_referenceSurfaceHalflength);
  declareProperty("EventsPerExecute",          m_eventsPerExecute );
  declareProperty("UseExtrapolator",           m_useExtrapolator );
}

//================ Destructor =================================================

Trk::ExtrapolatorTest::~ExtrapolatorTest()
{
  delete m_gaussDist;
  delete m_flatDist;
  // cleanup of the surfaces
   std::vector< std::vector< const Trk::Surface* > >::const_iterator surfaceTripleIter    = m_referenceSurfaceTriples.begin();
   std::vector< std::vector< const Trk::Surface* > >::const_iterator surfaceTripleIterEnd = m_referenceSurfaceTriples.end();
   for ( ; surfaceTripleIter != surfaceTripleIterEnd; ++surfaceTripleIter)
   {
     std::vector<const Trk::Surface*>::const_iterator surfIter = (*surfaceTripleIter).begin();
     std::vector<const Trk::Surface*>::const_iterator surfIterEnd = (*surfaceTripleIter).end();
     for ( ; surfIter != surfIterEnd; delete (*surfIter), ++surfIter);

   
   }

}


//================ Initialisation =================================================

StatusCode Trk::ExtrapolatorTest::initialize()
{
  // Code entered here will be executed once at program start.
  
  msg(MSG::INFO) << " initialize()" << endreq;

  // Get Extrapolator from ToolService   
  if (m_extrapolator.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve Tool " << m_extrapolator << ". Exiting."<<endreq;
        return StatusCode::FAILURE;
  }
  // Get Propagator from ToolService    
  if (m_propagator.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve Tool " << m_propagator << ". Exiting."<<endreq;
        return StatusCode::FAILURE;
  }

  if (m_magFieldSvc.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve Tool " << m_magFieldSvc << ". Exiting."<<endreq;
        return StatusCode::FAILURE;
  }
  m_magFieldProperties = new Trk::MagneticFieldProperties();

  if (m_referenceSurfaceRadius.size() == m_referenceSurfaceHalflength.size()) {
     // assign the size
     m_referenceSurfaces = m_referenceSurfaceRadius.size();
     // loop over it and create the 
     std::vector<double>::iterator radiusIter     = m_referenceSurfaceRadius.begin();
     std::vector<double>::iterator radiusIterEnd  = m_referenceSurfaceRadius.end();
     std::vector<double>::iterator halfZIter      = m_referenceSurfaceHalflength.begin();

     for ( ; radiusIter != radiusIterEnd; ++radiusIter, ++halfZIter){
          // create the Surface triplet
          std::vector< const Trk::Surface*> surfaceTriplet;
             surfaceTriplet.push_back(new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,*halfZIter)),0.,*radiusIter));
             surfaceTriplet.push_back(new Trk::CylinderSurface(new Amg::Transform3D,*radiusIter, *halfZIter));
             surfaceTriplet.push_back(new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,-(*halfZIter))),0.,*radiusIter));
	     
	     ATH_MSG_INFO("Creating surfaces: R " << *radiusIter << " Z " << *halfZIter);
	     m_referenceSurfaceTriples.push_back(surfaceTriplet);

	     m_referenceSurfaceNegativeBoundary.push_back(atan2(*radiusIter,-(*halfZIter)));
	     m_referenceSurfacePositiveBoundary.push_back(atan2(*radiusIter,(*halfZIter)));

     }
  }

  m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
  m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));

  msg(MSG::INFO) << "initialize() successful in " << endreq;

  if( m_eventsPerExecute > 0 ){
    m_perigees.reserve(m_eventsPerExecute);
    for( int i=0;i<m_eventsPerExecute;++i ) m_perigees.push_back(generatePerigee());
  }

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::ExtrapolatorTest::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

Trk::Perigee Trk::ExtrapolatorTest::generatePerigee() {
   // generate with random number generator
   double d0 = m_gaussDist->shoot() * m_sigmaD0;
   double z0 = m_gaussDist->shoot() * m_sigmaZ0;
   double phi = m_minPhi + (m_maxPhi-m_minPhi)* m_flatDist->shoot();
   double eta = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
   double theta = 2.*atan(exp(-eta));
   double p = m_minP + m_flatDist->shoot()*(m_maxP-m_minP);
   double charge = (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
   double qOverP = charge/(p);
   const Trk::PerigeeSurface perSurface;
   return Trk::Perigee(d0, z0, phi, theta, qOverP,perSurface); 
}

//================ Execution ====================================================

StatusCode Trk::ExtrapolatorTest::execute()
{
  
  if( m_eventsPerExecute <= 0 ) runTest(generatePerigee());
  else                          for( int i=0;i<m_eventsPerExecute;++i ) runTest(m_perigees[i]);
  return StatusCode::SUCCESS;
}


void Trk::ExtrapolatorTest::runTest( const Trk::Perigee& initialPerigee ) {
   Trk::PropDirection propagationDirection = m_direction > 0 ? Trk::alongMomentum : oppositeMomentum;
  
   ATH_MSG_VERBOSE("Starting from : "       << initialPerigee );
   ATH_MSG_VERBOSE(" ---> with direction: " << propagationDirection );



   std::vector< std::vector< const Trk::Surface* > >::const_iterator surfaceTripleIter    = m_referenceSurfaceTriples.begin();
   std::vector< std::vector< const Trk::Surface* > >::const_iterator surfaceTripleIterEnd = m_referenceSurfaceTriples.end();

   std::vector<double>::iterator negRefIter = m_referenceSurfaceNegativeBoundary.begin();
   std::vector<double>::iterator posRefIter = m_referenceSurfacePositiveBoundary.begin();

   double theta = initialPerigee.parameters()[Trk::theta];


   for (int refSurface = 0 ; surfaceTripleIter != surfaceTripleIterEnd; ++surfaceTripleIter, ++negRefIter, ++posRefIter ){
       // decide which reference surface to take
       refSurface = theta < (*posRefIter) ? 2 : 1;
       refSurface = theta > (*negRefIter) ? 0 : 1;

       const Trk::Surface* destinationSurface = (*surfaceTripleIter)[refSurface];
       
       const Trk::TrackParameters* destParameters = m_useExtrapolator ?
	 m_extrapolator->extrapolate(initialPerigee,
				     *destinationSurface, 
				     propagationDirection,
				     false,
				     (Trk::ParticleHypothesis)m_particleType) :
	 m_propagator->propagate(initialPerigee,
				 *destinationSurface, 
				 propagationDirection,
				 false,
				 *m_magFieldProperties,
				 (Trk::ParticleHypothesis)m_particleType);

       if (destParameters && msgLevel() <= MSG::VERBOSE) {
           // global position parameter
           const Amg::Vector3D& gp = destParameters->position();

           // intersection output
           ATH_MSG_VERBOSE(" [ intersection ] with surface at (x,y,z) = " << gp.x() << ", " << gp.y() << ", " << gp.z() );           

       } else if (!destParameters)
           ATH_MSG_DEBUG(" Extrapolation not successful! " );

       delete destParameters;

   } 
}

//============================================================================================

