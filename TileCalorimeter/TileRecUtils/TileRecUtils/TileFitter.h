/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef TILESIMALGS_TILEFITTER_H
#define TILESIMALGS_TILEFITTER_H

#include "TileRecUtils/TileFilterResult.h"

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include <vector>

using namespace CLHEP;

class StoreGateSvc;

class TileFitter
 {
 public:

   // Constructor
   TileFitter();
   TileFitter(bool debug, int nrow, int ncol, int index, HepMatrix& S, int Icon);

   // Destructor
   ~TileFitter();

   int FitAmp(TileFilterResult &tResult, bool lDebug=false);
   int getIndex();
   std::vector<double>& getErrRef();
   void PrintMat(HepMatrix &mat);
   void PrintVec(HepVector &vec);

 private:

   int Iconstraint;     // >0 => include constraint on Ped; >1 => also pileups.
   HepVector Camp;       // used for maxP fit with constraints only.
   double Ped_const;
   double F2_sigPed;
   double F2_sigPile;

   int ND;               // number of samples.
   int NP;               // number of parameters in fit
   int iconfig;          // number of this configuration.
   std::vector<int> vconfig;  // vector of crossings for this configuration
   HepMatrix SPD;      // NPxND matrix
   //   HepMatrix M;        // = SPD*SPDT
   //   HepMatrix MI;       // = M.inverse
   HepMatrix MISPD;    // = MI*SPD
   std::vector<double> ErrDiag;
 };

#endif // TILERECALGS_TILEFITTER_H


