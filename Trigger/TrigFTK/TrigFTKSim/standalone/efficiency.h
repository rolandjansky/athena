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
private:
  int m_barcode;
  int m_evtindex;

public:
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
//double dx, dy;
double vtxTruth[3];

// block of generic control histograms for the FTK tracks
TH2F *histocoordmasketa_ftk;
TH2F *histocoordmasketaphi_ftk[17];
TH2F *histocoordmaskz0_ftk;
TH2F *histocoordmaskphi_ftk;
TH2F *histonmisseta_ftk;
TH2F *histochisqndfeta_ftk;

TH1F *histontracks_ftk;
TH1F *histod0_ftk;
TH1F *histod0_ftks[20];
TH1F *histod0_ftk_poseta;
TH1F *histod0_ftk_negeta;
TH1F *histod0_ftk_pt2;
TH1F *histod0_ftk_pt5;
TH1F *histoz0_ftk;
TH1F *histophi_ftk;
TH2F *histophid0_ftk;
TH2F *histophid0_ftk_pt2;
TH2F *histophid0_ftk_pt5;
TH1F *histocurv_ftk;
TH1F *histoeta_ftk;
TH2F *histoetaphi_ftk;

TH2F *histoeff_etaphiplane[17];


TH2F *histoetaphi_ftk_IBL;
TH2F *histoetaphi_ftk_PixL0;
TH2F *histoetaphi_ftk_PixL1;
TH2F *histoetaphi_ftk_PixL2;
TH2F *histoetaz0_ftk;
TH2F *histoetaz0_ftk_IBL;
TH2F *histoetaz0_ftk_PixL0;
TH2F *histoetaz0_ftk_PixL1;
TH2F *histoetaz0_ftk_PixL2;
TH2F *histoetaphi_truth;
TH2F *histoetaphi_truthM;
TH2F *histoetaz0_truth;
TH2F *histoetaz0_truthM;
TH1F *histopt_ftk;

TH1F *histopt_ftk_lg;
TH1F *histopt_ftklo_lg;

TH2F *histoetaz0det_ftk_IBL;
TH2F *histoetaz0det_ftk_PixL0;
TH2F *histoetaz0det_ftk_PixL1;
TH2F *histoetaz0det_ftk_PixL2;

TH2F *histophiz0_ftk;
TH2F *histophiz0_ftk_IBL;
TH2F *histophiz0_ftk_PixL0;
TH2F *histophiz0_ftk_PixL1;
TH2F *histophiz0_ftk_PixL2;


TH1F *histopt_ftkzoom;
TH1F *histocurv_ftkzoom;

// FTK for fakes
TH1F *histontracks_goodftk;


TH1F *histopt_goodftk_lg;
TH1F *histopt_goodftklo_lg;

TH1F *histopt_goodftkUlo_lg;
TH1F *histopt_goodftkU_lg;

TH1F *histoetaabs_truth;
TH1F *histoeff_truth;
TH1F *histopt_truthlo_lg;
TH1F *histopt_truth_lg;
TH1F *histoetaabs_truthM;
TH1F *histoeff_truthM;

TH1F *histopt_truthMlo_lg;
TH1F *histopt_truthM_lg;


TH1F *histod0_goodftk;
TH1F *histoz0_goodftk;
TH1F *histocurv_goodftk;
TH1F *histoeta_goodftk;
TH1F *histophi_goodftk;
TH1F *histopt_goodftk;

TH1F *histontracks_goodftkU;
TH1F *histod0_goodftkU;
TH1F *histoz0_goodftkU;
TH1F *histocurv_goodftkU;
TH1F *histoeta_goodftkU;
TH1F *histophi_goodftkU;
TH1F *histopt_goodftkU;

// block of distribution related to truth tracks
TH1F *histontracks_truth;
TH1F *histod0_truth;
TH1F *histoz0_truth;
TH1F *histocurv_truth;
TH1F *histoeta_truth;
TH1F *histophi_truth;
TH1F *histopt_truth;

TH1F *histontracks_truthM;
TH1F *histod0_truthM;
TH1F *histoz0_truthM;
TH1F *histocurv_truthM;
TH1F *histoeta_truthM;
TH1F *histophi_truthM;
TH1F *histopt_truthM;
TH1F *histod0res;
TH1F *histoz0res;

TProfile *histod0res_veta;
TProfile *histoz0res_veta;
TProfile *histod0res_vphi;
TH2F *histo2d0res_vphi;
TH2F *histo2d0res_veta;
TH2F *histo2d0res_coordbit;
TH2F *histo2d0truth_vphi;
TH2F *histo2d0ftk_vphi;
TH2F *histo2curvCurv;
TProfile *histoz0res_vphi;
TProfile *histod0res_vz0;
TProfile *histoz0res_vz0;

TH1F *histocurvres;
TH1F *histoetares;
TH1F *histophires;
TH1F *historelptres;
TH2F *historelptrespt;

// block of distribution related to truth muon tracks
TH1F *histontracks_truth_muon;
TH1F *histod0_truth_muon;
TH1F *histoz0_truth_muon;
TH1F *histocurv_truth_muon;
TH1F *histoeta_truth_muon;
TH1F *histophi_truth_muon;
TH1F *histopt_truth_muon;
TH1F *histopt_truth_muonlo_lg;
TH1F *histopt_truth_muon_lg;

TH1F *histontracks_truthM_muon;
TH1F *histod0_truthM_muon;
TH1F *histoz0_truthM_muon;
TH1F *histocurv_truthM_muon;
TH1F *histoeta_truthM_muon;
TH1F *histophi_truthM_muon;
TH1F *histopt_truthM_muon;
TH1F *histopt_truthM_muonlo_lg;
TH1F *histopt_truthM_muon_lg;

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
Int_t Use1stStage;
