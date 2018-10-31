/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TMath.h>
#include <TH2F.h>
#include <TGraphAsymmErrors.h>
#include <map>
#include <iostream>
#include "../TrigFTKSim/FTKTrackStream.h"
#include "../TrigFTKSim/FTKRoadStream.h"
#include "../TrigFTKSim/FTKTruthTrack.h"
#include "TChain.h"
#include "TFile.h"
#include "TProfile.h"

using namespace std;

class MatchInfo {
// private:
//   int m_barcode;
//   int m_evtindex;

public:
  int m_barcode;
  int m_evtindex;
  MatchInfo() : m_barcode(0), m_evtindex(-1) {;}
  MatchInfo(int v1, int v2) : m_barcode(v1), m_evtindex(v2) {;}
  bool operator==(const MatchInfo& o) const { return (m_barcode==o.m_barcode)&&(m_evtindex==o.m_evtindex); }
  bool operator<(const MatchInfo& o) const { if (m_evtindex!=o.m_evtindex) return (m_evtindex<o.m_evtindex); else return m_barcode<o.m_barcode; }
};

typedef multimap<MatchInfo,const FTKTrack*> FTKBarcodeMM;
unsigned nbins;
double maxntracks;
double d0min, d0max;
double z0min, z0max;
double phimin, phimax;
double etamin, etamax;
double abscurvmax;
double ptmax;
double Dd0;
double Dphi;
double Drelpt;
double Dcurv;
double Deta;
double Dz0;
double ptmincut;
double dx, dy;

// block of generic control histograms for the FTK tracks
TH1F *histopt_ftk;
TH1F *histoeta_ftk;
TH1F *histophi_ftk;

TH1F *histopt_ftk_pre_hw;
TH1F *histoeta_ftk_pre_hw;
TH1F *histophi_ftk_pre_hw;

TH1F *histopt_ftkI;
TH1F *histoeta_ftkI;
TH1F *histophi_ftkI;

TH1F *histopt_ftkI_pre_hw;
TH1F *histoeta_ftkI_pre_hw;
TH1F *histophi_ftkI_pre_hw;

TH1F *histopt_ftk_pattern;
TH1F *histoeta_ftk_pattern;
TH1F *histophi_ftk_pattern;

TH1F *histopt_ftk_hits;
TH1F *histoeta_ftk_hits;
TH1F *histophi_ftk_hits;

// FTK for fakes
TH1F *histopt_goodftk;
TH1F *histoeta_goodftk;
TH1F *histophi_goodftk;

TH1F *histopt_goodftk_pre_hw;
TH1F *histoeta_goodftk_pre_hw;
TH1F *histophi_goodftk_pre_hw;

TH1F *histopt_goodftkI;
TH1F *histoeta_goodftkI;
TH1F *histophi_goodftkI;

TH1F *histopt_goodftkI_pre_hw;
TH1F *histoeta_goodftkI_pre_hw;
TH1F *histophi_goodftkI_pre_hw;

TH1F *histopt_goodftk_pattern;
TH1F *histoeta_goodftk_pattern;
TH1F *histophi_goodftk_pattern;

TH1F *histopt_goodftk_hits;
TH1F *histoeta_goodftk_hits;
TH1F *histophi_goodftk_hits;


TH1F *histopt_goodftkU;
TH1F *histoeta_goodftkU;
TH1F *histophi_goodftkU;

TH1F *histopt_goodftkU_pre_hw;
TH1F *histoeta_goodftkU_pre_hw;
TH1F *histophi_goodftkU_pre_hw;

TH1F *histopt_goodftkUI;
TH1F *histoeta_goodftkUI;
TH1F *histophi_goodftkUI;

TH1F *histopt_goodftkUI_pre_hw;
TH1F *histoeta_goodftkUI_pre_hw;
TH1F *histophi_goodftkUI_pre_hw;

TH1F *histopt_goodftkU_pattern;
TH1F *histoeta_goodftkU_pattern;
TH1F *histophi_goodftkU_pattern;

TH1F *histopt_goodftkU_hits;
TH1F *histoeta_goodftkU_hits;
TH1F *histophi_goodftkU_hits;

// block of distribution related to truth tracks
TH1F *histopt_truth;
TH1F *histoeta_truth;
TH1F *histophi_truth;

TH1F *histopt_truthM;
TH1F *histoeta_truthM;
TH1F *histophi_truthM;

TH1F *histopt_truthM_pre_hw;
TH1F *histoeta_truthM_pre_hw;
TH1F *histophi_truthM_pre_hw;

TH1F *histopt_truthMI;
TH1F *histoeta_truthMI;
TH1F *histophi_truthMI;

TH1F *histopt_truthMI_pre_hw;
TH1F *histoeta_truthMI_pre_hw;
TH1F *histophi_truthMI_pre_hw;

TH1F *histopt_truthM_pattern;
TH1F *histoeta_truthM_pattern;
TH1F *histophi_truthM_pattern;

TH1F *histopt_truthM_hits;
TH1F *histoeta_truthM_hits;
TH1F *histophi_truthM_hits;

TH1F *histopt_truthM_cond;
TH1F *histoeta_truthM_cond;
TH1F *histophi_truthM_cond;

// Things to access variables!
FTKTrackStream *tracks(0);
FTKRoadStream *roads(0);
std::vector<FTKTruthTrack> *truthTracks(0);
Int_t RunNumber, EventNumber;
TChain *t_ftkdata;
TChain *t_truth;
TChain *t_evtinfo;

int towerNumber;
/* TString outputname; */
std::string outputname;
std::string psfile;
int ientry2;