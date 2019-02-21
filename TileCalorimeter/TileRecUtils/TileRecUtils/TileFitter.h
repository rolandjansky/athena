/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************************************
// Filename :  TileFitter.h
// Author   :  F. Merritt, A. Aurisano
// Created  :  March 2004
//
// Description
//   Stores methods and matrices needed for a fitting a TileDigits object to a specific
//   configuration of energy deposits.
//
//
//***************************************************************************************************

#ifndef TILERECUTILS_TILEFITTER_H
#define TILERECUTILS_TILEFITTER_H

#include "TileRecUtils/TileFilterResult.h"

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include <vector>

//using namespace CLHEP;

//class StoreGateSvc;

class TileFitter
 {
 public:

   // Constructor
   TileFitter();
   TileFitter(bool debug, int nRow, int nColumn, int index, CLHEP::HepMatrix& S, int Icon);

   // Destructor
   ~TileFitter() = default;

   int fitAmp(TileFilterResult &tResult, bool lDebug=false);
   int getIndex();
   std::vector<double>& getErr();
   void printMat(CLHEP::HepMatrix &mat);
   void printVec(CLHEP::HepVector &vec);

 private:

   int m_iConstraint;     // >0 => include constraint on Ped; >1 => also pileups.
   CLHEP::HepVector m_cAmp;       // used for maxP fit with constraints only.
   double m_pedConst;
   double m_sigPedF2;
   double m_sigPileF2;

   int m_nSamples;               // number of samples.
   int m_nParameters;               // number of parameters in fit
   int m_iConfig;          // number of this configuration.
   std::vector<int> m_vConfig;  // vector of crossings for this configuration
   CLHEP::HepMatrix m_SPD;      // m_nParameters X m_nSamples matrix
   //   HepMatrix M;        // = SPD*SPDT
   //   HepMatrix MI;       // = M.inverse
   CLHEP::HepMatrix m_MISPD;    // = MI*SPD
   std::vector<double> m_errDiag;
 };

#endif // TILERECUTILS_TILEFITTER_H


