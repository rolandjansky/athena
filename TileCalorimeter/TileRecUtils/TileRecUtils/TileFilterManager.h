/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECALGS_TILEFILTERMANAGER_H
#define TILERECALGS_TILEFILTERMANAGER_H
// *************************************************************************************
// Filename : TileFilterManager.h
// Author   : F. Merritt, A. Aurisano
// Created  : March 2004

// DESCRIPTION
// Managers filtering of TileDigit objects to produce amplitudes of TileRawChannels.

// ************************************************************************************

#include "TileRecUtils/TileFitter.h"
#include "TileRecUtils/TileFilterResult.h"

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>
#include <string>
#include <vector>

using namespace CLHEP;
/** Auxiliary class for TileRawChannelMakerManyAmps. 
 */
class TileFilterManager 
{
 public:
  // =================================================================================
  //Constructor
  TileFilterManager(int mode, int level, int &nPar, int &nSam, int &inTsam, int &jbsam, int &jesam, 
		    int &ncr, int &intcr, int &jBcr, int &jEcr, std::vector<double> &Shape, bool lDebug=false);
  // =================================================================================
  // Destructor
  ~TileFilterManager();
  // =================================================================================
  // Find the crossing with largest amplitude.
  int FindHighestResidual(HepVector &digits) const;
  // =================================================================================
  // Find the crossing with lowest residual amplitude.
  int FindLowestCrossing(HepVector &digits) const;
  // =================================================================================
  // Fit the in-time amplitude for this digits vector.
  int FitDigits(TileFilterResult &tRes, bool lDebug = false);
  // =================================================================================
  // Fit the in-time amplitude for this digits vector.
  int FitDigits1(TileFilterResult &tRes, bool lDebug = false);
  // =================================================================================
  // Fit the in-time amplitude for this digits vector (alternate method).
  int FitDigits2(TileFilterResult &tRes, bool lDebug = false);
  // =================================================================================
  // Make the table of offsets used to locate Fitter matrices.
  void MakeFitterOffsetTables();
  // =================================================================================
  // Fill the SPD matrix for a new fit configuration.
  int MakeSPD(bool debug, std::vector<int>& vcross, HepMatrix& SPD);
  // =================================================================================
  // Make the TileFitter arrays which hold the solution matrices for each fit
  int MakeFitterArrays();
  // =================================================================================
  // Return the vector of crossing for a given fit index.
  void getVcross(int Npile, int iconfig, std::vector<int>& vcross);
  // =================================================================================
  // Return the cuts used in filtering.
  void getCuts(double& rchisq, double& chi);
  // =================================================================================
  std::vector<int>& get_NfitIndex();
  // =================================================================================
  // Return the FitIndex for a given crossing configuration.
  int getFitIndex(int Npile, std::vector<int>& vcross);
  // =================================================================================
  // Return the FitErr vector for a given crossing configuration.
  std::vector<double>& getFitterErr(int Npile, int iconfig);
  // =================================================================================


 private:

  std::string   m_infoName;
  //  StoreGateSvc* m_storeGate;
  //  StoreGateSvc* m_detStore;

  int Fmode;
  int Flevel;
  bool debug;
  int NParamMax;
  int Nshape;
  int InTshape;
  int Ndig;
  int InTdig;
  int jBsamp;
  int jEsamp;
  int Ncross;
  int InTcross;
  int jBcross;
  int jEcross;
  double rchisqCut;
  double chiCut;

  std::vector<int *> OffsetVector;
  std::vector<int> NfitIndex;
  std::vector<double *> CrossShape;
  int map_ind2icr[9];
  int map_icr2ind[9];
  std::vector<TileFitter *> vNparam;
  std::vector<std::vector<TileFitter> > vNpFitter;
};

#endif // TILERECALGS_TILEFILTERMANAGER_H
