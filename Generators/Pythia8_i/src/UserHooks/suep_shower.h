/*
 * This file is taken from the available public code at: 
 * https://gitlab.com/simonknapen/suep_generator 
 * by Simon Knapen.
 */
#ifndef SUEPSHOWER_h
#define SUEPSHOWER_h

#include <vector>
#include <math.h>
#include <functional> // for bind() 
#include <boost/math/tools/roots.hpp>
#include "Pythia8/PhaseSpace.h"
#include "Pythia8/Basics.h"

//** Auxiliary class for tolerance checks */
class tolerance {
 public:
  tolerance(double eps) :
  m_eps(eps) {
  }
  bool operator()(double a, double b) {
    return (std::abs(b - a) <= m_eps);
  }
 private:
  double m_eps;
};

/** Auxiliary class for SUEP generation.
 * Details on models available on arXiv:1612.00850. 
 */
class Suep_shower 
{ 
 public:
  
  /** Constructor.
   * @param mass Mass of the dark meson
   * @param temperature model parameter
   * @param energy total energy of decaying system
   * @param rndm random number generator, if any (if not provided will use the rand() function).
   */
  Suep_shower(double mass, double temperature, double energy, Pythia8::Rndm *rndm = 0);
  
  /** Generate a shower event, in the rest frame of the showe. */
  std::vector< Pythia8::Vec4 > generate_shower();
  
 protected: 

  /** Random number generator, if not provided will use rand() */
  Pythia8::Rndm *m_rndmEngine;
  
  /** Mass of the dark mesons to be generated in the shower */
  double m_m;
  
  /** Temperature parameter */
  double m_Temp;
  
  /** Total energy of the system */
  double m_Etot; 
  
 protected:
  
  /** Maxwell-Boltzman distribution, slightly massaged */
  double f(double p);
  
  /** Derivative of Maxwell-Boltzmann */
  double fp(double p);
  
  /** Test function to be solved for p_plus,p_minus */
  double test_fun(double p);
  
  /** generate one random 4 vector from the thermal distribution */
  Pythia8::Vec4 generateFourVector(); 
  
  /** auxiliary function which computes the total energy difference as a function of the 
   * momentum vectors and a scale factor "a" to balance energy */
  double reballance_func(double a, const std::vector< Pythia8::Vec4 > &event);
  
 private:
  /** mass/Temperature ratio */
  double m_A;
  /** convenience function of mass/Temperature ratio */
  double m_p_m;
  /** solutions for shower generation. See paper for details. */
  double m_p_plus, m_p_minus;
  /** solutions in Lambda for shower generation */
  double m_lambda_plus,m_lambda_minus;
  /** solutions for shower generation. See paper for details. */
  double m_q_plus, m_q_minus, m_q_m;
  
}; 

#endif
