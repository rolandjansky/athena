/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringUpdator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkExTools/MultipleScatteringUpdator.h"
#include "TrkGeometry/MaterialProperties.h"
// CLHEP

#include "TrkExUtils/MaterialInteraction.h"
#include "CLHEP/Random/RandFlat.h"

namespace{
// static particle masses
const Trk::ParticleMasses s_particleMasses{};
// static doubles
constexpr double s_main_RutherfordScott = 13.6 * Gaudi::Units::MeV;
constexpr double s_log_RutherfordScott = 0.038;

constexpr double s_main_RossiGreisen = 17.5 * Gaudi::Units::MeV;
constexpr double s_log_RossiGreisen = 0.125;

// ============================= Gaussian mixture model =============
constexpr double s_gausMixSigma1_a0 = 8.471e-1;
constexpr double s_gausMixSigma1_a1 = 3.347e-2;
constexpr double s_gausMixSigma1_a2 = -1.843e-3;

constexpr double s_gausMixEpsilon_a0 = 4.841e-2;
constexpr double s_gausMixEpsilon_a1 = 6.348e-3;
constexpr double s_gausMixEpsilon_a2 = 6.096e-4;

constexpr double s_gausMixEpsilon_b0 = -1.908e-2;
constexpr double s_gausMixEpsilon_b1 = 1.106e-1;
constexpr double s_gausMixEpsilon_b2 = -5.729e-3;
}


// constructor
Trk::MultipleScatteringUpdator::MultipleScatteringUpdator(const std::string &t, const std::string &n,
                                                          const IInterface *p) :
  AthAlgTool(t, n, p),
  m_useTrkUtils(true),
  m_log_include(true),
  m_gaussianMixture(false),
  m_optGaussianMixtureG4(true),
  m_rndGenSvc("AtRndmGenSvc", n),
  m_randomEngine(nullptr),
  m_randomEngineName("TrkExRnd") {
  declareInterface<IMultipleScatteringUpdator>(this);
  // multiple scattering parameters
  declareProperty("UseTrkUtils", m_useTrkUtils);
  declareProperty("MultipleScatteringLogarithmicTermOn", m_log_include);
  declareProperty("GaussianMixtureModel", m_gaussianMixture);
  declareProperty("G4OptimisedGaussianMixtureModel", m_optGaussianMixtureG4);
  // random service for Gaussian mixture model
  declareProperty("RandomNumberService", m_rndGenSvc, "Name of the random number service");
  declareProperty("RandomStreamName", m_randomEngineName, "Name of the random number stream");
}

// destructor
Trk::MultipleScatteringUpdator::~MultipleScatteringUpdator() {
}

// Athena standard methods
// initialize
StatusCode
Trk::MultipleScatteringUpdator::initialize() {
  // if Gaussian mixture is required
  if (m_gaussianMixture) {
    // get the random generator service
    if (m_rndGenSvc.retrieve().isFailure()) {
      ATH_MSG_WARNING("Could not retrieve " << m_rndGenSvc << " -> switching Gaussian mixture model off.");
      m_gaussianMixture = false;
    } else {
      ATH_MSG_VERBOSE("Successfully retrieved " << m_rndGenSvc);
    }

    // Get own engine with own seeds:
    m_randomEngine = m_gaussianMixture ? m_rndGenSvc->GetEngine(m_randomEngineName) : nullptr;
    if (!m_randomEngine) {
      ATH_MSG_WARNING(
        "Could not get random engine '" << m_randomEngineName << "' -> switching Gaussian mixture model off.");
      m_gaussianMixture = false;
    }
  }
  if (m_gaussianMixture) {
    ATH_MSG_VERBOSE("Gaussian mixture model = ON ... optimised = " << m_optGaussianMixtureG4);
  } else {
    ATH_MSG_VERBOSE("Gaussian mixture model = OFF");
  }
  ATH_MSG_INFO("initialize() successful");

  return StatusCode::SUCCESS;
}

double
Trk::MultipleScatteringUpdator::sigmaSquare(const MaterialProperties &mat,
                                            double p,
                                            double pathcorrection,
                                            ParticleHypothesis particle,
                                            double) const {
  if (mat.thicknessInX0() <= 0. || particle == Trk::geantino) {
    return 0.;
  }

  // make sure the path correction is positive to avoid a floating point exception
  pathcorrection *= pathcorrection < 0. ? (-1.) : (1);

  // scale the path length to the radiation length
  double t = pathcorrection * mat.thicknessInX0();

  // kinematics (relativistic)
  double m = s_particleMasses.mass[particle];
  double E = sqrt(p * p + m * m);
  double beta = p / E;

  double sigma2(0.);

  Trk::MaterialInteraction matInt;
  double sigma = matInt.sigmaMS(t, p, beta);
  sigma2 = sigma * sigma;

  if (m_useTrkUtils && particle != Trk::electron) {
    return sigma2;
  }

// Code below will not be used if the parameterization of TrkUtils is used


  if (particle != Trk::electron) {
    // the highland formula
    sigma2 = s_main_RutherfordScott / (beta * p);

    if (m_log_include) {
      sigma2 *= (1. + s_log_RutherfordScott * log(t));
    }

    sigma2 *= (sigma2 * t);
  }else {
    // Electron multiple scattering effects - see Highland NIM 129 (1975) p497-499
    // (Highland extension to the Rossi-Greisen formulation)
    // NOTE: The formula can be extended by replacing the momentum^2 term with pi * pf
    sigma2 = s_main_RossiGreisen / (beta * p);
    sigma2 *= (sigma2 * t);

    if (m_log_include) {
      double factor = 1. + s_log_RossiGreisen * log10(10. * t);
      factor *= factor;
      sigma2 *= factor;
    }
  }

  // use the Gaussian mixture model
  if (m_gaussianMixture) {
    // d_0'
    double dprime = t / (beta * beta);
    double log_dprime = log(dprime);
    // d_0''
    double log_dprimeprime = log(std::pow(mat.averageZ(), 2.0 / 3.0) * dprime);
    // get epsilon
    double epsilon = log_dprimeprime < 0.5 ?
                     s_gausMixEpsilon_a0 + s_gausMixEpsilon_a1 * log_dprimeprime + s_gausMixEpsilon_a2 *
                     log_dprimeprime * log_dprimeprime :
                     s_gausMixEpsilon_b0 + s_gausMixEpsilon_b1 * log_dprimeprime + s_gausMixEpsilon_b2 *
                     log_dprimeprime * log_dprimeprime;
    // the standard sigma
    double sigma1square = s_gausMixSigma1_a0 + s_gausMixSigma1_a1 * log_dprime + s_gausMixSigma1_a2 * log_dprime *
                          log_dprime;
    // G4 optimised / native double Gaussian model
    if (!m_optGaussianMixtureG4) {
      sigma2 = 225. * dprime / (p * p);
    }
    // throw the random number core/tail
    if (CLHEP::RandFlat::shoot(m_randomEngine) < epsilon) {
      sigma2 *= (1. - (1. - epsilon) * sigma1square) / epsilon;
    }
  }

  return sigma2;
}
