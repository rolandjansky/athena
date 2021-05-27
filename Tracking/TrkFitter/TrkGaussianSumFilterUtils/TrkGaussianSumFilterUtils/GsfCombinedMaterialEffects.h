/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfCombinedMaterialEffects.h
 * @date   Friday 11th January 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * Class definition for consideration of multiple scatter and energy
 * loss effects from material simultaneously
 */

#ifndef TrkGsfCombinedMaterialEffects_H
#define TrkGsfCombinedMaterialEffects_H

#include "TrkGaussianSumFilterUtils/ComponentParameters.h"
#include "TrkGaussianSumFilterUtils/GsfConstants.h"
#include "TrkGaussianSumFilterUtils/GsfMaterial.h"
//
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/MaterialProperties.h"

namespace Trk {

class GsfCombinedMaterialEffects
{
public:
  /** Helper Struct for multiple Gaussian components*/
  struct ComponentValues
  {
    double weight;
    double mean;
    double variance;
  };
  using MixtureParameters =
    std::array<ComponentValues, GSFConstants::maxNumberofBHComponents>;

  /** Helper struct for construction and evaluation of polynomial */
  struct Polynomial
  {
    // Evaluation of the polynomial for given material thickness (t)
    double operator()(const double& t) const
    {
      double sum(0.);
      for (size_t i = 0; i < GSFConstants::polynomialCoefficients; ++i) {
        sum = t * sum + coefficients[i];
      }
      return sum;
    }
    std::array<double, GSFConstants::polynomialCoefficients> coefficients;
  };

  // ctor with arguments
  GsfCombinedMaterialEffects(
    const std::string& parameterisationFileName,
    const std::string& parameterisationFileNameHighX0);

  // ctor with arguments
  GsfCombinedMaterialEffects() = default;
  GsfCombinedMaterialEffects(const GsfCombinedMaterialEffects&) = default;
  GsfCombinedMaterialEffects(GsfCombinedMaterialEffects&&) = default;
  GsfCombinedMaterialEffects& operator=(const GsfCombinedMaterialEffects&) =
    default;
  GsfCombinedMaterialEffects& operator=(GsfCombinedMaterialEffects&&) = default;
  ~GsfCombinedMaterialEffects() = default;

  void compute(GsfMaterial::Combined&,
               const Trk::ComponentParameters&,
               const Trk::MaterialProperties&,
               double,
               Trk::PropDirection = anyDirection,
               Trk::ParticleHypothesis = nonInteracting) const;

private:
  // Electron enrrgy loss due to Bremsstrahlung
  void BetheHeitler(
    GsfMaterial::EnergyLoss& cache,
    const ComponentParameters& componentParameters,
    const MaterialProperties& materialProperties,
    double pathLenght,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  int m_BHnumberOfComponents{};
  int m_BHtransformationCode{};
  int m_BHnumberOfComponentsHighX0{};
  int m_BHtransformationCodeHighX0{};

  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialWeights{};
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialMeans{};
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialVariances{};
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialWeightsHighX0{};
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialMeansHighX0{};
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialVariancesHighX0{};
};

} // end Trk namespace

#endif
