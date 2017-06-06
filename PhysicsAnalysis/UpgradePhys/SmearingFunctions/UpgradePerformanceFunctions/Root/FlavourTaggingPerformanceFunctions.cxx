/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FLAVOURTAGGINGPERFORMANCEFUNCTIONS_CXX
#define FLAVOURTAGGINGPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

#include <map>
using std::map;

#include <TF2.h>
#include <TFile.h>

#ifdef ROOTCORE
// Framework include(s):
#include "PathResolver/PathResolver.h"
#endif // ROOTCORE

void UpgradePerformanceFunctions::setFlavourTaggingCalibrationFilename(TString flavourTaggingCalibrationFilename) {
  std::cout << "Flavour tagging histogram input filename is " 
	    << flavourTaggingCalibrationFilename << std::endl;
  std::string file = flavourTaggingCalibrationFilename.Data();
#ifdef ROOTCORE
  // Get file from data path
  file = PathResolverFindCalibFile(file);
  std::cout << "Found flavour tagging histogram file: " << file << std::endl;
#endif // ROOTCORE
  m_flavourTaggingCalibrationFilename = file;
}

float UpgradePerformanceFunctions::getFlavourTagEfficiency(float ptMeV, float eta, char flavour, TString tagger, int operating_point, bool track_confirmation) {
  double ptGeV = ptMeV/1000.;

  // the function returns b/c/l-tagging efficincies obtained using ttbar samples

  // flavour is a char: IN CAPS!
  // 'B' (to get the b-tag efficiency) 
  // 'C' (to get the c-tag efficiency) 
  // 'L' (to get the mistag rate)
  // 'P' (to get the PU-tag efficiency)

  // pT range protection
  //  if (ptGeV<20) ptGeV = 20;
  if (ptGeV<20) return 0.;

  // high pT approximation
  double highpt_factor = 1;
  if (ptGeV>300) {
    double x = ptGeV; if (x>1000) x = 1000;
    if (tagger=="mv1_flat_high_pt") {
      if (flavour=='C') {
	highpt_factor = 1.6070808467835-0.00243838298663*x+1.4234528100804e-06*x*x-1.3617421117693e-10*x*x*x;
      } else if (flavour=='L' || flavour=='P') {
	highpt_factor = 7.5679905186556-0.029707488878509*x+2.8845677013756e-05*x*x-9.327955049668e-09*x*x*x;
	highpt_factor = exp(highpt_factor-1);
      }
      highpt_factor = 1./highpt_factor;
    } else if (tagger=="mv1") {
      double p1 = 0;
      if (flavour=='B') p1 = -6.17434e-04;
      else if (flavour=='C') p1 = -7.20231e-04;
      else /* L,P */ p1 = 2.78547e-04;
      highpt_factor = 1 + p1*(x-300);
    }
  }
  if (tagger=="mv1_flat_high_pt") tagger = "mv1";

  // load the parameterization
  // see r-tags list at https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AtlasProductionGroupMC12Upgrade
  const int nopt = 7;
  const char* chopt[nopt] = {
    "mu200_sc6226",
    "mu200_sc6285",
    "mu200_sc6284",
    "mu140_sc6153",
    "mu140_sc6295",
    "mu140_sc6294",
    "mu80_sc6296"
  };

  static double etamax[nopt] = {
    4,3.3,2.7,4,3.3,2.7,2.7
  };

  int ipar;
  if      (m_avgMu==200 && m_layout==gold  ) ipar = 0;
  else if (m_avgMu==200 && m_layout==silver) ipar = 1;
  else if (m_avgMu==200 && m_layout==bronze) ipar = 2;
  else if (m_avgMu==140 && m_layout==gold  ) ipar = 3;
  else if (m_avgMu==140 && m_layout==silver) ipar = 4;
  else if (m_avgMu==140 && m_layout==bronze) ipar = 5;
  else if (m_avgMu==80  && m_layout==bronze) ipar = 6;
  else {
    static bool first = true;
    if (first) {
      std::cout << "flavor tagging for mu=" << m_avgMu << ", layout=" << m_layout << " is not implemented" << std::endl;
      first = false;
    }
    return 0;
  }
  int iopt = ipar;

  // eta range protection
  if (eta<0) eta = -eta;
  if (eta>etamax[iopt]) return 0;

  const int ntag = 3;
  ipar *= ntag;
  if (tagger=="comb") { /* ipar += 0; */ }
  else if (tagger=="mv1")  ipar += 1;
  else if (tagger=="ip3d") ipar += 2;
  else {
    static bool first = true;
    if (first) {
      std::cout << "tagger \"" << tagger << "\" is not implemented" << std::endl;
      first = false;
    }
    return 0;
  }

  const int nopp = 2;
  ipar *=  nopp;
  if (operating_point==70) { /* ipar += 0; */ }
  else if (operating_point==85)  ipar += 1;
  else {
    static bool first = true;
    if (first) {
      std::cout << "operating point " << operating_point << " for tagger \"" << tagger << "\" is not implemented" << std::endl;
      first = false;
    }
    return 0;
  }

  const int ntc = 2;
  ipar *= ntc;
  if (track_confirmation) ++ipar;

  const int nfl = 4; // 0='L', 1='C', 2='B', 3='P'

  // load parameterizations
  static TF2* fitfun[nopt*ntag*nopp*ntc*nfl] = {0};
  static TFile* ff = 0;
  int ix = ipar*nfl;
  std::string calibFile = m_flavourTaggingCalibrationFilename.Data(); //"flavor_tags_v0.3.root";
  if (fitfun[ix]==0) {
    // load new parameterization
    std::cout << "Loading " << calibFile << std::endl;
    if (ff==0) ff = new TFile(calibFile.c_str(), "READ");
    for (int jfl = 0; jfl<nfl; ++jfl) {
      TString sff = "fitfun_"; sff += chopt[iopt]; sff += "_"; sff += tagger;
      if (track_confirmation) sff += "_TC";
      if (operating_point==85) sff += "_085";
      sff += "_"; sff += char('0'+jfl);
      std::cout << "Load <" << sff << ">" << std::endl;
      fitfun[ix+jfl] = (TF2*)ff->Get(sff);
    }
  }

  // load additional pt limits
  static double pt_limits[nfl] = {0};
  if (pt_limits[0]==0) {
    TF1* fun_pt_limits = (TF1*)ff->Get("pt_limits");
    if (fun_pt_limits) {
      std::cout << "Loading additional pT limits:";
      for (int i = 0; i<nfl; ++i) std::cout << " " << (pt_limits[i] = fun_pt_limits->Eval(i));
      std::cout << std::endl;
    } else {
      std::cout << "No additional pT limits, using default" << std::endl;
      for (int i = 0; i<nfl; ++i) pt_limits[i] = 300; // GeV
    }
  }
  if (flavour=='L' && ptGeV>pt_limits[0]) ptGeV = pt_limits[0];
  if (flavour=='C' && ptGeV>pt_limits[1]) ptGeV = pt_limits[1];
  if (flavour=='B' && ptGeV>pt_limits[2]) ptGeV = pt_limits[2];
  if (flavour=='P' && ptGeV>pt_limits[3]) ptGeV = pt_limits[3];

  // calculate the efficiency
  float eff = -1;
  if (flavour=='L') eff = fitfun[ix]->Eval(ptGeV,eta);
  else if (flavour=='C') eff = fitfun[ix+1]->Eval(ptGeV,eta);
  else if (flavour=='B') eff = fitfun[ix+2]->Eval(ptGeV,eta);
  else if (flavour=='P') eff = fitfun[ix+3]->Eval(ptGeV,eta);

  return eff*highpt_factor;
}

#endif
