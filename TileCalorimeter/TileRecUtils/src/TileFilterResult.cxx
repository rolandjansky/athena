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
  debug = false;
  int ND = dig.size();
  HepVector digtem(ND);
  digits = digtem;
  for(int id=0; id<ND; id++) {
    digits[id] = dig[id];
  }
  //int nrow = digits.num_row();
  //int nrowtem = digtem.num_row();
  sigDigit = sig;
  // Initialize some of the variables that will be set later.
  Nparam = 1;
  Npileup = 0;
  Vcross.clear();
  chisq = 999.;
  iFitIndex = -1;
}
//============================================================================= 
TileFilterResult::~TileFilterResult() {
  return;
}
//============================================================================= 
double TileFilterResult::getSigDig() const {
  return sigDigit;
}
//============================================================================= 
HepVector* TileFilterResult::getDigPt() {
  HepVector* pdigits = &digits;
  return pdigits;
}
//============================================================================= 
HepVector& TileFilterResult::getDigRef() {
  return digits;
}
//============================================================================= 
std::vector<int>& TileFilterResult::getVcrossRef() {
  return Vcross;
}
//============================================================================= 
int& TileFilterResult::getFitIndexRef() {
  return iFitIndex;
}
//============================================================================= 
int& TileFilterResult::getNparRef() {
  return Nparam;
}
//============================================================================= 
HepVector& TileFilterResult::getParamRef() {
  return fitParam;
}
//============================================================================= 
HepVector& TileFilterResult::getErrRef() {
  return fitErr;
}
//============================================================================= 
HepVector& TileFilterResult::getResidRef() {
  return residuals;
}
//============================================================================= 
double& TileFilterResult::getChisqRef() {
  return chisq;
}
//============================================================================= 
void TileFilterResult::PrintFitParam() {
  boost::io::ios_base_all_saver coutsave (std::cout);
  std::cout << " Print fitted param from TileFilterResult:  Nparam=" << Nparam 
       << ", chisq=" << chisq << std::endl;
  for(int ipar=0; ipar<Nparam; ipar++) {
    if(ipar==0) {
      std::cout << " i=" << ipar << ", kcr=P" << ", A=" << std::setw(5) << std::setprecision(2) 
	 << fitParam[ipar] << " +-" << fitErr[ipar] << std::endl;
    } else {
    std::cout << " i=" << ipar << ", kcr=" << Vcross[ipar-1] << ", A=" 
	 << fitParam[ipar] << " +-" << fitErr[ipar] << std::endl;
    }
  }
  return;
}
//============================================================================= 
double TileFilterResult::getInTime(double &amp, double &err, double &ped,
				   double &chi2, double &t) {
  amp = fitParam[1];
  err = fitErr[1];
  ped = fitParam[0];
  chi2 = chisq;
  t = 0.;

  return chisq;
}
//============================================================================= 
void TileFilterResult::SnapShot(int imode) {
  boost::io::ios_base_all_saver coutsave (std::cout);
  // This print a short snapshot of the FilterResult state.
  std::cout << " SnapShot: imode=" << imode << ".  Nparam=" << Nparam 
       << ", chisq=" << chisq << ", iFitIndex" << iFitIndex << ", Vcross=";
  int Namp=Nparam-1;
  for(int jamp=0; jamp<Namp; jamp++) {
    std::cout << " " << Vcross[jamp];
  }
  std::cout << std::endl;
  if(iFitIndex<0) return;

  if(imode>0) {
    std::cout << "   FitParam=";
    for(int ipar=0; ipar<Nparam; ipar++) {
      std::cout << std::setw(5) << std::setprecision(1) << fitParam[ipar] << "+-" << fitErr[ipar];
      if(ipar<Nparam-1) std::cout << ", ";
    }
    std::cout << std::endl;
  }
  if(imode>1) {
    int Ndig = digits.num_row();
    std::cout << "   Residuals=";
    for(int idig=0; idig<Ndig; idig++) {
      std::cout << " " << std::setw(4) << std::setprecision(3) << residuals[idig];
    }
    std::cout << std::endl;
  }
}
//============================================================================= 
int TileFilterResult::addCross(int kcrIndex) {

  int iret = -1;

  //Check that kcrIndex is not already in list.
  bool ldup = false;
  if(Nparam>1) {
    int Namp = Nparam-1;
    for(int icr=0; icr<Namp; icr++) {
      if(kcrIndex==Vcross[icr]) {
	ldup = true;
	if(debug) {
	  std::cout << " TileFilterResult.addCross: kcrIndex=" << kcrIndex 
	       << " is already in crossing list: Kcross =";
	  for (int j=0; j<Namp; j++) {
	    std::cout << " " << Vcross[j];
	  }
	  std::cout << std::endl;
	}
	if(ldup) break;
      }
    } // end for loop
  } // end "if(Nparam>1)"
  if(ldup) {
    iret=1;
    return iret;
  }

  // Add the new crossing.
  iret = 0;
  Vcross.push_back(kcrIndex);
  std::sort(Vcross.begin(), Vcross.end() );
  Nparam = Nparam + 1;

  // Since we have a new Vcross configuration, iFitIndex is not yet defined for it.
  iFitIndex = -1;

  if(debug) {
    int Namp = Nparam-1;
    std::cout << " TileFilterResult.addCross.  Exit with Nparam=" << Nparam 
	 << ", Vcross=";
    for(int icr=0; icr<Namp; icr++) {
      std::cout << " " << std::setw(3) << Vcross[icr];
    }
    std::cout << std::endl;
  }

  return iret;
}
//============================================================================= 
int TileFilterResult::dropCross(int idrop) {
  // Drop the crossing stored in amplitude # iamp.
  int iret = -1;
  int Namp = Nparam - 1;
  for(int iamp=1; iamp<Namp; iamp++) {
    if(Vcross[iamp] == idrop) {
      // Erase the crossing.
      iret = 0;
      Vcross.erase(Vcross.begin()+iamp); 
      //  std::sort(Vcross.begin(), Vcross.end() );
      Nparam = Nparam - 1;
    }
  } 
  if(iret != 0) {
    std::cout << "error in TileFilterResult.dropCross:  idrop=" << idrop << "but vcross =";
    for(int iamp=0; iamp<Namp; iamp++) {
      std::cout << " " << Vcross[iamp];
    }
    std::cout << std::endl;
    return iret;
  }


  // Since we have a new Vcross configuration, iFitIndex is not yet defined for it.
  iFitIndex = -1;

  if(debug) {
    int Namp = Nparam-1;
    std::cout << " TileFilterResult.dropCross.  Exit with Nparam=" << Nparam 
	 << ", Vcross=";
    for(int icr=0; icr<Namp; icr++) {
      std::cout << " " << std::setw(3) << Vcross[icr];
    }
    std::cout << std::endl;
  }

  return iret;
}
