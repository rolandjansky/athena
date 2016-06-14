/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILEFILTERMANAGER_H
#define TILERECUTILS_TILEFILTERMANAGER_H
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

//using namespace CLHEP;
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
  int findHighestResidual(CLHEP::HepVector &digits) const;
  // =================================================================================
  // Find the crossing with lowest residual amplitude.
  int findLowestCrossing(CLHEP::HepVector &digits) const;
  // =================================================================================
  // Fit the in-time amplitude for this digits vector.
  int fitDigits(TileFilterResult &tRes, bool lDebug = false);
  // =================================================================================
  // Fit the in-time amplitude for this digits vector.
  int fitDigits1(TileFilterResult &tRes, bool lDebug = false);
  // =================================================================================
  // Fit the in-time amplitude for this digits vector (alternate method).
  int fitDigits2(TileFilterResult &tRes, bool lDebug = false);
  // =================================================================================
  // Make the table of offsets used to locate Fitter matrices.
  void makeFitterOffsetTables();
  // =================================================================================
  // Fill the SPD matrix for a new fit configuration.
  int makeSPD(bool debug, std::vector<int>& vcross, CLHEP::HepMatrix& SPD);
  // =================================================================================
  // Make the TileFitter arrays which hold the solution matrices for each fit
  int makeFitterArrays();
  // =================================================================================
  // Return the vector of crossing for a given fit index.
  void getVcross(int nPile, int iconfig, std::vector<int>& vcross);
  // =================================================================================
  // Return the cuts used in filtering.
  void getCuts(double& rchisq, double& chi);
  // =================================================================================
  std::vector<int>& getNfitIndex();
  // =================================================================================
  // Return the FitIndex for a given crossing configuration.
  int getFitIndex(int nParam, std::vector<int>& vcross);
  // =================================================================================
  // Return the FitErr vector for a given crossing configuration.
  std::vector<double>& getFitterErr(int nParam, int iconfig);
  // =================================================================================


 private:

  std::string   m_infoName;
  //  StoreGateSvc* m_storeGate;
  //  StoreGateSvc* m_detStore;

  int m_filterMode;
  int m_filterLevel;
  bool m_debug;
  int m_nParamMax;
  int m_nShape;
  int m_inTshape;
  int m_nDig;
  int m_inTdig;
  int m_jBsamp;
  int m_jEsamp;
  int m_nCross;
  int m_inTcross;
  int m_jBcross;
  int m_jEcross;
  double m_rChi2Cut;
  double m_chiCut;

  std::vector<int *> m_offsetVector;
  std::vector<int> m_nFitIndex;
  std::vector<double *> m_crossShape;
  int m_ind2icrMap[9];
  int m_icr2indMap[9];
  std::vector<TileFitter *> m_vNparam;
  std::vector<std::vector<TileFitter> > m_vNpFitter;
};

#endif // TILERECUTILS_TILEFILTERMANAGER_H
