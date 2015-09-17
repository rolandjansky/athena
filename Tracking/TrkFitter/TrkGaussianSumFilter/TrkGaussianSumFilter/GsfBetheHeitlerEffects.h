/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfBetheHeitlerEffects.h  -  description
      -----------------------------------------
begin                : Friday 18th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Bethe-Heitler material effects for the GSF
*********************************************************************************/

#ifndef Trk_GsfBetheHeitlerEffects_H
#define Trk_GsfBetheHeitlerEffects_H

#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkGaussianSumFilter/MultiStateMaterialEffects.h"

namespace Trk{

class GsfBetheHeitlerEffects : public MultiStateMaterialEffects, virtual public IMultiStateMaterialEffects {

 private:

  /** Helper class for construction and evaluation of polynomial */
  class Polynomial {
  public:
    // Default constructor
    Polynomial () {};

    /** Constructor from a vector of coefficients (in decreasing order of powers of x */
    Polynomial (const std::vector<double>& coefficients)
      :
      m_coefficients(coefficients)
      {};

    // Evaluation of the polynomial for given material thickness (t)
    double operator () (const double& t) const
      {
  double sum(0.);
  std::vector<double>::const_iterator coefficient = m_coefficients.begin();

  for ( ; coefficient != m_coefficients.end(); ++coefficient)
    sum = t * sum + (*coefficient);

  return sum;
      }

  private:
    std::vector<double> m_coefficients;
  };

  struct ComponentValues {
    double weight;
    double mean;
    double variance;
  };

 public:
  GsfBetheHeitlerEffects(const std::string&, const std::string&, const IInterface*);

  virtual ~GsfBetheHeitlerEffects() {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

 private:
  typedef std::vector<ComponentValues> MixtureParameters;

 private:
  virtual void compute ( const ComponentParameters&,
       const MaterialProperties&,
       double,
       PropDirection direction = anyDirection,
       ParticleHypothesis particleHypothesis = nonInteracting ) const;

  // Read polynomial fit parameters from a specified file
  bool readParameters();

  // Read coeffients for a single polynomial fit
  Polynomial readPolynomial (std::ifstream&, const int);

  // Get mixture parameters
  void getMixtureParameters(const double, MixtureParameters&) const;
  
  // Get mixture parameters
  void getMixtureParametersHighX0(const double, MixtureParameters&) const;

  // Correct weights of components
  void correctWeights(MixtureParameters&) const;

  // Correct mean for component 1
  double correctedFirstMean(const double, const MixtureParameters&) const;

  // Correct variance for component 1
  double correctedFirstVariance(const double, const MixtureParameters&) const;

  // Logistic function - needed for transformation of weight and mean
  inline double logisticFunction (const double x) const {
    return (double) 1. / (1. + exp(-x) );
  }

  // First moment of the Bethe-Heitler distribution
  inline double betheHeitlerMean (const double r) const {
    return (double) exp(-r);
  }

  // Second moment of the Bethe-Heitler distribution
  inline double betheHeitlerVariance ( const double r) const {
    return (double) exp( -r * log(3.)/log(2.) ) - exp(-2 * r);
  }

  private:

  std::string m_parameterisationFileName;

  int m_numberOfComponents;
  int m_transformationCode;
  std::vector<Polynomial> m_polynomialWeights;
  std::vector<Polynomial> m_polynomialMeans;
  std::vector<Polynomial> m_polynomialVariances;


  int m_correctionFlag;
  
  std::string m_parameterisationFileNameHighX0;

  int m_numberOfComponentsHighX0;
  int m_transformationCodeHighX0;
  std::vector<Polynomial> m_polynomialWeightsHighX0;
  std::vector<Polynomial> m_polynomialMeansHighX0;
  std::vector<Polynomial> m_polynomialVariancesHighX0;

  double m_singleGaussianRange;
  double m_lowerRange;
  double m_xOverRange;
  double m_upperRange;
  bool   m_useHighX0; 
  double m_componentMeanCut;


  //mutable double m_lastDz;
  //mutable double m_lastP;
  //mutable PropDirection m_lastPropDirection;
  //mutable double m_lastRadLength;
       


};

}

#endif
