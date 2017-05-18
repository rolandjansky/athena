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

#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include "TMatrixD.h"

class MatrixUtils {

 public:
  MatrixUtils();

  double lepton_weight(bool loose, double r, double f,
		       double &nf, double &nr);

  void fill_matrix(double r, double f);
  void fill_matrix(double r, double f, double t);

  double lepton_tight_weight(bool loose);
  double lepton_loose_weight(bool loose);
  double lepton_tight_top_corr_weight(bool loose);
  double lepton_loose_top_corr_weight(bool loose);

  double dilepton_weight(bool l1, double r1, double f1,
			 bool l2, double r2, double f2,
			 double &nrf, double &nfr, double &nff);

  void fill_matrix(double r1, double f1, double r2, double f2);
  void fill_matrix(double r1, double f1, double t1, double r2, double f2, double t2);

  double dilepton_TT_weight(bool l1, bool l2);
  double dilepton_TL_weight(bool l1, bool l2);
  double dilepton_LT_weight(bool l1, bool l2);
  double dilepton_LL_weight(bool l1, bool l2);
  double dilepton_TT_top_corr_weight(bool l1, bool l2);
  double dilepton_TL_top_corr_weight(bool l1, bool l2);
  double dilepton_LT_top_corr_weight(bool l1, bool l2);
  double dilepton_LL_top_corr_weight(bool l1, bool l2);

  void fill_lll_matrix(double r1, double f1, double r2, double f2, double r3, double f3);
  double trilepton_weight(bool l1, bool l2, bool l3);

 private:

  TMatrixD ljets_matrix;
  TMatrixD ljets_matrix_top_corr;
  TMatrixD ll_matrix;
  TMatrixD ll_matrix_top_corr;
  TMatrixD lll_matrix;

  TMatrixD i_ljets_matrix;
  TMatrixD i_ljets_matrix_top_corr;
  TMatrixD i_ll_matrix;
  TMatrixD i_ll_matrix_top_corr;
  TMatrixD i_lll_matrix;
};
 
#endif
