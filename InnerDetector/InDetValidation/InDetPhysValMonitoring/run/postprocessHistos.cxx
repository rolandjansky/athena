/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*==============================================================================

Regenerate 1D profile plots or histograms from 2D histograms (TH2D) in an 
existing InDetStandardPlots.root. The file is updated with new histograms 
overriding the old ones).

This can be used to fix up histogram files after merging InDetStandardPlots.root
files using hadd (eg. during Grid validation).

Some of this code is copied from
InnerDetector/InDetValidation/InDetPerformanceRTT/src/IDStandardPerformance.cxx
create_registeredTH1F (etc) reads in existing histograms rather than creating 
new ones.

Usage (from ROOT command):

  root -l -b -q InDetStandardPlots.root postprocessHistos.cxx+

Usage (after compiling and linking against ROOT libraries):

  ./postprocessHistos InDetStandardPlots.root

Author: Soeren Prell

==============================================================================*/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"
#include "TFitResult.h"
#include "TROOT.h"
#include "TList.h"
#include "TKey.h"
#include "TClass.h"
#include "TObject.h"
#include "TObjString.h"
using std::string; 
using std::cout;
using std::cerr;
using std::endl;

class MonGroup;

class IDStandardPerformance {
public:
  IDStandardPerformance(TDirectory* top,
                        const char* tracksName="Tracks",
                        const char* histDirectoryName="SelectedGoodTracks",
                        int         verbose=0,
                        bool        plotsVsAbsEta=false)
      : m_top(top),
        m_verbose(verbose),
        m_tracksName(tracksName),
        m_histDirectoryName(histDirectoryName),
        m_trackPtBins(10),
        m_trackEtaBins(20),
        m_maxTrackEta(2.5),
        m_plotsVsAbsEta(plotsVsAbsEta)
  {
    m_owned.SetOwner();
  }

  virtual ~IDStandardPerformance() {};
  virtual void bookHistograms( bool isNewRun = true );
  virtual void procHistograms( bool isEndOfRun = true );
  virtual int  updateFile();
  TDirectory* GetDirectory() { return m_top; }
  void Add    (TObject *obj) { m_owned .Add(obj); }
  void Update (TObject *obj) { m_update.Add(obj); }

private:
  // 2D histograms
  TH2D* m_d0_vs_eta;
  TH2D* m_z0_vs_eta;
  TH2D* m_phi_vs_eta;
  TH2D* m_theta_vs_eta;
  TH2D* m_z0st_vs_eta;
  TH2D* m_qopt_vs_eta;

  TH2D* m_d0_vs_pt;
  TH2D* m_z0_vs_pt;
  TH2D* m_phi_vs_pt;
  TH2D* m_theta_vs_pt;
  TH2D* m_z0st_vs_pt;
  TH2D* m_qopt_vs_pt;

  TH2D* m_d0pull_vs_eta;
  TH2D* m_z0pull_vs_eta;
  TH2D* m_phipull_vs_eta;
  TH2D* m_thetapull_vs_eta;
  TH2D* m_z0stpull_vs_eta;
  TH2D* m_qoptpull_vs_eta;

  // summary plots of track parameter resolutions and means versus eta
  TH1D* m_d0_mean_vs_eta;
  TH1D* m_phi_mean_vs_eta;
  TH1D* m_qopt_mean_vs_eta;
  TH1D* m_z0_mean_vs_eta;
  TH1D* m_theta_mean_vs_eta;
  TH1D* m_z0st_mean_vs_eta;

  TH1D* m_d0_width_vs_eta;
  TH1D* m_phi_width_vs_eta;
  TH1D* m_qopt_width_vs_eta;
  TH1D* m_z0_width_vs_eta;
  TH1D* m_theta_width_vs_eta;
  TH1D* m_z0st_width_vs_eta;

  TH1D* m_d0pull_mean_vs_eta;
  TH1D* m_phipull_mean_vs_eta;
  TH1D* m_qoptpull_mean_vs_eta;
  TH1D* m_z0pull_mean_vs_eta;
  TH1D* m_thetapull_mean_vs_eta;
  TH1D* m_z0stpull_mean_vs_eta;
  
  TH1D* m_d0pull_width_vs_eta;
  TH1D* m_phipull_width_vs_eta;
  TH1D* m_qoptpull_width_vs_eta;
  TH1D* m_z0pull_width_vs_eta;
  TH1D* m_thetapull_width_vs_eta;
  TH1D* m_z0stpull_width_vs_eta;
  
  // summary plots of track parameter resolutions vs pt
  TH1D* m_d0_mean_vs_pt;
  TH1D* m_phi_mean_vs_pt;
  TH1D* m_qopt_mean_vs_pt;
  TH1D* m_z0_mean_vs_pt;
  TH1D* m_theta_mean_vs_pt;
  TH1D* m_z0st_mean_vs_pt;

  TH1D* m_d0_width_vs_pt;
  TH1D* m_phi_width_vs_pt;
  TH1D* m_qopt_width_vs_pt;
  TH1D* m_z0_width_vs_pt;
  TH1D* m_theta_width_vs_pt;
  TH1D* m_z0st_width_vs_pt;

  TH1D* m_d0_res_vs_pt;
  TH1D* m_phi_res_vs_pt;
  TH1D* m_qopt_res_vs_pt;
  TH1D* m_z0_res_vs_pt;
  TH1D* m_theta_res_vs_pt;
  TH1D* m_z0st_res_vs_pt;

  // efficiency histograms / TProfiles

  TH1D* m_eff_eta;
  TH1D* m_eff_phi;
  TH1D* m_eff_pt;
  TH1D* m_eff_pti;
  TH2D* m_eff_eta_pt;
  TH2D* m_eff_eta_phi;

  TH1D* m_effdenom_eta;
  TH1D* m_effdenom_phi;
  TH1D* m_effdenom_pt;
  TH2D* m_effdenom_eta_pt;
  TH2D* m_effdenom_eta_phi;

  TH1D* m_effnum_eta;
  TH1D* m_effnum_phi;
  TH1D* m_effnum_pt;
  TH2D* m_effnum_eta_pt;
  TH2D* m_effnum_eta_phi;

  TProfile* m_n_vs_jetDR_truth;
  TProfile* m_n_vs_jetDR_reco;
  TH1D* m_eff_jetDR; 

  TProfile* m_n_vs_jetPt_truth;
  TProfile* m_n_vs_jetPt_reco;
  TH1D* m_eff_jetPt; 

  TH1F* create_registeredTH1F(MonGroup &mon, const char* name);
  TH1D* create_registeredTH1D(MonGroup &mon, const char* name);
  TH2F* create_registeredTH2F(MonGroup &mon, const char* name);
  TH2D* create_registeredTH2D(MonGroup &mon, const char* name);
  TProfile* create_registeredTProfile(MonGroup &mon, const char* name);

  void SetSafeMinimumMaximum(TH1* h1, float min, float max);

  void CopyProfile (const TProfile* p, TH1* h);
  void projectStandardProfileY (const TH2* h, TH1* profMean, TH1* profWidth=0,
                                  const char* fitfun=0, Double_t cut_tail=3.0);
  void projectStandardProfileY (const std::vector<TH1D*>& hvec, TH1* profMean, TH1* profWidth=0,
                                  Int_t fold=-1, const char* fitfun=0, Double_t cut_tail=3.0);

  void calculateEfficiency(TH1 *hEff, TH1 *hDenom,  TH1* hNum);
  void calculateIntegrated(TH1 *puri, TH1 *matched, TH1* all);
  void inverseSum( TH1* in, TH1* out);

  TList m_owned, m_update;
  TDirectory* m_top;
  int m_verbose;

  std::string m_tracksName;
  std::string m_histDirectoryName;
  int m_trackPtBins;
  int m_trackEtaBins;
  float m_maxTrackEta;
  bool m_plotsVsAbsEta;
};


void GetPath (const TDirectory* dir, TString& path)
{
  const TDirectory* mom= dir->GetMotherDir();
  if (!mom) return;
  GetPath (mom, path);
  path += "/";
  path += dir->GetName();
}

void PrintHist (const TH1* h, int detail=1)
{
  if (detail>=1) {
    TString path;
    GetPath (h->GetDirectory(), path);
    std::cout << path << '/';
  }
  std::cout << h->GetName();
  Int_t d= h->GetDimension();
  if (detail>=2) {
    std::cout << " \"" << h->GetTitle();
    if (d>=1) {
      const char* s= h->GetXaxis() ? h->GetXaxis()->GetTitle() : 0;
      if (s && s[0]) std::cout << ';' << s;
    }
    if (d>=1) {
      const char* s= h->GetYaxis() ? h->GetYaxis()->GetTitle() : 0;
      if (s && s[0]) std::cout << ';' << s;
    }
    if (d>=2) {
      const char* s= h->GetZaxis() ? h->GetZaxis()->GetTitle() : 0;
      if (s && s[0]) std::cout << ';' << s;
    }
    std::cout << '"';
  }
  std::cout << ' '  << h->ClassName();
  std::cout << ", " << h->GetNbinsX();
  if (d>=2) std::cout << 'x' << h->GetNbinsY();
  if (d>=3) std::cout << 'x' << h->GetNbinsY();
  std::cout << " bins, " << Long64_t(h->GetEntries()) << " entries, sum " << h->GetSumOfWeights()
            << std::endl;
}


class MonGroup {
public:
  MonGroup( IDStandardPerformance* tool, const std::string& system, int level, int interval )
    : m_tool(tool), m_system(system), m_level(level), m_interval(interval) { }

  TH1* regHist( const char* name ) {
    TString path= Form("%s/%s",m_system.c_str(),name);

    //    cout << "Path: " << path << endl;
    //    cout << "m_level " << m_level << endl;

    TH1* h= 0;
    m_tool->GetDirectory()->GetObject (path, h);
    if (h) {
      if (m_level>=2) { cout << "Read "; PrintHist(h); }
      m_tool->Add(h);
      //      cout << "Histo added " << h->GetName() << endl;
      //      cout << "Path " << path << endl;
    } else if (m_level>=0)
      {;}      //      cerr << "TH1::"<<path<<" not found" << endl;
    //    cout << "Return h " << h << endl;
    return h;
  }

private:
  IDStandardPerformance* m_tool;
  std::string m_system;
  int m_level, m_interval;
};


TH1F* IDStandardPerformance::create_registeredTH1F(MonGroup &mon, const char* name)
{
  TH1F* hist = 0;
  TH1* myH = mon.regHist(name);
  if (!myH) return NULL;
  hist = dynamic_cast<TH1F*>(myH);
  return hist;
}

TH1D* IDStandardPerformance::create_registeredTH1D(MonGroup &mon, const char* name)
{
  TH1D* hist = 0;
  TH1* myH = mon.regHist(name);
  if (!myH) return NULL;
  hist = dynamic_cast<TH1D*>(myH);
  return hist;
}

TH2F* IDStandardPerformance::create_registeredTH2F(MonGroup &mon, const char* name)
{
  TH2F* hist = 0;
  hist = dynamic_cast<TH2F*>(mon.regHist(name));
  return hist;
}

TH2D* IDStandardPerformance::create_registeredTH2D(MonGroup &mon, const char* name)
{
  TH2D* hist = 0;
  hist = dynamic_cast<TH2D*>(mon.regHist(name));
  return hist;
}

TProfile* IDStandardPerformance::create_registeredTProfile(MonGroup &mon, const char* name)
{
  TProfile* hist = 0;
  hist = dynamic_cast<TProfile*>(mon.regHist(name));
  return hist;
}


void IDStandardPerformance::bookHistograms(bool isNewRun)
{
  //  if (m_verbose>=1) cout << "bookHistograms "<<m_histDirectoryName<<endl;
  //  cout << "bookHistograms "<<m_histDirectoryName<<endl;
  int expert=m_verbose, debug=m_verbose, shift=m_verbose, run=0;
  //MsgStream log( msgSvc(), name() );

  std::string outputDirName = "IDPerformanceMon/" + m_tracksName + "/" + m_histDirectoryName;

  MonGroup al_expert(this, outputDirName, expert, run);
  MonGroup al_shift(this, outputDirName, shift, run);
  MonGroup al_debug(this, outputDirName, debug, run);
  MonGroup effdenom(this, outputDirName+"Tracks/"+ m_histDirectoryName + "Denom",expert, run);
  MonGroup effnum(this, outputDirName+"Tracks/"+ m_histDirectoryName + "Num",expert, run);

  if (isNewRun) {

    char name_tmp[100];

    m_d0_vs_eta    = create_registeredTH2D(al_debug,"res_d0_vs_eta");
    m_z0_vs_eta    = create_registeredTH2D(al_debug,"res_z0_vs_eta");
    m_phi_vs_eta   = create_registeredTH2D(al_debug,"res_phi_vs_eta");
    m_theta_vs_eta = create_registeredTH2D(al_debug,"res_theta_vs_eta");
    m_z0st_vs_eta  = create_registeredTH2D(al_debug,"res_z0*sin(theta)_vs_eta");
    m_qopt_vs_eta  = create_registeredTH2D(al_debug,"res_qOverP_vs_eta");

    m_d0_vs_pt     = create_registeredTH2D(al_debug,"res_d0_vs_logpt");
    m_z0_vs_pt     = create_registeredTH2D(al_debug,"res_z0_vs_logpt");
    m_phi_vs_pt    = create_registeredTH2D(al_debug,"res_phi_vs_logpt");
    m_theta_vs_pt  = create_registeredTH2D(al_debug,"res_theta_vs_logpt");
    m_z0st_vs_pt   = create_registeredTH2D(al_debug,"res_z0*sin(theta)_vs_logpt");
    m_qopt_vs_pt   = create_registeredTH2D(al_debug,"res_qOverP_vs_logpt");

    m_d0pull_vs_eta    = create_registeredTH2D(al_debug,"pull_d0_vs_eta");
    m_z0pull_vs_eta    = create_registeredTH2D(al_debug,"pull_z0_vs_eta");
    m_phipull_vs_eta   = create_registeredTH2D(al_debug,"pull_phi_vs_eta");
    m_thetapull_vs_eta = create_registeredTH2D(al_debug,"pull_theta_vs_eta");
    m_z0stpull_vs_eta  = create_registeredTH2D(al_debug,"pull_z0*sin(theta)_vs_eta");
    m_qoptpull_vs_eta  = create_registeredTH2D(al_debug,"pull_qOverP_vs_eta");

    int nbins_eta = m_trackEtaBins;
    float min_eta = -m_maxTrackEta;
    float max_eta = +m_maxTrackEta;
    int nbins_pt = 20;
    int min_pt,max_pt;

    if (m_plotsVsAbsEta){
      nbins_eta = m_trackEtaBins/2;
      min_eta = 0;
    }
  

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions amnd means versus eta
    // ------------------------------------------------------------------

    m_d0_mean_vs_eta = create_registeredTH1D(al_shift,   "resmean_d0_vs_eta");
    m_z0_mean_vs_eta = create_registeredTH1D(al_shift,   "resmean_z0_vs_eta");
    m_phi_mean_vs_eta = create_registeredTH1D(al_shift,  "resmean_phi_vs_eta");
    m_theta_mean_vs_eta = create_registeredTH1D(al_shift,"resmean_theta_vs_eta");
    m_z0st_mean_vs_eta = create_registeredTH1D(al_shift, "resmean_z0*sin(theta)_vs_eta");
    //    m_qopt_mean_vs_eta = create_registeredTH1D(al_shift, "resmean_qOverP_vs_eta");

    m_d0_width_vs_eta = create_registeredTH1D(al_shift,   "reswidth_d0_vs_eta");
    m_z0_width_vs_eta = create_registeredTH1D(al_shift,   "reswidth_z0_vs_eta");
    m_phi_width_vs_eta = create_registeredTH1D(al_shift,  "reswidth_phi_vs_eta");
    m_theta_width_vs_eta = create_registeredTH1D(al_shift,"reswidth_theta_vs_eta");
    m_z0st_width_vs_eta = create_registeredTH1D(al_shift, "reswidth_z0*sin(theta)_vs_eta");
    //    m_qopt_width_vs_eta = create_registeredTH1D(al_shift, "reswidth_qOverP_vs_eta");
    
    m_d0pull_mean_vs_eta = create_registeredTH1D(al_shift,   "pullmean_d0_vs_eta");
    m_z0pull_mean_vs_eta = create_registeredTH1D(al_shift,   "pullmean_z0_vs_eta");
    m_phipull_mean_vs_eta = create_registeredTH1D(al_shift,  "pullmean_phi_vs_eta");
    m_thetapull_mean_vs_eta = create_registeredTH1D(al_shift,"pullmean_theta_vs_eta");
    m_z0stpull_mean_vs_eta = create_registeredTH1D(al_shift, "pullmean_z0*sin(theta)_vs_eta");
    //    m_qoptpull_mean_vs_eta = create_registeredTH1D(al_shift, "pullmean_qopt_vs_eta");

    m_d0pull_width_vs_eta = create_registeredTH1D(al_shift,   "pullwidth_d0_vs_eta");
    m_z0pull_width_vs_eta = create_registeredTH1D(al_shift,   "pullwidth_z0_vs_eta");
    m_phipull_width_vs_eta = create_registeredTH1D(al_shift,  "pullwidth_phi_vs_eta");
    m_thetapull_width_vs_eta = create_registeredTH1D(al_shift,"pullwidth_theta_vs_eta");
    m_z0stpull_width_vs_eta = create_registeredTH1D(al_shift, "pullwidth_z0*sin(theta)_vs_eta");
    //    m_qoptpull_width_vs_eta = create_registeredTH1D(al_shift, "pullwidth_qopt_vs_eta");

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions versus pt
    // ------------------------------------------------------------------

    m_d0_mean_vs_pt = create_registeredTH1D(al_shift,   "resmean_d0_vs_logpt");
    m_z0_mean_vs_pt= create_registeredTH1D(al_shift,    "resmean_z0_vs_logpt");
    m_phi_mean_vs_pt = create_registeredTH1D(al_shift,  "resmean_phi_vs_logpt");
    m_theta_mean_vs_pt = create_registeredTH1D(al_shift,"resmean_theta_vs_logpt");
    m_z0st_mean_vs_pt = create_registeredTH1D(al_shift, "resmean_z0*sin(theta)_vs_logpt");
    //    m_qopt_mean_vs_pt = create_registeredTH1D(al_shift, "resmean_qOverP_vs_logpt")'

    m_d0_width_vs_pt = create_registeredTH1D(al_shift,   "reswidth_d0_vs_logpt");
    m_z0_width_vs_pt= create_registeredTH1D(al_shift,    "reswidth_z0_vs_logpt");
    m_phi_width_vs_pt = create_registeredTH1D(al_shift,  "reswidth_phi_vs_logpt");
    m_theta_width_vs_pt = create_registeredTH1D(al_shift,"reswidth_theta_vs_logpt");
    m_z0st_width_vs_pt = create_registeredTH1D(al_shift, "reswidth_z0*sin(theta)_vs_logpt");
    //    m_qopt_width_vs_pt = create_registeredTH1D(al_shift, "reswidth_qOverP_vs_logpt");

    // ------------------------------------------------------------------
    // track efficiency plots 
    // ------------------------------------------------------------------

    m_eff_eta      = create_registeredTH1D(al_shift,   "Eff_eta");
    m_effdenom_eta = create_registeredTH1D(effdenom,   "eta");
    m_effnum_eta   = create_registeredTH1D(effnum,     "eta");

    m_eff_phi      = create_registeredTH1D(al_shift,   "Eff_phi");
    m_effdenom_phi = create_registeredTH1D(effdenom,   "phi");
    m_effnum_phi   = create_registeredTH1D(effnum,     "phi");
    
    m_eff_pt       = create_registeredTH1D(al_shift,   "Eff_pt");
    m_eff_pti      = create_registeredTH1D(al_shift,   "Eff_pti");
    m_effdenom_pt  = create_registeredTH1D(effdenom,   "pt");
    m_effnum_pt    = create_registeredTH1D(effnum,     "pt");
    
    m_eff_eta_pt      = create_registeredTH2D(al_shift,   "Eff_eta_pt");
    m_effdenom_eta_pt = create_registeredTH2D(effdenom,   "eta_pt");
    m_effnum_eta_pt   = create_registeredTH2D(effnum,     "eta_pt");

    m_eff_eta_phi      = create_registeredTH2D(al_shift,   "Eff_eta_phi");
    m_effdenom_eta_phi = create_registeredTH2D(effdenom,   "eta_phi");
    m_effnum_eta_phi   = create_registeredTH2D(effnum,     "eta_phi");

    m_n_vs_jetDR_truth = create_registeredTProfile(al_shift, "NTracks_vs_jetDR_truth");
    m_n_vs_jetDR_reco  = create_registeredTProfile(al_shift, "NTracks_vs_jetDR_reco");
    m_eff_jetDR        = create_registeredTH1D(al_shift,     "NTracks_vs_jetDR_eff");

    m_n_vs_jetPt_truth = create_registeredTProfile(al_shift, "NTracks_vs_jetPt_truth");
    m_n_vs_jetPt_reco  = create_registeredTProfile(al_shift, "NTracks_vs_jetPt_reco");
    m_eff_jetPt        = create_registeredTH1D(al_shift,     "NTracks_vs_jetPt_eff");

  }
  if (m_verbose>=1) cout << "bookHistograms "<<m_histDirectoryName<<" Done."<<endl;
}


// Utility routine used by projectStandardProfileY: fill one bin from a 1D slice.
void SetProfileBin (Int_t i, TH1* h1, TH1* pm, TH1* pw, const char* fitfun=0, Double_t cut_tail=3.0)
{
 
  if (h1->GetXaxis()->TestBit(TAxis::kAxisRange)) {
    // remove range if set previously
    h1->GetXaxis()->SetRange();
    h1->ResetStats();
  }
  Double_t x=0.0, xe=0.0, e=0.0, ee=0.0;
  if (h1->GetEntries() != 0.0) {
    x=  h1->GetMean();      e=  h1->GetRMS();
    xe= h1->GetMeanError(); ee= h1->GetRMSError();
    if (!strncmp(fitfun,"gaus",4)) {
      TFitResultPtr r= h1->Fit (fitfun, "QS0");
      if (r.Get() && r->Status()%1000==0) {
        x=  r->Parameter(1); e=  r->Parameter(2);
        xe= r->ParError (1); ee= r->ParError (2);
      }
    } else if (!strncmp(fitfun,"iter",4)){

      double temp_RMS = 0;
      double new_RMS = 10;
      double aymin,aymax;

      while(fabs(new_RMS - temp_RMS) > 0.01){
	temp_RMS = h1->GetRMS();
	aymin = (h1->GetMean()) - 5*temp_RMS;
	aymax = (h1->GetMean()) + 5*temp_RMS;
        h1->SetAxisRange(aymin, aymax);
	new_RMS = h1->GetRMS();
      }
      x = h1->GetMean();
      xe= h1->GetMeanError();
      e = new_RMS;
      ee= h1->GetRMSError();

    }else if (cut_tail!=0.0) {
      if (cut_tail<0.0) h1->SetAxisRange (  cut_tail*e,  -cut_tail*e);
      else              h1->SetAxisRange (x-cut_tail*e, x+cut_tail*e);
      x=  h1->GetMean();      e=  h1->GetRMS();
      xe= h1->GetMeanError(); ee= h1->GetRMSError();
    }
  }
  if (pm) {
    pm->SetBinContent (i, x);
    pm->SetBinError   (i, xe);
  } 
  if (pw) {
    pw->SetBinContent (i, e);
    pw->SetBinError   (i, ee);
  }
}


// projects a TH2 into a profile, expressed as a TH1D.
// This is similar to the TH2::ProfileX method with the difference
// that only the error within 3RMS around the mean is considered in the profile.
// Specify a different range with cut_tail (>0:mean+/-RMS, <0:0+/-RMS).
// Alternatively, fit a Gaussian by specifying fitfun="gaus".
void IDStandardPerformance::projectStandardProfileY (const TH2* h, TH1* profMean, TH1* profWidth,
                                                     const char* fitfun, Double_t cut_tail)
{
  if (!h)                      return;
  if (!profMean && !profWidth) return;
  Int_t    nb=  h->GetNbinsX();
  Double_t xlo= h->GetXaxis()->GetXmin(), xhi= h->GetXaxis()->GetXmax();
  if (profMean)  profMean ->SetBins (nb, xlo, xhi);
  if (profWidth) profWidth->SetBins (nb, xlo, xhi);
  for (Int_t i=1; i<=nb; i++) {
    TH1D* h1= h->ProjectionY(Form("%s_projy_bin%d",h->GetName(),i), i, i);
    SetProfileBin (i, h1, profMean, profWidth, fitfun, cut_tail);
    delete h1;
  }
  if (profMean) {
    profMean ->SetEntries(h->GetEntries());
    Update (profMean);
  }
  if (profWidth) {
    profWidth->SetEntries(h->GetEntries());
    Update (profWidth);
  }
}


void IDStandardPerformance::projectStandardProfileY (const std::vector<TH1D*>& hvec, TH1* profMean, TH1* profWidth,
                                                     Int_t fold, const char* fitfun, Double_t cut_tail)
{

  TH1 *hx= profMean ? profMean : profWidth;
  if (!hx) return;
  Int_t nvec= hvec.size();
  if (fold>nvec) fold= nvec;
  Int_t nb= fold>0 ? fold : nvec;
  Double_t xlo= hx->GetXaxis()->GetXmin(), xhi= hx->GetXaxis()->GetXmax();
  if (profMean)  profMean ->SetBins (nb, xlo, xhi);
  if (profWidth) profWidth->SetBins (nb, xlo, xhi);
  Double_t nent= 0.0;
  for (Int_t i=0; i<nb; i++) {
    Int_t j1=i, j2=-1;
    if (fold>0) {
      j1= fold+i;
      if (j1<nvec && hvec[j1]) j2= fold-i-1;
      else                     j1= fold-i-1;
    }
    if (!(j1>=0 && hvec[j1])) continue;
    TH1D* h1= dynamic_cast<TH1D*>(hvec[j1]->Clone(Form("%s_mod",hvec[j1]->GetName())));
    if (  j2>=0 && hvec[j2]) h1->Add (hvec[j2]);
    SetProfileBin (i+1, h1, profMean, profWidth, fitfun, cut_tail);
    nent += h1->GetEntries();
    delete h1;
  }
  // restore #entries, which was modified by SetBinContent in SetProfileBin
  if (profMean) {
    profMean ->SetEntries(nent);
    Update (profMean);
  }
  if (profWidth) {
    profWidth->SetEntries(nent);
    Update (profWidth);
  }
}

void IDStandardPerformance::calculateEfficiency (TH1* hEff, TH1* hDenom, TH1* hNum)
{

  if (!hEff) return;
  if (!hDenom) return;
  if (!hNum) return;

  hEff->Reset();

  hEff->Add(hNum);
  if (!hEff->GetSumw2N()) hEff->Sumw2();
  hEff->Divide(hEff,hDenom,1,1,"B");

  Update (hEff);

}

void IDStandardPerformance::calculateIntegrated(TH1* puri, TH1* all, TH1* matched) {

  if (!matched) return; 
  if (!puri) return;

  puri->Reset();
  inverseSum(matched,puri);

  int nbinsx = matched->GetXaxis()->GetNbins();
  double xmin = matched->GetXaxis()->GetXmin();
  double xmax = matched->GetXaxis()->GetXmax();

  TH1* alli = new TH1F("tmp","tmp",nbinsx,xmin,xmax);

  inverseSum(all,alli);
  if (!puri->GetSumw2N()) puri->Sumw2();
  puri->Divide(puri,alli,1,1,"B");

  Update (puri);
  delete alli;
}

void IDStandardPerformance::inverseSum( TH1* in, TH1* out){
  if( in->GetEntries() == 0 ) return;
  int nbinsx = in->GetXaxis()->GetNbins();
  int nbinsy = in->GetYaxis()->GetNbins();
  for(int i = 0 ; i < nbinsx; ++i) {
    float ntotal= 0;
    for(int j= nbinsy ; j>=0; --j) {
      ntotal += in->GetBinContent(i,j);
      out->SetBinContent(i,j,ntotal);
    }
  }
}

void IDStandardPerformance::procHistograms( bool isEndOfRun )
{

  if (m_verbose>=1) cout << "procHistograms "<<m_histDirectoryName<<endl;
  //MsgStream log( msgSvc(), name() );
  //  if (m_idHelper) delete m_idHelper;

  if ( isEndOfRun ) {
 
    int halfEtaBins = m_trackEtaBins/2;
    int fold= m_plotsVsAbsEta ? halfEtaBins : 0;

    //    cout << "procHistos" << endl;

    char fitter[5] = "iter";
    //    cout << "Fitter: " << fitter << endl; 

    projectStandardProfileY (m_d0_vs_eta,    m_d0_mean_vs_eta,    m_d0_width_vs_eta,    fitter);
    projectStandardProfileY (m_z0_vs_eta,    m_z0_mean_vs_eta,    m_z0_width_vs_eta,    fitter);
    projectStandardProfileY (m_phi_vs_eta,   m_phi_mean_vs_eta,   m_phi_width_vs_eta,   fitter);
    projectStandardProfileY (m_theta_vs_eta, m_theta_mean_vs_eta, m_theta_width_vs_eta, fitter);
    projectStandardProfileY (m_z0st_vs_eta,  m_z0st_mean_vs_eta,  m_z0st_width_vs_eta,  fitter);
    //    projectStandardProfileY (m_qopt_vs_eta,  m_qopt_mean_vs_eta,  m_qopt_width_vs_eta,  fitter);

    projectStandardProfileY (m_d0_vs_pt,    m_d0_mean_vs_pt,    m_d0_width_vs_pt,    fitter);
    projectStandardProfileY (m_z0_vs_pt,    m_z0_mean_vs_pt,    m_z0_width_vs_pt,    fitter);
    projectStandardProfileY (m_phi_vs_pt,   m_phi_mean_vs_pt,   m_phi_width_vs_pt,   fitter);
    projectStandardProfileY (m_theta_vs_pt, m_theta_mean_vs_pt, m_theta_width_vs_pt, fitter);
    projectStandardProfileY (m_z0st_vs_pt,  m_z0st_mean_vs_pt,  m_z0st_width_vs_pt,  fitter);
    //    projectStandardProfileY (m_qopt_vs_pt,  m_qopt_mean_vs_pt,  m_qopt_width_vs_pt,  fitter);

    projectStandardProfileY (m_d0pull_vs_eta,    m_d0pull_mean_vs_eta,    m_d0pull_width_vs_eta,    fitter);
    projectStandardProfileY (m_z0pull_vs_eta,    m_z0pull_mean_vs_eta,    m_z0pull_width_vs_eta,    fitter);
    projectStandardProfileY (m_phipull_vs_eta,   m_phipull_mean_vs_eta,   m_phipull_width_vs_eta,   fitter);
    projectStandardProfileY (m_thetapull_vs_eta, m_thetapull_mean_vs_eta, m_thetapull_width_vs_eta, fitter);
    projectStandardProfileY (m_z0stpull_vs_eta,  m_z0stpull_mean_vs_eta,  m_z0stpull_width_vs_eta,  fitter);
    //    projectStandardProfileY (m_qoptpull_vs_eta,  m_qoptpull_mean_vs_eta,  m_qoptpull_width_vs_eta,  fitter);

    calculateEfficiency (m_eff_eta,    m_effdenom_eta,    m_effnum_eta);
    calculateEfficiency (m_eff_phi,    m_effdenom_phi,    m_effnum_phi);
    calculateEfficiency (m_eff_pt,     m_effdenom_pt,     m_effnum_pt);
    calculateIntegrated (m_eff_pti,    m_effdenom_pt,     m_effnum_pt);
    calculateEfficiency (m_eff_eta_pt, m_effdenom_eta_pt, m_effnum_eta_pt);
    calculateEfficiency (m_eff_eta_phi,m_effdenom_eta_phi,m_effnum_eta_phi);

    calculateEfficiency (m_eff_jetDR,  m_n_vs_jetDR_truth,m_n_vs_jetDR_reco);
    calculateEfficiency (m_eff_jetPt,  m_n_vs_jetPt_truth,m_n_vs_jetPt_reco);

    SetSafeMinimumMaximum(m_d0_mean_vs_eta, -0.03, 0.03);
    SetSafeMinimumMaximum(m_phi_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_theta_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_z0st_mean_vs_eta, -0.05, 0.05);
    //    SetSafeMinimumMaximum(m_qopt_mean_vs_eta, -0.01, 0.01);
    
  }

  if (m_verbose>=1) cout << "procHistograms "<<m_histDirectoryName<<" Done."<<endl;
}

void IDStandardPerformance::SetSafeMinimumMaximum(TH1* h, float min, float max) {
  if (!h) return;
  float hmin = 0.9 * h->GetMinimum();
  float hmax = 1.1 * h->GetMaximum();

  if (hmin < min)
    h->SetMinimum(hmin);
  else
    h->SetMinimum(min);

  if (hmax > max)
    h->SetMaximum(hmax);
  else
    h->SetMaximum(max);
  return;
}

int IDStandardPerformance::updateFile()
{
  int n=0;
  for (TIter it= &m_update; TObject* o= it();) {
    TH1* h= dynamic_cast<TH1*>(o);
    if (!h) continue;
    TDirectory* d= h->GetDirectory();
    if (!d) continue;
    if (m_verbose>=1) {
      cout << "Updated ";
      PrintHist(h);
    }
    d->WriteTObject(h,h->GetName(),"Overwrite");
    //    cout << "Updated " << h->GetName() << endl;
    n++;
  }
  return n;
}

int postprocessHistos (TDirectory* file, const char* tracksName, const char* dirname, int verbose=0)
{
  TString dir;
  dir.Form("IDPerformanceMon/%s/%s",tracksName,dirname);
  TH1* h=0;

  cout << "Now searching for " << dir << endl;
  file->GetObject (dir+"/res_d0_vs_eta", h);
  if (!h) file->GetObject (dir+"/HitContent_NBlayerHits", h);
  if (!h) file->GetObject (dir+"/jetpT", h);

  if (!h) return 0;
  bool plotsVsAbsEta= (h->GetXaxis()->GetXmin() == 0.0);
  delete h;

  IDStandardPerformance idsp (file, tracksName, dirname, verbose, plotsVsAbsEta);
  idsp.bookHistograms();
  idsp.procHistograms();
  int n= idsp.updateFile();

  if (verbose>=0) {
    cout << "Updated " << n << " histograms in " << dir;
    if (plotsVsAbsEta) cout << " with |eta| range";
    cout << endl;
    cout << endl;
  }
  return 1;
}

int ScanDir (TList& dirs, TDirectory* file, TDirectory* dir, const char* tracksName=0, int verbose=0)
{
  int n= 0;
  if (verbose>=1) {
    TString path;
    GetPath (dir, path);
    cout << "Scan directory " << path << endl;
  }  for (TIter it= dir->GetListOfKeys(); TKey* k= dynamic_cast<TKey*>(it());) {
    const TClass* c= TClass::GetClass (k->GetClassName());
    if (!(c && c->InheritsFrom (TDirectory::Class()))) continue;
    if (TDirectory* subdir= dynamic_cast<TDirectory*>(k->ReadObj())) {
      if (!tracksName) n += ScanDir         (dirs, file, subdir, subdir->GetName(), verbose);
      else             dirs.Add (new TNamed (tracksName, subdir->GetName()));
      delete subdir;
    }
  }
  return n;
}

int postprocessHistos (TDirectory* file, int verbose=0)
{
  int n= 0;
  if (verbose>=0) cout << "Process file " << file->GetName() << endl;
  TDirectory* dir=0;
  file->GetObject ("IDPerformanceMon", dir);
  if (dir) {
    TList dirs;
    dirs.SetOwner();
    n += ScanDir (dirs, file, dir, 0, verbose);
    delete dir;
    for (TIter it= &dirs; TNamed* name= dynamic_cast<TNamed*>(it());)
      n += postprocessHistos (file, name->GetName(), name->GetTitle(), verbose);
  }
  if (!n) cerr << "No histogram directories found in file " << file->GetName() << endl;
  return n;
}

int postprocessHistos (const char* name, int verbose=0)
{
  if (gSystem->AccessPathName (name, kFileExists)) {
    cerr << "File " << name << " does not exist" << endl;
    return 2;
  }
  TFile* file= TFile::Open (name, "UPDATE");
  if (!file) {
    cerr << "Could not open file " << name << endl;
    return 3;
  }
  int n= postprocessHistos (file, verbose);
  delete file;
  return n ? 0 : 4;
}

void postprocessHistos (int verbose=0)
{
  for (TIter it= gROOT->GetListOfFiles(); TObject *o= it();) {
    TFile* file= dynamic_cast<TFile*>(o);
    if (!file) continue;
    postprocessHistos (file->GetName(), verbose);
  }
}

#if !defined(__CINT__) && !defined(__ACLIC__)
int main (int argc, const char** argv)
{
  int i=1, verbose=0;
  for (; i<argc; i++) {
    if (argv[i][0]!='-') break;
    for (const char* a= argv[i]+1; *a;)
      switch (*a++) {
        case 'v': verbose++; break;
        case 'q': verbose--; break;
        default:  i=argc;    break;
      }
  }
  if (i>=argc) {
    cout << "Usage: "<<argv[0]<<" [-vq] FILE.root [FILE.root...]" << endl;
    return 1;
  }
  int err=0;
  for (; i<argc; i++) {
    int stat= postprocessHistos (argv[i], verbose);
    if (!err) err= stat;
  }
  return err;
}
#endif
