/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// I compile this file using 
// gcc -c Aeik.c
// Link with fortran whilde compiling fortran-program
// g77 -lgsl -lgslcblas Aeik.o LitetFortran.f


#include <stdio.h>
#include <math.h>
// These have to be altered
#include <gsl/gsl_sf.h>
#include <gsl/gsl_integration.h>


/*void myerrorhandler (const char * reason,
		     const char * file,
		     int line,
		     int gsl_errno) {
  static int n = 0;
  ++n;
  if ( n <= 10 || n == 100 || n == 1000 || n == 10000 )
    printf("myerrorhandler %d: (%s) %s\n", n, file, reason);
    }*/



// This defines the integrand, with parameters
double f (double b, void * params) {
  double *pVec = (double *) params;
  double shat = pVec[0];
  double k = pVec[1];
  double Ms = pVec[2];
  double MD = pVec[3];
  double n = pVec[4];
  double Pi=3.14159;
  double Xconst=-(pow(MD,-2 - n)*pow(Ms,n)*pow(Pi,-1 + n/2.)*shat*
  		  exp(gsl_sf_lngamma(n/2.))/8.);
  //  printf("Xconst = %e.\Xconst");
  double f = 2.0*Pi*b*gsl_sf_bessel_J0(b*k)*(1.0-exp(Xconst*gsl_sf_hyperg_U(n/2.0, 1.0, Ms*Ms*b*b/4.0)));
  return f;
}

// This procedure does the actual interation
double aeik_(double *shat, double *k, double *Ms, double *MD,
	     int *n, double * bmin)
{
  //Changeing epsrel helped more for errors
  double result, error;    // To be returned
  double epsabs = 1.0e-10;  //Allowed absolute error
  double epsrel = 0.000001;   //Allowed relative error
  size_t subintervals = 1000000;
  
  // gsl_set_error_handler(&myerrorhandler);

  //  printf("c-function aeik_ invoked with arguments\n shat = %e, k = %e, Ms = %e, MD = %e, n = %e.\n", *shat, *k, *Ms, *MD,(double) *n);
  
  gsl_function F;
  F.function = &f;
  double pVec[5];
  pVec[0] = *shat;
  pVec[1] = *k;
  pVec[2] = *Ms;
  pVec[3] = *MD;
  pVec[4] = (double) *n;

  F.params = pVec; /* our integrand contains parameters */
  
  gsl_integration_workspace * ws  // memory space for use by the interator
    = gsl_integration_workspace_alloc (subintervals);
  
  // using gsl function for integrals over intervals of form (a,infinity)
  gsl_integration_qagiu (	&F, /* The interand */
				*bmin, /* integral from 0 to infinity */
                        	epsabs,  /* acceptable absolute error */
                        	epsrel, /* acceptable relative error */  
                        	subintervals, /* Maximum number of subintervals */
                        	ws, 	/* workspace to use during integration */
                        	&result, /* returns result here */
                        	&error /* returns error estimate here */
                        	);  
  double integral=result*2.0* *shat;
  //  printf("The result is %e with an estimated error bound of %e\n", result, error);

  gsl_integration_workspace_free (ws);

  return integral;
}


