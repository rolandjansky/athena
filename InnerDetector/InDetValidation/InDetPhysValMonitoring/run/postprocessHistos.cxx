/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*==============================================================================

   Regenerate 1D profile plots or histograms from 2D histograms in an
   existing InDetStandardPlots.root. The file is updated with new histograms
   overriding the old ones).

   This can be used to fix up histogram files after merging InDetStandardPlots.root
   files using hadd (eg. during Grid validation).

   Some of this code is copied from
   InnerDetector/InDetValidation/InDetPerformanceRTT/src/IDStandardPerformance.cxx
   create_registeredTH1F (etc) reads in existing histograms rather than creating
   new ones.

   Usage: 
   
   1) summ the files produced from the separate (grid) runs
   hadd summ.root file1.root file2.root ... fileN.root
   <=== LMTODO : this will be different for TEfficiencties ; 
   need a dedicated script at this step already 

   2) the post-processing will 
   * loop over TProfiles 
   * replace incorrect TProfiles in TEfficiencies inline

   to run use either of: 

   2.1) command-line:
   root -l -b -q summ.root postprocessHistos.cxx+

   2.2) compiled code: 
   NAME=postprocessHistos
   g++ -O2 -Wall -fPIC $(root-config --cflags) -o ${NAME} ${NAME}.cxx $(root-config --libs)
   ./$NAME summ.root

   -----------------------------------------------------------------------------
   Author: Soeren Prell, Liza Mijovic
   ==============================================================================*/
#define RLN cout << "l" << __LINE__ << ": ";
// 0, 1 or 2 in increasing order of verbosity
#define PRINTDBG 2

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
  IDStandardPerformance(TDirectory *top,
                        const char *tracksName = "Tracks",
                        const char *histDirectoryName = "SelectedGoodTracks") :
    m_d0_vs_eta{},
    m_z0_vs_eta{},
    m_phi_vs_eta{},
    m_theta_vs_eta{},
    m_z0st_vs_eta{},
    m_qopt_vs_eta{},

    m_d0_vs_pt{},
    m_z0_vs_pt{},
    m_phi_vs_pt{},
    m_theta_vs_pt{},
    m_z0st_vs_pt{},
    m_qopt_vs_pt{},

    m_d0pull_vs_eta{},
    m_z0pull_vs_eta{},
    m_phipull_vs_eta{},
    m_thetapull_vs_eta{},
    m_z0stpull_vs_eta{},
    m_qoptpull_vs_eta{},

    m_d0_mean_vs_eta{},
    m_phi_mean_vs_eta{},
    m_qopt_mean_vs_eta{},
    m_z0_mean_vs_eta{},
    m_theta_mean_vs_eta{},
    m_z0st_mean_vs_eta{},

    m_d0_width_vs_eta{},
    m_phi_width_vs_eta{},
    m_qopt_width_vs_eta{},
    m_z0_width_vs_eta{},
    m_theta_width_vs_eta{},
    m_z0st_width_vs_eta{},

    m_d0pull_mean_vs_eta{},
    m_phipull_mean_vs_eta{},
    m_qoptpull_mean_vs_eta{},
    m_z0pull_mean_vs_eta{},
    m_thetapull_mean_vs_eta{},
    m_z0stpull_mean_vs_eta{},

    m_d0pull_width_vs_eta{},
    m_phipull_width_vs_eta{},
    m_qoptpull_width_vs_eta{},
    m_z0pull_width_vs_eta{},
    m_thetapull_width_vs_eta{},
    m_z0stpull_width_vs_eta{},

    m_d0_mean_vs_pt{},
    m_phi_mean_vs_pt{},
    m_qopt_mean_vs_pt{},
    m_z0_mean_vs_pt{},
    m_theta_mean_vs_pt{},
    m_z0st_mean_vs_pt{},

    m_d0_width_vs_pt{},
    m_phi_width_vs_pt{},
    m_qopt_width_vs_pt{},
    m_z0_width_vs_pt{},
    m_theta_width_vs_pt{},
    m_z0st_width_vs_pt{},

    m_d0_res_vs_pt{},
    m_phi_res_vs_pt{},
    m_qopt_res_vs_pt{},
    m_z0_res_vs_pt{},
    m_theta_res_vs_pt{},
    m_z0st_res_vs_pt{},

    m_top(top),
    m_tracksName(tracksName),
    m_histDirectoryName(histDirectoryName) {
    m_owned.SetOwner();
  }

  virtual ~IDStandardPerformance() {
  };
  virtual void bookHistograms(bool isNewRun = true);
  virtual void procHistograms(bool isEndOfRun = true);
  virtual int updateFile();
  TDirectory *
  GetDirectory() {
    return m_top;
  }

  void
  Add(TObject *obj) {
    m_owned.Add(obj);
  }

  void
  Update(TObject *obj) {
    m_update.Add(obj);
  }

private:
  // 2D histograms
  TH2F *m_d0_vs_eta;
  TH2F *m_z0_vs_eta;
  TH2F *m_phi_vs_eta;
  TH2F *m_theta_vs_eta;
  TH2F *m_z0st_vs_eta;
  TH2F *m_qopt_vs_eta;

  TH2F *m_d0_vs_pt;
  TH2F *m_z0_vs_pt;
  TH2F *m_phi_vs_pt;
  TH2F *m_theta_vs_pt;
  TH2F *m_z0st_vs_pt;
  TH2F *m_qopt_vs_pt;

  TH2F *m_d0pull_vs_eta;
  TH2F *m_z0pull_vs_eta;
  TH2F *m_phipull_vs_eta;
  TH2F *m_thetapull_vs_eta;
  TH2F *m_z0stpull_vs_eta;
  TH2F *m_qoptpull_vs_eta;

  // summary plots of track parameter resolutions and means versus eta
  TH1F *m_d0_mean_vs_eta;
  TH1F *m_phi_mean_vs_eta;
  TH1F *m_qopt_mean_vs_eta;
  TH1F *m_z0_mean_vs_eta;
  TH1F *m_theta_mean_vs_eta;
  TH1F *m_z0st_mean_vs_eta;

  TH1F *m_d0_width_vs_eta;
  TH1F *m_phi_width_vs_eta;
  TH1F *m_qopt_width_vs_eta;
  TH1F *m_z0_width_vs_eta;
  TH1F *m_theta_width_vs_eta;
  TH1F *m_z0st_width_vs_eta;

  TH1F *m_d0pull_mean_vs_eta;
  TH1F *m_phipull_mean_vs_eta;
  TH1F *m_qoptpull_mean_vs_eta;
  TH1F *m_z0pull_mean_vs_eta;
  TH1F *m_thetapull_mean_vs_eta;
  TH1F *m_z0stpull_mean_vs_eta;

  TH1F *m_d0pull_width_vs_eta;
  TH1F *m_phipull_width_vs_eta;
  TH1F *m_qoptpull_width_vs_eta;
  TH1F *m_z0pull_width_vs_eta;
  TH1F *m_thetapull_width_vs_eta;
  TH1F *m_z0stpull_width_vs_eta;

  // summary plots of track parameter resolutions vs pt
  TH1F *m_d0_mean_vs_pt;
  TH1F *m_phi_mean_vs_pt;
  TH1F *m_qopt_mean_vs_pt;
  TH1F *m_z0_mean_vs_pt;
  TH1F *m_theta_mean_vs_pt;
  TH1F *m_z0st_mean_vs_pt;

  TH1F *m_d0_width_vs_pt;
  TH1F *m_phi_width_vs_pt;
  TH1F *m_qopt_width_vs_pt;
  TH1F *m_z0_width_vs_pt;
  TH1F *m_theta_width_vs_pt;
  TH1F *m_z0st_width_vs_pt;

  TH1F *m_d0_res_vs_pt;
  TH1F *m_phi_res_vs_pt;
  TH1F *m_qopt_res_vs_pt;
  TH1F *m_z0_res_vs_pt;
  TH1F *m_theta_res_vs_pt;
  TH1F *m_z0st_res_vs_pt;

  TH1F *create_registeredTH1F(MonGroup &mon, const char *name);
  TH1D *create_registeredTH1D(MonGroup &mon, const char *name);
  TH2F *create_registeredTH2F(MonGroup &mon, const char *name);
  TProfile *create_registeredTProfile(MonGroup &mon, const char *name);

  void SetSafeMinimumMaximum(TH1 *h1, float min, float max);

  void CopyProfile(const TProfile *p, TH1 *h);
  void projectStandardProfileY(const TH2 *h, TH1 *profMean, TH1 *profWidth = 0,
                               const char *fitfun = 0, Double_t cut_tail = 3.0);
  void projectStandardProfileY(const std::vector<TH1F *> &hvec, TH1 *profMean, TH1 *profWidth = 0,
                               Int_t fold = -1, const char *fitfun = 0, Double_t cut_tail = 3.0);

  void calculateIntegrated(TH1 *puri, TH1 *matched, TH1 *all);
  void inverseSum(TH1 *in, TH1 *out);

  TList m_owned, m_update;
  TDirectory *m_top;

  std::string m_tracksName;
  std::string m_histDirectoryName;

};


void
GetPath(const TDirectory *dir, TString &path) {
  const TDirectory *mom = dir->GetMotherDir();

  if (!mom) {
    return;
  }
  GetPath(mom, path);
  path += "/";
  path += dir->GetName();
}

// 
bool FileExists(const char *name) {
  return (gSystem->AccessPathName(name, kFileExists))?
    false : true;    
}

void
PrintHist(const TH1 *h, int detail = 1) {
  if (detail >= 1) {
    TString path;
    GetPath(h->GetDirectory(), path);
    std::cout << path << '/';
  }
  std::cout << h->GetName();
  Int_t d = h->GetDimension();
  if (detail >= 2) {
    std::cout << " \"" << h->GetTitle();
    if (d >= 1) {
      const char *s = h->GetXaxis() ? h->GetXaxis()->GetTitle() : 0;
      if (s && s[0]) {
        std::cout << ';' << s;
      }
    }
    if (d >= 1) {
      const char *s = h->GetYaxis() ? h->GetYaxis()->GetTitle() : 0;
      if (s && s[0]) {
        std::cout << ';' << s;
      }
    }
    if (d >= 2) {
      const char *s = h->GetZaxis() ? h->GetZaxis()->GetTitle() : 0;
      if (s && s[0]) {
        std::cout << ';' << s;
      }
    }
    std::cout << '"';
  }
  std::cout << ' ' << h->ClassName();
  std::cout << ", " << h->GetNbinsX();
  if (d >= 2) {
    std::cout << 'x' << h->GetNbinsY();
  }
  if (d >= 3) {
    std::cout << 'x' << h->GetNbinsY();
  }
  std::cout << " bins, " << Long64_t(h->GetEntries()) << " entries, sum " << h->GetSumOfWeights()
            << std::endl;
}

class MonGroup {
public:
  MonGroup(IDStandardPerformance *tool, const std::string &system, int level)
    : m_tool(tool), m_system(system), m_level(level) {
  }

  TH1 *
  regHist(const char *name) {
    TString path = Form("%s/%s", m_system.c_str(), name);

    if (PRINTDBG) { cout <<"\t"; RLN; cout << "Path: " << path << endl;}
    if (PRINTDBG) { cout <<"\t"; RLN; cout << "m_level " << m_level << endl;}

    TH1 *h = 0;

    m_tool->GetDirectory()->GetObject(path, h);
    if (h) {
      if (m_level >= 2) {
        cout << "Read ";
        PrintHist(h);
      }
      m_tool->Add(h);
      //      cout << "Histo added " << h->GetName() << endl;
      //      cout << "Path " << path << endl;
    } else if (m_level >= 0) {
      if (PRINTDBG) { cout <<"\t"; RLN; cout << "TH1::"<<path<<" not found " << endl; }
    }
    if (PRINTDBG) {cout <<"\t MonGroup regHist "; RLN; cout << "Return h " << h << endl;}
    return h;
  }

private:
  IDStandardPerformance *m_tool;
  std::string m_system;
  int m_level;
};


TH1F *
IDStandardPerformance::create_registeredTH1F(MonGroup &mon, const char *name) {
  TH1F *hist = 0;
  TH1 *myH = mon.regHist(name);

  if (!myH) {
    return NULL;
  }
  hist = dynamic_cast<TH1F *>(myH);
  return hist;
}

TH1D *
IDStandardPerformance::create_registeredTH1D(MonGroup &mon, const char *name) {
  TH1D *hist = 0;
  TH1 *myH = mon.regHist(name);

  if (!myH) {
    return NULL;
  }
  hist = dynamic_cast<TH1D *>(myH);
  return hist;
}

TH2F *
IDStandardPerformance::create_registeredTH2F(MonGroup &mon, const char *name) {
  TH2F *hist = 0;
  if (PRINTDBG) { cout <<"\t"; RLN; cout <<" create_registeredTH2 for " << name << endl; }
  hist = dynamic_cast<TH2F *>(mon.regHist(name));
  if (hist) {
    if (PRINTDBG) { cout <<"\t"; RLN; cout <<" returning " << hist->GetName() << endl; }
  }
  else {
    if (PRINTDBG) { cout <<"\t"; RLN; cout <<" IDStandardPerformance::create_registeredTH2F dynamic_cast<TH2F *> failed. returning null pointer " << endl; }
  }
  return hist;
}

TProfile *
IDStandardPerformance::create_registeredTProfile(MonGroup &mon, const char *name) {
  TProfile *hist = 0;

  hist = dynamic_cast<TProfile *>(mon.regHist(name));
  return hist;
}

void
IDStandardPerformance::bookHistograms(bool isNewRun) {

  cout <<"\t"; RLN; cout << "bookHistograms "<<m_histDirectoryName<<endl;
  std::string outputDirName = "IDPerformanceMon/" + m_tracksName + "/" + m_histDirectoryName;
  int run = 0;
  
  MonGroup al_expert(this, outputDirName, run);
  MonGroup al_shift(this, outputDirName, run);
  MonGroup al_debug(this, outputDirName, run);

  if (isNewRun) {
    if (PRINTDBG) { cout <<"\t"; RLN; cout << " isNewRun" << endl; }
    if (PRINTDBG) { cout <<"\t"; RLN; cout << "  m_d0_vs_eta = create_registeredTH2F " << endl; }
    m_d0_vs_eta = create_registeredTH2F(al_debug, "res_d0_vs_eta");
    m_z0_vs_eta = create_registeredTH2F(al_debug, "res_z0_vs_eta");
    m_phi_vs_eta = create_registeredTH2F(al_debug, "res_phi_vs_eta");
    m_theta_vs_eta = create_registeredTH2F(al_debug, "res_theta_vs_eta");
    m_z0st_vs_eta = create_registeredTH2F(al_debug, "res_z0*sin(theta)_vs_eta");
    m_qopt_vs_eta = create_registeredTH2F(al_debug, "res_qOverP_vs_eta");

    m_d0_vs_pt = create_registeredTH2F(al_debug, "res_d0_vs_logpt");
    m_z0_vs_pt = create_registeredTH2F(al_debug, "res_z0_vs_logpt");
    m_phi_vs_pt = create_registeredTH2F(al_debug, "res_phi_vs_logpt");
    m_theta_vs_pt = create_registeredTH2F(al_debug, "res_theta_vs_logpt");
    m_z0st_vs_pt = create_registeredTH2F(al_debug, "res_z0*sin(theta)_vs_logpt");
    m_qopt_vs_pt = create_registeredTH2F(al_debug, "res_qOverP_vs_logpt");

    m_d0pull_vs_eta = create_registeredTH2F(al_debug, "pull_d0_vs_eta");
    m_z0pull_vs_eta = create_registeredTH2F(al_debug, "pull_z0_vs_eta");
    m_phipull_vs_eta = create_registeredTH2F(al_debug, "pull_phi_vs_eta");
    m_thetapull_vs_eta = create_registeredTH2F(al_debug, "pull_theta_vs_eta");
    m_z0stpull_vs_eta = create_registeredTH2F(al_debug, "pull_z0*sin(theta)_vs_eta");
    m_qoptpull_vs_eta = create_registeredTH2F(al_debug, "pull_qOverP_vs_eta");

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions amnd means versus eta
    // ------------------------------------------------------------------

    m_d0_mean_vs_eta = create_registeredTH1F(al_shift, "resmean_d0_vs_eta");
    m_z0_mean_vs_eta = create_registeredTH1F(al_shift, "resmean_z0_vs_eta");
    m_phi_mean_vs_eta = create_registeredTH1F(al_shift, "resmean_phi_vs_eta");
    m_theta_mean_vs_eta = create_registeredTH1F(al_shift, "resmean_theta_vs_eta");
    m_z0st_mean_vs_eta = create_registeredTH1F(al_shift, "resmean_z0*sin(theta)_vs_eta");
    //    m_qopt_mean_vs_eta = create_registeredTH1F(al_shift, "resmean_qOverP_vs_eta");

    m_d0_width_vs_eta = create_registeredTH1F(al_shift, "reswidth_d0_vs_eta");
    m_z0_width_vs_eta = create_registeredTH1F(al_shift, "reswidth_z0_vs_eta");
    m_phi_width_vs_eta = create_registeredTH1F(al_shift, "reswidth_phi_vs_eta");
    m_theta_width_vs_eta = create_registeredTH1F(al_shift, "reswidth_theta_vs_eta");
    m_z0st_width_vs_eta = create_registeredTH1F(al_shift, "reswidth_z0*sin(theta)_vs_eta");
    //    m_qopt_width_vs_eta = create_registeredTH1F(al_shift, "reswidth_qOverP_vs_eta");

    m_d0pull_mean_vs_eta = create_registeredTH1F(al_shift, "pullmean_d0_vs_eta");
    m_z0pull_mean_vs_eta = create_registeredTH1F(al_shift, "pullmean_z0_vs_eta");
    m_phipull_mean_vs_eta = create_registeredTH1F(al_shift, "pullmean_phi_vs_eta");
    m_thetapull_mean_vs_eta = create_registeredTH1F(al_shift, "pullmean_theta_vs_eta");
    m_z0stpull_mean_vs_eta = create_registeredTH1F(al_shift, "pullmean_z0*sin(theta)_vs_eta");
    //    m_qoptpull_mean_vs_eta = create_registeredTH1F(al_shift, "pullmean_qopt_vs_eta");

    m_d0pull_width_vs_eta = create_registeredTH1F(al_shift, "pullwidth_d0_vs_eta");
    m_z0pull_width_vs_eta = create_registeredTH1F(al_shift, "pullwidth_z0_vs_eta");
    m_phipull_width_vs_eta = create_registeredTH1F(al_shift, "pullwidth_phi_vs_eta");
    m_thetapull_width_vs_eta = create_registeredTH1F(al_shift, "pullwidth_theta_vs_eta");
    m_z0stpull_width_vs_eta = create_registeredTH1F(al_shift, "pullwidth_z0*sin(theta)_vs_eta");
    //    m_qoptpull_width_vs_eta = create_registeredTH1F(al_shift, "pullwidth_qopt_vs_eta");

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions versus pt
    // ------------------------------------------------------------------

    m_d0_mean_vs_pt = create_registeredTH1F(al_shift, "resmean_d0_vs_logpt");
    m_z0_mean_vs_pt = create_registeredTH1F(al_shift, "resmean_z0_vs_logpt");
    m_phi_mean_vs_pt = create_registeredTH1F(al_shift, "resmean_phi_vs_logpt");
    m_theta_mean_vs_pt = create_registeredTH1F(al_shift, "resmean_theta_vs_logpt");
    m_z0st_mean_vs_pt = create_registeredTH1F(al_shift, "resmean_z0*sin(theta)_vs_logpt");
    //    m_qopt_mean_vs_pt = create_registeredTH1F(al_shift, "resmean_qOverP_vs_logpt")'

    m_d0_width_vs_pt = create_registeredTH1F(al_shift, "reswidth_d0_vs_logpt");
    m_z0_width_vs_pt = create_registeredTH1F(al_shift, "reswidth_z0_vs_logpt");
    m_phi_width_vs_pt = create_registeredTH1F(al_shift, "reswidth_phi_vs_logpt");
    m_theta_width_vs_pt = create_registeredTH1F(al_shift, "reswidth_theta_vs_logpt");
    m_z0st_width_vs_pt = create_registeredTH1F(al_shift, "reswidth_z0*sin(theta)_vs_logpt");
    //    m_qopt_width_vs_pt = create_registeredTH1F(al_shift, "reswidth_qOverP_vs_logpt");

  }
  if (PRINTDBG > 1) {
    cout <<"\t"; RLN; cout << "bookHistograms " << m_histDirectoryName << " Done." << endl;
  }
}

// Utility routine used by projectStandardProfileY: fill one bin from a 1D slice.
void
SetProfileBin(Int_t i, TH1 *h1, TH1 *pm, TH1 *pw, const char *fitfun = 0, Double_t cut_tail = 3.0) {
  if (h1->GetXaxis()->TestBit(TAxis::kAxisRange)) {
    // remove range if set previously
    h1->GetXaxis()->SetRange();
    h1->ResetStats();
  }
  Double_t x = 0.0, xe = 0.0, e = 0.0, ee = 0.0;
  if (h1->GetEntries() != 0.0) {
    x = h1->GetMean();
    e = h1->GetRMS();
    xe = h1->GetMeanError();
    ee = h1->GetRMSError();
    if (!strncmp(fitfun, "gaus", 4)) {
      TFitResultPtr r = h1->Fit(fitfun, "QS0");
      if (r.Get() && r->Status() % 1000 == 0) {
        x = r->Parameter(1);
        e = r->Parameter(2);
        xe = r->ParError(1);
        ee = r->ParError(2);
      }
    } else if (!strncmp(fitfun, "iter", 4)) {
      double temp_RMS = 0;
      double new_RMS = 10;
      double aymin, aymax;

      while (fabs(new_RMS - temp_RMS) > 0.01) {
        temp_RMS = h1->GetRMS();
        aymin = (h1->GetMean()) - 5 * temp_RMS;
        aymax = (h1->GetMean()) + 5 * temp_RMS;
        h1->SetAxisRange(aymin, aymax);
        new_RMS = h1->GetRMS();
      }
      x = h1->GetMean();
      xe = h1->GetMeanError();
      e = new_RMS;
      ee = h1->GetRMSError();
    }else if (cut_tail != 0.0) {
      if (cut_tail < 0.0) {
        h1->SetAxisRange(cut_tail * e, -cut_tail * e);
      } else {
        h1->SetAxisRange(x - cut_tail * e, x + cut_tail * e);
      }
      x = h1->GetMean();
      e = h1->GetRMS();
      xe = h1->GetMeanError();
      ee = h1->GetRMSError();
    }
  }
  if (pm) {
    pm->SetBinContent(i, x);
    pm->SetBinError(i, xe);
  }
  if (pw) {
    pw->SetBinContent(i, e);
    pw->SetBinError(i, ee);
  }
}

// projects a TH2 into a profile, expressed as a TH1F.
// This is similar to the TH2::ProfileX method with the difference
// that only the error within 3RMS around the mean is considered in the profile.
// Specify a different range with cut_tail (>0:mean+/-RMS, <0:0+/-RMS).
// Alternatively, fit a Gaussian by specifying fitfun="gaus".
void
IDStandardPerformance::projectStandardProfileY(const TH2 *h, TH1 *profMean, TH1 *profWidth,
                                               const char *fitfun, Double_t cut_tail) {
  if (PRINTDBG) { cout <<"\t"; RLN; cout << " projectStandardProfileY" << endl;}

  if (!h) {
    if (PRINTDBG) { cout <<"\t"; RLN; cout << " no TH2 h. No processing. " << endl; }
    return;
  }
  if (!profMean && !profWidth) {
    if (PRINTDBG) { cout <<"\t"; RLN; cout << " !profMean && !profWidth No processing. " << endl;}
    return;
  }
    if (PRINTDBG) { cout <<"\t"; RLN; cout << "processing for " << h->GetName() << endl;}
  Int_t nb = h->GetNbinsX();
  Double_t xlo = h->GetXaxis()->GetXmin(), xhi = h->GetXaxis()->GetXmax();
  if (profMean) {
    profMean->SetBins(nb, xlo, xhi);
  }
  if (profWidth) {
    profWidth->SetBins(nb, xlo, xhi);
  }
  for (Int_t i = 1; i <= nb; i++) {
    TH1D *h1 = h->ProjectionY(Form("%s_projy_bin%d", h->GetName(), i), i, i);
    SetProfileBin(i, h1, profMean, profWidth, fitfun, cut_tail);
    delete h1;
  }
  if (profMean) {
    profMean->SetEntries(h->GetEntries());
    Update(profMean);
  }
  if (profWidth) {
    profWidth->SetEntries(h->GetEntries());
    Update(profWidth);
  }
}

void
IDStandardPerformance::projectStandardProfileY(const std::vector<TH1F *> &hvec, TH1 *profMean, TH1 *profWidth,
                                               Int_t fold, const char *fitfun, Double_t cut_tail) {
  TH1 *hx = profMean ? profMean : profWidth;

  if (!hx) {
    return;
  }
  Int_t nvec = hvec.size();
  if (fold > nvec) {
    fold = nvec;
  }
  Int_t nb = fold > 0 ? fold : nvec;
  Double_t xlo = hx->GetXaxis()->GetXmin(), xhi = hx->GetXaxis()->GetXmax();
  if (profMean) {
    profMean->SetBins(nb, xlo, xhi);
  }
  if (profWidth) {
    profWidth->SetBins(nb, xlo, xhi);
  }
  Double_t nent = 0.0;
  for (Int_t i = 0; i < nb; i++) {
    Int_t j1 = i, j2 = -1;
    if (fold > 0) {
      j1 = fold + i;
      if (j1 < nvec && hvec[j1]) {
        j2 = fold - i - 1;
      } else {
        j1 = fold - i - 1;
      }
    }
    if (!(j1 >= 0 && hvec[j1])) {
      continue;
    }
    TH1F *h1 = dynamic_cast<TH1F *>(hvec[j1]->Clone(Form("%s_mod", hvec[j1]->GetName())));
    if (j2 >= 0 && hvec[j2]) {
      h1->Add(hvec[j2]);
    }
    SetProfileBin(i + 1, h1, profMean, profWidth, fitfun, cut_tail);
    nent += h1->GetEntries();
    delete h1;
  }
  // restore #entries, which was modified by SetBinContent in SetProfileBin
  if (profMean) {
    profMean->SetEntries(nent);
    Update(profMean);
  }
  if (profWidth) {
    profWidth->SetEntries(nent);
    Update(profWidth);
  }
}

void
IDStandardPerformance::calculateIntegrated(TH1 *puri, TH1 *all, TH1 *matched) {
  if (!matched) {
    return;
  }
  if (!puri) {
    return;
  }

  puri->Reset();
  inverseSum(matched, puri);

  int nbinsx = matched->GetXaxis()->GetNbins();
  double xmin = matched->GetXaxis()->GetXmin();
  double xmax = matched->GetXaxis()->GetXmax();

  TH1 *alli = new TH1F("tmp", "tmp", nbinsx, xmin, xmax);

  inverseSum(all, alli);
  if (!puri->GetSumw2N()) {
    puri->Sumw2();
  }
  puri->Divide(puri, alli, 1, 1, "B");

  Update(puri);
  delete alli;
}

void
IDStandardPerformance::inverseSum(TH1 *in, TH1 *out) {
  if (in->GetEntries() == 0) {
    return;
  }
  int nbinsx = in->GetXaxis()->GetNbins();
  int nbinsy = in->GetYaxis()->GetNbins();
  for (int i = 0; i < nbinsx; ++i) {
    float ntotal = 0;
    for (int j = nbinsy; j >= 0; --j) {
      ntotal += in->GetBinContent(i, j);
      out->SetBinContent(i, j, ntotal);
    }
  }
}

void
IDStandardPerformance::procHistograms(bool isEndOfRun) {
  if (PRINTDBG >= 1) {
    if (PRINTDBG) { cout <<"\t"; RLN; cout << "procHistograms " << m_histDirectoryName << endl;}
  }

  if (isEndOfRun) {

    char fitter[5] = "iter";
    if (PRINTDBG) { cout <<"\t"; RLN; cout << "trying m_d0_vs_eta " << endl;}
    projectStandardProfileY(m_d0_vs_eta, m_d0_mean_vs_eta, m_d0_width_vs_eta, fitter);
    projectStandardProfileY(m_z0_vs_eta, m_z0_mean_vs_eta, m_z0_width_vs_eta, fitter);
    projectStandardProfileY(m_phi_vs_eta, m_phi_mean_vs_eta, m_phi_width_vs_eta, fitter);
    projectStandardProfileY(m_theta_vs_eta, m_theta_mean_vs_eta, m_theta_width_vs_eta, fitter);
    projectStandardProfileY(m_z0st_vs_eta, m_z0st_mean_vs_eta, m_z0st_width_vs_eta, fitter);
    //    projectStandardProfileY (m_qopt_vs_eta,  m_qopt_mean_vs_eta,  m_qopt_width_vs_eta,  fitter);

    projectStandardProfileY(m_d0_vs_pt, m_d0_mean_vs_pt, m_d0_width_vs_pt, fitter);
    projectStandardProfileY(m_z0_vs_pt, m_z0_mean_vs_pt, m_z0_width_vs_pt, fitter);
    projectStandardProfileY(m_phi_vs_pt, m_phi_mean_vs_pt, m_phi_width_vs_pt, fitter);
    projectStandardProfileY(m_theta_vs_pt, m_theta_mean_vs_pt, m_theta_width_vs_pt, fitter);
    projectStandardProfileY(m_z0st_vs_pt, m_z0st_mean_vs_pt, m_z0st_width_vs_pt, fitter);
    //    projectStandardProfileY (m_qopt_vs_pt,  m_qopt_mean_vs_pt,  m_qopt_width_vs_pt,  fitter);

    projectStandardProfileY(m_d0pull_vs_eta, m_d0pull_mean_vs_eta, m_d0pull_width_vs_eta, fitter);
    projectStandardProfileY(m_z0pull_vs_eta, m_z0pull_mean_vs_eta, m_z0pull_width_vs_eta, fitter);
    projectStandardProfileY(m_phipull_vs_eta, m_phipull_mean_vs_eta, m_phipull_width_vs_eta, fitter);
    projectStandardProfileY(m_thetapull_vs_eta, m_thetapull_mean_vs_eta, m_thetapull_width_vs_eta, fitter);
    projectStandardProfileY(m_z0stpull_vs_eta, m_z0stpull_mean_vs_eta, m_z0stpull_width_vs_eta, fitter);

    SetSafeMinimumMaximum(m_d0_mean_vs_eta, -0.03, 0.03);
    SetSafeMinimumMaximum(m_phi_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_theta_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_z0st_mean_vs_eta, -0.05, 0.05);
  }

  if (PRINTDBG >= 1) {
    cout <<"\t"; RLN; cout << "procHistograms " << m_histDirectoryName << " Done." << endl;
  }
}

void
IDStandardPerformance::SetSafeMinimumMaximum(TH1 *h, float min, float max) {
  if (!h) {
    return;
  }
  float hmin = 0.9 * h->GetMinimum();
  float hmax = 1.1 * h->GetMaximum();

  if (hmin < min) {
    h->SetMinimum(hmin);
  } else {
    h->SetMinimum(min);
  }

  if (hmax > max) {
    h->SetMaximum(hmax);
  } else {
    h->SetMaximum(max);
  }
  return;
}

int
IDStandardPerformance::updateFile() {
  int n = 0;

  for (TIter it = &m_update; TObject *o = it(); ) {
    TH1 *h = dynamic_cast<TH1 *>(o);
    if (!h) {
      continue;
    }
    TDirectory *d = h->GetDirectory();
    if (!d) {
      continue;
    }
    if (PRINTDBG > 1) {
      cout << "Updated ";
      PrintHist(h);
    }
    d->WriteTObject(h, h->GetName(), "Overwrite");
    //    cout << "Updated " << h->GetName() << endl;
    n++;
  }
  return n;
}

int postprocessHistos(TDirectory *file, const char *tracksName, const char *dirname) {
  
  TString dir;
  dir.Form("IDPerformanceMon/%s/%s", tracksName, dirname);

  if (PRINTDBG>0) 
    cout << "Now processing histograms in " << dir << endl;

  IDStandardPerformance idsp(file, tracksName, dirname);
  idsp.bookHistograms();
  idsp.procHistograms();
  int n = idsp.updateFile();

  if (PRINTDBG > 0) 
    cout << "\t Updated " << n << " histograms in " << dir << "\n " << endl;
  
  return 1;
}

int
ScanDir(TList &dirs, TDirectory *file, TDirectory *dir, const char *tracksName = 0) {
  int n = 0;

  if (PRINTDBG  > 0) {
    TString path;
    GetPath(dir, path);
    cout << "Scan directory " << path << endl;
  }
  for (TIter it = dir->GetListOfKeys(); TKey *k = dynamic_cast<TKey *>(it()); ) {
    const TClass *c = TClass::GetClass(k->GetClassName());
    if (!(c && c->InheritsFrom(TDirectory::Class()))) {
      continue;
    }
    if (TDirectory *subdir = dynamic_cast<TDirectory *>(k->ReadObj())) {
      if (!tracksName) {
        n += ScanDir(dirs, file, subdir, subdir->GetName());
      } else {
        dirs.Add(new TNamed(tracksName, subdir->GetName()));
      }
      delete subdir;
    }
  }
  return n;
}

int postprocessHistos(const char *name) {

  if (PRINTDBG>0) 
    cout << "Process file " << name << endl;

  if (!FileExists(name)) {
    cerr << "No such file: " << name << endl;
    return 1;
  }
  TFile *file = TFile::Open(name, "UPDATE");
  if (!file) {
    cerr << "Could not UPDATE file " << name << endl;
    return 1;
  }
  
  int n = 0;
  TDirectory *dir = 0;
  file->GetObject("IDPerformanceMon", dir);
  if (dir) {
    TList dirs;
    dirs.SetOwner();
    n += ScanDir(dirs, file, dir, 0);
    delete dir;
    for (TIter it = &dirs; TNamed *name = dynamic_cast<TNamed *>(it()); ) {
      cout << "processing this directory " << name->GetName() << " , " << name->GetTitle() << endl;
      //n += postprocessHistos(file, name->GetName(), name->GetTitle());
    }
  }
  if (!n) {
    cerr << "No histogram directories found in file " << file->GetName() << endl;
    return 1;
  }
  return 0;
}

// for command-line running
void postprocessHistos() {
  for (TIter it = gROOT->GetListOfFiles(); TObject *o = it(); ) {
    TFile *file = dynamic_cast<TFile *>(o);
    if (!file) {
      continue;
    }
    postprocessHistos(file->GetName());
  }
}

#if !defined(__CINT__) && !defined(__ACLIC__)
int main(int argc, const char * *argv) {

  if (argc<2) {
    cout << "Usage: " << argv[0] << " FILE.root [FILE.root...]" << endl;
    return 1;
  }
  int err = 0;
  for (int i=0; i < argc; i++) {
    int stat = postprocessHistos(argv[i]);
    if (!err) {
      err = stat;
    }
  }
  return err;
}
#endif
