/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileFilterResult.cxx
// Authors  : F. Merritt, A. Aurisano
// Created  : March 2004
//
//****************************************************************************
#include "TileRecUtils/TileFilterResult.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "boost/io/ios_state.hpp"

// Constructor
TileFilterResult::TileFilterResult(std::vector<float> &dig, double sig) {
  m_debug = false;
  int ND = dig.size();
  CLHEP::HepVector digtem(ND);
  m_digits = digtem;
  for (int id = 0; id < ND; id++) {
    m_digits[id] = dig[id];
  }
  //int nrow = digits.num_row();
  //int nrowtem = digtem.num_row();
  m_sigDigit = sig;
  // Initialize some of the variables that will be set later.
  m_nParam = 1;
  m_nPileup = 0;
  m_vCross.clear();
  m_chi2 = 999.;
  m_iFitIndex = -1;
}
//============================================================================= 
TileFilterResult::~TileFilterResult() {
  return;
}
//============================================================================= 
double TileFilterResult::getSigDig() const {
  return m_sigDigit;
}

//============================================================================= 
CLHEP::HepVector& TileFilterResult::getDigRef() {
  return m_digits;
}
//============================================================================= 
std::vector<int>& TileFilterResult::getVcrossRef() {
  return m_vCross;
}
//============================================================================= 
int& TileFilterResult::getFitIndexRef() {
  return m_iFitIndex;
}
//============================================================================= 
int& TileFilterResult::getNparRef() {
  return m_nParam;
}
//============================================================================= 
CLHEP::HepVector& TileFilterResult::getParamRef() {
  return m_fitParam;
}
//============================================================================= 
CLHEP::HepVector& TileFilterResult::getErrRef() {
  return m_fitErr;
}
//============================================================================= 
CLHEP::HepVector& TileFilterResult::getResidRef() {
  return m_residuals;
}
//============================================================================= 
double& TileFilterResult::getChi2Ref() {
  return m_chi2;
}
//============================================================================= 
void TileFilterResult::printFitParam() {
  boost::io::ios_base_all_saver coutsave(std::cout);
  std::cout << " Print fitted param from TileFilterResult:  Nparam=" << m_nParam << ", chisq=" << m_chi2 << std::endl;
  for (int ipar = 0; ipar < m_nParam; ipar++) {
    if (ipar == 0) {
      std::cout << " i=" << ipar << ", kcr=P" << ", A=" << std::setw(5) << std::setprecision(2) << m_fitParam[ipar]
          << " +-" << m_fitErr[ipar] << std::endl;
    } else {
      std::cout << " i=" << ipar << ", kcr=" << m_vCross[ipar - 1] << ", A=" << m_fitParam[ipar] << " +-"
          << m_fitErr[ipar] << std::endl;
    }
  }
  return;
}
//============================================================================= 
double TileFilterResult::getInTime(double &amp, double &err, double &ped, double &chi2, double &t) {
  amp = m_fitParam[1];
  err = m_fitErr[1];
  ped = m_fitParam[0];
  chi2 = m_chi2;
  t = 0.;

  return m_chi2;
}
//============================================================================= 
void TileFilterResult::snapShot(int imode) {
  boost::io::ios_base_all_saver coutsave(std::cout);
  // This print a short snapshot of the FilterResult state.
  std::cout << " SnapShot: imode=" << imode << ".  Nparam=" << m_nParam << ", chisq=" << m_chi2 << ", iFitIndex"
      << m_iFitIndex << ", Vcross=";
  int Namp = m_nParam - 1;
  for (int jamp = 0; jamp < Namp; jamp++) {
    std::cout << " " << m_vCross[jamp];
  }
  std::cout << std::endl;
  if (m_iFitIndex < 0) return;

  if (imode > 0) {
    std::cout << "   FitParam=";
    for (int ipar = 0; ipar < m_nParam; ipar++) {
      std::cout << std::setw(5) << std::setprecision(1) << m_fitParam[ipar] << "+-" << m_fitErr[ipar];
      if (ipar < m_nParam - 1) std::cout << ", ";
    }
    std::cout << std::endl;
  }
  if (imode > 1) {
    int Ndig = m_digits.num_row();
    std::cout << "   Residuals=";
    for (int idig = 0; idig < Ndig; idig++) {
      std::cout << " " << std::setw(4) << std::setprecision(3) << m_residuals[idig];
    }
    std::cout << std::endl;
  }
}
//============================================================================= 
int TileFilterResult::addCross(int kcrIndex) {

  int iret = -1;

  //Check that kcrIndex is not already in list.
  bool ldup = false;
  if (m_nParam > 1) {
    int Namp = m_nParam - 1;
    for (int icr = 0; icr < Namp; icr++) {
      if (kcrIndex == m_vCross[icr]) {
        ldup = true;
        if (m_debug) {
          std::cout << " TileFilterResult.addCross: kcrIndex=" << kcrIndex << " is already in crossing list: Kcross =";
          for (int j = 0; j < Namp; j++) {
            std::cout << " " << m_vCross[j];
          }
          std::cout << std::endl;
        }
        if (ldup) break;
      }
    } // end for loop
  } // end "if(Nparam>1)"
  if (ldup) {
    iret = 1;
    return iret;
  }

  // Add the new crossing.
  iret = 0;
  m_vCross.push_back(kcrIndex);
  std::sort(m_vCross.begin(), m_vCross.end());
  m_nParam = m_nParam + 1;

  // Since we have a new Vcross configuration, iFitIndex is not yet defined for it.
  m_iFitIndex = -1;

  if (m_debug) {
    int Namp = m_nParam - 1;
    std::cout << " TileFilterResult.addCross.  Exit with Nparam=" << m_nParam << ", Vcross=";
    for (int icr = 0; icr < Namp; icr++) {
      std::cout << " " << std::setw(3) << m_vCross[icr];
    }
    std::cout << std::endl;
  }

  return iret;
}
//============================================================================= 
int TileFilterResult::dropCross(int idrop) {
  // Drop the crossing stored in amplitude # iamp.
  int iret = -1;
  int Namp = m_nParam - 1;
  for (int iamp = 1; iamp < Namp; iamp++) {
    if (m_vCross[iamp] == idrop) {
      // Erase the crossing.
      iret = 0;
      m_vCross.erase(m_vCross.begin() + iamp);
      //  std::sort(Vcross.begin(), Vcross.end() );
      m_nParam = m_nParam - 1;
    }
  }
  if (iret != 0) {
    std::cout << "error in TileFilterResult.dropCross:  idrop=" << idrop << "but vcross =";
    for (int iamp = 0; iamp < Namp; iamp++) {
      std::cout << " " << m_vCross[iamp];
    }
    std::cout << std::endl;
    return iret;
  }

  // Since we have a new Vcross configuration, iFitIndex is not yet defined for it.
  m_iFitIndex = -1;

  if (m_debug) {
    int Namp = m_nParam - 1;
    std::cout << " TileFilterResult.dropCross.  Exit with Nparam=" << m_nParam << ", Vcross=";
    for (int icr = 0; icr < Namp; icr++) {
      std::cout << " " << std::setw(3) << m_vCross[icr];
    }
    std::cout << std::endl;
  }

  return iret;
}
