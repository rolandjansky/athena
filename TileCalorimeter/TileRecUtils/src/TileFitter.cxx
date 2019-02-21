/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  : m_iConstraint(0)
  , m_cAmp()
  , m_pedConst(0.0)
  , m_sigPedF2(0.0)
  , m_sigPileF2(0.0)
  , m_nSamples(0)
  , m_nParameters(0)
  , m_iConfig(0)
  , m_vConfig()
  , m_SPD()
  , m_MISPD()
  , m_errDiag()
{
}

TileFitter::TileFitter(bool debug, int nrow, int ncol, int index, CLHEP::HepMatrix& S, int Icon)
  : m_iConstraint(0)
  , m_cAmp()
  , m_pedConst(0.0)
  , m_sigPedF2(0.0)
  , m_sigPileF2(0.0)
  , m_nSamples(0)
  , m_nParameters(0)
  , m_iConfig(0)
  , m_vConfig()
  , m_SPD()
  , m_MISPD()
  , m_errDiag()
{

  // Set P_const.
  m_iConstraint = Icon;
  m_nParameters = nrow;
  m_nSamples = ncol;
  m_iConfig = index;

  //  if(nrow>ncol) debug=true;
  m_pedConst = 50.0;
  m_sigPedF2 = std::pow(1.6 / 10., 2);
  m_sigPileF2 = std::pow(1.6 / 50., 2);

  //  std::cout << "TileFitter constructor called for NP=" << NP << ", iconfig=" << iconfig << std::endl;
  m_SPD = S;

  //Calculate the matrices needed for fitting.  Save SPD and MISPD.
  CLHEP::HepMatrix SPDT = m_SPD.T();
  CLHEP::HepMatrix M = m_SPD * SPDT;

  // Add constraint terms to chisquare if Iconstraint>0.
  if (m_iConstraint > 0) {
    if (debug) {
      std::cout << " add f*p:  F2_sigPed=" << m_sigPedF2 << ", Ped_const=" << m_pedConst << ", F2_sigPile="
          << m_sigPileF2 << std::endl;
    }
    M[0][0] = M[0][0] + m_sigPedF2;
    if (m_iConstraint > 1) {
      for (int i = 2; i < m_nParameters; i++) {
        M[i][i] = M[i][i] + m_sigPileF2;
      }
    }
  }

  int err;
  CLHEP::HepMatrix MI = M.inverse(err);
  m_MISPD = MI * m_SPD;

  if (m_iConstraint > 0) {
    CLHEP::HepVector temp(m_nParameters);
    for (int i = 0; i < 10; i++) {
      temp[i] = MI[0][i];
    }
    m_cAmp = m_pedConst * m_sigPedF2 * temp;
    if (debug) {
      std::cout << " Camp: ";
      printVec(m_cAmp);
    }
  }

  for (int i = 0; i < m_nParameters; i++) {
    double err = sqrt(MI[i][i]);
    m_errDiag.push_back(err);
  }

  if (debug) {
    std::cout << " SPD: ";
    printMat(m_SPD);
    std::cout << " SPDT: ";
    printMat(SPDT);
    CLHEP::HepMatrix Ident = M * MI;
    std::cout << " M*MI:  ";
    printMat(Ident);
    std::cout << " MISPD: ";
    printMat(m_MISPD);
    std::cout << " errDiag=";
    for (int i = 0; i < m_nParameters; i++) {
      std::cout << m_errDiag[i] << " ";
    }
    std::cout << std::endl;
  }
}
// ============================================================================== 
int TileFitter::fitAmp(TileFilterResult &tResult, bool lDebug) {
  int iret = -1;
  bool debugFitAmp = lDebug;
  // Get reference to variables which will store results.
  double sigDig = tResult.getSigDig();
  CLHEP::HepVector& digits = tResult.getDigRef();
  // int nrow = digits.num_row();
  CLHEP::HepVector& Amp = tResult.getParamRef();
  CLHEP::HepVector& Err = tResult.getErrRef();
  CLHEP::HepVector& residuals = tResult.getResidRef();
  double& chisq = tResult.getChi2Ref();
  if (debugFitAmp) {
    std::cout << " Enter FitAmp. NP=" << m_nParameters << ", ND=" << m_nSamples << std::endl;
    std::cout << " In FitAmp:  MISPD=" << std::endl;
    printMat(m_MISPD);
  }

  // Multiply MISPD into digits to get fitted parameters. 

  Amp = m_MISPD * digits;
  if (m_iConstraint > 0) Amp = Amp + m_cAmp;

  // Right-multiply Fit into SPD to get predicted digits.
  CLHEP::HepVector predict(m_nSamples);
  predict = m_SPD.T() * Amp;

  // Subtract digits from predict to get residuals, sum to get chisq*sig.
  residuals = digits - predict;
  chisq = residuals.normsq() / (sigDig * sigDig);
  if (m_iConstraint > 0) chisq += std::pow(Amp[0] - m_pedConst, 2) * m_sigPedF2;
  if (m_iConstraint > 1) {
    for (int iamp = 2; iamp < m_nParameters; iamp++) {
      chisq = chisq + Amp[iamp] * Amp[iamp] * m_sigPileF2;
    }
  }

  // Get error on each parameter from ErrDiag.
  CLHEP::HepVector newErr(m_nParameters);
  for (int ip = 0; ip < m_nParameters; ip++) {
    newErr[ip] = sigDig * m_errDiag[ip];
  }
  Err = newErr;

  if (debugFitAmp) {
    std::cout << " predict: ";
    printVec(predict);
    std::cout << " residuals: ";
    printVec(residuals);
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
  return m_iConfig;
}
// ============================================================================== 
std::vector<double>& TileFitter::getErr() {
  return m_errDiag;
}
// ============================================================================== 
void TileFitter::printMat(CLHEP::HepMatrix &mat) {
  boost::io::ios_base_all_saver coutsave(std::cout);
  int nrow = mat.num_row();
  int ncol = mat.num_col();
  std::cout << "  nrow=" << nrow << ", ncol=" << ncol << std::endl;
  std::streamsize oldprec = std::cout.precision(4);
  for (int irow = 0; irow < nrow; irow++) {
    std::cout << " irow=" << irow << ": Mat=";
    for (int icol = 0; icol < ncol; icol++) {
      std::cout << std::setw(7) /* << std::setprecision(4) */<< mat[irow][icol];
    }
    std::cout << std::endl;
  }
  std::cout.precision(oldprec);
}
// ============================================================================== 
void TileFitter::printVec(CLHEP::HepVector &vec) {
  int nr = vec.num_row();
  int nc = vec.num_col();
  std::cout << " nr=" << nr << ", nc=" << nc << ",  vec=";
  for (int i = 0; i < nr; i++) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}
