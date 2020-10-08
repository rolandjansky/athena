/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GsfBetheHeitlerEffects.cxx
 * @begin         Sunday 20th February 2005
 * @author        Anthony Morley, Tom  Atkinson, Christos Anastopoulos
 * @brief         Implementation code for the Bethe-Heitler material effects in
 * the GSF.
 */

#include "TrkGaussianSumFilter/GsfBetheHeitlerEffects.h"
#include "PathResolver/PathResolver.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkSurfaces/Surface.h"

#include <cmath>
#include <fstream>

namespace {

using BH = Trk::GsfBetheHeitlerEffects;

template<class T>
bool
inRange(const T& var, const T& lo, const T& hi)
{
  return ((var <= hi) and (var >= lo));
}

// Logistic function - needed for transformation of weight and mean
inline double
logisticFunction(const double x)
{
  return 1. / (1. + std::exp(-x));
}

// First moment of the Bethe-Heitler distribution
inline double
betheHeitlerMean(const double r)
{
  return std::exp(-r);
}

// Second moment of the Bethe-Heitler distribution
inline double
betheHeitlerVariance(const double r)
{
  return std::exp(-r * std::log(3.) / std::log(2.)) - std::exp(-2 * r);
}

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

double
correctedFirstMean(const double pathlengthInX0,
                   const BH::MixtureParameters& mixture,
                   const int numberOfComponents)
{
  if (numberOfComponents < 1) {
    return 0;
  }
  // Obtain the difference between the true and weighted sum
  double meanBH = betheHeitlerMean(pathlengthInX0);
  for (int i = 1; i < numberOfComponents; ++i) {
    meanBH -= mixture[i].weight * mixture[i].mean;
  }

  // return the corrected mean for the first component
  return std::max(std::min(meanBH / mixture[0].weight, 1.), 0.);
}

double
correctedFirstVariance(const double pathlengthInX0,
                       const BH::MixtureParameters& mixture,
                       const int numberOfComponents)
{
  if (numberOfComponents < 1) {
    return 0;
  }

  // Obtain the difference between the true and weighed sum variances
  double varianceBH =
    betheHeitlerVariance(pathlengthInX0) +
    (betheHeitlerMean(pathlengthInX0) * betheHeitlerMean(pathlengthInX0));

  varianceBH -= mixture[0].weight * mixture[0].mean * mixture[0].mean;

  for (int i = 1; i < numberOfComponents; ++i) {
    varianceBH -= mixture[i].weight *
                  (mixture[i].mean * mixture[i].mean + mixture[i].variance);
  }

  return std::max(varianceBH / mixture[0].weight, 0.);
}

BH::MixtureParameters
getTranformedMixtureParameters(
  const std::array<BH::Polynomial, BH::maxNumberofComponents>&
    polynomialWeights,
  const std::array<BH::Polynomial, BH::maxNumberofComponents>& polynomialMeans,
  const std::array<BH::Polynomial, BH::maxNumberofComponents>&
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
  const std::array<BH::Polynomial, BH::maxNumberofComponents>&
    polynomialWeights,
  const std::array<BH::Polynomial, BH::maxNumberofComponents>& polynomialMeans,
  const std::array<BH::Polynomial, BH::maxNumberofComponents>&
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

} // end of Anonymous namespace for Helper methods

Trk::GsfBetheHeitlerEffects::GsfBetheHeitlerEffects(const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_numberOfComponents(0)
  , m_transformationCode(0)
  , m_correctionFlag(0)
  , m_numberOfComponentsHighX0(0)
  , m_transformationCodeHighX0(0)
  , m_parameterisationFileName("GeantSim_LT01_cdf_nC6_O5.par")
  , m_parameterisationFileNameHighX0("GeantSim_GT01_cdf_nC6_O5.par")
{

  declareInterface<IBetheHeitlerEffects>(this);
  declareProperty("BetheHeitlerParameterisationFileName",
                  m_parameterisationFileName);
  declareProperty("BetheHeitlerParameterisationFileNameHighX0",
                  m_parameterisationFileNameHighX0);
  declareProperty("BetheHeitlerParameterisationCorrectionFlag",
                  m_correctionFlag);
  declareProperty("SingleGaussianRange", m_singleGaussianRange = 0.0001);
  declareProperty("LowerRange", m_lowerRange = 0.002);
  declareProperty("XoverRange", m_xOverRange = 0.10);
  declareProperty("UpperRange", m_upperRange = 0.20);
  declareProperty("UseHighX0", m_useHighX0 = true);
  declareProperty("ComponentMeanCut", m_componentMeanCut = 0.0);
}

StatusCode
Trk::GsfBetheHeitlerEffects::initialize()
{
  if (m_correctionFlag == 1) {
    ATH_MSG_INFO("1st moment of mixture will be corrected");
  } else if (m_correctionFlag == 2) {
    ATH_MSG_INFO("1st and 2nd moment of mixture will be corrected");
  } else if (m_correctionFlag == 0) {
    ATH_MSG_INFO("Moments of mixture will not be corrected");
  } else {
    ATH_MSG_ERROR("Inappropriate setting for Bethe-Heitler mixture correction! "
                  "...Exiting!");
    return StatusCode::FAILURE;
  }
  if (this->readParameters()) {
    ATH_MSG_INFO("Parameters successfully imported from file");
    ATH_MSG_INFO("Initialisation of " << name() << " was successful");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_ERROR("Parameters could NOT be successfully imported from file");
  return StatusCode::FAILURE;
}

bool
Trk::GsfBetheHeitlerEffects::readParameters()
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
  fin >> m_numberOfComponents;
  fin >> orderPolynomial;
  fin >> m_transformationCode;
  //
  if (not inRange(m_numberOfComponents, 0, maxNumberofComponents)) {
    ATH_MSG_ERROR("numberOfComponents Parameter out of range 0- "
                  << maxNumberofComponents << " : " << m_numberOfComponents);
    return false;
  }
  if (not inRange(orderPolynomial, 0, maxOrderPolynomial)) {
    ATH_MSG_ERROR("orderPolynomial Parameter out of range 0- "
                  << maxOrderPolynomial << " : " << orderPolynomial);
    return false;
  }
  if (not inRange(m_transformationCode, 0, 10)) {
    ATH_MSG_ERROR("transformationCode Parameter out of range 0-10: "
                  << m_transformationCode);
    return false;
  }
  if (!fin) {
    ATH_MSG_ERROR("Error while reading file : " << resolvedFileName);
    return false;
  }

  int componentIndex = 0;
  for (; componentIndex < m_numberOfComponents; ++componentIndex) {
    m_polynomialWeights[componentIndex] = readPolynomial(fin, orderPolynomial);
    m_polynomialMeans[componentIndex] = readPolynomial(fin, orderPolynomial);
    m_polynomialVariances[componentIndex] =
      readPolynomial(fin, orderPolynomial);
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

    fin >> m_numberOfComponentsHighX0;
    fin >> orderPolynomial;
    fin >> m_transformationCodeHighX0;
    //
    if (not inRange(m_numberOfComponentsHighX0, 0, maxNumberofComponents)) {
      ATH_MSG_ERROR("numberOfComponentsHighX0 Parameter out of range 0- "
                    << maxNumberofComponents << " : "
                    << m_numberOfComponentsHighX0);
      return false;
    }
    if (m_numberOfComponentsHighX0 != m_numberOfComponents) {
      ATH_MSG_ERROR(" numberOfComponentsHighX0 != numberOfComponents");
      return false;
    }

    if (not inRange(orderPolynomial, 0, maxOrderPolynomial)) {
      ATH_MSG_ERROR("orderPolynomial Parameter out of range 0- "
                    << maxOrderPolynomial << " : " << orderPolynomial);
      return false;
    }
    if (not inRange(m_transformationCodeHighX0, 0, 10)) {
      ATH_MSG_ERROR("transformationCode Parameter out of range 0-10: "
                    << m_transformationCodeHighX0);
      return false;
    }
    if (fin.bad()) {
      ATH_MSG_ERROR("Error reading file: " << resolvedFileName);
      return false;
    }

    int componentIndex = 0;
    for (; componentIndex < m_numberOfComponentsHighX0; ++componentIndex) {
      m_polynomialWeightsHighX0[componentIndex] =
        readPolynomial(fin, orderPolynomial);
      m_polynomialMeansHighX0[componentIndex] =
        readPolynomial(fin, orderPolynomial);
      m_polynomialVariancesHighX0[componentIndex] =
        readPolynomial(fin, orderPolynomial);
    }
  }
  return true;
}

Trk::GsfBetheHeitlerEffects::Polynomial
Trk::GsfBetheHeitlerEffects::readPolynomial(std::ifstream& fin, const int order)
{
  std::vector<double> coefficients(order + 1);
  int orderIndex = 0;
  for (; orderIndex < (order + 1); ++orderIndex) {
    if (!fin) {
      throw std::runtime_error(
        "Reached end of stream but still expecting data.");
    }
    fin >> coefficients[orderIndex];
  }
  return Polynomial(coefficients);
}

void
Trk::GsfBetheHeitlerEffects::compute(
  Trk::GSFEnergyLossCache& cache,
  const Trk::ComponentParameters& componentParameters,
  const Trk::MaterialProperties& materialProperties,
  double pathLength,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis) const
{
  // Clear cache
  cache.reset();

  const Trk::TrackParameters* trackParameters = componentParameters.first.get();
  const Amg::Vector3D& globalMomentum = trackParameters->momentum();

  const double radiationLength = materialProperties.x0();
  const double momentum = globalMomentum.mag();
  double pathlengthInX0 = pathLength / radiationLength;

  if (pathlengthInX0 < m_singleGaussianRange) {
    cache.weights.push_back(1.);
    cache.deltaPs.push_back(0.);
    cache.deltaQOvePCov.push_back(0.);
    return;
  }

  // If the amount of material is between 0.0001 and 0.01 return the gaussian
  // approximation to the Bethe-Heitler distribution
  if (pathlengthInX0 < m_lowerRange) {
    const double meanZ = std::exp(-1. * pathlengthInX0);
    const double sign = (direction == Trk::oppositeMomentum) ? 1. : -1.;
    const double varZ = std::exp(-1. * pathlengthInX0 * std::log(3.) / std::log(2.)) -
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
    cache.deltaPs.push_back(deltaP);
    cache.weights.push_back(1.);
    cache.deltaQOvePCov.push_back(varQoverP);
    return;
  }

  // Now we do the full calculation
  if (pathlengthInX0 > m_upperRange) {
    pathlengthInX0 = m_upperRange;
  }

  // Get proper mixture parameters
  MixtureParameters mixture;
  if (m_useHighX0 && pathlengthInX0 > m_xOverRange) {
    if (m_transformationCodeHighX0) {
      mixture = getTranformedMixtureParameters(m_polynomialWeightsHighX0,
                                               m_polynomialMeansHighX0,
                                               m_polynomialVariancesHighX0,
                                               pathlengthInX0,
                                               m_numberOfComponents);
    } else {
      mixture = getMixtureParameters(m_polynomialWeightsHighX0,
                                     m_polynomialMeansHighX0,
                                     m_polynomialVariancesHighX0,
                                     pathlengthInX0,
                                     m_numberOfComponents);
    }
  } else {
    if (m_transformationCode) {
      mixture = getTranformedMixtureParameters(m_polynomialWeights,
                                               m_polynomialMeans,
                                               m_polynomialVariances,
                                               pathlengthInX0,
                                               m_numberOfComponents);
    } else {
      mixture = getMixtureParameters(m_polynomialWeights,
                                     m_polynomialMeans,
                                     m_polynomialVariances,
                                     pathlengthInX0,
                                     m_numberOfComponents);
    }
  }

  // Correct the mixture
  correctWeights(mixture, m_numberOfComponents);

  if (m_correctionFlag == 1) {
    mixture[0].mean =
      correctedFirstMean(pathlengthInX0, mixture, m_numberOfComponents);
  }
  if (m_correctionFlag == 2) {
    mixture[0].mean =
      correctedFirstMean(pathlengthInX0, mixture, m_numberOfComponents);
    mixture[0].variance =
      correctedFirstVariance(pathlengthInX0, mixture, m_numberOfComponents);
  }

  int componentIndex = 0;
  double weightToBeRemoved(0.);
  int componentWithHighestMean(0);
  for (; componentIndex < m_numberOfComponents; ++componentIndex) {
    if (mixture[componentIndex].mean > mixture[componentWithHighestMean].mean) {
      componentWithHighestMean = componentIndex;
    }
    if (mixture[componentIndex].mean >= m_componentMeanCut) {
      continue;
    }
    weightToBeRemoved += mixture[componentIndex].weight;
  }
  // Fill the cache to be returned
  componentIndex = 0;
  for (; componentIndex < m_numberOfComponents; ++componentIndex) {
    double varianceInverseMomentum;
    // This is not mathematically correct but it does stabilize the GSF
    if (mixture[componentIndex].mean < m_componentMeanCut) {
      continue;
    }
    if (componentIndex == componentWithHighestMean) {
      cache.weights.push_back(mixture[componentIndex].weight +
                              weightToBeRemoved);
    } else {
      cache.weights.push_back(mixture[componentIndex].weight);
    }

    double deltaP(0.);
    if (direction == alongMomentum) {
      // For forward propagation
      deltaP = momentum * (mixture[componentIndex].mean - 1.);
      cache.deltaPs.push_back(deltaP);
      const double f = 1. / (momentum * mixture[componentIndex].mean);
      varianceInverseMomentum = f * f * mixture[componentIndex].variance;
    } // end forward propagation if clause
    else {
      // For backwards propagation
      deltaP = momentum * (1. / mixture[componentIndex].mean - 1.);
      cache.deltaPs.push_back(deltaP);
      varianceInverseMomentum =
        mixture[componentIndex].variance / (momentum * momentum);
    } // end backwards propagation if clause
    cache.deltaQOvePCov.push_back(varianceInverseMomentum);
  } // end for loop over all components
}

