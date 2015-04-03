/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************************************
// Filename : TileFitter.cxx
// Author   : F. Merritt, A. Aurisano
// Created  : March 2004
//
// DESCRIPTION
//
//*************************************************************************************

#include "TileRecUtils/TileFitter.h"
#include "TileRecUtils/TileFilterResult.h"
#include <iostream>
#include <iomanip>
#include "boost/io/ios_state.hpp"

//std::vector<double> Amp;

// Constructors
TileFitter::TileFitter()
  : Iconstraint(0)
  , Camp()
  , Ped_const(0.0)
  , F2_sigPed(0.0)
  , F2_sigPile(0.0)
  , ND(0)
  , NP(0)
  , iconfig(0)
  , vconfig()
  , SPD()
  , MISPD()
  , ErrDiag()
{
}

TileFitter::TileFitter(bool debug, int nrow, int ncol, int index, HepMatrix& S, int Icon)
  : Iconstraint(0)
  , Camp()
  , Ped_const(0.0)
  , F2_sigPed(0.0)
  , F2_sigPile(0.0)
  , ND(0)
  , NP(0)
  , iconfig(0)
  , vconfig()
  , SPD()
  , MISPD()
  , ErrDiag()
{

  // Set P_const.
  Iconstraint = Icon;
  NP = nrow;
  ND = ncol;
  iconfig = index;

  //  if(nrow>ncol) debug=true;
  Ped_const = 50.0;
  F2_sigPed  = pow(1.6/10.,2.);
  F2_sigPile = pow(1.6/50.,2.);

  //  std::cout << "TileFitter constructor called for NP=" << NP << ", iconfig=" << iconfig << std::endl;
  SPD = S;

  //Calculate the matrices needed for fitting.  Save SPD and MISPD.
  HepMatrix SPDT = SPD.T();
  HepMatrix M    = SPD*SPDT;

  // Add constraint terms to chisquare if Iconstraint>0.
  if(Iconstraint>0) {
    if(debug) {
      std::cout << " add f*p:  F2_sigPed=" << F2_sigPed << ", Ped_const=" << Ped_const 
	   << ", F2_sigPile=" << F2_sigPile << std::endl;
    }
    M[0][0] = M[0][0] + F2_sigPed;
    if(Iconstraint>1) {
      for(int i=2; i<NP; i++) {
	M[i][i] = M[i][i] + F2_sigPile;
      }
    }
  }

  int err;
  HepMatrix MI   = M.inverse(err);
  MISPD = MI*SPD;

  if(Iconstraint>0) {
    HepVector temp(NP);
    for(int i=0; i<10; i++) {
      temp[i]=MI[0][i];
    }
    Camp = Ped_const*F2_sigPed*temp;
    if(debug) {
      std::cout << " Camp: ";
      PrintVec(Camp);
    }
  }

  for(int i=0; i<NP; i++) {
    double err = sqrt(MI[i][i]);
    ErrDiag.push_back(err);
  }

  if(debug) { 
    std::cout << " SPD: ";
    PrintMat(SPD);
    std::cout << " SPDT: ";
    PrintMat(SPDT);
    HepMatrix Ident = M*MI;
    std::cout << " M*MI:  ";
    PrintMat(Ident);
    std::cout << " MISPD: ";
    PrintMat(MISPD);
    std::cout << " errDiag=";
    for(int i=0; i<NP; i++) {
      std::cout  << ErrDiag[i] << " ";
    }
  std::cout << std::endl;
  }
}
// ============================================================================== 
// Destructor.
TileFitter::~TileFitter() {
  //  std::cout << " TileFitter destructor called: NP=" << NP << ", iconfig=" << iconfig << std::endl;
  return;
}
// ============================================================================== 
int TileFitter::FitAmp(TileFilterResult &tResult, bool lDebug) {
  int iret = -1;
  bool debugFitAmp = lDebug;
  // Get reference to variables which will store results.
  double sigDig = tResult.getSigDig();
  HepVector& digits = tResult.getDigRef();
  // int nrow = digits.num_row();
  HepVector& Amp = tResult.getParamRef();
  HepVector& Err = tResult.getErrRef();
  HepVector& residuals = tResult.getResidRef();
  double& chisq = tResult.getChisqRef();
  if(debugFitAmp) {
    std::cout << " Enter FitAmp. NP=" << NP << ", ND=" << ND << std::endl;
    std::cout << " In FitAmp:  MISPD=" << std::endl;
    PrintMat(MISPD);
  }

  // Multiply MISPD into digits to get fitted parameters. 

  Amp = MISPD*digits;
  if(Iconstraint>0) Amp = Amp + Camp;

  // Right-multiply Fit into SPD to get predicted digits.
  HepVector predict(ND);
  predict = SPD.T()*Amp;

  // Subtract digits from predict to get residuals, sum to get chisq*sig.
  residuals = digits - predict;
  chisq = residuals.normsq()/(sigDig*sigDig);
  if(Iconstraint>0) chisq += pow(Amp[0]-Ped_const,2.)*F2_sigPed;
  if(Iconstraint>1) {
    for(int iamp=2; iamp<NP; iamp++) {
      chisq = chisq + Amp[iamp]*Amp[iamp]*F2_sigPile;
    }
  }

  // Get error on each parameter from ErrDiag.
  HepVector newErr(NP);
  for(int ip=0; ip<NP; ip++) {
    newErr[ip]=sigDig*ErrDiag[ip];
  }
  Err = newErr;

  if(debugFitAmp) {
    std::cout << " predict: ";
    PrintVec(predict);
    std::cout << " residuals: ";
    PrintVec(residuals);
    //Check chisq.
    //    double dig2 = digits.normsq();
    //    double chisq2 = dig2 - dot(M*Amp,Amp);
    //    std::cout << " chisq=" << chisq << ", chisq2=" << chisq2 << std::endl;
  }
  iret = 0;
  return iret;
}

// ============================================================================== 
int TileFitter::getIndex() {
  return iconfig;
}
// ============================================================================== 
std::vector<double>& TileFitter::getErrRef() {
  return ErrDiag;
}
// ============================================================================== 
void TileFitter::PrintMat(HepMatrix &mat) {
  boost::io::ios_base_all_saver coutsave (std::cout);
  int nrow = mat.num_row();
  int ncol = mat.num_col();
  std::cout << "  nrow=" << nrow << ", ncol=" << ncol << std::endl;
  std::streamsize oldprec = std::cout.precision(4);
  for (int irow=0; irow<nrow; irow++) {
    std::cout << " irow=" << irow << ": Mat=";
    for (int icol=0; icol<ncol; icol++) {
      std::cout << std::setw(7) /* << std::setprecision(4) */ << mat[irow][icol];
    }
    std::cout << std::endl;
  }
  std::cout.precision(oldprec);
}
// ============================================================================== 
void TileFitter::PrintVec(HepVector &vec) {
  int nr = vec.num_row();
  int nc = vec.num_col();
  std::cout << " nr=" << nr << ", nc=" << nc << ",  vec=";
  for(int i=0; i<nr; i++) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}
