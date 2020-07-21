/*
 * This file is taken from the available public code at: 
 * https://gitlab.com/simonknapen/suep_generator 
 * by Simon Knapen.
 */
#ifndef SUEPSHOWER_h
#define SUEPSHOWER_h

#include <vector>
#include <math.h>
#include <boost/math/tools/roots.hpp>
#include <boost/bind.hpp>

//** Auxiliary class for tolerance checks */
class tolerance {
 public:
  tolerance(double eps) :
  _eps(eps) {
  }
  bool operator()(double a, double b) {
    return (std::abs(b - a) <= _eps);
  }
 private:
  double _eps;
};

/** Auxiliary class for SUEP generation.
 * Details on models available on arXiv:1612.00850. 
 */
class Suep_shower 
{ 
 public:
  
  // Constructor
  Suep_shower(double mass, double temperature, double energy);
  
  /** Generate a shower event, in the rest frame of the showe. */
  std::vector< std::vector <double> > generate_shower();
  
 protected: 
  
  /** Mass of the dark mesons to be generated in the shower */
  double m;
  
  /** Temperature parameter */
  double Temp;
  
  /** Total energy of the system */
  double Etot; 
  
 protected:
  
  /** Maxwell-Boltzman distribution, slightly massaged */
  double f(double p);
  
  /** Derivative of Maxwell-Boltzmann */
  double fp(double p);
  
  /** Test function to be solved for p_plus,p_minus */
  double test_fun(double p);
  
  /** generate one random 4 vector from the thermal distribution */
  std::vector<double> generateFourVector(); 
  
  /** auxiliary function which computes the total energy difference as a function of the 
   * momentum vectors and a scale factor "a" to balance energy */
  double reballance_func(double a, const std::vector< std::vector <double> > &event);
  
 private:
  /** mass/Temperature ratio */
  double A;
  /** convenience function of mass/Temperature ratio */
  double p_m;
  /** solutions for shower generation. See paper for details. */
  double p_plus, p_minus;
  /** solutions in Lambda for shower generation */
  double lambda_plus,lambda_minus;
  /** solutions for shower generation. See paper for details. */
  double q_plus, q_minus, q_m;
  
}; 

#endif
