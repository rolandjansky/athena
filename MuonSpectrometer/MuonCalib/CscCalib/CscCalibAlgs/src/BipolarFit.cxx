/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
// A programme to determine the parameters of 
// a bipolar pulse by chi2 minimization
//
// http://knikolop.home.cern.ch/knikolop/csc/bipolarfit.c
//
// for the function of the bipolar pulse look
// http://positron.ps.uci.edu/~schernau/ROD/SIT/results/cluster1.html
//
// Konstantinos Nikolopoulos
// 23/5/2007
// 
// This is temporary

// this version : 27/7/2007
//////////////////////////////////////////////////////
#include "BipolarFit.h"
#include <stdio.h>
#include <iostream>
BipolarFit::BipolarFit()
{
  m_n=12.;
  m_powcachez = -9999.;
  m_powcachezn= -9999.;
  m_zmax= m_n+1 - std::sqrt(m_n+1.0);
  m_bipolarNormalization = FindPow(m_zmax)*(1-m_zmax/(m_n+1))*std::exp(-m_zmax);
  m_tsampling = 25.;

}

BipolarFit::~BipolarFit()
{}

  double 
BipolarFit::FindPow(double z)
{
  if(std::abs(m_powcachez-z)<1.e-4)
    return m_powcachezn;

  double zpower = z*z*z;
  zpower *= zpower;
  zpower *= zpower; 

  m_powcachez = z;
  m_powcachezn= zpower;
  return zpower;
}

  double 
BipolarFit::FindInitValues(double*x,double *initValues,int *maxsample)
{
  // find maximum sample imax:
  double peakingTime=-99.;        // interpolated peaking time in samples
  double amplitude=-99.;      // interpolated amplitude
  double amin, amax; 	
  int imax = 0;
  const int numSample=4;
  amax = x[0];
  amin = x[0];
  for(int j=1; j<numSample; j++)
  {
    if(amax<x[j])
    {
      amax = x[j];
      imax = j;
    }
    if(amin>x[j]) 
      amin = x[j];
  }

  // calculate peak and amplitude:
  (*maxsample)=imax;
  if((imax==0) || (imax==numSample-1))  // no interpolation possible
  {
    peakingTime = imax;
    amplitude = amax;
  } 
  else  // do parabolic interpolation
  {      
    double a, b, c; // coeffients of parabola y=a*x*x+b*x+c
    a = 0.5*(x[imax+1]+x[imax-1]-2.0*x[imax]);
    b = 0.5*(x[imax+1]-x[imax-1]);
    c = x[imax];

    peakingTime = -0.5*b/a;
    amplitude = a*peakingTime*peakingTime + b*peakingTime + c;
    peakingTime += imax; // it was relative to imax
  }

  initValues[0] = amplitude;
  initValues[1] = peakingTime - m_zmax*initValues[2]/m_tsampling;
  return x[imax];
}

  double 
BipolarFit::bipolar(double *x, double *parm) // the bipolar pulse function
{
  // the bipolar pulse function is
  //
  // z = (x-parm[0])*m_tsampling/parm[3]
  // m_zmax = m_n+1 - sqrt(m_n+1)
  // aa = exp(m_n*log(m_zmax))*(1-m_zmax/(m_n+1))*exp(-m_zmax)
  // parm[0]*exp(m_n*log(z))*(1-z/(m_n+1))*exp(-z)/aa
  //
  // for timing reasons instead of x (ie # of sample) 
  // the z is given
  double z = x[0];


  //const double m_tsampling = 25.;// nsec
  //z=(x[0]-parm[1])*m_tsampling/(parm[2]);
  if(z<0.)
    return 0.;

  return parm[0]*FindPow(z)*(1-z/(m_n+1))*std::exp(-z)/m_bipolarNormalization;
}

  void 
BipolarFit::Derivative(double A[][3],double fp[][1], double p0[][1],int imeas, int*meas)
{
  //calculate the derivatives and the 0th order approximation 
  //around the ADC samplings
  double norm = p0[0][0];
  //double parm[3]={1.,p0[1][0],p0[2][0]};
  for(int i=0;i<imeas;i++)
  {
    int ii = meas[i];
    double z = (ii-p0[1][0])*m_tsampling/p0[2][0];
    double repquant = 0.;
    double dFdzNormalized = 0.;
    if(z>0.)
    {
      repquant = FindPow(z)*std::exp(-z)/m_bipolarNormalization;
      dFdzNormalized= repquant*(m_n/z+z/13.-2.);
    }

    A[ii][0] = repquant*(1.-z/13.);
    //A[ii][0] = bipolar(&z,parm);
    fp[ii][0] = norm * A[ii][0];

    //double normOverZmax = norm/m_bipolarNormalization;
    double commonpart = norm* dFdzNormalized;//(z,parm);
    A[ii][1] = commonpart * (-m_tsampling/p0[2][0]);
    A[ii][2] = commonpart * (-z/p0[2][0]);
  }
  // end of derivative/zeroth order calculations
}

//void BipolarFit::InvertMatrix(HepMatrix& matrix,const int dim,int*correspdim)
void BipolarFit::InvertMatrix(double matrix[][3],const int dim,int*correspdim)
{
  // invert 2x2 or 3x3 symmetric matrix
  if(dim==2)
  {
    int ii=correspdim[0];
    int jj=correspdim[1];
    double determinant= -matrix[jj][ii]*matrix[ii][jj] +matrix[ii][ii]*matrix[jj][jj];
    double i00 = matrix[ii][ii];
    matrix[ii][ii] = matrix[jj][jj]/determinant;
    matrix[jj][jj] = i00/determinant;
    matrix[ii][jj] = -matrix[ii][jj]/determinant;
    matrix[jj][ii] = matrix[ii][jj];

  }
  else if(dim==3)
  {
    double sm12 = matrix[0][1]*matrix[0][1];
    double sm13 = matrix[0][2]*matrix[0][2];
    double sm23 = matrix[1][2]*matrix[1][2];
    double determinant = matrix[0][0]*matrix[1][1]*matrix[2][2]
      -matrix[0][0]*sm23-sm12*matrix[2][2]
      +2.*matrix[0][1]*matrix[0][2]*matrix[1][2]
      -matrix[1][1]*sm13;

    double i00 = matrix[1][1]*matrix[2][2]-sm23;
    double i11 = matrix[0][0]*matrix[2][2]-sm13;
    double i22 = matrix[0][0]*matrix[1][1]-sm12;

    matrix[1][0] = (matrix[0][2]*matrix[1][2]-matrix[2][2]*matrix[0][1])/determinant;
    matrix[2][0] = (matrix[0][1]*matrix[1][2]-matrix[0][2]*matrix[1][1])/determinant;
    matrix[2][1] = (matrix[0][1]*matrix[0][2]-matrix[0][0]*matrix[1][2])/determinant;
    matrix[0][1] = matrix[1][0];
    matrix[0][2] = matrix[2][0];
    matrix[1][2] = matrix[2][1];
    matrix[0][0] = i00/determinant;
    matrix[1][1] = i11/determinant;
    matrix[2][2] = i22/determinant;

  }
  else
  {
    //int ierr;
    //matrix.invert(ierr);
    printf("this is not a 2x2 or 3x3 matrix\n");
  }
}

  int 
BipolarFit::Fit(double *x,const double ex,const double pedestal, const double predefinedwidth,double *result,double * /**errors*/,double *chi2)
{
  /**
Input :
x -> the ADC measurements in the 4 samplings
ex -> the electronic noise in ADC counts
Output:
result -> the result of the fit 
errors -> the errors of the fit parameters
chi2   -> the chi2 of the fit
Returns:
status of the fit

The function return an integer representing different exit status.
--- Bipolar fit tried ---
0 --> Bipolar fit converged OK
1 --> The fit converged but its chi2 is bad
2 --> The fit converged but estimated shaping time very different from expected
3 --> Maximum number of iterations reached OK
4 --> Diverging fit. Return parabola interpolation OK
5 --> One sample saturated. Bipolar fit tried nevertheless
  //6 --> Late pulse the. The constant shaping time fit converged but the full fit didn't. 
  //Return fix shaping time fit results.	   OK
  --- Bipolar fit not tried ---
  6 --> More samples saturated. Return estimate based on parabola interpolation
  7 --> Parabola interpolation indicates amplitude <5sigmaNoise. 
  Return parabola estimated parameters. OK
  8 --> Distorted pulse shape. Fit likely to fail if attempted.parabola interpolation returned
  9 --> Too late pulse. Bipolar fit not attempted, return rough estimate OK
  10 --> It seems that a reasonable pulse exist, but the fit gave substantially lower value than the parabola. return the parabola (which is kind of lower bound for normal pulses).
   **/

  // initial parameter estimates using parabola interpolation
  double initValues[3]={0.};
  int imax = -1;
  initValues[2]=predefinedwidth;
  double samplemax = FindInitValues(x,initValues,&imax);
  result[0] = initValues[0];
  result[1] = initValues[1];
  result[2] = initValues[2];

  // do not fit noise
  if(samplemax < 5.*ex)
    return 7;

  bool onesaturated = false;
  if(imax==0)
  {
    if(x[imax+1]<0. && x[imax+2]<0.)
      return 8;
  }
  else if(imax==3)
  {
    // don't fit too late pulses
    if(initValues[1]+m_zmax*initValues[2]/m_tsampling>2.75)
      return 9; 
  }
  else
  {
    if(x[imax-1]<0. && (x[imax+1]<2. || -x[imax-1]-x[imax]>0.))
      return 8;
    if(x[imax]+pedestal>4000.)
    {
      if(x[imax-1]+pedestal>4000. || x[imax+1]+pedestal>4000.)
        return 6;
      else
      {
        onesaturated=true;
      }
    }  
  }
  //always fix width and fit two parameters
  bool fitpar[3] = {true,true,false};
  bool usemeas[4] = {true,true,true,true};
  if(initValues[1]+m_zmax*initValues[2]/m_tsampling<2.0)
  {
    fitpar[2] = false;
    usemeas[3]= false;
  }
  //if(initValues[1]>0.)
  //{
  //  usemeas[0]= false;
  //  fitpar[2] = false;
  //}

  if(onesaturated)
  {
    usemeas[imax]= false;
    fitpar[2] = false; 
  }
  int imeas =0;
  int meas[4] = {0};
  for(int i=0;i<4;i++)
    if(usemeas[i])
    {
      meas[imeas]=i;
      imeas++;
    }
  int ipar =0;
  int par[3] = {0};
  for(int i=0;i<3;i++)
    if(fitpar[i])
    {
      par[ipar]=i;
      ipar++;
    }  

  int FitStatus = TheFitter(x,ex,initValues,imeas,meas,ipar,par,chi2,result);
  // the parabola interpolated estimate is most of the time a lower bound (for high pulses)!
  if(result[0]> 10.*ex && result[0]<0.90*initValues[0])
  {
    result[0] = initValues[0];
    return 10;
  }

  if(onesaturated)
    return 5;


  return FitStatus;
}

  void 
BipolarFit::InvertSymmetric4x4(double W[][4])
{
  double Determinant = W[0][3]*W[0][3]*W[1][2]*W[1][2] - 2.*W[0][2]*W[0][3]*W[1][2]*W[1][3]
    +W[0][2]*W[0][2]*W[1][3]*W[1][3]-W[0][3]*W[0][3]*W[1][1]*W[2][2] + 2.*W[0][1]*W[0][3]*W[1][3]*W[2][2] - W[0][0]*W[1][3]*W[1][3]*W[2][2]+2.*W[0][2]*W[0][3]*W[1][1]*W[2][3]
    - 2.*W[0][1]*W[0][3]*W[1][2]*W[2][3] - 2.*W[0][1]*W[0][2]*W[1][3]*W[2][3]+2.*W[0][0]*W[1][2]*W[1][3]*W[2][3]+W[0][1]*W[0][1]*W[2][3]*W[2][3]-W[0][0]*W[1][1]*W[2][3]*W[2][3]-W[0][2]*W[0][2]*W[1][1]*W[3][3]+2.*W[0][1]*W[0][2]*W[1][2]*W[3][3]-W[0][0]*W[1][2]*W[1][2]*W[3][3]-W[0][1]*W[0][1]*W[2][2]*W[3][3]+W[0][0]*W[1][1]*W[2][2]*W[3][3];

  W[0][1] = W[3][0]*W[3][1]*W[2][2]-W[3][0]*W[2][1]*W[3][2] - W[2][0]*W[3][1]*W[3][2]+W[1][0]*W[3][2]*W[3][2]+W[2][0]*W[2][1]*W[3][3]-W[1][0]*W[2][2]*W[3][3];
  W[0][2] = -W[3][0]*W[2][1]*W[3][1]+W[2][0]*W[3][1]*W[3][1]+W[3][0]*W[1][1]*W[3][2]-W[1][0]*W[3][1]*W[3][2]-W[2][0]*W[1][1]*W[3][3]+W[1][0]*W[2][1]*W[3][3];
  W[0][3] = W[3][0]*W[2][1]*W[2][1]-W[2][0]*W[2][1]*W[3][1]-W[3][0]*W[1][1]*W[2][2]+W[1][0]*W[3][1]*W[2][2]+W[2][0]*W[1][1]*W[3][2]-W[1][0]*W[2][1]*W[3][2];
  W[1][2] = W[3][0]*W[3][0]*W[2][1]-W[2][0]*W[3][0]*W[3][1]-W[1][0]*W[3][0]*W[3][2]+W[0][0]*W[3][1]*W[3][2]+W[1][0]*W[2][0]*W[3][3]-W[0][0]*W[2][1]*W[3][3];

  W[1][3] = -W[2][0]*W[3][0]*W[2][1]+W[2][0]*W[2][0]*W[3][1]+W[1][0]*W[3][0]*W[2][2]-W[0][0]*W[3][1]*W[2][2]-W[1][0]*W[2][0]*W[3][2]+W[0][0]*W[2][1]*W[3][2];
  W[2][3] = W[2][0]*W[3][0]*W[1][1]-W[1][0]*W[3][0]*W[2][1]-W[1][0]*W[2][0]*W[3][1]+W[0][0]*W[2][1]*W[3][1]+W[1][0]*W[1][0]*W[3][2]-W[0][0]*W[1][1]*W[3][2];

  double W00 = -W[3][1]*W[3][1]*W[2][2]+2.*W[2][1]*W[3][1]*W[3][2]-W[1][1]*W[3][2]*W[3][2]-W[2][1]*W[2][1]*W[3][3]+W[1][1]*W[2][2]*W[3][3];
  double W11 = -W[3][0]*W[3][0]*W[2][2]+2.*W[2][0]*W[3][0]*W[3][2]-W[0][0]*W[3][2]*W[3][2]-W[2][0]*W[2][0]*W[3][3]+W[0][0]*W[2][2]*W[3][3];
  double W22 = -W[3][0]*W[3][0]*W[1][1]+2.*W[1][0]*W[3][0]*W[3][1]-W[0][0]*W[3][1]*W[3][1]-W[1][0]*W[1][0]*W[3][3]+W[0][0]*W[1][1]*W[3][3];
  double W33 = -W[2][0]*W[2][0]*W[1][1]+2.*W[1][0]*W[2][0]*W[2][1]-W[0][0]*W[2][1]*W[2][1]-W[1][0]*W[1][0]*W[2][2]+W[0][0]*W[1][1]*W[2][2];

  for(int i=0;i<3;i++)
  {
    for(int j=1;j<4;j++)
    {
      if(i>=j)
        continue;
      W[i][j] = W[i][j]/Determinant;
      W[j][i] = W[i][j];
    }
  }
  W[0][0] = W00/Determinant;
  W[1][1] = W11/Determinant;
  W[2][2] = W22/Determinant;
  W[3][3] = W33/Determinant;
}

  int
BipolarFit::TheFitter(double*x,const double ex,double *initValues, int imeas, int *meas, int ipar, int *par,double *chi2,double *result)
{
  // maximum iterations
  const int maxIter = 7;
  // tolerances
  double fitTolerance0 = 0.10;
  double fitTolerance1 = 0.01;
  //HepMatrix p0(3,1,0); // the matrix of the initial fit parameters 
  double p0[3][1]={{0.}};
  for(int j=0;j<3;j++)
    p0[j][0] = initValues[j];

  //HepMatrix m(4,1,0); // the matrix of ADC measurements (samples: 0,1,2,3)
  double m[4][1]={{0.}};
  //HepMatrix W(4,4,0); // the error matrix of the ADC measurements 
  double W[4][4]={{0.}};
  for(int i=0;i<4;i++)
  {
    m[i][0] = x[i];
    W[i][i] = ex*ex;
  }
  // covariances 
  W[0][1] =  0.03*ex*ex;
  W[0][2] = -0.411*ex*ex;
  W[0][3] = -0.188*ex*ex;
  W[1][2] =  0.0275*ex*ex;
  W[1][3] = -0.4303*ex*ex;
  W[2][3] =  0.*ex*ex;
  W[1][0] = W[0][1];
  W[2][0] = W[0][2];
  W[3][0] = W[0][3];
  W[2][1] = W[1][2];
  W[3][1] = W[1][3];
  W[3][2] = W[2][3]; 

  //WW.invert(ierr);
  InvertSymmetric4x4(W);

  // Taylor expansion of the bipolar pulse model around the
  // samplings : F(x) = F(p0) + A *(p-p0) + higher.order
  //HepMatrix fp(4,1,0); // the matrix of 0th order approximation 
  double fp[4][1]={{0.}};
  //HepMatrix A(4,3,0);  // the matrix of derivatives 
  double A[4][3]={{0.}};
  // remarks : 
  // if the pulse peaks in the last sampling fit with a constant shaping time
  // if the pulse peaks in the first sampling fit without using the last sampling 
  // (too large contribution to the chi2
  int counter=0;
  bool converged=false;
  double amplitudeChangeOld = 0.;
  bool diverganceCandidate = false;
  //HepMatrix weight(3,3,1); // weight matrix allocated once
  // the non-fitted parts are taken care appropriately
  // at least if the fitting parameters or measurements
  // don't change during the fitting procedure
  double weight[3][3]={{0.}};
  weight[0][0]=1.;
  weight[1][1]=1.;
  weight[2][2]=1.;

  //HepMatrix residFactor(3,4,0); // residFactor allocated once
  double residFactor[3][4]={{0.}};
  while(!converged && counter<maxIter) // fit loop
  {
    Derivative(A,fp,p0,imeas,meas);// calculate the matrix of derivatives and 0th order approximation
    // matrix multiplication
    // the weight matrix is symmetric
    // weight= A.T()*W*A;//.assign(A.T()*W*A);

    double helpmatrix[4][3]={{0.}};
    for(int i=0;i<imeas;i++)
    {
      int ii=meas[i];
      for(int j=0;j<ipar;j++)
      {
        int jj=par[j];
        for(int k=0;k<imeas;k++)
        {
          int kk=meas[k];
          helpmatrix[ii][jj] += W[ii][kk]*A[kk][jj];
        }
      }
    }
    for(int i=0;i<ipar;i++)
    {	
      int ii=par[i];
      for(int j=i;j<ipar;j++)
      {
        int jj=par[j];
        weight[ii][jj] = 0.; 
        for(int k=0;k<imeas;k++)
        {
          int kk=meas[k];
          weight[ii][jj] += A[kk][ii]*helpmatrix[kk][jj];//A[kk][ii]*A[kk][jj];
        }
        //weight[ii][jj]*=W[0][0];
        weight[jj][ii] =weight[ii][jj];
      }
    }
    //weight.invert(ierr); // inversion of weight matrix
    // hand-made inversion of 2x2 or 3x3 symmetric matrix
    InvertMatrix(weight,ipar,par);

    //calculate W*(A.T()*W)
    //residFactor = weight*(A.T()*W);
    double helpmatrix2[3][4]={{0.}};
    for(int i=0;i<ipar;i++)
    {
      int ii=par[i];
      for(int j=0;j<imeas;j++)
      {
        int jj=meas[j];
        for(int k=0;k<imeas;k++)
        {
          int kk=meas[k];
          helpmatrix2[ii][jj] += A[kk][ii] * W[kk][jj];
        }
      }
    }

    for(int i=0;i<ipar;i++)
    {	
      int ii = par[i];
      for(int j=0;j<imeas;j++)
      {
        int jj=meas[j];
        residFactor[ii][jj]=0.;
        for(int k=0;k<ipar;k++)
        {
          int kk=par[k];
          residFactor[ii][jj] += weight[ii][kk]*helpmatrix2[kk][jj];
        }
        //residFactor[ii][jj]*=W[0][0];
      }
    }

    double paramDiff[3] = {0.};
    for(int i=0;i<ipar;i++)
    {
      int ii=par[i];
      //estimation of new parameters
      //paramDiff[i][0] += (weight*(A.T()*W)*(m-fp))[i][0];
      for(int j=0;j<imeas;j++)
      {
        int jj = meas[j];
        paramDiff[ii] += residFactor[ii][jj]*(m[jj][0]-fp[jj][0]);
      }
      p0[ii][0] += paramDiff[ii];
    }
    // if the parameters are going nuts keep them sensible
    if(p0[1][0]>1. || p0[1][0]<-3.)
      p0[1][0] = initValues[1];

    double amplitudeChangeNew = std::abs(paramDiff[0]);
    if(std::abs(paramDiff[0])<fitTolerance0 && std::abs(paramDiff[1])<fitTolerance1)
    {
      converged = true;
      // calculate chi2
      double residual[4]= {0.};
      for(int i=0;i<imeas;i++)
      {
        int ii=meas[i];
        residual[i] = m[ii][0]-fp[ii][0];
      }

      double tmpChi2 = 0.;
      double helpmatrixchi2[4][1]={{0.}};
      for(int i=0;i<imeas;i++)
      {
        int ii=meas[i];
        for(int k=0;k<imeas;k++)
        {
          int kk=meas[k];
          helpmatrixchi2[ii][0] += W[ii][kk]*residual[kk];
        }
      }
      for(int k=0;k<imeas;k++)
      {
        int kk=meas[k];
        tmpChi2 += residual[kk]*helpmatrixchi2[kk][0];
      }
      (*chi2) = tmpChi2;
    }
    else if(counter>4 && (amplitudeChangeNew>4.*amplitudeChangeOld))
    {
      if(diverganceCandidate)
      {
        //diverging fit
        //return parabola interpolation
        printf("Diverging fit\n");
        return 4;
      }
      else
        diverganceCandidate = true;
    }
    if(p0[0][0]<0.)
    {
      //negative amplitude
      //fit diverged
      // return parabola
      return 4;
    }
    //if after a couple of iterations the amplitude is low
    // reduce the tolerances (or the maximum iterations) 
    // low amplitude pulses tend to oscillate and exhaust all iterations
    if(p0[0][0]<20.)
    {
      fitTolerance0 = 0.1;
      fitTolerance1 = 0.05;
    }
    amplitudeChangeOld = amplitudeChangeNew;
    counter++;
  }
  result[0]=p0[0][0];
  result[1]=m_zmax*p0[2][0]/m_tsampling+p0[1][0];
  result[2]=p0[2][0];

  if(counter==maxIter)
    return 3;
  return 0;
}
