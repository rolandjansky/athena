/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*==============================================================================
$Id: reprocessHistos.cxx 545060 2013-04-18 11:23:48Z adye $

Regenerate 1D profile plots (stored as TH1F, not TProfile) from 2D histograms
(TH2F or vector<TH1F>) in an existing InDetStandardPlots.root. The file is
updated with new histograms overriding the old ones (new cycle number).

This can be used to fix up histogram files after merging InDetStandardPlots.root
files using hadd (eg. during Grid validation).

Much of this code is copied from
InnerDetector/InDetValidation/InDetPerformanceRTT/src/IDStandardPerformance.cxx
with dummy create_registeredTH1F (etc) to read in existing histograms rather
than creating new ones.

Usage (from ROOT command):

  root -l -b -q InDetStandardPlots.root reprocessHistos.cxx+

Usage (after compiling and linking against ROOT libraries):

  ./reprocessHistos InDetStandardPlots.root

Author: Tim Adye.

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

using std::string;
using std::cout;
using std::cerr;
using std::endl;

class MonGroup;

class IDStandardPerformance {
public:
  IDStandardPerformance(TDirectory* top,
                        const char* tracksName="Tracks",
                        const char* histDirectoryName="AllTracks",
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
    void Add    (TObject* obj) { m_owned .Add(obj); }
    void Update (TObject* obj) { m_update.Add(obj); }

private:
    // 1D histograms used to determine the track parameter resolutions and possbile biases
    std::vector<TH1F*> m_hz0;
    std::vector<TH1F*> m_hd0;
    std::vector<TH1F*> m_hphi;
    std::vector<TH1F*> m_htheta;
    std::vector<TH1F*> m_hz0st;
    std::vector<TH1F*> m_hqopt;

    std::vector<TH1F*> m_pullz0;
    std::vector<TH1F*> m_pulld0;
    std::vector<TH1F*> m_pullphi;
    std::vector<TH1F*> m_pulltheta;
    std::vector<TH1F*> m_pullz0st;
    std::vector<TH1F*> m_pullqopt;

    std::vector<TH1F*> m_hptz0;
    std::vector<TH1F*> m_hptd0;
    std::vector<TH1F*> m_hptphi;
    std::vector<TH1F*> m_hpttheta;
    std::vector<TH1F*> m_hptz0st;
    std::vector<TH1F*> m_hptqopt;

    // summary plots of track parameter resolutions and means versus eta
    TH1F* m_d0_res_vs_eta;
    TH1F* m_phi_res_vs_eta;
    TH1F* m_qopt_res_vs_eta;
    TH1F* m_z0_res_vs_eta;
    TH1F* m_theta_res_vs_eta;
    TH1F* m_z0st_res_vs_eta;

    TH1F* m_d0_mean_vs_eta;
    TH1F* m_phi_mean_vs_eta;
    TH1F* m_qopt_mean_vs_eta;
    TH1F* m_z0_mean_vs_eta;
    TH1F* m_theta_mean_vs_eta;
    TH1F* m_z0st_mean_vs_eta;

    TH1F* m_d0_pullres_vs_eta;
    TH1F* m_phi_pullres_vs_eta;
    TH1F* m_qopt_pullres_vs_eta;
    TH1F* m_z0_pullres_vs_eta;
    TH1F* m_theta_pullres_vs_eta;
    TH1F* m_z0st_pullres_vs_eta;

    TH1F* m_d0_pullmean_vs_eta;
    TH1F* m_phi_pullmean_vs_eta;
    TH1F* m_qopt_pullmean_vs_eta;
    TH1F* m_z0_pullmean_vs_eta;
    TH1F* m_theta_pullmean_vs_eta;
    TH1F* m_z0st_pullmean_vs_eta;

    // summary plots of track parameter resolutions vs pt
    TH1F* m_d0_res_vs_pt;
    TH1F* m_phi_res_vs_pt;
    TH1F* m_qopt_res_vs_pt;
    TH1F* m_z0_res_vs_pt;
    TH1F* m_theta_res_vs_pt;
    TH1F* m_z0st_res_vs_pt;

    TH1F* m_pullMeanPhi_vs_phi_pixelEndcap;
    TH1F* m_pullWidthPhi_vs_phi_pixelEndcap;
    TH1F* m_pullMeanPhi_vs_phi_pixelBarrel;
    TH1F* m_pullWidthPhi_vs_phi_pixelBarrel;

    std::vector<TH1F*> m_pullMeanPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_pullWidthPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_pullMeanPhi_vs_incident_pixel_endcap_disk;
    std::vector<TH1F*> m_pullWidthPhi_vs_incident_pixel_endcap_disk;
    std::vector<TH1F*> m_residualMeanPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_residualWidthPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_residualMeanPhi_vs_incident_pixel_endcap_disk;
    std::vector<TH1F*> m_residualWidthPhi_vs_incident_pixel_endcap_disk;

    std::vector<TH1F*> m_pullMeanPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_pullWidthPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_pullMeanPhi_vs_eta_pixel_endcap_disk;
    std::vector<TH1F*> m_pullWidthPhi_vs_eta_pixel_endcap_disk;
    std::vector<TH1F*> m_residualMeanPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_residualWidthPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_residualMeanPhi_vs_eta_pixel_endcap_disk;
    std::vector<TH1F*> m_residualWidthPhi_vs_eta_pixel_endcap_disk;

    std::vector<TH1F*> m_pullMeanPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_pullWidthPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_pullMeanPhi_vs_phi_pixel_endcap_disk;
    std::vector<TH1F*> m_pullWidthPhi_vs_phi_pixel_endcap_disk;
    std::vector<TH1F*> m_residualMeanPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_residualWidthPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_residualMeanPhi_vs_phi_pixel_endcap_disk;
    std::vector<TH1F*> m_residualWidthPhi_vs_phi_pixel_endcap_disk;

    std::vector<TH1F*> m_pullMeanZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_pullWidthZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_pullMeanZ_vs_incident_pixel_endcap_disk;
    std::vector<TH1F*> m_pullWidthZ_vs_incident_pixel_endcap_disk;
    std::vector<TH1F*> m_residualMeanZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_residualWidthZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1F*> m_residualMeanZ_vs_incident_pixel_endcap_disk;
    std::vector<TH1F*> m_residualWidthZ_vs_incident_pixel_endcap_disk;

    std::vector<TH1F*> m_pullMeanZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_pullWidthZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_pullMeanZ_vs_eta_pixel_endcap_disk;
    std::vector<TH1F*> m_pullWidthZ_vs_eta_pixel_endcap_disk;
    std::vector<TH1F*> m_residualMeanZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_residualWidthZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1F*> m_residualMeanZ_vs_eta_pixel_endcap_disk;
    std::vector<TH1F*> m_residualWidthZ_vs_eta_pixel_endcap_disk;

    std::vector<TH1F*> m_pullMeanZ_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_pullWidthZ_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_pullMeanZ_vs_phi_pixel_endcap_disk;
    std::vector<TH1F*> m_pullWidthZ_vs_phi_pixel_endcap_disk;
    std::vector<TH1F*> m_residualMeanZ_vs_phi_pixel_barrel_layer;
    std::vector<TH1F*> m_residualWidthZ_vs_phi_pixel_barrel_layer;

    std::vector<TH1F*> m_residualMeanZ_vs_phi_pixel_endcap_disk;
    std::vector<TH1F*> m_residualWidthZ_vs_phi_pixel_endcap_disk;

    std::vector< std::vector<TH1F*> > m_residualMeanPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH1F*> > m_residualWidthPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH1F*> > m_pullMeanPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH1F*> > m_pullWidthPhi_vs_phi_pixel_barrel_etaBins;

    // ****************** residual and pull vs. incident, eta and phi *********************
    TH2F* m_pullPhi_vs_phi_pixelBarrel;
    TH2F* m_pullPhi_vs_phi_pixelEndcap;
    std::vector<TH2F*> m_pullZ_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_pullZ_vs_incident_pixel_endcap_d;
    std::vector<TH2F*> m_residualZ_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_residualZ_vs_incident_pixel_endcap_d;

    std::vector<TH2F*> m_pullPhi_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_pullPhi_vs_incident_pixel_endcap_d;
    std::vector<TH2F*> m_residualPhi_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_residualPhi_vs_incident_pixel_endcap_d;

    std::vector<TH2F*> m_pullZ_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_pullZ_vs_eta_pixel_endcap_d;
    std::vector<TH2F*> m_residualZ_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_residualZ_vs_eta_pixel_endcap_d;

    std::vector<TH2F*> m_pullPhi_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_pullPhi_vs_eta_pixel_endcap_d;
    std::vector<TH2F*> m_residualPhi_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_residualPhi_vs_eta_pixel_endcap_d;

    std::vector<TH2F*> m_pullZ_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_pullZ_vs_phi_pixel_endcap_d;
    std::vector<TH2F*> m_residualZ_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_residualZ_vs_phi_pixel_endcap_d;

    std::vector<TH2F*> m_pullPhi_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_pullPhi_vs_phi_pixel_endcap_d;
    std::vector<TH2F*> m_residualPhi_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_residualPhi_vs_phi_pixel_endcap_d;

    // ****************** residual and pull vs. incident, eta and phi *********************

    // Plots of the residual and pulls in bins of eta
    std::vector< std::vector<TH2F*> > m_residualPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH2F*> > m_pullPhi_vs_phi_pixel_barrel_etaBins;

    TH2F* m_residualPhi_vs_incident_pixel;
    TH2F* m_pullPhi_vs_incident_pixel;

    TH1F* m_residualMeanPhi_vs_incident_pixel;
    TH1F* m_pullMeanPhi_vs_incident_pixel;
    TH1F* m_residualWidthPhi_vs_incident_pixel;
    TH1F* m_pullWidthPhi_vs_incident_pixel;

    TH1F* create_registeredTH1F(MonGroup &mon
	, const char* name
	, const char* title
	, int nBins
	, double min
	, double max);
    TH2F* create_registeredTH2F(MonGroup &mon
	, const char* name
	, const char* title
	, int nBinsX
	, double minX
	, double maxX
	, int nBinsY
	, double minY
	, double maxY);
    TProfile* create_registeredTProfile(MonGroup &mon
	, const char* name
	, const char* title
	, int nBins
	, double min
	, double max
	, double ylow
	, double yup);
    TProfile* create_registeredTProfile(MonGroup &mon
	, const char* name
	, const char* title
	, int nBins
	, double min
	, double max);

    void SetSafeMinimumMaximum(TH1* h1, float min, float max);

    void CopyProfile (const TProfile* p, TH1* h);
    void projectStandardProfileY (const TH2* h, TH1* profMean, TH1* profWidth=0,
                                  const char* fitfun=0, Double_t cut_tail=3.0);
    void projectStandardProfileY (const std::vector<TH1F*>& hvec, TH1* profMean, TH1* profWidth=0,
                                  Int_t fold=-1, const char* fitfun=0, Double_t cut_tail=3.0);

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
    TH1* h= 0;
    m_tool->GetDirectory()->GetObject (path, h);
    if (h) {
      if (m_level>=2) { cout << "Read "; PrintHist(h); }
      m_tool->Add(h);
    } else if (m_level>=0)
      cerr << "TH1::"<<path<<" not found" << endl;
    return h;
  }
private:
  IDStandardPerformance* m_tool;
  std::string m_system;
  int m_level, m_interval;
};


TH1F* IDStandardPerformance::create_registeredTH1F(MonGroup &mon
                                           , const char* name
                                           , const char* 
                                           , int 
                                           , double 
                                           , double)
{
  TH1F* hist = 0;//new TH1F(name, title, nBins, min, max);
  hist = dynamic_cast<TH1F*>(mon.regHist(name));
  return hist;
}

TH2F* IDStandardPerformance::create_registeredTH2F(MonGroup &mon
                                          , const char* name
                                          , const char* 
                                          , int 
                                          , double 
                                          , double 
                                          , int 
                                          , double 
                                          , double)
{
    TH2F* hist = 0;//new TH2F(name, title, nBinsX, minX, maxX, nBinsY, minY, maxY);
    hist = dynamic_cast<TH2F*>(mon.regHist(name));
    return hist;
}


TProfile* IDStandardPerformance::create_registeredTProfile(MonGroup &mon
                                               , const char* name
                                               , const char* 
                                               , int 
                                               , double 
                                               , double 
                                               , double 
                                               , double)
{
  TProfile* hist = 0;//new TProfile(name, title, nBins, min, max, ylow, yup);
  hist = dynamic_cast<TProfile*>(mon.regHist(name));
  return hist;
}

TProfile* IDStandardPerformance::create_registeredTProfile(MonGroup &mon
                                               , const char* name
                                               , const char* 
                                               , int 
                                               , double 
                                               , double)
{
  TProfile* hist = 0;//new TProfile(name, title, nBins, min, max);
  hist = dynamic_cast<TProfile*>(mon.regHist(name));
  return hist;
}


void IDStandardPerformance::bookHistograms(bool isNewRun)
{
  if (m_verbose>=1) cout << "bookHistograms "<<m_histDirectoryName<<endl;
  int expert=m_verbose, debug=m_verbose, shift=m_verbose, run=0;
  //MsgStream log( msgSvc(), name() );

  std::string outputDirName = "IDPerformanceMon/" + m_tracksName + "/" + m_histDirectoryName;
  //  std::string outputDirName =  "/" + m_tracksName + "/" + m_histDirectoryName;
  MonGroup al_expert(this, outputDirName, expert, run);
  MonGroup al_shift(this, outputDirName, shift, run);
  MonGroup pull_parent(this, outputDirName+"/residualPulls", shift, run);
  MonGroup pull_shift(this, outputDirName+"/residualPulls/raw", shift, run);
  MonGroup al_debug(this, outputDirName, debug, run);

  if (isNewRun) {

    // reserve the eta bins 
    m_hd0.reserve(m_trackEtaBins);
    m_hz0.reserve(m_trackEtaBins);
    m_hphi.reserve(m_trackEtaBins);
    m_htheta.reserve(m_trackEtaBins);
    m_hz0st.reserve(m_trackEtaBins);
    m_hqopt.reserve(m_trackEtaBins);
    m_pulld0.reserve(m_trackEtaBins);
    m_pullz0.reserve(m_trackEtaBins);
    m_pullphi.reserve(m_trackEtaBins);
    m_pulltheta.reserve(m_trackEtaBins);
    m_pullz0st.reserve(m_trackEtaBins);
    m_pullqopt.reserve(m_trackEtaBins);

    // base histograms for resolution plots versus eta and pt
    for (int i=0;i<m_trackEtaBins;i++) {
      // residuals 
      char name[100];
      sprintf(name,"d0res_bin%i",i);
      m_hd0.push_back(create_registeredTH1F(al_debug, name,name,2000,-1.0,1.0));

      sprintf(name,"z0res_bin%i",i);
      m_hz0.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      sprintf(name,"phires_bin%i",i);
      m_hphi.push_back(create_registeredTH1F(al_debug, name,name,3000,-0.03,0.03));

      sprintf(name,"thetares_bin%i",i);
      m_htheta.push_back(create_registeredTH1F(al_debug, name,name,2000,-0.1,0.1));

      sprintf(name,"z0stres_bin%i",i);
      m_hz0st.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      sprintf(name,"qoptres_bin%i",i);
      m_hqopt.push_back(create_registeredTH1F(al_debug, name,name,200,-0.4,0.4));

      // pull distributions
      sprintf(name,"d0pullres_bin%i",i);
      m_pulld0.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      
      sprintf(name,"z0pullres_bin%i",i);
      m_pullz0.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      sprintf(name,"phipullres_bin%i",i);
      m_pullphi.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      sprintf(name,"thetapullres_bin%i",i);
      m_pulltheta.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      sprintf(name,"z0stpullres_bin%i",i);
      m_pullz0st.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      sprintf(name,"qoptpullres_bin%i",i);
      m_pullqopt.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
   }

    // reserve the pt bins
    m_hptd0.reserve(m_trackPtBins);
    m_hptz0.reserve(m_trackPtBins);
    m_hptphi.reserve(m_trackPtBins);
    m_hpttheta.reserve(m_trackPtBins);
    m_hptz0st.reserve(m_trackPtBins);
    m_hptqopt.reserve(m_trackPtBins);

    // resolutions versus pt
    for (int i=0;i<m_trackPtBins;i++) {
      char name[100];
      sprintf(name,"d0res_ptbin%i",i);
      m_hptd0.push_back(create_registeredTH1F(al_debug, name,name,2000,-1.0,1.0));

      sprintf(name,"z0res_ptbin%i",i);
      m_hptz0.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      sprintf(name,"phires_ptbin%i",i);
      m_hptphi.push_back(create_registeredTH1F(al_debug, name,name,3000,-0.03,0.03));

      sprintf(name,"thetares_ptbin%i",i);
      m_hpttheta.push_back(create_registeredTH1F(al_debug, name,name,2000,-0.1,0.1));

      sprintf(name,"z0stres_ptbin%i",i);
      m_hptz0st.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      sprintf(name,"qoptres_ptbin%i",i);
      m_hptqopt.push_back(create_registeredTH1F(al_debug, name,name,200,-0.4,0.4));
    }

    int nbins_eta = m_trackEtaBins;
    float min_eta = -m_maxTrackEta;
    float max_eta = +m_maxTrackEta;
    if (m_plotsVsAbsEta){
      nbins_eta = m_trackEtaBins/2;
      min_eta = 0;
    }

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions amnd means versus eta
    // ------------------------------------------------------------------
    m_d0_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_d0","Track Resolution: d_{0} versus #eta;#eta;#sigma(d^{rec}_{0}-d^{tru}_{0})",nbins_eta,min_eta,max_eta);
    m_phi_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_phi","Track Resolution: #phi versus #eta;#eta;#sigma(#phi^{rec}-#phi^{tru})",nbins_eta,min_eta,max_eta);
    m_qopt_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_qopt","Track Resolution: 1/p_{T} versus #eta;#eta;#sigma(q/p_{T}^{rec}-q/p_{T}^{tru})",nbins_eta,min_eta,max_eta);
    m_z0_res_vs_eta= create_registeredTH1F(al_shift, "res_vs_eta_z0","Track Resolution: z_{0} versus #eta;#eta;#sigma(z^{rec}_{0}-z^{tru}_{0})",nbins_eta,min_eta,max_eta);
    m_theta_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_theta","Track Resolution: #theta versus #eta;#eta;#sigma(#theta^{rec}-#theta^{tru})",nbins_eta,min_eta,max_eta);
    m_z0st_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_z0st","Track Resolution: z_{0}*sin(#theta) versus #eta;#eta;#sigma(z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru})",nbins_eta,min_eta,max_eta);

    m_d0_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_d0","Track Measurement Bias: d_{0} versus #eta;#eta;d^{rec}_{0}-d^{tru}_{0}",nbins_eta,min_eta,max_eta);
    m_phi_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_phi","Track Measurement Bias: #phi versus #eta;#eta;#phi^{rec}-#phi^{tru}",nbins_eta,min_eta,max_eta);
    m_qopt_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_qopt","Track Measurement Bias: 1/p_{T} versus #eta;#eta;q/p_{T}^{rec}-q/p_{T}^{tru}",nbins_eta,min_eta,max_eta);
    m_z0_mean_vs_eta= create_registeredTH1F(al_shift, "mean_vs_eta_z0","Track Measurement Bias: z_{0} versus #eta;#eta;z^{rec}_{0}-z^{tru}_{0}",nbins_eta,min_eta,max_eta);
    m_theta_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_theta","Track Measurement Bias: #theta versus #eta;#eta;#theta^{rec}-#theta^{tru}",nbins_eta,min_eta,max_eta);
    m_z0st_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_z0st","Track Measurement Bias: z_{0}*sin(#theta) versus #eta;#eta;z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru}",nbins_eta,min_eta,max_eta);

    m_d0_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_d0","Track Pull Width: d_{0} versus #eta;#eta;#sigma(pull(d_{0}))",nbins_eta,min_eta,max_eta);
    m_phi_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_phi","Track Pull Width: #phi versus #eta;#eta;#sigma(pull(#phi))",nbins_eta,min_eta,max_eta);
    m_qopt_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_qopt","Track Pull Width: 1/p_{T} versus #eta;#eta;#sigma(pull(q/p_{T}))",nbins_eta,min_eta,max_eta);
    m_z0_pullres_vs_eta= create_registeredTH1F(al_shift, "pullres_vs_eta_z0","Track Pull Width: z_{0} versus #eta;#eta;#sigma(pull(z_{0}))",nbins_eta,min_eta,max_eta);
    m_theta_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_theta","Track Pull Width: #theta versus #eta;#eta;#sigma(pull(#theta))",nbins_eta,min_eta,max_eta);
    m_z0st_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_z0st","Track Pull Width: z_{0}*sin(#theta) versus #eta;#eta;#sigma(pull(z_{0}sin(#theta)))",nbins_eta,min_eta,max_eta);

    m_d0_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_d0","Track Pull Mean: d_{0} versus #eta;#eta;pull(d_{0})",nbins_eta,min_eta,max_eta);
    m_phi_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_phi","Track Pull Mean: #phi versus #eta;#eta;pull(#phi)",nbins_eta,min_eta,max_eta);
    m_qopt_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_qopt","Track Pull Mean: 1/p_{T} versus #eta;#eta;pull(q/p_{T})",nbins_eta,min_eta,max_eta);
    m_z0_pullmean_vs_eta= create_registeredTH1F(al_shift, "pullmean_vs_eta_z0","Track Pull Mean: z_{0} versus #eta;#eta;pull(z_{0})",nbins_eta,min_eta,max_eta);
    m_theta_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_theta","Track Pull Mean: #theta versus #eta;#eta;pull(#theta)",nbins_eta,min_eta,max_eta);
    m_z0st_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_z0st","Track Pull Mean: z_{0}*sin(#theta) versus #eta;#eta;pull(z_{0}sin(#theta))",nbins_eta,min_eta,max_eta);

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions versus pt
    // ------------------------------------------------------------------
    m_d0_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_d0","Track Resolution: d_{0} versus log10(p_{T});p_{T}(GeV/c);#sigma(d^{rec}_{0}-d^{tru}_{0})",m_trackPtBins,-0.5,2);
    m_phi_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_phi","Track Resolution: #phi versus log10(p_{T});p_{T}(GeV/c);#sigma(#phi^{rec}-#phi^{tru})",m_trackPtBins,-0.5,2);
    m_qopt_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_qopt","Track Resolution: p_{T} versus log10(p_{T});p_{T}(GeV/c);#sigma(q/p_{T}^{rec}-q/p_{T}^{tru})",m_trackPtBins,-0.5,2);
    m_z0_res_vs_pt= create_registeredTH1F(al_shift, "res_vs_pt_z0","Track Resolution: z_{0} versus log10(p_{T});p_{T}(GeV/c);#sigma(z^{rec}_{0}-z^{tru}_{0})",m_trackPtBins,-0.5,2);
    m_theta_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_theta","Track Resolution: #theta versus log10(p_{T});p_{T}(GeV/c);#sigma(#theta^{rec}-#theta^{tru})",m_trackPtBins,-0.5,2);
    m_z0st_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_z0st","Track Resolution: z_{0}*sin(#theta) versus log10(p_{T});p_{T}(GeV/c);#sigma(z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru})",m_trackPtBins,-0.5,2);

    const double min_normal = 0;
    const double max_normal = TMath::PiOver2();
    const double min_phi = 0;
    const double max_phi = TMath::PiOver4();
    const double min_phi_endcap = -0.5*TMath::PiOver4();
    const double max_phi_endcap =  0.5*TMath::PiOver4();
    const double min_eta_endcap = 0;
    const double max_eta_endcap = TMath::PiOver4();
    const double min_normal_endcap = 0.1;
    const double max_normal_endcap = 0.6;// TMath::PiOver4();
    m_pullPhi_vs_incident_pixel = create_registeredTH2F(pull_shift, "pullPhi_vs_incident_pixel", "Pull (r-phi) vs. incident on the pixel plane;angle w/ normal;pull (r-phi)", 25, min_normal, max_normal, 100, -5, 5);
    m_residualPhi_vs_incident_pixel = create_registeredTH2F(pull_shift, "residualPhi_vs_incident_pixel", "Residual (r-phi) vs. incident on the pixel plane;angle w/ normal;residual (r-phi)", 25, min_normal, max_normal, 200, -0.5, 0.5);
    m_pullMeanPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "pullMeanPhi_vs_incident_pixel", "Pull Mean (r-phi) vs. incident on the pixel plane;angle w/ normal;pull (r-phi) mean", 100, -5, 5);
    m_pullWidthPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "pullWidthPhi_vs_incident_pixel", "Pull Width (r-phi) vs. incident on the pixel plane;angle w/ normal;pull (r-phi) width", 100, -5, 5);
    m_residualMeanPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "residualMeanPhi_vs_incident_pixel", "Residual Mean (r-phi) vs. incident on the pixel plane;angle w/ normal;residual (r-phi) mean", 20, 0, 1);
    m_residualWidthPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "residualWidthPhi_vs_incident_pixel", "Residual Width (r-phi) vs. incident on the pixel plane;angle w/ normal;residual (r-phi) width", 20, 0, 1);
    m_pullPhi_vs_phi_pixelBarrel = create_registeredTH2F(pull_shift, "pullPhi_vs_phi_pixelBarrel", "Pull (r-phi) vs. phi, Barrel", 25, min_phi, max_phi, 100, -5, 5);
    m_pullPhi_vs_phi_pixelEndcap = create_registeredTH2F(pull_shift, "pullPhi_vs_phi_pixelEndcap", "Pull (r-phi) vs. phi, Endcap", 25, min_phi_endcap, max_phi_endcap, 100, -5, 5);
    m_pullMeanPhi_vs_phi_pixelBarrel = create_registeredTH1F(pull_parent, "pullMeanPhi_vs_phi_pixelBarrel", "Pull Mean (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);
    m_pullWidthPhi_vs_phi_pixelBarrel = create_registeredTH1F(pull_parent, "pullWidthPhi_vs_phi_pixelBarrel", "Pull Width (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);
    m_pullMeanPhi_vs_phi_pixelEndcap = create_registeredTH1F(pull_parent, "pullMeanPhi_vs_phi_pixelEndcap", "Pull Mean (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);
    m_pullWidthPhi_vs_phi_pixelEndcap = create_registeredTH1F(pull_parent, "pullWidthPhi_vs_phi_pixelEndcap", "Pull Width (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);

    // the number of Pixel layers and disks
    int pixelBarrelLayers = 3; // m_PIX_Mgr->numerology().numLayers();
    int pixelEndcapDisks  = 3; // m_PIX_Mgr->numerology().numDisks();
    int nPixelLayers = pixelBarrelLayers > pixelEndcapDisks ? pixelBarrelLayers : pixelEndcapDisks;

    m_pullZ_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualZ_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullPhi_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualPhi_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullZ_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualZ_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullPhi_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualPhi_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullZ_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualZ_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullPhi_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualPhi_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullMeanPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);


    // initialize the eta binned histograms
    for (int ieta=0; ieta<5; ieta++){
        m_residualPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH2F*>(pixelBarrelLayers));
        m_pullPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH2F*>(pixelBarrelLayers));
        m_residualMeanPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
        m_residualWidthPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
        m_pullMeanPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
        m_pullWidthPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
    }

    m_pullZ_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualZ_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullPhi_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualPhi_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullZ_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualZ_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullPhi_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualPhi_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullZ_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualZ_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullPhi_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualPhi_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullMeanPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);




    for (int ilayer=0; ilayer<nPixelLayers; ilayer++) {
      char name[200];
      char title[200];

      // pixel barrel section
      if (ilayer < pixelBarrelLayers){
        sprintf(name, "pullPhi_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull: Pixel Barrel Phi vs incident normal Layer %i;angle w/ normal;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 100, -5, 5));
        sprintf(name, "residualPhi_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual: Pixel Barrel Phi vs incident normal Layer %i;angle w/ normal;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 200, -0.5, 0.5));

        sprintf(name, "pullPhi_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull: Pixel Barrel Phi vs incident eta Layer %i;pixel eta;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 100, -5, 5));
        sprintf(name, "residualPhi_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual: Pixel Barrel Phi vs incident eta Layer %i;pixel eta;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 200, -0.5, 0.5));

        sprintf(name, "pullPhi_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull: Pixel Barrel Phi vs incident phi Layer %i;pixel phi;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 100, -5, 5));
        sprintf(name, "residualPhi_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual: Pixel Barrel Phi vs incident phi Layer %i;pixel phi;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 200, -0.5, 0.5));
// --
        sprintf(name, "pullZ_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull: Pixel Barrel Z vs incident normal Layer %i;angle w/ normal;pull (r-z)" ,ilayer);
        m_pullZ_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 100, -5, 5));
        sprintf(name, "residualZ_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual: Pixel Barrel Z vs incident normal Layer %i;angle w/ normal;residual (r-z)" ,ilayer);
        m_residualZ_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 200, -1, 1));

        sprintf(name, "pullZ_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull: Pixel Barrel Z vs incident eta Layer %i;pixel eta;pull (r-z)" ,ilayer);
        m_pullZ_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 100, -5, 5));
        sprintf(name, "residualZ_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual: Pixel Barrel Z vs incident eta Layer %i;pixel eta;residual (r-z)" ,ilayer);
        m_residualZ_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 200, -1, 1));

        sprintf(name, "pullZ_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull: Pixel Barrel Z vs incident phi Layer %i;pixel phi;pull (r-z)" ,ilayer);
        m_pullZ_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 100, -5, 5));
        sprintf(name, "residualZ_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual: Pixel Barrel Z vs incident phi Layer %i;pixel phi;residual (r-z)" ,ilayer);
        m_residualZ_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 200, -1, 1));

        // One profile each for the value and the width
        // The axis is meaningless, as it's projected from the 2d hist
        sprintf(name, "pullMeanPhi_vs_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthPhi_vs_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanPhi_vs_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        sprintf(name, "pullWidthPhi_vs_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanPhi_vs_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthPhi_vs_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanPhi_vs_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;residual (r-phi) mean" ,ilayer);

        m_residualMeanPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        sprintf(name, "residualWidthPhi_vs_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanPhi_vs_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualWidthPhi_vs_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanPhi_vs_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
      
        sprintf(name, "residualWidthPhi_vs_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanZ_vs_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull mean: Pixel Barrel Z Layer %i vs. incident normal;angle w/ normal;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        sprintf(name, "pullWidthZ_vs_incident_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull width: Pixel Barrel Z Layer %i vs. incident normal;angle w/ normal;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanZ_vs_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull mean: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthZ_vs_eta_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull width: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanZ_vs_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull mean: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthZ_vs_phi_pixel_barrel_l%i", ilayer);
        sprintf(title, "Pull width: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      sprintf(name, "residualMeanZ_vs_incident_pixel_barrel_l%i", ilayer);
      sprintf(title, "Residual mean: Pixel Barrel Z Layer %i vs. incident normal;angle w/ normal;residual (r-z) mean" ,ilayer);
      m_residualMeanZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      sprintf(name, "residualWidthZ_vs_incident_pixel_barrel_l%i", ilayer);
      sprintf(title, "Residual width: Pixel Barrel Z Layer %i vs. incident normal;angle w/normal;residual (r-z) width" ,ilayer);
      m_residualWidthZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      sprintf(name, "residualMeanZ_vs_eta_pixel_barrel_l%i", ilayer);
      sprintf(title, "Residual mean: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;residual (r-z) mean" ,ilayer);
      m_residualMeanZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      sprintf(name, "residualWidthZ_vs_eta_pixel_barrel_l%i", ilayer);
      sprintf(title, "Residual width: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;residual (r-z) width" ,ilayer);
      m_residualWidthZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      sprintf(name, "residualMeanZ_vs_phi_pixel_barrel_l%i", ilayer);
      sprintf(title, "Residual mean: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;residual (r-z) mean" ,ilayer);
      m_residualMeanZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      sprintf(name, "residualWidthZ_vs_phi_pixel_barrel_l%i", ilayer);
      sprintf(title, "Residual width: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;residual (r-z) width" ,ilayer);
      m_residualWidthZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

     }
// --
      // pixel endcap section
      if (ilayer < pixelEndcapDisks){
        sprintf(name, "pullPhi_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull: Pixel Endcap Phi vs incident normal Disk %i;angle w/ normal;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal_endcap, max_normal_endcap, 100, -5, 5));
        sprintf(name, "residualPhi_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual: Pixel Endcap Phi vs incident normal Disk %i;angle w/ normal;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal_endcap, max_normal_endcap, 200, -0.5, 0.5));

        sprintf(name, "pullPhi_pixel_eta_endcap_d%i", ilayer);
        sprintf(title, "Pull: Pixel Endcap Phi vs incident eta Disk %i;pixel eta;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta_endcap, max_eta_endcap, 100, -5, 5));
        sprintf(name, "residualPhi_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual: Pixel Endcap Phi vs incident eta Disk %i;pixel eta;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta_endcap, max_eta_endcap, 200, -0.5, 0.5));

        sprintf(name, "pullPhi_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull: Pixel Endcap Phi vs incident phi Disk %i;pixel phi;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 100, -5, 5));

        sprintf(name, "residualPhi_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual: Pixel Endcap Phi vs incident phi Disk %i;pixel phi;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 200, -0.5, 0.5));
// --
        sprintf(name, "pullZ_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull: Pixel Endcap Z vs incident normal Disk %i;angle w/ normal;pull (r-z)" ,ilayer);
        m_pullZ_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_normal_endcap, max_normal_endcap, 100, -5, 5));

        sprintf(name, "residualZ_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual: Pixel Endcap Z vs incident normal Disk %i;angle w/ normal;residual (r-z)" ,ilayer);
        m_residualZ_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_normal_endcap, max_normal_endcap, 200, -1, 1));

        sprintf(name, "pullZ_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull: Pixel Endcap Z vs incident eta Disk %i;pixel eta;pull (r-z)" ,ilayer);
        m_pullZ_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_eta_endcap, max_eta_endcap, 100, -5, 5));

        sprintf(name, "residualZ_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual: Pixel Endcap Z vs incident eta Disk %i;pixel eta;residual (r-z)" ,ilayer);
        m_residualZ_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_eta_endcap, max_eta_endcap, 200, -1, 1));

        sprintf(name, "pullZ_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull: Pixel Endcap Z vs incident phi Disk %i;pixel phi;pull (r-z)" ,ilayer);
        m_pullZ_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 100, -5, 5));

        sprintf(name, "residualZ_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual: Pixel Endcap Z vs incident phi Disk %i;pixel eta;residual (r-z)" ,ilayer);
        m_residualZ_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 200, -1, 1));

        sprintf(name, "pullMeanPhi_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull mean: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthPhi_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanPhi_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull mean: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthPhi_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanPhi_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull mean: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullWidthPhi_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanPhi_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual mean: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualWidthPhi_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual width: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanPhi_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual mean: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
      
        sprintf(name, "residualWidthPhi_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual width: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanPhi_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual mean: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualWidthPhi_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual width: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanZ_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        sprintf(name, "pullWidthZ_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanZ_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        sprintf(name, "pullWidthZ_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "pullMeanZ_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        sprintf(name, "pullWidthZ_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Pull width: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanZ_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual mean: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualWidthZ_vs_incident_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual width: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanZ_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual mean: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualWidthZ_vs_eta_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual width: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        sprintf(name, "residualMeanZ_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual mean: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
      
        sprintf(name, "residualWidthZ_vs_phi_pixel_endcap_d%i", ilayer);
        sprintf(title, "Residual width: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      }

      // eta binned regions
      string etaBinLabels[5];
      etaBinLabels[0] = "0 < eta <= 0.5";
      etaBinLabels[1] = "0.5 < eta <= 1.0";
      etaBinLabels[2] = "1.0 < eta <= 1.5";
      etaBinLabels[3] = "1.5 < eta <= 2.0";
      etaBinLabels[4] = "2.0 < eta <= 2.5";
      for (int iEtaBin=0; iEtaBin<5; iEtaBin++) {
          if (ilayer < pixelBarrelLayers){
            sprintf(name, "pullPhi_phi_pixel_endcap_d%i_etaBin%i", ilayer, iEtaBin);
            sprintf(title, "Pull: Pixel Barrel Phi vs incident phi Layer %i, %s;pixel phi;pull (r-phi)" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_pullPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi, max_phi, 200, -5, 5);
            sprintf(name, "residualPhi_phi_pixel_endcap_d%i_etaBin%i", ilayer, iEtaBin);
            sprintf(title, "Residual: Pixel Barrel Phi vs incident phi Layer %i, %s;pixel phi;residual (r-phi)" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_residualPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi, max_phi, 200, -1, 1);
            // One profile each for the value and the width
            // The axis is meaningless, as it's projected from the 2d hist
            sprintf(name, "pullMeanPhi_vs_incident_pixel_barrel_l%i_etaBin%i", ilayer, iEtaBin);
            sprintf(title, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;pull (r-phi) mean" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_pullMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
            sprintf(name, "pullWidthPhi_vs_incident_pixel_barrel_l%i_etaBin_%i", ilayer, iEtaBin);
            sprintf(title, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;pull (r-phi) width" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_pullWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
            sprintf(name, "residualMeanPhi_vs_incident_pixel_barrel_l%i_etaBin%i", ilayer, iEtaBin);
            sprintf(title, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;residual (r-phi) mean" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_residualMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
            sprintf(name, "residualWidthPhi_vs_incident_pixel_barrel_l%i_etaBin%i", ilayer, iEtaBin);
            sprintf(title, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;residual(r-phi) width" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_residualWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
         }
      }
// --
    } // Pixel Layer section done 
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
    if (fitfun) {
      TFitResultPtr r= h1->Fit (fitfun, "QS0");
      if (r.Get() && r->Status()%1000==0) {
        x=  r->Parameter(1); e=  r->Parameter(2);
        xe= r->ParError (1); ee= r->ParError (2);
      }
    } else if (cut_tail!=0.0) {
      Double_t rmin, rmax;
      if (cut_tail<0.0) { rmin=   cut_tail*e; rmax=  -cut_tail*e; }
      else              { rmin= x-cut_tail*e; rmax= x+cut_tail*e; }
      // Work round ROOT 5.34.04 change that would include the under/overflow bins
      // if the new range extends beyond the histogram edges.
      if (rmin < h1->GetBinLowEdge(1))              rmin= h1->GetBinLowEdge(1);
      if (rmax > h1->GetBinCenter(h1->GetNbinsX())) rmax= h1->GetBinCenter(h1->GetNbinsX());
      h1->SetAxisRange (rmin, rmax);
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


// projects a TH2 into a profile, expressed as a TH1F.
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
  for (Int_t i=0; i<=nb+1; i++) {
    TH1D* h1= h->ProjectionY(Form("%s_projy_bin%d",h->GetName(),i), i, i);
    SetProfileBin (i, h1, profMean, profWidth, fitfun, cut_tail);
    delete h1;
  }
  // restore #entries, which was modified by SetBinContent in SetProfileBin
  if (profMean) {
    profMean ->SetEntries(h->GetEntries());
    Update (profMean);
  }
  if (profWidth) {
    profWidth->SetEntries(h->GetEntries());
    Update (profWidth);
  }
}


void IDStandardPerformance::projectStandardProfileY (const std::vector<TH1F*>& hvec, TH1* profMean, TH1* profWidth,
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
    TH1F* h1= dynamic_cast<TH1F*>(hvec[j1]->Clone(Form("%s_mod",hvec[j1]->GetName())));
    if (!h1) continue;
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


void IDStandardPerformance::procHistograms( bool isEndOfRun )
{

  if (m_verbose>=1) cout << "procHistograms "<<m_histDirectoryName<<endl;
  //MsgStream log( msgSvc(), name() );
  //  if (m_idHelper) delete m_idHelper;

  if ( isEndOfRun ) {
 
    int halfEtaBins = m_trackEtaBins/2;
    int fold= m_plotsVsAbsEta ? halfEtaBins : 0;

    projectStandardProfileY (m_hd0,    m_d0_mean_vs_eta,    m_d0_res_vs_eta,    fold, 0, -3.0);
    projectStandardProfileY (m_hz0,    m_z0_mean_vs_eta,    m_z0_res_vs_eta,    fold, 0, -3.0);
    projectStandardProfileY (m_hz0st,  m_z0st_mean_vs_eta,  m_z0st_res_vs_eta,  fold, 0, -3.0);
    projectStandardProfileY (m_hphi,   m_phi_mean_vs_eta,   m_phi_res_vs_eta,   fold, 0, -3.0);
    projectStandardProfileY (m_hqopt,  m_qopt_mean_vs_eta,  m_qopt_res_vs_eta,  fold, 0, -3.0);
    projectStandardProfileY (m_htheta, m_theta_mean_vs_eta, m_theta_res_vs_eta, fold, 0, -3.0);

    projectStandardProfileY (m_pulld0,    m_d0_pullmean_vs_eta,    m_d0_pullres_vs_eta,    fold, "gaus");
    projectStandardProfileY (m_pullz0,    m_z0_pullmean_vs_eta,    m_z0_pullres_vs_eta,    fold, "gaus");
//  projectStandardProfileY (m_pullz0st,  m_z0st_pullmean_vs_eta,  m_z0st_pullres_vs_eta,  fold, "gaus");
    projectStandardProfileY (m_pullphi,   m_phi_pullmean_vs_eta,   m_phi_pullres_vs_eta,   fold, "gaus");
    projectStandardProfileY (m_pullqopt,  m_qopt_pullmean_vs_eta,  m_qopt_pullres_vs_eta,  fold, "gaus");
    projectStandardProfileY (m_pulltheta, m_theta_pullmean_vs_eta, m_theta_pullres_vs_eta, fold, "gaus");

    projectStandardProfileY (m_hptd0,    0, m_d0_res_vs_pt,    -1, 0, -3.0);
    projectStandardProfileY (m_hptz0,    0, m_z0_res_vs_pt,    -1, 0, -3.0);
    projectStandardProfileY (m_hptz0st,  0, m_z0st_res_vs_pt,  -1, 0, -3.0);
    projectStandardProfileY (m_hptphi,   0, m_phi_res_vs_pt,   -1, 0, -3.0);
    projectStandardProfileY (m_hptqopt,  0, m_qopt_res_vs_pt,  -1, 0, -3.0);
    projectStandardProfileY (m_hpttheta, 0, m_theta_res_vs_pt, -1, 0, -3.0);

    SetSafeMinimumMaximum(m_d0_mean_vs_eta, -0.03, 0.03);
    SetSafeMinimumMaximum(m_phi_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_qopt_mean_vs_eta, -0.01, 0.01);
    SetSafeMinimumMaximum(m_theta_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_z0st_mean_vs_eta, -0.05, 0.05);
  }
  projectStandardProfileY(m_pullPhi_vs_incident_pixel
  , m_pullMeanPhi_vs_incident_pixel, m_pullWidthPhi_vs_incident_pixel);
  SetSafeMinimumMaximum(m_pullMeanPhi_vs_incident_pixel, -0.3, 0.3);
  SetSafeMinimumMaximum(m_pullWidthPhi_vs_incident_pixel, 0.7, 1.3);
  projectStandardProfileY(m_residualPhi_vs_incident_pixel
  , m_residualMeanPhi_vs_incident_pixel, m_residualWidthPhi_vs_incident_pixel);
  SetSafeMinimumMaximum(m_residualMeanPhi_vs_incident_pixel, -0.3, 0.3);
  SetSafeMinimumMaximum(m_residualWidthPhi_vs_incident_pixel, 0, 0.5);

  projectStandardProfileY(m_pullPhi_vs_phi_pixelBarrel
  , m_pullMeanPhi_vs_phi_pixelBarrel, m_pullWidthPhi_vs_phi_pixelBarrel);
  SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixelBarrel, -0.3, 0.3);
  SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixelBarrel, 0.7, 1.3);
  projectStandardProfileY(m_pullPhi_vs_phi_pixelEndcap
  , m_pullMeanPhi_vs_phi_pixelEndcap, m_pullWidthPhi_vs_phi_pixelEndcap);
  SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixelEndcap, -0.3, 0.3);
  SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixelEndcap, 0.7, 1.3);
  
  // same procedure as before
  // the number of Pixel layers and disks
  int pixelBarrelLayers = 3; // m_PIX_Mgr->numerology().numLayers();
  int pixelEndcapDisks  = 3; // m_PIX_Mgr->numerology().numDisks();
  int nPixelLayers = pixelBarrelLayers > pixelEndcapDisks ? pixelBarrelLayers : pixelEndcapDisks;


  for (int ilayer=0; ilayer<nPixelLayers; ilayer++) {
    // ------------- residualPulls in the pixels ---------------------------------------------------
    if (ilayer < pixelBarrelLayers){
   projectStandardProfileY(m_pullPhi_vs_incident_pixel_barrel_l[ilayer]
      , m_pullMeanPhi_vs_incident_pixel_barrel_layer[ilayer], m_pullWidthPhi_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_incident_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_incident_pixel_barrel_layer[ilayer], 0.7, 1.3);
    projectStandardProfileY(m_pullPhi_vs_eta_pixel_barrel_l[ilayer]
      , m_pullMeanPhi_vs_eta_pixel_barrel_layer[ilayer], m_pullWidthPhi_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_eta_pixel_barrel_layer[ilayer], -1.5, 1.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_eta_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullPhi_vs_phi_pixel_barrel_l[ilayer]
      , m_pullMeanPhi_vs_phi_pixel_barrel_layer[ilayer], m_pullWidthPhi_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixel_barrel_layer[ilayer], -1.5, 1.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixel_barrel_layer[ilayer], 0.5, 1.5);

    projectStandardProfileY(m_residualPhi_vs_incident_pixel_barrel_l[ilayer]
      , m_residualMeanPhi_vs_incident_pixel_barrel_layer[ilayer], m_residualWidthPhi_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_incident_pixel_barrel_layer[ilayer], -0.05, 0.05);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_incident_pixel_barrel_layer[ilayer], -0.05, 0.05);
    projectStandardProfileY(m_residualPhi_vs_eta_pixel_barrel_l[ilayer]
      , m_residualMeanPhi_vs_eta_pixel_barrel_layer[ilayer], m_residualWidthPhi_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_eta_pixel_barrel_layer[ilayer], -0.2, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_eta_pixel_barrel_layer[ilayer], -0.05, 0.3);
    projectStandardProfileY(m_residualPhi_vs_phi_pixel_barrel_l[ilayer]
      , m_residualMeanPhi_vs_phi_pixel_barrel_layer[ilayer], m_residualWidthPhi_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_phi_pixel_barrel_layer[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_phi_pixel_barrel_layer[ilayer], -0.05, 0.05);

    projectStandardProfileY(m_pullZ_vs_incident_pixel_barrel_l[ilayer]
      , m_pullMeanZ_vs_incident_pixel_barrel_layer[ilayer], m_pullWidthZ_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_incident_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_incident_pixel_barrel_layer[ilayer], 0.7, 1.3);
    projectStandardProfileY(m_pullZ_vs_eta_pixel_barrel_l[ilayer]
      , m_pullMeanZ_vs_eta_pixel_barrel_layer[ilayer], m_pullWidthZ_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_eta_pixel_barrel_layer[ilayer], -1, 1);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_eta_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullZ_vs_phi_pixel_barrel_l[ilayer]
      , m_pullMeanZ_vs_phi_pixel_barrel_layer[ilayer], m_pullWidthZ_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_phi_pixel_barrel_layer[ilayer], -0.5, 0.5);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_phi_pixel_barrel_layer[ilayer], 0.5, 1.5);

    projectStandardProfileY(m_residualZ_vs_incident_pixel_barrel_l[ilayer]
      , m_residualMeanZ_vs_incident_pixel_barrel_layer[ilayer], m_residualWidthZ_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_incident_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_incident_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_eta_pixel_barrel_l[ilayer]
      , m_residualMeanZ_vs_eta_pixel_barrel_layer[ilayer], m_residualWidthZ_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_eta_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_eta_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_phi_pixel_barrel_l[ilayer]
      , m_residualMeanZ_vs_phi_pixel_barrel_layer[ilayer], m_residualWidthZ_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_phi_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_phi_pixel_barrel_layer[ilayer], 0.5, 1.5);
    }

    if (ilayer < pixelEndcapDisks){
    projectStandardProfileY(m_pullPhi_vs_incident_pixel_endcap_d[ilayer]
      ,  m_pullMeanPhi_vs_incident_pixel_endcap_disk[ilayer],  m_pullWidthPhi_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_incident_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_incident_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullPhi_vs_eta_pixel_endcap_d[ilayer]
      ,  m_pullMeanPhi_vs_eta_pixel_endcap_disk[ilayer],  m_pullWidthPhi_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_eta_pixel_endcap_disk[ilayer], -0.5, 0.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_eta_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullPhi_vs_phi_pixel_endcap_d[ilayer]
      ,  m_pullMeanPhi_vs_phi_pixel_endcap_disk[ilayer],  m_pullWidthPhi_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixel_endcap_disk[ilayer], -1.5, 1.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixel_endcap_disk[ilayer], -0.2, 1.5);

    projectStandardProfileY(m_residualPhi_vs_incident_pixel_endcap_d[ilayer]
      , m_residualMeanPhi_vs_incident_pixel_endcap_disk[ilayer], m_residualWidthPhi_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_incident_pixel_endcap_disk[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_incident_pixel_endcap_disk[ilayer], -0.05, 0.05);
    projectStandardProfileY(m_residualPhi_vs_eta_pixel_endcap_d[ilayer]
      , m_residualMeanPhi_vs_eta_pixel_endcap_disk[ilayer], m_residualWidthPhi_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_eta_pixel_endcap_disk[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_eta_pixel_endcap_disk[ilayer], -0.05, 0.05);
    projectStandardProfileY(m_residualPhi_vs_phi_pixel_endcap_d[ilayer]
      , m_residualMeanPhi_vs_phi_pixel_endcap_disk[ilayer], m_residualWidthPhi_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_phi_pixel_endcap_disk[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_phi_pixel_endcap_disk[ilayer], -0.05, 0.05);

    projectStandardProfileY(m_pullZ_vs_incident_pixel_endcap_d[ilayer]
      , m_pullMeanZ_vs_incident_pixel_endcap_disk[ilayer], m_pullWidthZ_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_incident_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_incident_pixel_endcap_disk[ilayer], 0.5, 1.5);
   projectStandardProfileY(m_pullZ_vs_eta_pixel_endcap_d[ilayer]
      , m_pullMeanZ_vs_eta_pixel_endcap_disk[ilayer], m_pullWidthZ_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_eta_pixel_endcap_disk[ilayer], -0.03, 0.03);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_eta_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullZ_vs_phi_pixel_endcap_d[ilayer]
      , m_pullMeanZ_vs_phi_pixel_endcap_disk[ilayer], m_pullWidthZ_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_phi_pixel_endcap_disk[ilayer], -0.03, 0.03);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_phi_pixel_endcap_disk[ilayer], 0.5, 1.5);

    projectStandardProfileY(m_residualZ_vs_incident_pixel_endcap_d[ilayer]
      , m_residualMeanZ_vs_incident_pixel_endcap_disk[ilayer], m_residualWidthZ_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_incident_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_incident_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_eta_pixel_endcap_d[ilayer]
      , m_residualMeanZ_vs_eta_pixel_endcap_disk[ilayer], m_residualWidthZ_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_eta_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_eta_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_phi_pixel_endcap_d[ilayer]
      , m_residualMeanZ_vs_phi_pixel_endcap_disk[ilayer], m_residualWidthZ_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_phi_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_phi_pixel_endcap_disk[ilayer], 0.5, 1.5);

    }


    for (int iEtaBin=0; iEtaBin<5; iEtaBin++) {

      if (ilayer < pixelBarrelLayers){
        projectStandardProfileY(m_residualPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]
            , m_residualMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], m_residualWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]);
        SetSafeMinimumMaximum(m_residualMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], -0.3, 0.3);
        SetSafeMinimumMaximum(m_residualWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], 0., 0.25);
       }
 
       if (ilayer < pixelBarrelLayers){
        projectStandardProfileY(m_pullPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]
            , m_pullMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], m_pullWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]);
        SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], -0.5, 0.5);
        SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], 0.5, 1.5);
       }
    }
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
    d->WriteTObject(h);
    n++;
  }
  return n;
}

int reprocessHistos (TDirectory* file, const char* tracksName, const char* dirname, int verbose=0)
{
  TString dir;
  dir.Form("IDPerformanceMon/%s/%s",tracksName,dirname);
  TH1* h=0;
  file->GetObject (dir+"/res_vs_eta_d0", h);
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
  }
  for (TIter it= dir->GetListOfKeys(); TKey* k= dynamic_cast<TKey*>(it());) {
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

int reprocessHistos (TDirectory* file, int verbose=0)
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
      n += reprocessHistos (file, name->GetName(), name->GetTitle(), verbose);
  }
  if (!n) cerr << "No histogram directories found in file " << file->GetName() << endl;
  return n;
}

int reprocessHistos (const char* name, int verbose=0)
{
  if (gSystem->AccessPathName (name, kFileExists)) {
    cerr << "File " << name << " does not exist" << endl;
    return 2;
  }
  TFile* file= TFile::Open (name, "update");
  if (!file) {
    cerr << "Could not open file " << name << endl;
    return 3;
  }
  int n= reprocessHistos (file, verbose);
  delete file;
  return n ? 0 : 4;
}

void reprocessHistos (int verbose=0)
{
  for (TIter it= gROOT->GetListOfFiles(); TObject* o= it();) {
    TFile* file= dynamic_cast<TFile*>(o);
    if (!file) continue;
    reprocessHistos (file->GetName(), verbose);
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
    int stat= reprocessHistos (argv[i], verbose);
    if (!err) err= stat;
  }
  return err;
}
#endif
