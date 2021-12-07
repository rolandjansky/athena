/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************************************************************
// Filename: TileFilterTester.cxx
// Author:   F. Merritt
// Created:  April 2004

// DESCRIPTION
// This class is to be called from the TileHitToRawChannel initialization section,
// after TileFilterManager has been instantiated.  A reference to TileFilterManager 
// is passed in the call, so it is possible to thoroughly test the program outside 
// of the event loop.
//
// *************************************************************************************************
#include "GaudiKernel/Chrono.h"
//#include "GaudiKernel/ISvcLocator.h"

#include "TileRecUtils/TileFilterTester.h"
#include "TileRecUtils/TileFilterManager.h"
#include "TileRecUtils/TileFitter.h"
#include "TileRecUtils/TileFilterResult.h"
//#include "TileConditions/TileInfo.h"

#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

using namespace CLHEP;

#include <iostream>
#include <iomanip>

// ==============================================================================================

// Constructor
TileFilterTester::TileFilterTester(TileFilterManager* filterManager, int filterMode, int filterTest, bool debug)
  : m_debug(debug)
  , m_filterMode(filterMode)
  , m_filterTest(filterTest)
  , m_cMode(2)
  , m_nCross(9)
  , m_tileFilterManager(filterManager)
{

  std::cout << " Enter TileFilterTester constructor.  April" << std::endl;

  m_nPileup = m_filterTest - 2;
  if (m_nPileup < 0) m_nPileup = 0;
  if (m_nPileup > m_nCross) m_nPileup = m_nCross;
  m_nAmp = m_nPileup + 1;
  m_iAmpVec.reserve(m_nAmp);
  m_ampVec.reserve(m_nAmp);
  m_nConfig = m_tileFilterManager->getNfitIndex();
  if (m_debug) {
    std::cout << " TileFilterTester:  Cmode=" << m_cMode << ", Npileup=" << m_nPileup << std::endl;
    for (int i = 0; i < m_nAmp; i++) {
      std::cout << "    Namp=" << i << " => Nconfig=" << m_nConfig[i] << std::endl;
    }
  }

  return;
}

// Destructor
TileFilterTester::~TileFilterTester() {
}

//
// **********************************************************************************************
//
// Generate events for testing TileFilterManager and related classes.
void TileFilterTester::genEvents(int nEvent) {
  //
  // Set up the vector of Hit amplitudes that will be used to generate fake events.
  // This are indexed according to the "crossing index", 0=intime and 1 to Ncross-1
  // for the pileup amplitudes.  All amps are in RC units.
  //
  // Define the range of amplitudes to be generated.  
  double ApileMin = 30.;
  double ApileMax = 400.;
  double AinTime = 100.;
  double digSigma = 1.6;
  // Define the pileup configuration that will be used to generate events.
  if (m_cMode == 1) {     // Fixed configuration. Must define iAmpVec for first Namp elements.
    m_iAmpVec[0] = 0;
    m_iAmpVec[1] = 2;
    m_iAmpVec[2] = 5;
  }
  bool FSM = true;
  if (FSM) {
    nEvent = 1000000;
  }
  // Initialization for sums.
  int Ngen = 0;
  int Nrec = 0;
  const int ncodeSize = 8;
  int ncode[ncodeSize];
  int ncodeg[ncodeSize];
  int ncodeb[ncodeSize];
  for (int i = 0; i < ncodeSize; i++) {
    ncode[i] = 0;
    ncodeg[i] = 0;
    ncodeb[i] = 0;
  }
  double Dsum = 0.;
  double D2sum = 0.;
  double E2sum = 0.;
  int Nrecg = 0;
  double Dsumg = 0.;
  double D2sumg = 0.;
  double E2sumg = 0.;
  int Nrecb = 0;
  double D2sumb = 0.;
  double E2sumb = 0.;
  int Nover = 0;
  int Nunder = 0;
  int Nmixed = 0;
  int NampFinal[12];

  bool lPrint = m_debug;
  lPrint = true;
  bool lPrint2 = true;
  int nPrint = 0;
  int nPrint2 = 0;
  for (int i = 0; i < 12; i++) {
    NampFinal[i] = 0;
  }
  //
  // ***** Start event loop. *****
  //  std::cout << " Start event loop in TileFilterTester:  Nevent =" << Nevent << std::endl;
  for (int ievent = 0; ievent < nEvent; ievent++) {
    if (ievent > 5) m_debug = false;
    lPrint = m_debug;
    if (ievent == 9) lPrint = true;
    if (ievent == 23) lPrint = true;
    if (ievent == 235) lPrint = true;
    if (ievent == 372) lPrint = true;
    if (ievent == 460) lPrint = true;
    if (ievent == 483) lPrint = true;
    if (ievent == 501) lPrint = true;
    if (ievent == 823) lPrint = true;
    if (ievent == 959) lPrint = true;
    if (ievent == 1220) lPrint = true;
    m_iAmpVec.clear();
    m_iAmpVec.reserve(m_nAmp);
    int iConfig = 0;

    if (lPrint)
      std::cout << " TileFilterTester:  Start new event; ievent=" << ievent << ", Fmode=" << m_filterMode << ", Cmode="
          << m_cMode << std::endl;
    // Generate the pileup configuration.
    if (m_cMode == 2) {
      int Nindex = m_nConfig[m_nPileup];
      double Ranflat = RandFlat::shoot();
      iConfig = (int) (Ranflat * Nindex);
      if (m_nPileup == 0) iConfig = 0; // Special case!
      m_tileFilterManager->getVcross(m_nPileup, iConfig, m_iAmpVec);
    }
    //Generate the amplitudes.
    m_ampVec[0] = AinTime;
    if (m_nPileup > 0) {
      for (int ipileup = 0; ipileup < m_nPileup; ipileup++) {
        double Ranflat = RandFlat::shoot();
        m_ampVec[ipileup + 1] = ApileMin + Ranflat * (ApileMax - ApileMin);
      }
    }

    if (lPrint) {
      for (int iAmp = 0; iAmp < m_nAmp; iAmp++) {
        std::cout << " i=" << iAmp << ", ipar=" << m_iAmpVec[iAmp] << ", Amp=" << m_ampVec[iAmp] << std::endl;
      }
    }
    //Generate the TileDigits amplitudes.
    int Nparam = m_nPileup + 2;
    const int Ndig = 9;
    HepVector digitsHep;
    std::vector<float> digits(Ndig);
    HepVector Param(Nparam);
    HepMatrix SPD(Nparam, Ndig);
    Param[0] = 50.;
    double chisqGen = 0.;
    for (int i = 1; i < Nparam; i++) {
      Param[i] = m_ampVec[i - 1];
    }
    //int iret;
    /*iret =*/m_tileFilterManager->makeSPD(false, m_iAmpVec, SPD);
    HepMatrix SDP = SPD.T();

    digitsHep = SDP * Param;

    //    if(lPrint) std::cout << " digits=";
    // Convert to digits and add noise
    double sigmaGen = 1.6;
    for (int idig = 0; idig < Ndig; idig++) {
      double rang = RandGauss::shoot();
      double noise = rang * sigmaGen;
      chisqGen += rang * rang;
      digits[idig] = digitsHep[idig] + noise;
      //      if(lPrint) std::cout << " " << std::setw(6) << std::setprecision(2) << digitsHep[idig];
    }
    //    if(lPrint) std::cout << std::endl;

    // ***************  Now test the fitting code!  ********************
    TileFilterResult tResult(digits, digSigma);
    int icode = m_tileFilterManager->fitDigits(tResult, lPrint);
    if (lPrint) std::cout << "TileFilterTester.GenEvents: ievent=" << ievent << ", icode=" << icode << std::endl;

    // Compare reconstruction to generation   
    std::vector<int>& vcross = tResult.getVcrossRef();
    int ncrgen = m_iAmpVec.size();
    int ncrrec = vcross.size();
    NampFinal[ncrrec] += 1;
    int& iFitIndex = tResult.getFitIndexRef();
    // Get the theoretical error on the in-time amplitude.
    std::vector<double>& fitterErr = m_tileFilterManager->getFitterErr(Nparam, iConfig);
    double Qerr = sigmaGen * fitterErr[1];

    // Define lconfigOK if reconstructed configuration = generated configuration.
    bool lconfigOK = (ncrrec == ncrgen);
    if (iConfig != iFitIndex) lconfigOK = false;
    if (lconfigOK) {
      for (int icr = 0; icr < ncrgen; icr++) {
        if (m_iAmpVec[icr] != vcross[icr]) lconfigOK = false;
      }
    }
    lPrint2 = false;
    if (!lconfigOK) {
      lPrint2 = true;
      nPrint2 += 1;
      if (nPrint2 > 100) lPrint2 = false;
      if (lPrint2) {
        std::cout << std::endl;
        std::cout << " Discrepancy: ievent=" << ievent << ",  ncrgen=" << ncrgen << ", ncrrec=" << ncrrec << ", icode="
            << icode << "    iConfig=" << iConfig << ", iFitIndex=" << iFitIndex << std::endl;
        std::cout << "        icrGen=";
        for (int i = 0; i < ncrgen; i++) {
          std::cout << " " << m_iAmpVec[i];
        }
        std::cout << "        icrRec=";
        for (int i = 0; i < ncrrec; i++) {
          std::cout << " " << vcross[i];
        }
        double& chisq = tResult.getChi2Ref();
        std::cout << "  (chi2=" << chisq << ", chi2G=" << chisqGen << ")" << std::endl;

        for (int iAmp = 0; iAmp < m_nAmp; iAmp++) {
          std::cout << " i=" << iAmp << ", ipar=" << m_iAmpVec[iAmp] << ", Amp=" << m_ampVec[iAmp] << std::endl;
        }

        nPrint += 1;
        std::cout << "  digits=";
        for (int idig = 0; idig < Ndig; idig++) {
          std::cout << " " << std::setw(6) << std::setprecision(2) << digits[idig];
        }
        std::cout << std::endl;
        tResult.printFitParam();
        tResult.snapShot(2);
        std::cout << std::endl;
        if (nPrint > 10) lPrint = false;
      }
    }
    // Now print out the results for comparison with generated amplitudes.
    if (lPrint) {

      tResult.printFitParam();
      if (nPrint2 > 20) lPrint = false;
    }
    double& chisq = tResult.getChi2Ref();
    //    std::vector<int>& vcross = tResult.getVcrossRef();
    HepVector& fitParam = tResult.getParamRef();
    HepVector& fitErr = tResult.getErrRef();
    double diff_ch = fitParam[1] - m_ampVec[0];
    int Npar = fitParam.num_row();
    Ngen = Ngen + 1;
    //    double err = fitErr[1];
    double err = Qerr;
    Nrec += 1;
    if (icode >= 0) ncode[icode] += 1;
    Dsum += diff_ch;
    D2sum += diff_ch * diff_ch;
    E2sum += err * err;

    if (lconfigOK) {
      Nrecg += 1;
      if (icode >= 0) ncodeg[icode] += 1;
      Dsumg += diff_ch;
      D2sumg += diff_ch * diff_ch;
      E2sumg += err * err;
    } else {
      Nrecb += 1;
      if (ncrrec > ncrgen) Nover += 1;
      if (ncrrec < ncrgen) Nunder += 1;
      if (ncrrec == ncrgen) Nmixed += 1;
      if (icode >= 0) ncodeb[icode] += 1;
      D2sumb += diff_ch * diff_ch;
      E2sumb += err * err;
    } // end of ievent loop.

    if (lPrint)
      std::cout << "TileFilterTester event: Npar=" << Npar << ",  diff_ch =" << std::setw(6) << std::setprecision(2)
          << diff_ch << " +-" << fitErr[1] << ", chi2=" << chisq << ", chi2Gen=" << chisqGen << std::endl << std::endl;
  }
  // Calculate the mean displacement and sigma of the reconstructed events.
  std::cout << std::endl;
  std::cout << " *** TileFilterTester Summary:  Fmode=" << m_filterMode << ", Ftest=" << m_filterTest << ", NparamGen ="
      << m_nPileup + 2 << ", Cmode=" << m_cMode << ", Nevent=" << nEvent << std::endl;
  double rchisqCut;
  double chiCut;
  m_tileFilterManager->getCuts(rchisqCut, chiCut);
  std::cout << " Cuts applied:  rchisqCut=" << rchisqCut << ", chiCut=" << chiCut << std::endl;
  std::cout << "     ApileMin=" << ApileMin << ", ApileMax=" << ApileMax << ", AmpInTime=" << AinTime << std::endl;
  std::cout << " Compare difference (rec-gen) for:   Ngen=" << Ngen << ", Nrec=" << Nrec << std::endl;

  int den = Nrec ? Nrec : 1;
  double rm = Dsum / den;
  double errsig = pow(E2sum / den, 0.5);
  double rsig = pow(D2sum / den, 0.5);

  std::cout << " All  configurations:  N=" << std::setw(7) << Nrec << ",  Diff =" << std::setw(6)
      << std::setprecision(3) << rm << ",  sig =" << rsig << "  (errsig=" << errsig << ")" << std::endl;

  if (Nrecg) {
    double rmg = Dsumg / Nrecg;
    double errsigg = pow(E2sumg / Nrecg, 0.5);
    double rsigg = pow(D2sumg / Nrecg, 0.5);

    std::cout << " Good configurations:  N=" << std::setw(7) << Nrecg << ",  Diff =" << std::setw(6)
        << std::setprecision(3) << rmg << ",  sig =" << rsigg << "  (errsig=" << errsigg << ")" << std::endl;
  }

  if (Nrecb) {
    double rmb = Dsumg / Nrecb;
    double errsigb = pow(E2sumb / Nrecb, 0.5);
    double rsigb = pow(D2sumb / Nrecb, 0.5);

    //if(Nrecb>3)
    std::cout << " Bad  configurations:  N=" << std::setw(7) << Nrecb << ",  Diff =" << std::setw(6)
        << std::setprecision(3) << rmb << ",  sig =" << rsigb << "  (errsig=" << errsigb << ")" << std::endl;
  }

  std::cout << "     Nover=" << Nover << ",  Nunder=" << Nunder << ",  Nmixed=" << Nmixed << std::endl;

  std::cout << std::endl;
  // Print out summary of icode counts:
  for (int i = 0; i < ncodeSize; i++) {
    std::cout << "     icode=" << i << " ==> ncnt=" << std::setw(7) << ncode[i] << ",   ncntg=" << ncodeg[i]
        << ",   ncntb=" << ncodeb[i] << std::endl;
  }
  std::cout << std::endl;
  // Print out number of final amplitudes.
  std::cout << " Number of final amplitudes:" << std::endl;
  for (int i = 0; i < 12; i++) {
    if (NampFinal[i] != 0) std::cout << "     Namp=" << i << "  =>" << NampFinal[i] << " events" << std::endl;
  }

  return;
}
