/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *********************************************************************
// 
// NAME:     muCombUtil.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuComb
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  Utility namespace for LVL2 Combined Muon Reco FEX Algorithm
// *********************************************************************

#ifndef TRIGMUCOMB_MUCOMBUTIL_H 
#define TRIGMUCOMB_MUCOMBUTIL_H

#include <vector>

class MuonFeature;
class TrigInDetTrack;

namespace muCombUtil {

  ///utility vector set
  void   setMuFastRes(std::vector<double>&, double,double,double,double,double,double);
  ///utility vector set
  void   setIDSCANRes(std::vector<double>&,double,double);
  ///Get parametrized muFast 1/pt resolution (extrapolated)
  double getMuFastRes(std::vector<double>, const MuonFeature*);

  ///utility function for getMuFastRes
  int    whichECRegion(const float, const float);

  ///Get parametrized IDSCAN 1/pt resolution
  double getIDSCANRes(std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, double, double);

  ///Get parametrized muFast Eta resolution (extrapolated)
  double getMuFastEtaRes(const MuonFeature*);
  ///Get parametrized muFast Phi resolution (extrapolated)
  double getMuFastPhiRes(const MuonFeature*);
  ///Get parametrized Geant4 Eta resolution (extrapolated)
  double getG4ExtEtaRes(double, double);
  ///Get parametrized Geant4 Phi resolution (extrapolated)
  double getG4ExtPhiRes(double, double);

  ///Get DeltaPhi
  double getDeltaPhi(double, double);
  ///Get DeltaEta
  double getDeltaEta(double, double);
  ///Get DeltaR
  double getDeltaR(double, double, double, double);
  ///Get weighted mean
  double getCombinedAverage(double, double, double, double);
  ///Get sigma of weighted mean
  double getCombinedAverageSigma(double, double);
  ///Get OLD style (i.e. muFast time) Chi2
  double getChi2(int&,   double, 
		 double, double, double, double, double, double, 
		 double, double, double, double, double, double, bool);
  ///Get Std Chi2
  double getStdChi2(int&,
		 double, double, double, double, double, double, 
		 double, double, double, double, double, double, bool);
}
#endif
