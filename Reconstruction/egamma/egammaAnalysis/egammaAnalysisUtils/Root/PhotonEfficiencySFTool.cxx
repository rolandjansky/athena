/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/PhotonEfficiencySFTool.h"

#include <string>
#include <iostream>

#include <TMath.h>
#include <TROOT.h>
#include <TAxis.h>
#include <TRandom3.h>


PhotonEfficiencySFTool::PhotonEfficiencySFTool(std::string fileName) :
  h_SFFFMC_u(NULL),
  h_SFFFMC_c(NULL), 
  h_effErr_u(NULL),
  h_effErr_c(NULL), 
  h_isoErr_u(NULL),
  h_isoErr_c(NULL),
  h_fragErr_u(NULL),
  h_fragErr_c(NULL)
{

  // Open systematic uncertainty file

  TFile* file = new TFile(fileName.c_str());
  if (!file) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", (fileName+" not found").c_str() );
    return;
  } else {
    Info("PhotonEfficiencySFTool::PhotonEfficiencySFTool", ("Open root file: "+ fileName).c_str() );
  }

  // Read scale factors and uncertainties

  h_SFFFMC_u = (TH2D*)file->Get( "eff/h_SFFFMC_u" );
  h_SFFFMC_c = (TH2D*)file->Get( "eff/h_SFFFMC_c" );
  if (!h_SFFFMC_u) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: SFFFMC/UNCONV not found");
    return;
  }
  if (!h_SFFFMC_c) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: SFFFMC/CONV   not found");
    return;
  }
  h_SFFFMC_u->SetDirectory(gROOT);
  h_SFFFMC_c->SetDirectory(gROOT);

  // data-driven uncertainties

  h_effErr_u = (TH2D*)file->Get( "eff/h_efferr_u" );
  h_effErr_c = (TH2D*)file->Get( "eff/h_efferr_c" );
  if (!h_effErr_u) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: EffErr/UNCONV not found");
    return;
  }
  if (!h_effErr_c) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: EffErr/CONV   not found");
    return;
  }
  h_effErr_u->SetDirectory(gROOT);
  h_effErr_c->SetDirectory(gROOT);

  // isolation systematic uncertainty

  h_isoErr_u = (TH2D*)file->Get( "iso/h_isoerr_u" );
  h_isoErr_c = (TH2D*)file->Get( "iso/h_isoerr_c" );
  if (!h_isoErr_u) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: IsoErr/UNCONV not found");
    return;
  }
  if (!h_isoErr_c) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: IsoErr/CONV   not found");
    return;
  }
  h_isoErr_u->SetDirectory(gROOT);
  h_isoErr_c->SetDirectory(gROOT);

  // hard/fragmentation photon systematic uncertainty

  h_fragErr_u = (TH2D*)file->Get( "frag/h_fragerr_u" );
  h_fragErr_c = (TH2D*)file->Get( "frag/h_fragerr_c" );
  if (!h_fragErr_u) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: FragErr/UNCONV not found");
    return;
  }
  if (!h_fragErr_c) {
    Error("PhotonEfficiencySFTool::PhotonEfficiencySFTool", "Histogram: FragErr/CONV   not found");
    return;
  }
  h_fragErr_u->SetDirectory(gROOT);
  h_fragErr_c->SetDirectory(gROOT);


  file->Close();
  delete file;
  
  defSF  = 1.;
  defErr = -1.;

}


PhotonEfficiencySFTool::~PhotonEfficiencySFTool()
{
  // garbage collection
  delete h_SFFFMC_u;
  delete h_SFFFMC_c; 
  delete h_effErr_u;
  delete h_effErr_c; 
  delete h_isoErr_u;
  delete h_isoErr_c;
  delete h_fragErr_u;
  delete h_fragErr_c;
}


double PhotonEfficiencySFTool::GetSFFFMC(double eta, double pt, int isConv)
{
  if (!h_SFFFMC_u || !h_SFFFMC_c )     
    return defSF;
  switch (isConv) {
  case 0:
    return getValue(h_SFFFMC_u,eta,pt,defSF);
  case 1:
    return getValue(h_SFFFMC_c,eta,pt,defSF);
  default:
    return defSF;
  }
}

double PhotonEfficiencySFTool::GetEffSyst(double eta, double pt, int isConv)
{
  if (!h_effErr_u || !h_effErr_c )     
    return defErr;
  switch (isConv) {
  case 0:
    return getValue(h_effErr_u,eta,pt,defErr);
  case 1:
    return getValue(h_effErr_c,eta,pt,defErr);
  default:
    return defErr;
  }
}

double PhotonEfficiencySFTool::GetIsoSyst(double eta, double pt, int isConv)
{
  if (!h_isoErr_u || !h_isoErr_c )     
    return defErr;
  switch (isConv) {
  case 0:
    return getValue(h_isoErr_u,eta,pt,defErr);
  case 1:
    return getValue(h_isoErr_c,eta,pt,defErr);
  default:
    return defErr;
  }
}

double PhotonEfficiencySFTool::GetFragSyst(double eta, double pt, int isConv)
{
  if (!h_fragErr_u || !h_fragErr_c )     
    return defErr;
  switch (isConv) {
  case 0:
    return getValue(h_fragErr_u,eta,pt,defErr);
  case 1:
    return getValue(h_fragErr_c,eta,pt,defErr);
  default:
    return defErr;
  }
}


double PhotonEfficiencySFTool::GetTotSyst(double eta, double pt, int isConv)
{
  double effErr = this->GetEffSyst(eta,pt,isConv);
  double isoErr = this->GetIsoSyst(eta,pt,isConv);
  double fragErr = this->GetFragSyst(eta,pt,isConv);
  if ( effErr==defErr || isoErr==defErr || fragErr==defErr ) return defErr;   
  return sqrt( effErr*effErr + isoErr*isoErr + fragErr*fragErr ); // uncorrelated
}


double PhotonEfficiencySFTool::getValue(TH2D* h, double eta, double pt, double def) 
{
  if (!h) return def;

  eta = fabs(eta); // assume symmetric detector
  pt  = pt/1000.;  // convert MeV to GeV

  // Check eta range 
  if ( eta < h->GetXaxis()->GetXmin() || eta >= h->GetXaxis()->GetXmax()) return def; 

  // Handle pT values above and below the range
  if ( pt <  h->GetYaxis()->GetXmin() ) pt = h->GetYaxis()->GetXmin();
  if ( pt >= h->GetYaxis()->GetXmax() ) pt = h->GetYaxis()->GetXmax()-1e-6; // subtract 1 eV in case we are above or equal to upper pT boundary
  
  //find bins in eta and pt
  int bin = h->FindBin(eta,pt);
  return h->GetBinContent(bin);
}

double PhotonEfficiencySFTool::GetCorr(double eta1, double /*pt1*/, int isConv1, 
				       double eta2, double /*pt2*/, int isConv2) 
{  
  if (isConv1 != isConv2) return 0.; // unconverted and converted are considered uncorrelated

  // Find bins in eta. Ignore pt bin (assumed always correlated, if in same eta region)
  // histogram does not matter: they all have the same binning
  int bin1 = h_SFFFMC_u->FindBin(eta1,1);
  int bin2 = h_SFFFMC_u->FindBin(eta2,1);

  if (bin1 == bin2 ) // same eta region
    return 1.;
  else
    return 0.;  
}
