/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPhotonMCShifterTool_h
#define TPhotonMCShifterTool_h

/* @author Giovanni Marchiori <giovanni.marchiori@cern.ch>
 *
 *  Reimplementation of FudgeMCTool with photon fudge factors
 *  stored in ROOT files instead of a C++ file
 *
 */

#include <iostream>
#include <stdio.h>
#include <cmath>
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TSystem.h"

namespace IDVAR
{
	const int RHAD1  = 0;
	const int RHAD   = 1;
	const int E277   = 2;
	const int RETA   = 3;
	const int RPHI   = 4;
	const int WETA2  = 5;
	const int F1     = 6;
	const int FSIDE  = 7;
	const int WTOT   = 8;
	const int W1     = 9;
	const int DE     = 10;
	const int ERATIO = 11;
}


class TPhotonMCShifterTool
{

 public:

  TPhotonMCShifterTool();
  ~TPhotonMCShifterTool();

  //fudge shower shapes for preselection chosen.
  void FudgeShowers( double pt     ,
		     double eta2   ,
		     double& rhad1  ,
		     double& rhad   ,
		     double& e277   ,
		     double& reta   ,
		     double& rphi   ,
		     double& weta2  ,
		     double& f1     ,
		     double& fside  ,
		     double& wtot   ,
		     double& w1     ,
		     double& deltae ,
		     double& eratio ,
		     int    isConv  ,
		     int    preselection=-999);

  //fudge shower shapes for preselection chosen.
  void FudgeShowers( float pt     ,
		     float eta2   ,
		     float& rhad1  ,
		     float& rhad   ,
		     float& e277   ,
		     float& reta   ,
		     float& rphi   ,
		     float& weta2  ,
		     float& f1     ,
		     float& fside  ,
		     float& wtot   ,
		     float& w1     ,
		     float& deltae ,
		     float& eratio ,
		     int    isConv  ,
		     int    preselection);

  //fudge shower shapes for previously-loaded preselection.
  void FudgeShowers( float pt     ,
		     float eta2   ,
		     float& rhad1  ,
		     float& rhad   ,
		     float& e277   ,
		     float& reta   ,
		     float& rphi   ,
		     float& weta2  ,
		     float& f1     ,
		     float& fside  ,
		     float& wtot   ,
		     float& w1     ,
		     float& deltae ,
		     float& eratio ,
		     int    isConv) const;

  // fudge showers using D3PD vectors (except for eratio)
  void FudgeShowers( std::vector<float> clE,
		     std::vector<float> eta2   ,
		     std::vector<float>& rhad1  ,
		     std::vector<float>& rhad   ,
		     std::vector<float>& e277   ,
		     std::vector<float>& reta   ,
		     std::vector<float>& rphi   ,
		     std::vector<float>& weta2  ,
		     std::vector<float>& f1     ,
		     std::vector<float>& fside  ,
		     std::vector<float>& wtot   ,
		     std::vector<float>& w1     ,
		     std::vector<float>& deltae ,
		     std::vector<float>& eratio ,
		     std::vector<int> isConv  ,
		     int    preselection=-999);

  /** Calculate Eratio (the only variable which is not stored in the egamma/PhotonD3PD)
    * given emaxs1 and Emax2. Return a pointer to a vector<float>
    **/
  static std::vector<float>* getEratio(std::vector<float> emaxs1, std::vector<float> Emax2)
  {
      std::vector<float> *eratio = new std::vector<float>();
      for (unsigned int i = 0; i < emaxs1.size(); ++i)
	eratio->push_back(emaxs1[i] + Emax2[i] == 0 ? 0 :
			  (emaxs1[i] - Emax2[i])/(emaxs1[i] + Emax2[i]));
      return eratio;
  }

  // get fudge factor for predefined preselection
  double GetFF_Rhad1 (double pt, double eta2, int conv) const {
    if (conv)
    { return h_c_rhad1->GetBinContent(h_u_rhad1->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_rhad1->GetBinContent(h_u_rhad1->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Rhad  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_rhad->GetBinContent(h_c_rhad->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_rhad->GetBinContent(h_u_rhad->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_E277  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_e277->GetBinContent(h_c_e277->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_e277->GetBinContent(h_u_e277->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Reta  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_reta->GetBinContent(h_c_reta->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_reta->GetBinContent(h_u_reta->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Rphi  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_rphi->GetBinContent(h_c_rphi->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_rphi->GetBinContent(h_u_rphi->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Weta2 (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_weta2->GetBinContent(h_c_weta2->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_weta2->GetBinContent(h_u_weta2->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_F1    (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_f1->GetBinContent(h_c_f1->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_f1->GetBinContent(h_u_f1->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_DE    (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_de->GetBinContent(h_c_de->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_de->GetBinContent(h_u_de->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Eratio(double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_eratio->GetBinContent(h_c_eratio->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_eratio->GetBinContent(h_u_eratio->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Fside (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_fside->GetBinContent(h_c_fside->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_fside->GetBinContent(h_u_fside->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_Wtot  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_wtot->GetBinContent(h_c_wtot->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_wtot->GetBinContent(h_u_wtot->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF_W1    (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_w1->GetBinContent(h_c_w1->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_w1->GetBinContent(h_u_w1->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Rhad1 (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_rhad1->GetBinError(h_u_rhad1->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_rhad1->GetBinError(h_u_rhad1->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Rhad  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_rhad->GetBinError(h_c_rhad->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_rhad->GetBinError(h_u_rhad->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_E277  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_e277->GetBinError(h_c_e277->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_e277->GetBinError(h_u_e277->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Reta  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_reta->GetBinError(h_c_reta->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_reta->GetBinError(h_u_reta->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Rphi  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_rphi->GetBinError(h_c_rphi->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_rphi->GetBinError(h_u_rphi->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Weta2 (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_weta2->GetBinError(h_c_weta2->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_weta2->GetBinError(h_u_weta2->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_F1    (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_f1->GetBinError(h_c_f1->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_f1->GetBinError(h_u_f1->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_DE    (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_de->GetBinError(h_c_de->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_de->GetBinError(h_u_de->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Eratio(double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_eratio->GetBinError(h_c_eratio->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_eratio->GetBinError(h_u_eratio->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Fside (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_fside->GetBinError(h_c_fside->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_fside->GetBinError(h_u_fside->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_Wtot  (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_wtot->GetBinError(h_c_wtot->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_wtot->GetBinError(h_u_wtot->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFFerr_W1    (double pt, double eta2, int conv) const {
    if (conv)
      { return h_c_w1->GetBinError(h_c_w1->FindFixBin(pt, fabs(eta2))); }
    else
      { return h_u_w1->GetBinError(h_u_w1->FindFixBin(pt, fabs(eta2))); }
  }

  double GetFF (int var, double pt, double eta2, int conv) const {
    switch (var) {
    case IDVAR::RHAD1: return GetFF_Rhad1( pt, eta2, conv );
    case IDVAR::RHAD: return GetFF_Rhad( pt, eta2, conv );
    case IDVAR::E277: return GetFF_E277( pt, eta2, conv );
    case IDVAR::RETA: return GetFF_Reta( pt, eta2, conv );
    case IDVAR::RPHI: return GetFF_Rphi( pt, eta2, conv );
    case IDVAR::WETA2: return GetFF_Weta2( pt, eta2, conv );
    case IDVAR::F1: return GetFF_F1( pt, eta2, conv );
    case IDVAR::FSIDE: return GetFF_Fside( pt, eta2, conv );
    case IDVAR::WTOT: return GetFF_Wtot( pt, eta2, conv );
    case IDVAR::W1: return GetFF_W1( pt, eta2, conv );
    case IDVAR::DE: return GetFF_DE( pt, eta2, conv );
    case IDVAR::ERATIO: return GetFF_Eratio( pt, eta2, conv );
    default: return 0.0;
    }
  }

  // fudge a specific variable
  double Fudge_Rhad1 ( double rhad1,  double pt, double eta2, int conv) const { return ( rhad1  + GetFF_Rhad1  ( pt, eta2, conv ) ); }
  double Fudge_Rhad  ( double rhad,   double pt, double eta2, int conv) const { return ( rhad   + GetFF_Rhad   ( pt, eta2, conv ) ); }
  double Fudge_E277  ( double e277,   double pt, double eta2, int conv) const { return ( e277   + GetFF_E277   ( pt, eta2, conv ) ); }
  double Fudge_Reta  ( double reta,   double pt, double eta2, int conv) const { return ( reta   + GetFF_Reta   ( pt, eta2, conv ) ); }
  double Fudge_Rphi  ( double rphi,   double pt, double eta2, int conv) const { return ( rphi   + GetFF_Rphi   ( pt, eta2, conv ) ); }
  double Fudge_Weta2 ( double weta2,  double pt, double eta2, int conv) const { return ( weta2  + GetFF_Weta2  ( pt, eta2, conv ) ); }
  double Fudge_F1    ( double f1,     double pt, double eta2, int conv) const { return ( f1     + GetFF_F1     ( pt, eta2, conv ) ); }
  double Fudge_DE    ( double deltae, double pt, double eta2, int conv) const { return ( deltae + GetFF_DE     ( pt, eta2, conv ) ); }
  double Fudge_Eratio( double eratio, double pt, double eta2, int conv) const { return ( eratio + GetFF_Eratio ( pt, eta2, conv ) ); }
  double Fudge_Fside ( double fside,  double pt, double eta2, int conv) const { return ( fside  + GetFF_Fside  ( pt, eta2, conv ) ); }
  double Fudge_Wtot  ( double wtot,   double pt, double eta2, int conv) const { return ( wtot   + GetFF_Wtot   ( pt, eta2, conv ) ); }
  double Fudge_W1    ( double w1,     double pt, double eta2, int conv) const { return ( w1     + GetFF_W1     ( pt, eta2, conv ) ); }

  // set shower preselection cuts
  // *** 2010
  // 0 = tight  isolated
  // 1 = loose  isolated
  // 2 = tightx isolated
  // 3 = tighty isolated
  // 4 = tight  isolated . Distorted material
  // *** 2011
  // 5 = tight  isolated
  // 6 = tight  non-isolated
  // ***
  // 10= tight  isolated. Old menu rel15 (tune 3 in PhotonIDTool).
  // ..
  void SetVerbose( bool verbose=true ){ m_verbose=verbose; }
  int  GetPreselection(){ return m_preselection; };

  //drawing methods
  TGraphErrors* GetFFmap       (int var, double eta, int isConv, int preselection);

  TGraphErrors* GetFFmap_Rhad1 (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Rhad  (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_E277  (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Reta  (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Rphi  (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Weta2 (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_F1    (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Fside (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Wtot  (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_W1    (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_DE    (double eta, int isConv, int preselection);
  TGraphErrors* GetFFmap_Eratio(double eta, int isConv, int preselection);
  //  TH2D* GetFFTH2D(int var, int isConv, int preselection);
  void LoadFFs(int preselection, const std::string& file);

 private:
  bool m_verbose;

  //shower preselection to extract FFs
  int m_preselection;

  std::string m_corr_file;

  // collections of fudge factors
  TH2D* h_u_rhad1;
  TH2D* h_u_rhad;
  TH2D* h_u_e277;
  TH2D* h_u_reta;
  TH2D* h_u_rphi;
  TH2D* h_u_weta2;
  TH2D* h_u_f1;
  TH2D* h_u_fside;
  TH2D* h_u_wtot;
  TH2D* h_u_w1;
  TH2D* h_u_de;
  TH2D* h_u_eratio;
  TH2D* h_c_rhad1;
  TH2D* h_c_rhad;
  TH2D* h_c_e277;
  TH2D* h_c_reta;
  TH2D* h_c_rphi;
  TH2D* h_c_weta2;
  TH2D* h_c_f1;
  TH2D* h_c_fside;
  TH2D* h_c_wtot;
  TH2D* h_c_w1;
  TH2D* h_c_de;
  TH2D* h_c_eratio;

  void SafeDeleteHistograms();
};

#endif // #ifdef TPhotonMCShifterTool_h

#ifdef TPhotonMCShifterTool_cxx

TPhotonMCShifterTool::TPhotonMCShifterTool()
{
  m_preselection = -1;
  h_u_rhad1 = 0;
  h_u_rhad = 0;
  h_u_e277 = 0;
  h_u_reta = 0;
  h_u_rphi = 0;
  h_u_weta2 = 0;
  h_u_f1 = 0;
  h_u_fside = 0;
  h_u_wtot = 0;
  h_u_w1 = 0;
  h_u_de = 0;
  h_u_eratio = 0;
  h_c_rhad1 = 0;
  h_c_rhad = 0;
  h_c_e277 = 0;
  h_c_reta = 0;
  h_c_rphi = 0;
  h_c_weta2 = 0;
  h_c_f1 = 0;
  h_c_fside = 0;
  h_c_wtot = 0;
  h_c_w1 = 0;
  h_c_de = 0;
  h_c_eratio = 0;

  m_verbose = false;
  //  std::cout << "Initiliazing the tool ---->>> preselection " << preselection << std::endl;

}

TPhotonMCShifterTool::~TPhotonMCShifterTool()
{
  SafeDeleteHistograms();
}

void TPhotonMCShifterTool::SafeDeleteHistograms() {
  if (h_u_rhad1) {delete h_u_rhad1; h_u_rhad1 = 0;}
  if (h_u_rhad) {delete h_u_rhad; h_u_rhad = 0;}
  if (h_u_e277) {delete h_u_e277; h_u_e277 = 0;}
  if (h_u_reta) {delete h_u_reta; h_u_reta = 0;}
  if (h_u_rphi) {delete h_u_rphi; h_u_rphi = 0;}
  if (h_u_weta2) {delete h_u_weta2; h_u_weta2 = 0;}
  if (h_u_f1) {delete h_u_f1; h_u_f1 = 0;}
  if (h_u_fside) {delete h_u_fside; h_u_fside = 0;}
  if (h_u_wtot) {delete h_u_wtot; h_u_wtot = 0;}
  if (h_u_w1) {delete h_u_w1; h_u_w1 = 0;}
  if (h_u_de) {delete h_u_de; h_u_de = 0;}
  if (h_u_eratio) {delete h_u_eratio; h_u_eratio = 0;}
  if (h_c_rhad1) {delete h_c_rhad1; h_c_rhad1 = 0;}
  if (h_c_rhad) {delete h_c_rhad; h_c_rhad = 0;}
  if (h_c_e277) {delete h_c_e277; h_c_e277 = 0;}
  if (h_c_reta) {delete h_c_reta; h_c_reta = 0;}
  if (h_c_rphi) {delete h_c_rphi; h_c_rphi = 0;}
  if (h_c_weta2) {delete h_c_weta2; h_c_weta2 = 0;}
  if (h_c_f1) {delete h_c_f1; h_c_f1 = 0;}
  if (h_c_fside) {delete h_c_fside; h_c_fside = 0;}
  if (h_c_wtot) {delete h_c_wtot; h_c_wtot = 0;}
  if (h_c_w1) {delete h_c_w1; h_c_w1 = 0;}
  if (h_c_de) {delete h_c_de; h_c_de = 0;}
  if (h_c_eratio) {delete h_c_eratio; h_c_eratio = 0;}
}

#endif// #ifdef TPhotonMCShifterTool_cxx

