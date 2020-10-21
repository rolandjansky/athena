/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************************************************
// Filename : TileFilterManager.cxx
// Author   : F. Merritt, A. Aurisano
// Created  : March 2004

//**************************************************************************************

#include "TileRecUtils/TileFilterManager.h"
#include "TileConditions/TileInfo.h"

#include <algorithm>
#include <iostream>
#include "boost/io/ios_state.hpp"

// =======================================================================================

// Constructor
TileFilterManager::TileFilterManager(int mode, int level, int &npMax, int &nSam, int &inTdig, int &ibSam, int &ieSam,
    int &bCr, int &eCr, int &nShape, int &inShape, std::vector<double> &Shape, bool lDebug)
    : m_ind2icrMap(), m_icr2indMap() {
  /* Store information needed for fitting. */

  m_debug = lDebug;     // Stored as a private data member.
  m_filterMode = mode;
  m_filterLevel = level;
  m_nParamMax = npMax;
  m_nShape = nShape;
  m_inTshape = inShape;
  m_nDig = nSam;
  m_inTdig = inTdig;
  m_jBsamp = ibSam;
  m_jEsamp = ieSam;
  m_jBcross = bCr;
  m_jEcross = eCr;
  m_inTcross = -m_jBcross;
  m_nCross = m_jEcross - m_jBcross + 1;
  std::cout << " TileFilterManager constructor.  NParamMax =" << m_nParamMax << ", Ndig =" << m_nDig << std::endl;

  // Parameters that should be read in externally.
  m_rChi2Cut = 4.0;
  m_chiCut = 3.5;
  int NParamMin = 2;
  std::cout << " FilterMode=" << m_filterMode << ", NParamMax=" << m_nParamMax << ", NParamMin=" << NParamMin
      << ", chiCut=" << m_chiCut << ", rchisqCut=" << m_rChi2Cut << std::endl;
  //
  // Create the tables of offsets for calculating Fitter indices.

  makeFitterOffsetTables();

  //
  // Create the maps taking us from "digit number" idig to "crossing index" ind.  
  // "idig" corresponds to the array of digits which are read out, ranging from zero
  // to Ndig, with the in-time digit defined by InTdig.  It is more convenient for
  // fitting to re-order these so that the index of the in-time crossing is zero,
  // and the other are sequentially numbered 1,2,..., Ncr.
  //
  std::cout << " Number of digits read =" << m_nDig << " .  In-time digit has idig=" << m_inTdig << "." << std::endl;
  int ind = 0;
  for (int icr = 0; icr < m_nCross; icr++) {
    if (icr == m_inTdig) ind = 0;
    if (icr < m_inTdig) ind = icr - m_jBcross + 1;
    if (icr > m_inTdig) ind = icr - m_jBcross;
    m_icr2indMap[icr] = ind;
    m_ind2icrMap[ind] = icr;
  }
  if (m_debug) {
    for (int ind = 0; ind < m_nCross; ind++) {
      int icr = m_ind2icrMap[ind];
      int kdisp = icr - m_inTdig;
      std::cout << " Index =" << ind << "  crossing #  =" << icr << ", displacement from InTdig =" << kdisp
          << " crossings." << std::endl;
    }
  }
  /* Create the shaping matrix.  Row = crossing (of deposit), col = digit. */
  //  std::vector<double> ShapeMat[Ncross][Ndig];
  for (int ind = 0; ind < m_nCross; ind++) {
    int icr = m_ind2icrMap[ind];
    int OffSet = m_inTshape;
    double * Xshape = new double[m_nDig];
    for (int idig = 0; idig < m_nDig; idig++) {
      int k = OffSet - icr + idig;
      if (k < 0) k = 0;
      if (k >= m_nShape) k = m_nShape - 1;
      Xshape[idig] = Shape[k];
    }
    m_crossShape.push_back(Xshape);
  }
  /* Print out the Shape Matrix. */
  if (m_debug) {
    boost::io::ios_base_all_saver coutsave(std::cout);
    std::cout << " TileFilterManager: ShapingMatrix.   Nshape=" << m_nShape << ", InTshape=" << m_inTshape << ", Ndig="
        << m_nDig << ", InTdig=" << m_inTdig << ", Ncross=" << m_nCross << std::endl;
    for (int ind = 0; ind < m_nCross; ind++) {
      double * Xshape = m_crossShape[ind];
      std::cout << " ind=" << ind << " Shape=";
      for (int idig = 0; idig < m_nDig; idig++) {
        std::cout << " " << std::setw(6) << std::setprecision(3) << Xshape[idig];
      }
      std::cout << std::endl;
    }
  }

  //  vNparam.reserve(NParamMax);
  //Print the FitterIndex arrays.
  std::vector<int> Crossings;
  //int NpileMax = NParamMax - 2;
  int NampMax = m_nParamMax - 1;
  for (int ipile = 0; ipile < NampMax; ipile++) {
    int Nmax = m_nFitIndex[ipile];
    if (m_debug)
      std::cout << " Crossing configurations for Nparam=" << ipile + 2 << ", Npileup=" << ipile << ": " << Nmax
          << " configurations." << std::endl;
    for (int iconfig = 0; iconfig < Nmax; iconfig++) {
      Crossings.clear();
      getVcross(ipile, iconfig, Crossings);
      int nParam = ipile + 2;
      int kFitIndex = getFitIndex(nParam, Crossings);
      int ncr = Crossings.size();
      if (m_debug) {
        std::cout << "     Npile=" << std::setw(2) << ipile << ", iconfig=" << std::setw(3) << iconfig << " (kF="
            << std::setw(3) << kFitIndex << ")  => Vcross=";
        for (int icr = 0; icr < ncr; icr++) {
          std::cout << " " << std::setw(3) << Crossings[icr];
        }
        std::cout << std::endl;
      }
    }
  }
  // Make the Fitter Arrays
  /* int iok = */makeFitterArrays();

  /* Initialization has successfully completed.  */
  // Set debug = false for execution phase.
  m_debug = true;
  return;
}

// ===================================================================================

// Destructor
TileFilterManager::~TileFilterManager() {

  for (unsigned int ind = 0; ind < m_crossShape.size(); ++ind) {
    delete[] m_crossShape[ind];
  }

  for (unsigned int ind = 0; ind < m_offsetVector.size(); ++ind) {
    delete[] m_offsetVector[ind];
  }
}

// ===================================================================================

int TileFilterManager::fitDigits(TileFilterResult &tResult, bool lDebug) {
  int icode = -1;

  //  std::cout << " FitDigits:  Fmode=" << Fmode << ", lDebug=" << lDebug << std::endl;
  if (m_filterMode == 2) {
    icode = fitDigits1(tResult, lDebug);
    return icode;
  }
  if (m_filterMode == 3) {
    icode = fitDigits2(tResult, lDebug);
    return icode;
  }
  std::cout << " ERROR in TileFitDigits !!  Fmode =" << m_filterMode << std::endl;

  return icode;
}

// ===================================================================================

int TileFilterManager::fitDigits1(TileFilterResult &tResult, bool lDebug) {
  int icode = -1;
  m_debug = lDebug;
  // Get references to the key variable in tResult.
  CLHEP::HepVector& digits = tResult.getDigRef();
  //CLHEP::HepVector& fitAmp = tResult.getParamRef();
  //CLHEP::HepVector& fitErr = tResult.getErrRef();
  CLHEP::HepVector& residuals = tResult.getResidRef();
  double& chisqRef = tResult.getChi2Ref();
  int& Npar = tResult.getNparRef();
  std::vector<int>& vcross = tResult.getVcrossRef();
  int& iFitIndex = tResult.getFitIndexRef();

  // First find crossing with highest amplitude.
  int jcross = findHighestResidual(digits);
  int jparam = ((jcross < 0) ? 0 : m_icr2indMap[jcross]);
  //  if(debug) std::cout << " Highest crossing = " << jcross << "(cind=" << jparam
  //		 << "), amp=" << digits[jcross] << std::endl;

  // Initialize fitting parameters
  Npar = 1;
  iFitIndex = -1;
  chisqRef = 999.;

  jparam = 0; // Set this to be intime crossing [FSM, 7/30/04]
  // Include in-time crossing (jparam=0) in fit even if it is not the maximum.
  //if(jparam != 0) iret = tResult.addCross(0);

  // Start loop over fits, adding a new parameter each time until reduced chisq is OK.
  double rchisq = 999.;
  icode = -1;
  double digSigma = tResult.getSigDig();
  while (Npar < m_nParamMax) {
    if (m_debug)
      std::cout << " FilterManager.FitDigits1, while loop. Npar=" << Npar << ", NParamMax=" << m_nParamMax << std::endl;
    int iret = tResult.addCross(jparam);
    if (iret != 0) {
      icode = iret;
      break;
    }
    //    if(debug) tResult.SnapShot(0);
    iFitIndex = getFitIndex(Npar, vcross);
    // cppcheck-suppress negativeContainerIndex
    // Npar is changed as a side-effect of calling addCross() above.
    std::vector<TileFitter>& vFitter = m_vNpFitter[Npar - 2];
    TileFitter& tileFitter = vFitter[iFitIndex];
    (void) tileFitter.fitAmp(tResult, false);
    if (m_debug) tResult.snapShot(2);
    rchisq = chisqRef / (m_nDig - Npar);

    // Chisq is small enough, so the fit has been successful.
    if (rchisq < m_rChi2Cut) {
      icode = 0;
      break;
    }
    // Have hit max param even though chisq is still big (problem!).  
    if (Npar == m_nParamMax) {
      icode = 5;
      break;
    }
    // Find index of the highest residual.
    jcross = findHighestResidual(residuals);
    jparam = ((jcross < 0) ? 0 : m_icr2indMap[jcross]);
    // If jparam is already in list, terminate search (problem?).
    bool ldup = false;
    int Namp = vcross.size();
    for (int i = 1; i < Namp; i++) {
      if (vcross[i] == jparam) ldup = true;
    }
    if (ldup) {
      icode = 4;
      break;
    }

    double chi = ((jcross < 0) ? 0.0 : residuals[jcross] / digSigma);
    if (chi < m_chiCut) {
      icode = 3;
      break;
    }
  }
  if (m_debug) {
    std::cout << " End of loop.  icode =" << icode << ", Npar=" << Npar << std::endl;
    tResult.snapShot(1);
  }
  return icode;
}

// ===================================================================================

int TileFilterManager::fitDigits2(TileFilterResult &tResult, bool lDebug) {
  //int iret = -1;
  int icode = 0;
  m_debug = lDebug;
  // Get references to the key variable in tResult.
  CLHEP::HepVector& digits = tResult.getDigRef();
  CLHEP::HepVector& fitAmp = tResult.getParamRef();
  CLHEP::HepVector& fitErr = tResult.getErrRef();
  //CLHEP::HepVector& residuals = tResult.getResidRef();
  double& chisqRef = tResult.getChi2Ref();
  int& Npar = tResult.getNparRef();
  int Namp = Npar - 1;
  std::vector<int>& vcross = tResult.getVcrossRef();
  int& iFitIndex = tResult.getFitIndexRef();
  if (m_debug) {
    boost::io::ios_base_all_saver coutsave(std::cout);
    //    tResult.SnapShot(0);
    std::cout << " digits=";
    for (int i = 0; i < m_nDig; i++) {
      std::cout << " " << std::setw(6) << std::setprecision(2) << digits[i];
    }
    std::cout << std::endl;
  }

  // Make a crossing vector that contains all allowed amplitudes.
  Namp = m_filterLevel - 1;
  for (int iamp = 0; iamp < Namp; iamp++) {
    /*iret =*/tResult.addCross(iamp);
  }

  //Namp and Npar could have changed. Npar is incremented by addCross.
  Namp = Npar - 1;
  if (m_debug) tResult.snapShot(0);

  // Initialize fitting parameters
  iFitIndex = -1;
  chisqRef = 999.;

  // Start loop over fits, removing one or more amplitudes each time.
  // double rchisq = 999.;
  icode = -1;

  int Npass = 0;
  while (Npar > 1) {
    Npass += 1;
    if (m_debug)
      std::cout << " FilterManager.FitDigits2, while loop. Npar=" << Npar << ", NParamMax=" << m_nParamMax << std::endl;
    //    if(debug) tResult.SnapShot(0);
    iFitIndex = getFitIndex(Npar, vcross);
    if (m_debug) std::cout << " Npar=" << Npar << ", iFitIndex=" << iFitIndex << std::endl;
    std::vector<TileFitter>& vFitter = m_vNpFitter[Npar - 2];
    TileFitter& tileFitter = vFitter[iFitIndex];
    //    if(debug) std::cout << " Ready to call tileFitter.FitAmp" << std::endl;
    /*iret =*/tileFitter.fitAmp(tResult, false);
    if (m_debug) tResult.snapShot(2);
    // If Npar is down to 2 parameters (ped + inTime), terminate fitting search.
    if (Npar <= 2) {
      icode = 3;
      break;
    }

    // Calculate significance chi of each fit parameter..
    // double chiCutLow[4] = {chiCut, 1.50, 0.75, 0.};
    const int Ndim = 12;
    double chiAmp[Ndim];
    int iAmp[Ndim];
    int Npile = 0;
    for (int i = 2; i < Npar; i++) {
      chiAmp[Npile] = fitAmp[i] / fitErr[i];
      iAmp[Npile] = vcross[i - 1];
      if (m_debug)
        std::cout << " set chiAmp: i=" << i << ", iAmp=" << iAmp[Npile] << ",  chi=" << chiAmp[Npile] << std::endl;
      Npile += 1;
    }
    int ndrop = 0;
    int crdrop = 0;
    int ndropMax = 4;
    while (ndrop < ndropMax) {
      if (m_debug) std::cout << " top of drop loop.  ndrop=" << ndrop << ", Npass=" << Npass << std::endl;
      double chiMin = m_chiCut;
      //      chiMin = chiCutLow[ndrop];
      int idrop = -1;
      for (int i = 0; i < Npile; i++) {
        //	if(debug) std::cout << "drop candidate: i=" << i << ", iAmp=" << iAmp[i]
        //       << ", chiAmp=" << chiAmp[i] <<", chiMin=" << chiMin << std::endl;
        if (iAmp[i] < 0) continue;
        if (chiAmp[i] > chiMin) continue;
        chiMin = chiAmp[i];
        idrop = i;
        crdrop = iAmp[i];
      }
      if (m_debug)
        std::cout << " end of Npile loop.  idrop=" << idrop << ", crdrop=" << crdrop << ", ndrop=" << ndrop
            << std::endl;
      if (idrop > -1) {
        /*iret =*/tResult.dropCross(crdrop);
        ndrop += 1;
        iAmp[idrop] = -iAmp[idrop];
        icode = 1;
        if (m_debug)
          std::cout << " ndrop=" << ndrop << ", idrop=" << idrop << ", crdrop=" << crdrop << ", chiMin=" << chiMin
              << std::endl;
      } else {
        icode = 6;
        break;
      }
      if (m_debug) std::cout << "FitDig2:  Npass=" << Npass << ", ndrop=" << ndrop << std::endl;
    } // end of ndrop loop
    if (m_debug) std::cout << " have fallen out of drop loop.  ndrop=" << ndrop << ", Npass=" << Npass << std::endl;
    if (ndrop == 0) {
      icode = 0;
      break;
    }
  } // end of while loop

  if (m_debug) {
    std::cout << " TileFilterManager:  End of pass loop.  icode =" << icode << ", Npar=" << Npar << ", Npass=" << Npass
        << std::endl;
    tResult.snapShot(2);
  }
  return icode;
}

// ===================================================================================

int TileFilterManager::findHighestResidual(CLHEP::HepVector &digits) const {
  int icrMax = -1;
  double ampMax = -999.;
  for (int icr = 0; icr < m_nCross; icr++) {
    if (digits[icr] > ampMax) {
      icrMax = icr;
      ampMax = digits[icr];
    }
  }
  return icrMax;
}

// ===================================================================================

int TileFilterManager::findLowestCrossing(CLHEP::HepVector &digits) const {
  int icrMin = -1;
  double ampMin = +9999.;
  for (int icr = 0; icr < m_nCross; icr++) {
    if (icr == m_inTdig) continue;
    if (digits[icr] < ampMin) {
      icrMin = icr;
      ampMin = digits[icr];
    }
  }
  return icrMin;
}

// ===================================================================================

void TileFilterManager::makeFitterOffsetTables() {

  int NpileupMax = m_nParamMax - 2;
  int NampMax = m_nParamMax - 1;
  int Npileup = NpileupMax;
  if (m_debug)
    std::cout << " Enter MakeFitterOffsetTables:  Npileup=" << Npileup << ", NampMax=" << NampMax << ", Ncross="
        << m_nCross << std::endl;
//For ipileup=0 (special case), offset = index of crossing.
  int * Offset0 = new int[m_nCross];
  for (int index = 0; index < m_nCross; index++) {
    Offset0[index] = index;
  }
  m_offsetVector.push_back(Offset0);
  //
  //For ipileup=1, number offsets sequentially starting with zero for index=1.
  int * Offset1 = new int[m_nCross];
  for (int index = 0; index < m_nCross; index++) {
    if (index < 2)
      Offset1[index] = 0;
    else
      Offset1[index] = index - 1;
  }
  m_offsetVector.push_back(Offset1);

  // For Npileup>1, use iterative formula (offsets = coeff of binary expansion)
  if (NampMax > 2) {
    for (int ipile = 2; ipile < NampMax; ipile++) {
      int * vlast = m_offsetVector[ipile - 1];
      int * Offset = new int[m_nCross];
      for (int index = 0; index < m_nCross; index++) {
        if (index <= ipile) {
          Offset[index] = 0;
        } else {
          Offset[index] = Offset[index - 1] + vlast[index - 1];
        }
      }
      m_offsetVector.push_back(Offset);
    }
  }
  // Find the number of FitIndex values for each number of parameters.
  for (int ipile = 0; ipile < NampMax; ipile++) {
    int Nmax;
    int * Offset = m_offsetVector[ipile];
    if (ipile <= 1) Nmax = Offset[m_nCross - 1] + 1;
    if (ipile > 1) {
      int * vlast = m_offsetVector[ipile - 1];
      Nmax = Offset[m_nCross - 1] + vlast[m_nCross - 1];
    }
    m_nFitIndex.push_back(Nmax);
  }

  // Print out the Offset table.
  if (m_debug) {
    std::cout << " *** TileFilter Offset table for Npileup=" << Npileup << " and Ncross=" << m_nCross << std::endl;
    for (int ipile = 0; ipile < NampMax; ipile++) {
      int * Offset = m_offsetVector[ipile];
      std::cout << "       ipile=" << std::setw(3) << ipile << ":  Offsets = ";
      for (int index = 0; index < m_nCross; index++) {
        std::cout << " " << std::setw(3) << Offset[index];
      }
      std::cout << ";  NfitIndex=" << std::setw(3) << m_nFitIndex[ipile] << std::endl;
    }
  }

  return;
}

// ===================================================================================

int TileFilterManager::makeSPD(bool debugMakeSPD, std::vector<int>& vcross, CLHEP::HepMatrix& SPD) {
  int iret = -1;
  int Namp = vcross.size();
  int Nparam = Namp + 1;
  // First row of SPD is always for the pedestal.
  for (int idig = 0; idig < m_nDig; idig++) {
    SPD[0][idig] = 1.0;
  }
  // The remaining rows correspond to the crossing amplitudes specified by vcross.
  for (int ipar = 1; ipar < Nparam; ipar++) {
    int jcr = vcross[ipar - 1];
    double * Xshape = m_crossShape[jcr];
    for (int idig = 0; idig < m_nDig; idig++) {
      SPD[ipar][idig] = Xshape[idig];
    }
  }
  if (debugMakeSPD) {
    std::cout << " Make SPD for NP=" << Nparam << ", vcross=";
    for (int iamp = 0; iamp < Namp; iamp++) {
      std::cout << " " << vcross[iamp];
    }
    std::cout << std::endl;
    for (int ipar = 0; ipar < Nparam; ipar++) {
      std::cout << " ip=" << ipar << " SPD=";
      for (int idig = 0; idig < m_nDig; idig++) {
        std::cout << " " << SPD[ipar][idig];
      }
      std::cout << std::endl;
    }
  } // end debugMakeSPD printout
  return iret;
}

// ===================================================================================

int TileFilterManager::makeFitterArrays() {

  if (m_debug)
    std::cout << " TileFilterManager::MakeFitterArrays.  Will print out first matrix "
        << "only for each vFitter vector (one for each value of Nparam)." << std::endl;
  bool cdebug = false;
  int NampMax = m_nParamMax - 1;
  for (int iamp = 0; iamp < NampMax; iamp++) {
    int Nparam = iamp + 2;       // number of parameters in this series of fits.
    int Nindex = m_nFitIndex[iamp];  // number of configurations for this Nparam.
    if (m_debug)
      std::cout << " ===>    Nparam=" << Nparam << " => Nindex=" << Nindex << " TileFitter objects:" << std::endl;
    std::vector<TileFitter> vFitter(Nindex);
    for (int index = 0; index < Nindex; index++) {
      //      if(debug) cdebug = (index==0)||(index==Nindex-1);
      if (m_debug) cdebug = (index == Nindex - 1);
      std::vector<int> vcross;
      getVcross(iamp, index, vcross);
      CLHEP::HepMatrix SPD(Nparam, m_nDig);
      makeSPD(cdebug, vcross, SPD);            // fill the matrix elements
      // If constraints are needed, set Icon.
      int Icon = 0;
      if (Nparam > m_nDig - 1) Icon = 1;
      if (Nparam > m_nDig) Icon = 2;
      TileFitter * tileFitter = new TileFitter(cdebug, Nparam, m_nDig, index, SPD, Icon);
      vFitter[index] = *tileFitter;
      delete tileFitter;
    }
    m_vNpFitter.push_back(vFitter);
  }
  return 0;
}

// ===================================================================================

std::vector<int>& TileFilterManager::getNfitIndex() {
  return m_nFitIndex;
}

// ===================================================================================

void TileFilterManager::getCuts(double& rchisqC, double& chiC) {
  rchisqC = m_rChi2Cut;
  chiC = m_chiCut;
  return;
}

// ===================================================================================

void TileFilterManager::getVcross(int nPileup, int iconfig, std::vector<int>& vcross) {

  vcross.clear();
  int kconfig = iconfig;
  int icrmax = m_nCross - 1;
  for (int ipile = nPileup; ipile > -1; ipile--) {
    int * Offset = m_offsetVector[ipile];
    for (int icross = icrmax; icross >= 0; icross--) {
      //      std::cout << "icross=" << icross << ", Offset[icross]=" << Offset[icross]
      //	   << ", kconfig=" << kconfig << ", icrmax=" << icrmax << std::endl;
      if (Offset[icross] <= kconfig) {
        int icr = icross;
        icrmax = icross - 1;
        kconfig = kconfig - Offset[icross];
        vcross.push_back(icr);
        break;
      }
      if (kconfig < 0) {
        std::cout << " ERROR!! In getVcross, kconfig=" << kconfig << std::endl;
      }
    }
  }
  sort(vcross.begin(), vcross.end());

  return;
}

// ===================================================================================

int TileFilterManager::getFitIndex(int nParam, std::vector<int>& vcross) {

  int Index = 0;
  int Namp = nParam - 1;
  if (Namp <= 0) {
    std::cout << " TileFilterManager.getFitIndex called when Nparam=" << nParam << std::endl;
  }
  for (int ipar = 0; ipar < Namp; ipar++) {
    int * Offset = m_offsetVector[ipar];
    int jcr = vcross[ipar];
    Index += Offset[jcr];
  }

  //  if(debug) {
  //    std::cout << " getFitIndex: Nparam=" << Nparam << ", Index=" << Index
  //	 << ", Vcross=";
  //    for(int ipar=0; ipar<Namp; ipar++) {
  //      std::cout << " " << vcross[ipar];
  //    }
  //    std::cout << std::endl;
  //  }

  return Index;
}

// ===================================================================================

std::vector<double>& TileFilterManager::getFitterErr(int nParam, int iconfig) {
  int ipile = nParam - 2;
  std::vector<TileFitter>& vFitter = m_vNpFitter[ipile];
  TileFitter& tileFitter = vFitter[iconfig];
  std::vector<double>& fitErr = tileFitter.getErr();
  return fitErr;
}

// ===================================================================================
