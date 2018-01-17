/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowCellIntegration.h
 *
 *  Created on: 12.08.2013
 *      Author: tlodd
 */

#ifndef EFLOWCELLINTEGRATION_H_
#define EFLOWCELLINTEGRATION_H_

#include <stdexcept>
#include <iostream>

#include <math.h>

#include "eflowRec/eflowUtil.h"
#include "eflowRec/eflowLookupExp.h"
#include "eflowRec/LegendreWeights.h"

/* Enum used as template argument to switch between std::exp and the lookup-table based version */
enum Exp_t {stdExp = 0, lookupExp};

/* Class to perform a generic recursive Gauss-Legendre Integration, see
 * http://en.wikipedia.org/wiki/Gaussian_quadrature#Gauss.E2.80.93Legendre_quadrature
 * Templated in the type of the integrand. All the integrand class needs to have, is an evaluate() method,
 * that takes a double as a parameter and returns a double.
 * Templates are used instead of polymorphism here due to the better CPU performance.
 */
template <class IntegrandType>
class eflowRecursiveGaussLegendreIntegrator {
public:
  eflowRecursiveGaussLegendreIntegrator(IntegrandType* integrand, double error) :
    m_integrand(integrand), m_error(error), m_depth(1) {
    assert(m_integrand);
  }
  virtual ~eflowRecursiveGaussLegendreIntegrator() { }

  virtual double integrate(const eflowRange& range) {
    /* Try a 5th and 6th order Gauss–Legendre quadrature */
    double I5 = DoGaussLegendreIntegration(range, 5);
    double I6 = DoGaussLegendreIntegration(range, 6);

    /* If results agree within m_error threshold, return the mean... */
    double Imean = (I5 + I6) / 2.0;
    if (fabs(I5 - I6) / Imean <= m_error) {
      return Imean;
    } else {
      /* ...else recursively part up the integration into n subRanges */
      return RecurseIntegration(range, 2);
    }
  }

  double getError() const {return m_error;}
  
private:

  double DoGaussLegendreIntegration(const eflowRange& range, int nOrder){
    /* Perform nth order Gauss–Legendre quadrature, see
     * http://en.wikipedia.org/wiki/Gaussian_quadrature#Gauss.E2.80.93Legendre_quadrature */

    /* Array offset for legendre weights/roots */
    int j = nOrder * (nOrder - 1) / 2;
    double* roots   = legendreRoots   + j;
    double* weights = legendreWeights + j;

    double rangeCenter    = range.getCenter();
    double rangeHalfWidth = range.getWidth()/2;

    double I = 0.0;
    double x;
    if(m_integrand){
      for (int i = 0; i < nOrder; i++) {
	x = rangeCenter + roots[i] * rangeHalfWidth;
	I += weights[i] * m_integrand->evaluate(x);
      }
    }
    else{
      std::cerr << "eflowCellIntergrator::DoGaussLegendreIntegration ERROR : Invalid pointer to IntegrandType and cannot perform integration" << std::endl;
    }
    if (I < 0. || rangeHalfWidth < 0.) std::cerr << "eflowCellIntergrator::DoGaussLegendreIntegration WARNING: I = " << I << "\trange = " << range.print() << std::endl;

    return I * rangeHalfWidth;
  }

  double RecurseIntegration(const eflowRange& range, int nSubRanges){
    m_depth++;
    /* Part up integration range in n subRanges */
    double subRangeWidth = range.getWidth() / nSubRanges;
    eflowRange subRange(range.getMin(), range.getMin() + subRangeWidth);

    /* Loop over subranges */
    double Integral(0.0);
    for (int i = 0; i < nSubRanges; i++) {
      Integral += integrate(subRange);
      subRange.shift(subRangeWidth);
    }
    m_depth--;

    return Integral;
  }

  IntegrandType* m_integrand;
  double m_error;
  int m_depth;
};

/* Class to represent the 2D Gaussian integrand.
 * EtaSq is set via a set method, phi is a parameter to the evalutate() method.
 * Hence the class is only the direct integrator of the inner (i.e. phi) integration,
 * while the eflowCell Integrator acts as the integrand for the outer (i.e. eta)
 * integration.
 * To evaluate the integrand, either std::exp or eflowLookupExp is used,
 * depending on the value of the template parameter.
 * Templates are used instead of polymorphism here due to the better CPU performance.
 * */
template <Exp_t expType> class eflowCellIntegrand {
public:
  eflowCellIntegrand(double sigma): m_lookupExp(eflowLookupExp::getInstance()),
  m_oneOverTwoSigmaSq(1 / (2 * sigma * sigma)), m_norm(m_oneOverTwoSigmaSq / M_PI), m_etaSq(NAN) { }
  ~eflowCellIntegrand() { }

  inline void setEtaSq(double xSq) { m_etaSq = xSq; }

  inline double evaluateStdExp(double rSq) { return m_norm * exp(-rSq * m_oneOverTwoSigmaSq); }
  inline double evaluateLookupExp(double rSq) { return m_lookupExp->evaluate(rSq * m_oneOverTwoSigmaSq)*m_norm; }

  /* The evaluate method for the integration. The implementation depends on the template parameter */
  inline double evaluate(double y);

private:
  eflowLookupExp* m_lookupExp;
  double m_oneOverTwoSigmaSq;
  double m_norm;
  double m_etaSq;
};
template<> inline double eflowCellIntegrand<stdExp>::evaluate(double phi) { return evaluateStdExp(m_etaSq+phi*phi); }
template<> inline double eflowCellIntegrand<lookupExp>::evaluate(double phi) { return evaluateLookupExp(m_etaSq+phi*phi); }

/* Class that controls the 2D integration.
 * Holds two eflowRecursiveGaussLegendreIntegrators, one to perform the outer (i.e. eta)
 * and one to perform the inner (i.e. phi) integration.
 * The class itself acts as the integrand of the outer integration. Its evaluate method
 * performs a full integral in phi at the given eta value. (This is how the 2D integration
 * is achieved.) The integrand of the inner (i.e. phi) integration is an eflowCellIntegrand.
 *
 * To evaluate the actual Gaussian, either std::exp or eflowLookupExp is used, depending on
 * the value of the template parameter (see eflowCellIntegrand).
 * Templates are used instead of polymorphism here due to the better CPU performance.
 */
template <int expType> class eflowCellIntegrator {
public:
  eflowCellIntegrator(double stdDev, double error) : m_integrand2D(std::make_unique<eflowCellIntegrand<(Exp_t)expType> >(stdDev)), m_outerIntegrator(this, error), m_innerIntegrator(m_integrand2D.get(), error) { }
  eflowCellIntegrator(const eflowCellIntegrator& original) : m_integrand2D(std::make_unique<eflowCellIntegrand<(Exp_t)expType> >(*(original.m_integrand2D.get()))), m_outerIntegrator(this, original.m_outerIntegrator.getError()), m_innerIntegrator( m_integrand2D.get(), original.m_innerIntegrator.getError()) {
    m_rangePhi = original.m_rangePhi;
  }
  eflowCellIntegrator&  operator=(const eflowCellIntegrator& original){
    m_integrand2D(std::make_unique<eflowCellIntegrand<(Exp_t)expType> >(*(original.m_integrand2D.get())));
    m_outerIntegrator(this, original.m_outerIntegrator.getError());
    m_innerIntegrator( m_integrand2D.get(), original.m_innerIntegrator.getError());
    m_rangePhi = original.m_rangePhi;
  }
  ~eflowCellIntegrator() {}

  /* Main method, which starts the integration */
  inline double integrate(const eflowRange& etaRange, const eflowRange& phiRange) {
    /* Store the phi range for the inner integration */
    m_rangePhi = phiRange;
    /* Invoke the outer integration */
    return m_outerIntegrator.integrate(etaRange);
  }

  /* Evaluate method for the outer (i.e. eta) integration (invoked by m_outerIntegrator) */
  inline double evaluate(double eta) {
    /* Set the eta (square) value for the inner integration */
    m_integrand2D->setEtaSq(eta*eta);
    /* Perform the inner (i.e. phi) integration (will invoke the evaluate() method on m_innerIntegrator) */
    return m_innerIntegrator.integrate(m_rangePhi);
  }

private:
  std::unique_ptr<eflowCellIntegrand<(Exp_t)expType> > m_integrand2D;
  eflowRecursiveGaussLegendreIntegrator<eflowCellIntegrator<expType>  > m_outerIntegrator;
  eflowRecursiveGaussLegendreIntegrator<eflowCellIntegrand<(Exp_t)expType> > m_innerIntegrator;
  eflowRange m_rangePhi;
};


#endif /* EFLOWCELLINTEGRATION_H_ */
