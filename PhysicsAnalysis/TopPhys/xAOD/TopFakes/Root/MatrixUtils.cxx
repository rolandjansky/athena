/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Generic matrix methods fake lepton estimates for lepton and dilepton.
//
// Given loose/tight lepton ID, real efficiency r,
// and fake efficiency f builds the fake estimate event weight
// 
// Authors:
//   Karl Gellerstedt (kargel@fysik.su.se)
//   Jorgen Sjolin (sjolin@fysik.su.se)
//

#include "TopFakes/MatrixUtils.h"
#include <iostream>
using namespace std;

MatrixUtils::MatrixUtils() :
  ljets_matrix(2,2),
  ljets_matrix_top_corr(2,2),
  ll_matrix(4,4),
  ll_matrix_top_corr(4,4),
  lll_matrix(8,8),
  i_ljets_matrix(2,2),
  i_ljets_matrix_top_corr(2,2),
  i_ll_matrix(4,4),
  i_ll_matrix_top_corr(4,4),
  i_lll_matrix(8,8)
{

}

double MatrixUtils::lepton_weight(bool loose, double r, double f,
				  double &nf, double & /*nr*/) {
  // fix to not return NaN
  if(r == 0 && f == 0) 
    return nf;

  double nl=loose ? 1 :0;
  double nt=!loose ? 1 :0;
  double a=1./(r-f);
  nf=a*f*r*nl + a*f*(r - 1.)*nt;
  if(nf!=nf) nf = 0.;  // protect against "nan"
  return nf;
}

void MatrixUtils::fill_matrix(double r, double f)
{
  ljets_matrix[0][0] = r;
  ljets_matrix[0][1] = f;
  ljets_matrix[1][0] = (1. - r);
  ljets_matrix[1][1] = (1. - f);
  i_ljets_matrix = ljets_matrix;
  i_ljets_matrix.InvertFast();

  ljets_matrix_top_corr[0][0] = r;
  ljets_matrix_top_corr[0][1] = f;
  ljets_matrix_top_corr[1][0] = (1. - r);
  ljets_matrix_top_corr[1][1] = (1. - f);  
  i_ljets_matrix_top_corr = ljets_matrix_top_corr;
  i_ljets_matrix_top_corr.InvertFast();
}

void MatrixUtils::fill_matrix(double r, double f, double t)
{
  ljets_matrix[0][0] = r;
  ljets_matrix[0][1] = f;
  ljets_matrix[1][0] = (1. - r);
  ljets_matrix[1][1] = (1. - f);
  i_ljets_matrix = ljets_matrix;
  i_ljets_matrix.InvertFast();

  ljets_matrix_top_corr[0][0] = r*t;
  ljets_matrix_top_corr[0][1] = f;
  ljets_matrix_top_corr[1][0] = (1. - r*t);
  ljets_matrix_top_corr[1][1] = (1. - f);  
  i_ljets_matrix_top_corr = ljets_matrix_top_corr;
  i_ljets_matrix_top_corr.InvertFast();
}

double MatrixUtils::lepton_tight_weight(bool loose)
{
  double nl=loose ? 1 :0;
  double nt=!loose ? 1 :0;
  double nf=i_ljets_matrix[1][0]*nt + i_ljets_matrix[1][1]*nl;
  double nf_tight=ljets_matrix[0][1]*nf;
  return nf_tight;
}

double MatrixUtils::lepton_loose_weight(bool loose)
{
  double nl=loose ? 1 :0;
  double nt=!loose ? 1 :0;
  double nf=i_ljets_matrix[1][0]*nt + i_ljets_matrix[1][1]*nl;
  double nf_loose=ljets_matrix[1][1]*nf;
  return nf_loose;
}

double MatrixUtils::lepton_tight_top_corr_weight(bool loose)
{
  double nl=loose ? 1 :0;
  double nt=!loose ? 1 :0;
  double nf=i_ljets_matrix_top_corr[1][0]*nt + i_ljets_matrix_top_corr[1][1]*nl;
  double nf_tight=ljets_matrix_top_corr[0][1]*nf;
  return nf_tight;  
}

double MatrixUtils::lepton_loose_top_corr_weight(bool loose)
{
  double nl=loose ? 1 :0;
  double nt=!loose ? 1 :0;
  double nf=i_ljets_matrix_top_corr[1][0]*nt + i_ljets_matrix_top_corr[1][1]*nl;
  double nf_tight=ljets_matrix_top_corr[0][1]*nf;
  return nf_tight;
}


double MatrixUtils::dilepton_weight(bool l1, double r1, double f1,
				    bool l2, double r2, double f2,
				    double &nrf, double &nfr,
				    double &nff) { 
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double a=1./((r1-f1)*(r2-f2));

  nrf= a*r1*f2*(   -(1.-f1)*(1.-r2)*ntt    +    (1.-f1)*r2*ntl    +    f1*(1.-r2)*nlt    -    f1*r2*nll    );

  nfr= a*f1*r2*(   -(1.-r1)*(1.-f2)*ntt    +    (1.-r1)*f2*ntl    +    r1*(1.-f2)*nlt    -    r1*f2*nll    );

  nff= a*f1*f2*(   +(1.-r1)*(1.-r2)*ntt    -    (1.-r1)*r2*ntl    -    r1*(1.-r2)*nlt    +    r1*r2*nll    );
  
  if(nrf!=nrf) nrf = 0.;  // protect against "nan"
  if(nfr!=nfr) nfr = 0.;  // protect against "nan"
  if(nff!=nff) nff = 0.;  // protect against "nan"
  return nrf+nfr+nff;
}

void MatrixUtils::fill_matrix(double r1, double f1, double r2, double f2)
{
  ll_matrix[0][0] = r1 * r2;
  ll_matrix[0][1] = r1 * f2;
  ll_matrix[0][2] = f1 * r2;
  ll_matrix[0][3] = f1 * f2;
  ll_matrix[1][0] = r1 * (1. - r2);
  ll_matrix[1][1] = r1 * (1. - f2);
  ll_matrix[1][2] = f1 * (1. - r2);
  ll_matrix[1][3] = f1 * (1. - f2);
  ll_matrix[2][0] = (1. - r1) * r2;
  ll_matrix[2][1] = (1. - r1) * f2;
  ll_matrix[2][2] = (1. - f1) * r2;
  ll_matrix[2][3] = (1. - f1) * f2;
  ll_matrix[3][0] = (1. - r1) * (1. - r2);
  ll_matrix[3][1] = (1. - r1) * (1. - f2);
  ll_matrix[3][2] = (1. - f1) * (1. - r2);
  ll_matrix[3][3] = (1. - f1) * (1. - f2);
  i_ll_matrix = ll_matrix;
  i_ll_matrix.InvertFast();

  ll_matrix_top_corr[0][0] = r1 * r2; // RR
  ll_matrix_top_corr[0][1] = r1 * f2;
  ll_matrix_top_corr[0][2] = f1 * r2;
  ll_matrix_top_corr[0][3] = f1 * f2;
  ll_matrix_top_corr[1][0] = r1 * (1. - r2); // RR
  ll_matrix_top_corr[1][1] = r1 * (1. - f2);
  ll_matrix_top_corr[1][2] = f1 * (1. - r2);
  ll_matrix_top_corr[1][3] = f1 * (1. - f2);
  ll_matrix_top_corr[2][0] = (1. - r1) * r2; // RR
  ll_matrix_top_corr[2][1] = (1. - r1) * f2;
  ll_matrix_top_corr[2][2] = (1. - f1) * r2;
  ll_matrix_top_corr[2][3] = (1. - f1) * f2;
  ll_matrix_top_corr[3][0] = (1. - r1) * (1. - r2); // RR
  ll_matrix_top_corr[3][1] = (1. - r1) * (1. - f2);
  ll_matrix_top_corr[3][2] = (1. - f1) * (1. - r2);
  ll_matrix_top_corr[3][3] = (1. - f1) * (1. - f2);
  i_ll_matrix_top_corr = ll_matrix_top_corr;
  i_ll_matrix_top_corr.InvertFast();
}

void MatrixUtils::fill_matrix(double r1, double f1, double t1, double r2, double f2, double t2)
{
  ll_matrix[0][0] = r1 * r2;
  ll_matrix[0][1] = r1 * f2;
  ll_matrix[0][2] = f1 * r2;
  ll_matrix[0][3] = f1 * f2;
  ll_matrix[1][0] = r1 * (1. - r2);
  ll_matrix[1][1] = r1 * (1. - f2);
  ll_matrix[1][2] = f1 * (1. - r2);
  ll_matrix[1][3] = f1 * (1. - f2);
  ll_matrix[2][0] = (1. - r1) * r2;
  ll_matrix[2][1] = (1. - r1) * f2;
  ll_matrix[2][2] = (1. - f1) * r2;
  ll_matrix[2][3] = (1. - f1) * f2;
  ll_matrix[3][0] = (1. - r1) * (1. - r2);
  ll_matrix[3][1] = (1. - r1) * (1. - f2);
  ll_matrix[3][2] = (1. - f1) * (1. - r2);
  ll_matrix[3][3] = (1. - f1) * (1. - f2);
  i_ll_matrix = ll_matrix;
  i_ll_matrix.InvertFast();

  // This is to fill and invert the matrix using the correction applied only to the real/real column
  ll_matrix_top_corr[0][0] = r1 * t1 * r2 * t2; // RR
  ll_matrix_top_corr[0][1] = r1 * f2;
  ll_matrix_top_corr[0][2] = f1 * r2;
  ll_matrix_top_corr[0][3] = f1 * f2;
  ll_matrix_top_corr[1][0] = r1 * t1 * (1. - r2 * t2); // RR
  ll_matrix_top_corr[1][1] = r1 * (1. - f2);
  ll_matrix_top_corr[1][2] = f1 * (1. - r2);
  ll_matrix_top_corr[1][3] = f1 * (1. - f2);
  ll_matrix_top_corr[2][0] = (1. - r1 * t1) * r2 * t2; // RR
  ll_matrix_top_corr[2][1] = (1. - r1) * f2;
  ll_matrix_top_corr[2][2] = (1. - f1) * r2;
  ll_matrix_top_corr[2][3] = (1. - f1) * f2;
  ll_matrix_top_corr[3][0] = (1. - r1 * t1) * (1. - r2 * t2); // RR
  ll_matrix_top_corr[3][1] = (1. - r1) * (1. - f2);
  ll_matrix_top_corr[3][2] = (1. - f1) * (1. - r2);
  ll_matrix_top_corr[3][3] = (1. - f1) * (1. - f2);
  i_ll_matrix_top_corr = ll_matrix_top_corr;
  i_ll_matrix_top_corr.InvertFast();
}

double MatrixUtils::dilepton_TT_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix[1][0] * ntt + i_ll_matrix[1][1] * ntl + i_ll_matrix[1][2] * nlt + i_ll_matrix[1][3] * nll;
  double nfr = i_ll_matrix[2][0] * ntt + i_ll_matrix[2][1] * ntl + i_ll_matrix[2][2] * nlt + i_ll_matrix[2][3] * nll;
  double nff = i_ll_matrix[3][0] * ntt + i_ll_matrix[3][1] * ntl + i_ll_matrix[3][2] * nlt + i_ll_matrix[3][3] * nll;

  double nrf_TT = ll_matrix[0][1] * nrf;
  double nfr_TT = ll_matrix[0][2] * nfr;
  double nff_TT = ll_matrix[0][3] * nff;

  return nrf_TT + nfr_TT + nff_TT;
}

double MatrixUtils::dilepton_TL_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix[1][0] * ntt + i_ll_matrix[1][1] * ntl + i_ll_matrix[1][2] * nlt + i_ll_matrix[1][3] * nll;
  double nfr = i_ll_matrix[2][0] * ntt + i_ll_matrix[2][1] * ntl + i_ll_matrix[2][2] * nlt + i_ll_matrix[2][3] * nll;
  double nff = i_ll_matrix[3][0] * ntt + i_ll_matrix[3][1] * ntl + i_ll_matrix[3][2] * nlt + i_ll_matrix[3][3] * nll;

  double nrf_TL = ll_matrix[1][1] * nrf;
  double nfr_TL = ll_matrix[1][2] * nfr;
  double nff_TL = ll_matrix[1][3] * nff;

  return nrf_TL + nfr_TL + nff_TL;
}

double MatrixUtils::dilepton_LT_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix[1][0] * ntt + i_ll_matrix[1][1] * ntl + i_ll_matrix[1][2] * nlt + i_ll_matrix[1][3] * nll;
  double nfr = i_ll_matrix[2][0] * ntt + i_ll_matrix[2][1] * ntl + i_ll_matrix[2][2] * nlt + i_ll_matrix[2][3] * nll;
  double nff = i_ll_matrix[3][0] * ntt + i_ll_matrix[3][1] * ntl + i_ll_matrix[3][2] * nlt + i_ll_matrix[3][3] * nll;

  double nrf_LT = ll_matrix[2][1] * nrf;
  double nfr_LT = ll_matrix[2][2] * nfr;
  double nff_LT = ll_matrix[2][3] * nff;

  return nrf_LT + nfr_LT + nff_LT;
}

double MatrixUtils::dilepton_LL_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix[1][0] * ntt + i_ll_matrix[1][1] * ntl + i_ll_matrix[1][2] * nlt + i_ll_matrix[1][3] * nll;
  double nfr = i_ll_matrix[2][0] * ntt + i_ll_matrix[2][1] * ntl + i_ll_matrix[2][2] * nlt + i_ll_matrix[2][3] * nll;
  double nff = i_ll_matrix[3][0] * ntt + i_ll_matrix[3][1] * ntl + i_ll_matrix[3][2] * nlt + i_ll_matrix[3][3] * nll;

  double nrf_LL = ll_matrix[3][1] * nrf;
  double nfr_LL = ll_matrix[3][2] * nfr;
  double nff_LL = ll_matrix[3][3] * nff;

  return nrf_LL + nfr_LL + nff_LL;
}

double MatrixUtils::dilepton_TT_top_corr_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix_top_corr[1][0] * ntt + i_ll_matrix_top_corr[1][1] * ntl + i_ll_matrix_top_corr[1][2] * nlt + i_ll_matrix_top_corr[1][3] * nll;
  double nfr = i_ll_matrix_top_corr[2][0] * ntt + i_ll_matrix_top_corr[2][1] * ntl + i_ll_matrix_top_corr[2][2] * nlt + i_ll_matrix_top_corr[2][3] * nll;
  double nff = i_ll_matrix_top_corr[3][0] * ntt + i_ll_matrix_top_corr[3][1] * ntl + i_ll_matrix_top_corr[3][2] * nlt + i_ll_matrix_top_corr[3][3] * nll;

  double nrf_TT = ll_matrix_top_corr[0][1] * nrf;
  double nfr_TT = ll_matrix_top_corr[0][2] * nfr;
  double nff_TT = ll_matrix_top_corr[0][3] * nff;

  return nrf_TT + nfr_TT + nff_TT;
}

double MatrixUtils::dilepton_TL_top_corr_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix_top_corr[1][0] * ntt + i_ll_matrix_top_corr[1][1] * ntl + i_ll_matrix_top_corr[1][2] * nlt + i_ll_matrix_top_corr[1][3] * nll;
  double nfr = i_ll_matrix_top_corr[2][0] * ntt + i_ll_matrix_top_corr[2][1] * ntl + i_ll_matrix_top_corr[2][2] * nlt + i_ll_matrix_top_corr[2][3] * nll;
  double nff = i_ll_matrix_top_corr[3][0] * ntt + i_ll_matrix_top_corr[3][1] * ntl + i_ll_matrix_top_corr[3][2] * nlt + i_ll_matrix_top_corr[3][3] * nll;

  double nrf_TL = ll_matrix_top_corr[1][1] * nrf;
  double nfr_TL = ll_matrix_top_corr[1][2] * nfr;
  double nff_TL = ll_matrix_top_corr[1][3] * nff;

  return nrf_TL + nfr_TL + nff_TL;
}

double MatrixUtils::dilepton_LT_top_corr_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix_top_corr[1][0] * ntt + i_ll_matrix_top_corr[1][1] * ntl + i_ll_matrix_top_corr[1][2] * nlt + i_ll_matrix_top_corr[1][3] * nll;
  double nfr = i_ll_matrix_top_corr[2][0] * ntt + i_ll_matrix_top_corr[2][1] * ntl + i_ll_matrix_top_corr[2][2] * nlt + i_ll_matrix_top_corr[2][3] * nll;
  double nff = i_ll_matrix_top_corr[3][0] * ntt + i_ll_matrix_top_corr[3][1] * ntl + i_ll_matrix_top_corr[3][2] * nlt + i_ll_matrix_top_corr[3][3] * nll;

  double nrf_LT = ll_matrix_top_corr[2][1] * nrf;
  double nfr_LT = ll_matrix_top_corr[2][2] * nfr;
  double nff_LT = ll_matrix_top_corr[2][3] * nff;

  return nrf_LT + nfr_LT + nff_LT;
}

double MatrixUtils::dilepton_LL_top_corr_weight(bool l1, bool l2)
{
  double ntt=((!l1)&&(!l2)?1.:0.);
  double ntl=((!l1)&&( l2)?1.:0.);
  double nlt=(( l1)&&(!l2)?1.:0.);
  double nll=(( l1)&&( l2)?1.:0.);

  double nrf = i_ll_matrix_top_corr[1][0] * ntt + i_ll_matrix_top_corr[1][1] * ntl + i_ll_matrix_top_corr[1][2] * nlt + i_ll_matrix_top_corr[1][3] * nll;
  double nfr = i_ll_matrix_top_corr[2][0] * ntt + i_ll_matrix_top_corr[2][1] * ntl + i_ll_matrix_top_corr[2][2] * nlt + i_ll_matrix_top_corr[2][3] * nll;
  double nff = i_ll_matrix_top_corr[3][0] * ntt + i_ll_matrix_top_corr[3][1] * ntl + i_ll_matrix_top_corr[3][2] * nlt + i_ll_matrix_top_corr[3][3] * nll;

  double nrf_LL = ll_matrix_top_corr[3][1] * nrf;
  double nfr_LL = ll_matrix_top_corr[3][2] * nfr;
  double nff_LL = ll_matrix_top_corr[3][3] * nff;

  return nrf_LL + nfr_LL + nff_LL;
}

void MatrixUtils::fill_lll_matrix(double r1, double f1, double r2, double f2, double r3, double f3)
{

  lll_matrix[0][0] = r1 * r2 * r3;
  lll_matrix[0][1] = r1 * r2 * f3;
  lll_matrix[0][2] = r1 * f2 * r3;
  lll_matrix[0][3] = r1 * f2 * f3;
  lll_matrix[0][4] = f1 * r2 * r3;
  lll_matrix[0][5] = f1 * r2 * f3;
  lll_matrix[0][6] = f1 * f2 * r3;
  lll_matrix[0][7] = f1 * f2 * f3;

  lll_matrix[1][0] = r1 * r2 * (1. - r3);
  lll_matrix[1][1] = r1 * r2 * (1. - f3);
  lll_matrix[1][2] = r1 * f2 * (1. - r3);
  lll_matrix[1][3] = r1 * f2 * (1. - f3);
  lll_matrix[1][4] = f1 * r2 * (1. - r3);
  lll_matrix[1][5] = f1 * r2 * (1. - f3);
  lll_matrix[1][6] = f1 * f2 * (1. - r3);
  lll_matrix[1][7] = f1 * f2 * (1. - f3);

  lll_matrix[2][0] = r1 * (1. - r2) * r3;
  lll_matrix[2][1] = r1 * (1. - r2) * f3;
  lll_matrix[2][2] = r1 * (1. - f2) * r3;
  lll_matrix[2][3] = r1 * (1. - f2) * f3;
  lll_matrix[2][4] = f1 * (1. - r2) * r3;
  lll_matrix[2][5] = f1 * (1. - r2) * f3;
  lll_matrix[2][6] = f1 * (1. - f2) * r3;
  lll_matrix[2][7] = f1 * (1. - f2) * f3;

  lll_matrix[3][0] = r1 * (1. - r2) * (1. - r3);
  lll_matrix[3][1] = r1 * (1. - r2) * (1. - f3);
  lll_matrix[3][2] = r1 * (1. - f2) * (1. - r3);
  lll_matrix[3][3] = r1 * (1. - f2) * (1. - f3);
  lll_matrix[3][4] = f1 * (1. - r2) * (1. - r3);
  lll_matrix[3][5] = f1 * (1. - r2) * (1. - f3);
  lll_matrix[3][6] = f1 * (1. - f2) * (1. - r3);
  lll_matrix[3][7] = f1 * (1. - f2) * (1. - f3);

  lll_matrix[4][0] = (1. - r1) * r2 * r3;
  lll_matrix[4][1] = (1. - r1) * r2 * f3;
  lll_matrix[4][2] = (1. - r1) * f2 * r3;
  lll_matrix[4][3] = (1. - r1) * f2 * f3;
  lll_matrix[4][4] = (1. - f1) * r2 * r3;
  lll_matrix[4][5] = (1. - f1) * r2 * f3;
  lll_matrix[4][6] = (1. - f1) * f2 * r3;
  lll_matrix[4][7] = (1. - f1) * f2 * f3;

  lll_matrix[5][0] = (1. - r1) * r2 * (1. - r3);
  lll_matrix[5][1] = (1. - r1) * r2 * (1. - f3);
  lll_matrix[5][2] = (1. - r1) * f2 * (1. - r3);
  lll_matrix[5][3] = (1. - r1) * f2 * (1. - f3);
  lll_matrix[5][4] = (1. - f1) * r2 * (1. - r3);
  lll_matrix[5][5] = (1. - f1) * r2 * (1. - f3);
  lll_matrix[5][6] = (1. - f1) * f2 * (1. - r3);
  lll_matrix[5][7] = (1. - f1) * f2 * (1. - f3);

  lll_matrix[6][0] = (1. - r1) * (1. - r2) * r3;
  lll_matrix[6][1] = (1. - r1) * (1. - r2) * f3;
  lll_matrix[6][2] = (1. - r1) * (1. - f2) * r3;
  lll_matrix[6][3] = (1. - r1) * (1. - f2) * f3;
  lll_matrix[6][4] = (1. - f1) * (1. - r2) * r3;
  lll_matrix[6][5] = (1. - f1) * (1. - r2) * f3;
  lll_matrix[6][6] = (1. - f1) * (1. - f2) * r3;
  lll_matrix[6][7] = (1. - f1) * (1. - f2) * f3;

  lll_matrix[7][0] = (1. - r1) * (1. - r2) * (1. - r3);
  lll_matrix[7][1] = (1. - r1) * (1. - r2) * (1. - f3);
  lll_matrix[7][2] = (1. - r1) * (1. - f2) * (1. - r3);
  lll_matrix[7][3] = (1. - r1) * (1. - f2) * (1. - f3);
  lll_matrix[7][4] = (1. - f1) * (1. - r2) * (1. - r3);
  lll_matrix[7][5] = (1. - f1) * (1. - r2) * (1. - f3);
  lll_matrix[7][6] = (1. - f1) * (1. - f2) * (1. - r3);
  lll_matrix[7][7] = (1. - f1) * (1. - f2) * (1. - f3);

  i_lll_matrix = lll_matrix;
  i_lll_matrix.InvertFast();

}

double MatrixUtils::trilepton_weight(bool l1, bool l2, bool l3){

  double nttt=(((!l1)&&(!l2)&&(!l3))?1.:0.);
  double nttl=(((!l1)&&(!l2)&&( l3))?1.:0.);
  double ntlt=(((!l1)&&( l2)&&(!l3))?1.:0.);
  double ntll=(((!l1)&&( l2)&&( l3))?1.:0.);
  double nltt=((( l1)&&(!l2)&&(!l3))?1.:0.);
  double nltl=((( l1)&&(!l2)&&( l3))?1.:0.);
  double nllt=((( l1)&&( l2)&&(!l3))?1.:0.);
  double nlll=((( l1)&&( l2)&&( l3))?1.:0.);

  double nrrf = i_lll_matrix[1][0] * nttt + i_lll_matrix[1][1] * nttl + i_lll_matrix[1][2] * ntlt + i_lll_matrix[1][3] * ntll
              + i_lll_matrix[1][4] * nltt + i_lll_matrix[1][5] * nltl + i_lll_matrix[1][6] * nllt + i_lll_matrix[1][7] * nlll;
  double nrfr = i_lll_matrix[2][0] * nttt + i_lll_matrix[2][1] * nttl + i_lll_matrix[2][2] * ntlt + i_lll_matrix[2][3] * ntll
              + i_lll_matrix[2][4] * nltt + i_lll_matrix[2][5] * nltl + i_lll_matrix[2][6] * nllt + i_lll_matrix[2][7] * nlll;
  double nrff = i_lll_matrix[3][0] * nttt + i_lll_matrix[3][1] * nttl + i_lll_matrix[3][2] * ntlt + i_lll_matrix[3][3] * ntll
              + i_lll_matrix[3][4] * nltt + i_lll_matrix[3][5] * nltl + i_lll_matrix[3][6] * nllt + i_lll_matrix[3][7] * nlll;
  double nfrr = i_lll_matrix[4][0] * nttt + i_lll_matrix[4][1] * nttl + i_lll_matrix[4][2] * ntlt + i_lll_matrix[4][3] * ntll
              + i_lll_matrix[4][4] * nltt + i_lll_matrix[4][5] * nltl + i_lll_matrix[4][6] * nllt + i_lll_matrix[4][7] * nlll;
  double nfrf = i_lll_matrix[5][0] * nttt + i_lll_matrix[5][1] * nttl + i_lll_matrix[5][2] * ntlt + i_lll_matrix[5][3] * ntll
              + i_lll_matrix[5][4] * nltt + i_lll_matrix[5][5] * nltl + i_lll_matrix[5][6] * nllt + i_lll_matrix[5][7] * nlll;
  double nffr = i_lll_matrix[6][0] * nttt + i_lll_matrix[6][1] * nttl + i_lll_matrix[6][2] * ntlt + i_lll_matrix[6][3] * ntll
              + i_lll_matrix[6][4] * nltt + i_lll_matrix[6][5] * nltl + i_lll_matrix[6][6] * nllt + i_lll_matrix[6][7] * nlll;
  double nfff = i_lll_matrix[7][0] * nttt + i_lll_matrix[7][1] * nttl + i_lll_matrix[7][2] * ntlt + i_lll_matrix[7][3] * ntll
              + i_lll_matrix[7][4] * nltt + i_lll_matrix[7][5] * nltl + i_lll_matrix[7][6] * nllt + i_lll_matrix[7][7] * nlll;

  double nrrf_TT = lll_matrix[0][1] * nrrf;
  double nrfr_TT = lll_matrix[0][2] * nrfr;
  double nrff_TT = lll_matrix[0][3] * nrff;
  double nfrr_TT = lll_matrix[0][4] * nfrr;
  double nfrf_TT = lll_matrix[0][5] * nfrf;
  double nffr_TT = lll_matrix[0][6] * nffr;
  double nfff_TT = lll_matrix[0][6] * nfff;

  return nrrf_TT + nrfr_TT + nrff_TT + nfrr_TT + nfrf_TT + nffr_TT + nfff_TT;

}
