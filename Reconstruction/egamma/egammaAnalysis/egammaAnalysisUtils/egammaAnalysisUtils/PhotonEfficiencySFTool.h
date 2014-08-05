/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PHOTONEFFICIENCYSFTOOL_H
#define __PHOTONEFFICIENCYSFTOOL_H

/*

  Name: PhotonEfficiencySFTool.h

  This class provide an interface for the photon efficiency
  uncorrelated and correlated uncertainties. Heavily inspired to
  ElectronEfficiencySFTool

*/

#include "TFile.h"
#include "TH2D.h"
#include <string>

class PhotonEfficiencySFTool {

 public:

  PhotonEfficiencySFTool(std::string fileName);
  ~PhotonEfficiencySFTool();

  double GetSFFFMC(double eta, double pt, int isConv);
  double GetEffSyst(double eta, double pt, int isConv);
  double GetIsoSyst(double eta, double pt, int isConv);
  double GetFragSyst(double eta, double pt, int isConv);
  double GetTotSyst(double eta, double pt, int isConv);
  
  double GetCorr(double eta1, double /*pt1*/, int isConv1, 
		 double eta2, double /*pt2*/, int isConv2);

 private:
  
  double getValue(TH2D* h, double eta, double pt, double def);

  TH2D* h_SFFFMC_u;
  TH2D* h_SFFFMC_c;
  TH2D* h_effErr_u;
  TH2D* h_effErr_c;
  TH2D* h_isoErr_u;
  TH2D* h_isoErr_c;
  TH2D* h_fragErr_u;
  TH2D* h_fragErr_c;

  double defSF;
  double defErr;

};

#endif // PhotonEfficiencySFTool
