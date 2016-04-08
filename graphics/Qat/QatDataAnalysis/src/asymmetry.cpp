/*------------------------------------------------------------------------*/
/*                                                                        */
/* Routine to calculate error bars for efficiency plots                   */
/*                                                                        */
/* Joe Boudreau October 1994                                              */
/*                                                                        */
/*------------------------------------------------------------------------*/
#include "QatDataAnalysis/asymmetry.h"
#include <cmath>
double ConfidenceLevel(int M, int N, double x);
double ConfidenceLevel(int M, int N, double x);
double Binomial(int M, int N);
double xsearch(int M, int N, double cl);

int asymmetry (int M, int N, double & xmean, double & xplus, double & xminus) {

  // No statistics, no information
  
  if (M==0) return ASYMM_ERROR;

  // Decide which whether to "flip" the problem for efficiency

  int invert=0;
  if (N>(M+1)/2) {
    N = M-N;
    invert = true;
  }

  // Find the confidence level of the mean

  double cmean,cplus,cminus; 
  xmean = double(N)/double (M);
  if (M-N>25) {
    double p = double(N)/double(M), q= 1.0-p;
    xplus = xmean+sqrt(p*q/M);
    xminus= xmean-sqrt(p*q/M);
  }
  else {
    if (N==M)       cmean=1.0; 
    else if (N==0)  cmean=0.0;
    else            cmean=ConfidenceLevel(M,N,xmean); 

    // Search of upper and lower confidence intervals

    const double OneStd=0.6827;
    if (cmean>(1.0-OneStd/2.0)) {
      xplus  = 1.0;
      cminus = 1-OneStd;
      xminus = xsearch(M,N,cminus);
    }
    else if ((cmean<OneStd/2.0)) {
      xminus = 0.0;
      cplus  = OneStd;
      xplus  = xsearch(M,N,cplus);
    }
    else {
      cplus   = cmean + OneStd/2.0;
      cminus  = cmean - OneStd/2.0;
      xplus   = xsearch (M,N,cplus);
      xminus  = xsearch (M,N,cminus);
    }
  }
    // Now if we "flipped" the problem, then flip it back

  if (invert) {
    double xp = xplus, xm=xminus;
    xmean   = 1.0 - xmean;
    xplus   = 1.0 - xm;
    xminus  = 1.0 - xp;
  }

  return ASYMM_NORMAL;
}
  
 
/*------------------------------------------------------------------------*/
/*                                                                        */
/* Search for x when the confidence level of x is given                   */
/*                                                                        */
/* Joe Boudreau October 1994                                              */
/*                                                                        */
/*------------------------------------------------------------------------*/
double xsearch(int M, int N, double cl) {

  double X,interval[2]={0.0,1.0},precision=1.0;
  while (precision>0.00001) {
    X = (interval[0]+interval[1])/2.0;
    precision = (interval[1]-interval[0]);
    if (ConfidenceLevel(M,N,X)<cl){
      interval[0]=X;
    }
    else if (ConfidenceLevel(M,N,X)>cl){
      interval[1]=X;
    }
    else {
      return X;
    }
  }
  return X;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/* Search for x when the confidence level of x is given                   */
/*                                                                        */
/* Joe Boudreau October 1994                                              */
/*                                                                        */
/*------------------------------------------------------------------------*/

double ConfidenceLevel(int M, int N, double x) {
  double CL=0.0;
  if (x<=0.0) {
    return 0.0;
  } 
  else if (CL>=1.0) {
    return 1.0;
  }
  else if (N<(M+1)/2) {
    CL = 1.0;
    for (int i=0;i<=N;i++) CL -= (Binomial(M+1,i)*pow(x,i)*pow(1.0-x,M+1-i));
    return CL;
  }
  else {
    for (int i=N+1;i<=M+1;i++) CL += (Binomial(M+1,i)*pow(x,i)*pow(1.0-x,M+1-i));
    return CL;
  }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/* Binomial coefficients from the C math library                          */
/*                                                                        */
/* Joe Boudreau October 1994                                              */
/*                                                                        */
/*------------------------------------------------------------------------*/

double Binomial(int M, int N){
  return floor(0.5 + exp(lgamma(double(M+1)) -lgamma(double(N+1)) - lgamma(double(M-N+1))));
}

