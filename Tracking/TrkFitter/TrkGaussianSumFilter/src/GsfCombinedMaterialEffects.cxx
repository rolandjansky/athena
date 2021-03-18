/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file GsfCombinedMaterialEffects.cxx
 * @begin         Friday 11th January 2005
 * @author        Anthony Morley, Tom  Atkinson, Christos Anastopoulos
 * @brief         Implementation code for GsfCombinedMaterialEffects class
 */

#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"
#include "PathResolver/PathResolver.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkExUtils/MaterialInteraction.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include <cmath>
#include <fstream>

namespace {
const Trk::ParticleMasses s_particleMasses{};
constexpr double s_singleGaussianRange = 0.0001;
constexpr double s_lowerRange = 0.002;
constexpr double s_xOverRange = 0.10;
constexpr double s_upperRange = 0.20;
constexpr double s_componentMeanCut = 0.0;
inline bool
inRange(int var, int lo, int hi)
{
  return ((var <= hi) and (var >= lo));
}

// Logistic function - needed for transformation of weight and mean
inline double
logisticFunction(const double x)
{
  return 1. / (1. + std::exp(-x));
}

using BH = Trk::GsfCombinedMaterialEffects;
void
correctWeights(BH::MixtureParameters& mixture, const int numberOfComponents)
{

  if (numberOfComponents < 1) {
    return;
  }
  // Obtain the sum of weights
  double weightSum(0.);
  for (int i = 0; i < numberOfComponents; ++i) {
    weightSum += mixture[i].weight;
  }
  // Rescale so that total weighting is 1
  for (int i = 0; i < numberOfComponents; ++i) {
    mixture[i].weight /= weightSum;
  }
}

BH::MixtureParameters
getTranformedMixtureParameters(
  const std::array<BH::Polynomial, GSFConstants::maxNumberofBHComponents>&
    polynomialWeights,
  const std::array<BH::Polynomial, GSFConstants::maxNumberofBHComponents>&
    polynomialMeans,
  const std::array<BH::Polynomial, GSFConstants::maxNumberofBHComponents>&
    polynomialVariances,
  const double pathlengthInX0,
  const int numberOfComponents)
{

  BH::MixtureParameters mixture{};
  for (int i = 0; i < numberOfComponents; ++i) {
    const double updatedWeight = polynomialWeights[i](pathlengthInX0);
    const double updatedMean = polynomialMeans[i](pathlengthInX0);
    const double updatedVariance = polynomialVariances[i](pathlengthInX0);
    mixture[i] = { logisticFunction(updatedWeight),
                   logisticFunction(updatedMean),
                   std::exp(updatedVariance) };
  }
  return mixture;
}

BH::MixtureParameters
getMixtureParameters(
  const std::array<BH::Polynomial, GSFConstants::maxNumberofBHComponents>&
    polynomialWeights,
  const std::array<BH::Polynomial, GSFConstants::maxNumberofBHComponents>&
    polynomialMeans,
  const std::array<BH::Polynomial, GSFConstants::maxNumberofBHComponents>&
    polynomialVariances,
  const double pathlengthInX0,
  const int numberOfComponents)
{

  BH::MixtureParameters mixture{};
  for (int i = 0; i < numberOfComponents; ++i) {
    const double updatedWeight = polynomialWeights[i](pathlengthInX0);
    const double updatedMean = polynomialMeans[i](pathlengthInX0);
    const double updatedVariance = polynomialVariances[i](pathlengthInX0);
    mixture[i] = { updatedWeight,
                   updatedMean,
                   updatedVariance * updatedVariance };
  }
  return mixture;
}

Trk::GsfCombinedMaterialEffects::Polynomial
readPolynomial(std::ifstream& fin)
{
  Trk::GsfCombinedMaterialEffects::Polynomial poly{};
  for (size_t i = 0; i < GSFConstants::polynomialCoefficients; ++i) {
    if (!fin) {
      throw std::runtime_error(
        "Reached end of stream but still expecting data.");
    }
    fin >> poly.coefficients[i];
  }
  return poly;
}

} // end of Anonymous namespace for Helper methods

// GsfCombinedMaterialEffects methods
Trk::GsfCombinedMaterialEffects::GsfCombinedMaterialEffects(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IMultiStateMaterialEffects>(this);
}

Trk::GsfCombinedMaterialEffects::~GsfCombinedMaterialEffects() = default;

StatusCode
Trk::GsfCombinedMaterialEffects::initialize()
{
  ATH_CHECK(m_EnergyLossUpdator.retrieve());
  if (!readBHParameters()) {
    ATH_MSG_ERROR(
      "Bethe Heitler Parameters could NOT be successfully imported from file");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool
Trk::GsfCombinedMaterialEffects::readBHParameters()
{

  // Read std polynomial
  std::string resolvedFileName =
    PathResolver::find_file(m_parameterisationFileName, "DATAPATH");
  if (!resolvedFileName.empty()) {
    ATH_MSG_INFO("Parameterisation file found: " << resolvedFileName);
  } else {
    ATH_MSG_ERROR("Parameterisation file not found");
    return false;
  }

  const char* filename = resolvedFileName.c_str();
  std::ifstream fin(filename);
  if (fin.bad()) {
    ATH_MSG_ERROR("Error opening file: " << resolvedFileName);
    return false;
  }

  int orderPolynomial;
  fin >> m_BHnumberOfComponents;
  fin >> orderPolynomial;
  fin >> m_BHtransformationCode;
  //
  if (not inRange(
        m_BHnumberOfComponents, 0, GSFConstants::maxNumberofBHComponents)) {
    ATH_MSG_ERROR("numberOfComponents Parameter out of range 0- "
                  << GSFConstants::maxNumberofBHComponents << " : "
                  << m_BHnumberOfComponents);
    return false;
  }
  if (orderPolynomial != (GSFConstants::polynomialCoefficients - 1)) {
    ATH_MSG_ERROR("orderPolynomial  order !=  "
                  << (GSFConstants::polynomialCoefficients - 1));
    return false;
  }
  if (not inRange(m_BHtransformationCode, 0, 1)) {
    ATH_MSG_ERROR("transformationCode Parameter out of range 0-1: "
                  << m_BHtransformationCode);
    return false;
  }
  if (!fin) {
    ATH_MSG_ERROR("Error while reading file : " << resolvedFileName);
    return false;
  }

  // Fill the polynomials
  int componentIndex = 0;
  for (; componentIndex < m_BHnumberOfComponents; ++componentIndex) {
    m_BHpolynomialWeights[componentIndex] = readPolynomial(fin);
    m_BHpolynomialMeans[componentIndex] = readPolynomial(fin);
    m_BHpolynomialVariances[componentIndex] = readPolynomial(fin);
  }

  // Read the high X0 polynomial
  if (m_useHighX0) {
    resolvedFileName =
      PathResolver::find_file(m_parameterisationFileNameHighX0, "DATAPATH");
    if (!resolvedFileName.empty()) {
      ATH_MSG_INFO("Parameterisation file found: " << resolvedFileName);
    } else {
      ATH_MSG_ERROR("Parameterisation file not found");
      return false;
    }

    const char* filename = resolvedFileName.c_str();
    std::ifstream fin(filename);

    if (fin.bad()) {
      ATH_MSG_ERROR("Error opening file: " << resolvedFileName);
      return false;
    }

    fin >> m_BHnumberOfComponentsHighX0;
    fin >> orderPolynomial;
    fin >> m_BHtransformationCodeHighX0;
    //
    if (not inRange(m_BHnumberOfComponentsHighX0,
                    0,
                    GSFConstants::maxNumberofBHComponents)) {
      ATH_MSG_ERROR("numberOfComponentsHighX0 Parameter out of range 0- "
                    << GSFConstants::maxNumberofBHComponents << " : "
                    << m_BHnumberOfComponentsHighX0);
      return false;
    }
    if (m_BHnumberOfComponentsHighX0 != m_BHnumberOfComponents) {
      ATH_MSG_ERROR(" numberOfComponentsHighX0 != numberOfComponents");
      return false;
    }
    if (orderPolynomial != (GSFConstants::polynomialCoefficients - 1)) {
      ATH_MSG_ERROR("orderPolynomial  order !=  "
                    << (GSFConstants::polynomialCoefficients - 1));
      return false;
    }
    if (not inRange(m_BHtransformationCodeHighX0, 0, 1)) {
      ATH_MSG_ERROR("transformationCode Parameter out of range "
                    "0-1: "
                    << m_BHtransformationCodeHighX0);
      return false;
    }
    if (fin.bad()) {
      ATH_MSG_ERROR("Error reading file: " << resolvedFileName);
      return false;
    }

    // Fill the polynomials
    int componentIndex = 0;
    for (; componentIndex < m_BHnumberOfComponentsHighX0; ++componentIndex) {
      m_BHpolynomialWeightsHighX0[componentIndex] = readPolynomial(fin);
      m_BHpolynomialMeansHighX0[componentIndex] = readPolynomial(fin);
      m_BHpolynomialVariancesHighX0[componentIndex] = readPolynomial(fin);
    }
  }
  return true;
}

void
Trk::GsfCombinedMaterialEffects::compute(
  IMultiStateMaterialEffects::Cache& cache,
  const Trk::ComponentParameters& componentParameters,
  const Trk::MaterialProperties& materialProperties,
  double pathLength,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{
  const AmgSymMatrix(5)* measuredCov = componentParameters.first->covariance();
  /*
   * 1.  Retrieve multiple scattering corrections
   */
  Trk::GSFScatteringCache cache_multipleScatter;
  this->scattering(
    cache_multipleScatter, componentParameters, materialProperties, pathLength);

  /*
   * 2. Retrieve energy loss corrections
   */
  Trk::GSFEnergyLossCache cache_energyLoss;
  if (particleHypothesis == electron) {
    this->BetheHeitler(cache_energyLoss,
                       componentParameters,
                       materialProperties,
                       pathLength,
                       direction);
  } else if (particleHypothesis != nonInteracting) {
    this->energyLoss(cache_energyLoss,
                     componentParameters,
                     materialProperties,
                     pathLength,
                     direction,
                     particleHypothesis);
  }

  // Protect if there are no new energy loss
  // components
  // we want at least on dummy to "combine"
  // with scattering
  if (cache_energyLoss.numElements == 0) {
    cache_energyLoss.elements[0] = { 1, 0, 0 };
    cache_energyLoss.numElements = 1;
  }
  /*
   * 3. Combine the multiple scattering with each of the  energy loss components
   */
  // Reset everything before computation
  cache.reset();
  for (int i = 0; i < cache_energyLoss.numElements; ++i) {
    double combinedWeight = cache_energyLoss.elements[i].weight;
    double combinedDeltaP = cache_energyLoss.elements[i].deltaP;
    cache.weights[i] = combinedWeight;
    cache.deltaPs[i] = combinedDeltaP;
    if (measuredCov) {
      // Create the covariance
      const double covPhi = cache_multipleScatter.deltaPhiCov;
      const double covTheta = cache_multipleScatter.deltaThetaCov;
      const double covQoverP = cache_energyLoss.elements[i].deltaQOvePCov;
      cache.deltaCovariances[i] << 0, 0, 0, 0, 0, // 5
        0, 0, 0, 0, 0,                            // 10
        0, 0, covPhi, 0, 0,                       // 15
        0, 0, 0, covTheta, 0,                     // 20
        0, 0, 0, 0, covQoverP;
    } else {
      cache.deltaCovariances[i].setZero();
    }
    ++cache.numWeights;
    ++cache.numDeltaPs;
    ++cache.numDeltaCovariance;
  } // end for loop over energy loss components
}

void
Trk::GsfCombinedMaterialEffects::scattering(
  Trk::GSFScatteringCache& cache,
  const ComponentParameters& componentParameters,
  const MaterialProperties& materialProperties,
  double pathLength) const
{
  // Reset the cache
  cache.reset();

  // Request track parameters from component parameters
  const Trk::TrackParameters* trackParameters = componentParameters.first.get();
  const AmgSymMatrix(5)* measuredTrackCov = trackParameters->covariance();

  if (!measuredTrackCov) {
    ATH_MSG_DEBUG("No measurement associated with track parameters... "
                  "returning original parameters");
    return;
  }

  const Amg::Vector3D& globalMomentum = trackParameters->momentum();
  const double p = globalMomentum.mag();
  double pathcorrection = 1.;
  if (materialProperties.thickness() != 0) {
    pathcorrection = pathLength / materialProperties.thickness();
  }
  const double t = pathcorrection * materialProperties.thicknessInX0();
  // We were/are using muon here,
  // not sure is what we want 100%.
  const double m = s_particleMasses.mass[Trk::muon];
  const double E = sqrt(p * p + m * m);
  const double beta = p / E;
  const Trk::MaterialInteraction matInt;
  const double sigma = matInt.sigmaMS(t, p, beta);
  const double angularVariation = sigma * sigma;

  const double sinTheta = std::sin(trackParameters->parameters()[Trk::theta]);
  cache.deltaThetaCov = angularVariation;
  cache.deltaPhiCov = angularVariation / (sinTheta * sinTheta);
}

void
Trk::GsfCombinedMaterialEffects::energyLoss(
  Trk::GSFEnergyLossCache& cache,
  const ComponentParameters& componentParameters,
  const MaterialProperties& materialProperties,
  double pathLength,
  PropDirection direction,
  ParticleHypothesis particleHypothesis) const
{

  cache.numElements = 0;

  // Request track parameters from component parameters
  const Trk::TrackParameters* trackParameters = componentParameters.first.get();
  const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();
  if (!measuredCov) {
    ATH_MSG_DEBUG("No measurement on track parameters... returning original "
                  "track parameters");
    return;
  }
  double pathcorrection = pathLength / materialProperties.thickness();
  const Amg::Vector3D& globalMomentum = trackParameters->momentum();

  std::unique_ptr<EnergyLoss> energyLoss(
    m_EnergyLossUpdator->energyLoss(materialProperties,
                                    globalMomentum.mag(),
                                    pathcorrection,
                                    direction,
                                    particleHypothesis,
                                    true));

  // update for mean energy loss
  const double deltaE = energyLoss ? energyLoss->deltaE() : 0;
  const double sigmaDeltaE = energyLoss ? energyLoss->sigmaDeltaE() : 0;
  // Calculate the pathlength encountered by the track
  const double p = globalMomentum.mag();
  const double m = s_particleMasses.mass[particleHypothesis];
  const double E = sqrt(p * p + m * m);
  const double beta = p / E;

  // Calculate energy loss values uncertainty
  const double sigmaQoverP = sigmaDeltaE / pow(beta * p, 2);

  cache.elements[0] = { 1., deltaE, sigmaQoverP * sigmaQoverP };
  cache.numElements = 1;
}

void
Trk::GsfCombinedMaterialEffects::BetheHeitler(
  Trk::GSFEnergyLossCache& cache,
  const Trk::ComponentParameters& componentParameters,
  const Trk::MaterialProperties& materialProperties,
  double pathLength,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis) const
{
  cache.numElements = 0;

  const Trk::TrackParameters* trackParameters = componentParameters.first.get();
  const Amg::Vector3D& globalMomentum = trackParameters->momentum();

  const double radiationLength = materialProperties.x0();
  const double momentum = globalMomentum.mag();
  double pathlengthInX0 = pathLength / radiationLength;

  if (pathlengthInX0 < s_singleGaussianRange) {
    cache.elements[0] = { 1., 0., 0. };
    cache.numElements = 1;
    return;
  }

  // If the amount of material is between 0.0001 and 0.01 return the gaussian
  // approximation to the Bethe-Heitler distribution
  if (pathlengthInX0 < s_lowerRange) {
    const double meanZ = std::exp(-1. * pathlengthInX0);
    const double sign = (direction == Trk::oppositeMomentum) ? 1. : -1.;
    const double varZ =
      std::exp(-1. * pathlengthInX0 * std::log(3.) / std::log(2.)) -
      std::exp(-2. * pathlengthInX0);
    double deltaP(0.);
    double varQoverP(0.);
    if (direction == Trk::alongMomentum) {
      deltaP = sign * momentum * (1. - meanZ);
      varQoverP = 1. / (meanZ * meanZ * momentum * momentum) * varZ;
    } else {
      deltaP = sign * momentum * (1. / meanZ - 1.);
      varQoverP = varZ / (momentum * momentum);
    }
    cache.elements[0] = { 1., deltaP, varQoverP };
    cache.numElements = 1;
    return;
  }

  // Now we do the full calculation
  if (pathlengthInX0 > s_upperRange) {
    pathlengthInX0 = s_upperRange;
  }

  // Get proper mixture parameters
  MixtureParameters mixture;
  if (m_useHighX0 && pathlengthInX0 > s_xOverRange) {
    if (m_BHtransformationCodeHighX0) {
      mixture = getTranformedMixtureParameters(m_BHpolynomialWeightsHighX0,
                                               m_BHpolynomialMeansHighX0,
                                               m_BHpolynomialVariancesHighX0,
                                               pathlengthInX0,
                                               m_BHnumberOfComponents);
    } else {
      mixture = getMixtureParameters(m_BHpolynomialWeightsHighX0,
                                     m_BHpolynomialMeansHighX0,
                                     m_BHpolynomialVariancesHighX0,
                                     pathlengthInX0,
                                     m_BHnumberOfComponents);
    }
  } else {
    if (m_BHtransformationCode) {
      mixture = getTranformedMixtureParameters(m_BHpolynomialWeights,
                                               m_BHpolynomialMeans,
                                               m_BHpolynomialVariances,
                                               pathlengthInX0,
                                               m_BHnumberOfComponents);
    } else {
      mixture = getMixtureParameters(m_BHpolynomialWeights,
                                     m_BHpolynomialMeans,
                                     m_BHpolynomialVariances,
                                     pathlengthInX0,
                                     m_BHnumberOfComponents);
    }
  }

  // Correct the mixture
  correctWeights(mixture, m_BHnumberOfComponents);

  int componentIndex = 0;
  double weightToBeRemoved(0.);
  int componentWithHighestMean(0);
  for (; componentIndex < m_BHnumberOfComponents; ++componentIndex) {
    if (mixture[componentIndex].mean > mixture[componentWithHighestMean].mean) {
      componentWithHighestMean = componentIndex;
    }
    if (mixture[componentIndex].mean >= s_componentMeanCut) {
      continue;
    }
    weightToBeRemoved += mixture[componentIndex].weight;
  }
  // Fill the cache to be returned
  componentIndex = 0;
  for (; componentIndex < m_BHnumberOfComponents; ++componentIndex) {
    double varianceInverseMomentum;
    // This is not mathematically correct but it does stabilize the GSF
    if (mixture[componentIndex].mean < s_componentMeanCut) {
      continue;
    }
    double weight = mixture[componentIndex].weight;
    if (componentIndex == componentWithHighestMean) {
      weight += weightToBeRemoved;
    }
    double deltaP(0.);
    if (direction == alongMomentum) {
      // For forward propagation
      deltaP = momentum * (mixture[componentIndex].mean - 1.);
      const double f = 1. / (momentum * mixture[componentIndex].mean);
      varianceInverseMomentum = f * f * mixture[componentIndex].variance;
    } // end forward propagation if clause
    else {
      // For backwards propagation
      deltaP = momentum * (1. / mixture[componentIndex].mean - 1.);
      varianceInverseMomentum =
        mixture[componentIndex].variance / (momentum * momentum);
    } // end backwards propagation if clause

    // set in the cache and increase the elements
    cache.elements[cache.numElements] = { weight,
                                          deltaP,
                                          varianceInverseMomentum };
    ++cache.numElements;
  } // end for loop over all components
}

