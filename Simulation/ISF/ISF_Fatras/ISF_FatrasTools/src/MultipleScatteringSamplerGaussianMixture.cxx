/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringSamplerGaussianMixture.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkGeometry/MaterialProperties.h"

#include "ISF_FatrasTools/MultipleScatteringSamplerGaussianMixture.h"

#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

// static particle masses
Trk::ParticleMasses iFatras::MultipleScatteringSamplerGaussianMixture::s_particleMasses;
// static doubles
double iFatras::MultipleScatteringSamplerGaussianMixture::s_main_RutherfordScott = 13.6*Gaudi::Units::MeV;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_log_RutherfordScott  =  0.038;

double iFatras::MultipleScatteringSamplerGaussianMixture::s_main_RossiGreisen = 17.5*Gaudi::Units::MeV;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_log_RossiGreisen  =  0.125;

// ============================= Gaussian mixture model =============
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixSigma1_a0  =  8.471e-1;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixSigma1_a1  =  3.347e-2;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixSigma1_a2  = -1.843e-3;

double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixEpsilon_a0 =  4.841e-2;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixEpsilon_a1 =  6.348e-3;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixEpsilon_a2 =  6.096e-4;

double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixEpsilon_b0 = -1.908e-2;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixEpsilon_b1 =  1.106e-1;
double iFatras::MultipleScatteringSamplerGaussianMixture::s_gausMixEpsilon_b2 = -5.729e-3;

double iFatras::MultipleScatteringSamplerGaussianMixture::s_projectionFactor  =  sqrt(2.);

// constructor
iFatras::MultipleScatteringSamplerGaussianMixture::MultipleScatteringSamplerGaussianMixture(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_log_include(true),
  m_optGaussianMixtureG4(true),
  m_rndGenSvc("AtRndmGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("TrkExRnd")
{
  // multiple scattering parameters
  declareProperty("MultipleScatteringLogarithmicTermOn", m_log_include);
  declareProperty("G4OptimisedGaussianMixtureModel",     m_optGaussianMixtureG4);
  // random service for Gaussian mixture model
  declareProperty("RandomNumberService",                 m_rndGenSvc,            "Name of the random number service");
  declareProperty("RandomStreamName",                    m_randomEngineName,     "Name of the random number stream");
}

// destructor
iFatras::MultipleScatteringSamplerGaussianMixture::~MultipleScatteringSamplerGaussianMixture()
{}

// Athena standard methods
// initialize
StatusCode iFatras::MultipleScatteringSamplerGaussianMixture::initialize()
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
  
  ATH_MSG_VERBOSE( "Gaussian mixture model optimised = " << m_optGaussianMixtureG4 );

  ATH_MSG_INFO( "initialize() successful" );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::MultipleScatteringSamplerGaussianMixture::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}


double iFatras::MultipleScatteringSamplerGaussianMixture::simTheta(const Trk::MaterialProperties& mat,
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
  
  // d_0'
  double dprime          = t/(beta*beta);
  double log_dprime      = log(dprime);
  // d_0'' 
  double log_dprimeprime = log(std::pow(mat.averageZ(),2.0/3.0) * dprime);
  // get epsilon
  double epsilon = log_dprimeprime < 0.5 ? 
    s_gausMixEpsilon_a0 + s_gausMixEpsilon_a1*log_dprimeprime + s_gausMixEpsilon_a2*log_dprimeprime*log_dprimeprime :
    s_gausMixEpsilon_b0 + s_gausMixEpsilon_b1*log_dprimeprime + s_gausMixEpsilon_b2*log_dprimeprime*log_dprimeprime;
  // the standard sigma
  double sigma1square = s_gausMixSigma1_a0 + s_gausMixSigma1_a1*log_dprime + s_gausMixSigma1_a2*log_dprime*log_dprime;
  // G4 optimised / native double Gaussian model
  if (!m_optGaussianMixtureG4) sigma2 = 225.*dprime/(p*p);
  // throw the random number core/tail
  if ( CLHEP::RandFlat::shoot(m_randomEngine) < epsilon) {
    sigma2 *= (1.-(1.-epsilon)*sigma1square)/epsilon;
  }
  
  return s_projectionFactor*sqrt(sigma2)*CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
  
}




