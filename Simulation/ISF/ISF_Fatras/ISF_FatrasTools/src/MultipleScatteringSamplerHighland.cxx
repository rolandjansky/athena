/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringSamplerHighland.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkGeometry/MaterialProperties.h"
#include "TrkExUtils/MaterialInteraction.h"

#include "ISF_FatrasTools/MultipleScatteringSamplerHighland.h"

#include "CLHEP/Random/RandGaussZiggurat.h"

// static particle masses
Trk::ParticleMasses iFatras::MultipleScatteringSamplerHighland::s_particleMasses;
// static doubles
double iFatras::MultipleScatteringSamplerHighland::s_main_RutherfordScott = 13.6*Gaudi::Units::MeV;
double iFatras::MultipleScatteringSamplerHighland::s_log_RutherfordScott  =  0.038;

double iFatras::MultipleScatteringSamplerHighland::s_main_RossiGreisen = 17.5*Gaudi::Units::MeV;
double iFatras::MultipleScatteringSamplerHighland::s_log_RossiGreisen  =  0.125;

double iFatras::MultipleScatteringSamplerHighland::s_projectionFactor  =  sqrt(2.);

Trk::MaterialInteraction       m_matInt;

// constructor
iFatras::MultipleScatteringSamplerHighland::MultipleScatteringSamplerHighland(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_log_include(true),
  m_rndGenSvc("AtRndmGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("TrkExRnd")
{
  // multiple scattering parameters
  declareProperty("MultipleScatteringLogarithmicTermOn", m_log_include);
  // random service
  declareProperty("RandomNumberService",                 m_rndGenSvc,            "Name of the random number service");
  declareProperty("RandomStreamName",                    m_randomEngineName,     "Name of the random number stream");
}

// destructor
iFatras::MultipleScatteringSamplerHighland::~MultipleScatteringSamplerHighland()
{}

// Athena standard methods
// initialize
StatusCode iFatras::MultipleScatteringSamplerHighland::initialize()
{

  // get the random generator service
  if (m_rndGenSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve " << m_rndGenSvc );
  } else {
    ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );
  }
  
  //Get own engine with own seeds:
  m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE ("Successfully got random engine '" << m_randomEngineName << "'");
  
  ATH_MSG_INFO( "initialize() successful" );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::MultipleScatteringSamplerHighland::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}


double iFatras::MultipleScatteringSamplerHighland::simTheta(const Trk::MaterialProperties& mat,
							    double p,
							    double pathcorrection,
							    Trk::ParticleHypothesis particle,
							    double ) const
{
  if (mat.thicknessInX0()<=0. || particle==Trk::geantino) return 0.;
  
  // make sure the path correction is positive to avoid a floating point exception
  pathcorrection *= pathcorrection < 0. ? (-1.) : (1) ;
  
  // scale the path length to the radiation length
  double t = pathcorrection * mat.thicknessInX0();

  // kinematics (relativistic)
  double m    = s_particleMasses.mass[particle];
  double E    = sqrt(p*p + m*m);
  double beta = p/E;
  
  double sigma2(0.);
  
  double sigma = m_matInt.sigmaMS(t, p, beta);
  sigma2 = sigma*sigma;
  
  // Code below will not be used if the parameterization of TrkUtils is used 
  if (particle != Trk::electron) {

    // the highland formula
    sigma2 = s_main_RutherfordScott/(beta*p);

    if (m_log_include)
      sigma2 *= (1.+s_log_RutherfordScott*log(t));
    
    sigma2 *= (sigma2*t);
  }
  
  else {
    
    // Electron multiple scattering effects - see Highland NIM 129 (1975) p497-499
    // (Highland extension to the Rossi-Greisen formulation)
    // NOTE: The formula can be extended by replacing the momentum^2 term with pi * pf
    sigma2 = s_main_RossiGreisen / ( beta * p );
    sigma2 *= (sigma2*t);
    
    if ( m_log_include ) {
      double factor = 1. + s_log_RossiGreisen * log10( 10. * t );
      factor *= factor;
      sigma2 *= factor;
    }
  }
  
  return s_projectionFactor*sqrt(sigma2)*CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
  
}




