/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BIPOLARFIT_H
#define BIPOLARFIT_H
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
// this version : 27/7/2007
//////////////////////////////////////////////////////
#include<math.h>
//#include"CLHEP/Matrix/Matrix.h"
//#include"CLHEP/Matrix/SymMatrix.h"
class BipolarFit
{
 public:
  BipolarFit();
  ~BipolarFit();
  int Fit(double *x,const double ex,const double pedestal, const double predefinedwidth,double *result,double *errors,double *chi2);

 private:
  void InvertMatrix(double matrix[][3],const int dim,int*);
  void InvertSymmetric4x4(double W[][4]);
  double FindInitValues(double*x,double *initValues,int *maxsample);
  int TheFitter(double*x,const double ex,double *initValues, int imeas, int *meas, int ipar, int *par,double *chi2,double *result);
  //void Derivative(HepMatrix&,HepMatrix&,HepMatrix&,bool*usemeas);
  void Derivative(double A[][3],double fp[][1], double p0[][1],int imeas, int*meas);
  //double dFdz(double,double *);
  double bipolar(double*, double*);
  double FindPow(double z);

  double m_n;
  double m_powcachez;
  double m_powcachezn;
  double m_zmax;
  double m_bipolarNormalization;
  double m_tsampling;
};
#endif
