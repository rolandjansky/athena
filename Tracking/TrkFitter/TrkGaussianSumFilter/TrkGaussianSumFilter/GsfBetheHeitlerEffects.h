/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfBetheHeitlerEffects.h
 * @date   Friday 18th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Bethe-Heitler material effects for electrons, used in the GSF
 */

#ifndef Trk_GsfBetheHeitlerEffects_H
#define Trk_GsfBetheHeitlerEffects_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGaussianSumFilter/GsfConstants.h"
#include "TrkGaussianSumFilter/IBetheHeitlerEffects.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <array>
namespace Trk {

class GsfBetheHeitlerEffects
  : public AthAlgTool
  , virtual public IBetheHeitlerEffects
{

public:
  struct ComponentValues
  {
    double weight;
    double mean;
    double variance;
  };

  using MixtureParameters =
    std::array<ComponentValues, GSFConstants::maxNumberofBHComponents>;

  /** Helper class for construction and evaluation of polynomial */
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

  GsfBetheHeitlerEffects(const std::string&,
                         const std::string&,
                         const IInterface*);

  virtual ~GsfBetheHeitlerEffects() = default;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override final;

  virtual void compute(Trk::GSFEnergyLossCache& cache,
                       const ComponentParameters& componentParameters,
                       const MaterialProperties& materialProperties,
                       double pathLenght,
                       PropDirection direction = anyDirection,
                       ParticleHypothesis particleHypothesis =
                         nonInteracting) const override final;

private:
  // Read polynomial fit parameters from a specified file
  bool readParameters();

  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_polynomialWeights;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_polynomialMeans;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_polynomialVariances;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_polynomialWeightsHighX0;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_polynomialMeansHighX0;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_polynomialVariancesHighX0;

  int m_numberOfComponents;
  int m_transformationCode;
  int m_correctionFlag;
  int m_numberOfComponentsHighX0;
  int m_transformationCodeHighX0;

  double m_singleGaussianRange;
  double m_lowerRange;
  double m_xOverRange;
  double m_upperRange;
  double m_componentMeanCut;

  bool m_useHighX0;
  std::string m_parameterisationFileName;
  std::string m_parameterisationFileNameHighX0;
};

}

#endif
