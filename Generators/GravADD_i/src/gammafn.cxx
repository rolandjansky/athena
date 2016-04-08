/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GravADD_i/gammafn.h"
#include <math.h>
#include <gsl/gsl_sf.h>


//#include <include/gsl/gsl_sf.h>


double gammafn_(double * x) {
  return tgamma(*x);

}

float fgammafn_(float * x) {
  return tgammaf(*x);
 
}

double igamma_(double * a, double * x) {
  //return 0.01;
/* Non-normalized Incomplete Gamma Function
 *
 * Gamma(a,x) := Integral[ t^(a-1) e^(-t), {t,x,Infinity} ]
 *
 * x >= 0.0
 *   Gamma(a, 0) := Gamma(a)
 *
 * exceptions: GSL_EDOM
 */ 
 return gsl_sf_gamma_inc(*a, *x);
 //return (ROOT::Math::inc_gamma_c(*a, *x))*(ROOT::Math::tgamma(*a) );
}
